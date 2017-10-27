#pragma once

namespace detail {
/**
 A dynamic wrapper that can hold a value of any copy-constructible type.
 
 The type of the held value is erased. So to extract the held value (using `any::get()`), you have to provide the type of the held value.
 
 Two `any` instances are equality-comparable. If an instance `a` is compared to an instance `b` as in `a == b`, and both hold a scalar value (e.g. int, float, bool), the scalar values are converted and compared. So `var(1.f) == var(1)`. If both hold an object, it casts `b` to the type of `a`. If that succeeds, it compares them using `a`'s `operator==`. If `a` is not equality-comparable, it checks if the addresses of the wrapped values in `a` and `b` are equal. This may be false if both `a` and `b` were contructed from the same value, because the value may have been copied when constructing. Otherwise, `a` and `b` are considered to be non-equal.
 
 This class is used to create a dynamic layer between the type-safe varx::Observable and the type-safe rxcpp::observable.
*/
///@cond INTERNAL
class any
{
public:
    ///@{
    /**
     Creates an instance from a primitive value.
     */
    explicit any(int value);
    explicit any(juce::int64 value);
    explicit any(bool value);
    explicit any(float value);
    explicit any(double value);
    ///@}

    /// Checks if T is not any. Used to prevent nested any(any(...)) instances. 
    template<typename T>
    using DisableIfAny = typename std::enable_if<!std::is_base_of<any, typename std::decay<T>::type>::value>::type;

    /// Checks if T is a scalar type (e.g. numeric, bool, enum). 
    template<typename T>
    using IsScalar = std::is_scalar<typename std::decay<T>::type>;

    /**
     Creates a new instance, wrapping an arbitrary copy- or move-constructible type.
     
     If you use this constructor, make sure that the value you're passing in actually has the type that you try to `get()` later on. Example: You have a type `T` that is implicitly convertible to `U`, but not a subclass. If you pass in a `T` here, and then call `get<U>()`, it will throw an exception. One way to ensure this is to use `any(static_cast<U>(myT))`.
     */
    template<typename T, typename Enable = DisableIfAny<T>>
    explicit any(T&& value)
    : any(std::forward<T>(value), IsScalar<T>())
    {}

    /// Default move constructor 
    any(any&&) = default;

    /// Default copy constructor. If the wrapped value is scalar, it is copied. Otherwise, it is shared by reference. 
    any(const any&) = default;

    /// Default copy assignment operator. If the wrapped value is scalar, it is copied. Otherwise, it is shared by reference. 
    any& operator=(const any&) = default;

    /// Extracts the held value as a T. Throws an exception if the held value is not a T.
    template<typename T>
    inline T get() const
    {
        return _get<T>(IsScalar<T>());
    }

    /**
     Checks whether the held value is a T. For class types, it returns true if the wrapped type is a T or an instance of a subclass of T.
     
     **It returns false if the wrapped type is just implicitly convertible to T.**
     */
    template<typename T>
    inline bool is() const
    {
        return _is<T>(IsScalar<T>());
    }

    /**
     Compares the held value to that of another instance.
     
     If the wrapped type is equality-comparable (i.e. has operator==), the values are compared using ==. Otherwise, it returns true if the wrapped values are the same instance.
     
     Scalar types are converted as needed (i.e. float to int).
     */
    bool equals(const any& other) const;

private:
    // Type-erased wrapper
    struct Object
    {
        Object(const std::type_info& typeInfo);
        virtual ~Object() {}
        virtual bool equals(const Object& other) const = 0;

        const std::type_info& typeInfo;
    };

    // Object subclass that is also a T. It inherits from T to make this work: any(Derived()).get<Base>();
    template<typename T>
    struct TypedObject : T, Object
    {
        template<typename U>
        TypedObject(U&& value)
        : T(std::forward<U>(value)),
          Object(typeid(U))
        {}
    };

    // Checks if T has operator==
    template<typename T>
    using HasEqualityOperator = typename std::enable_if<true, decltype(std::declval<T&>() == std::declval<T&>(), (void)0)>::type;

    // Implements the equals() function using pointer comparison
    template<typename T, typename Enable = void>
    struct EquatableTypedObject : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;

        bool equals(const Object& other) const override
        {
            // Compare by address. dynamic_cast<void*> downcasts to the most derived class, so if this and other are the same instance, the expression is true.
            return (dynamic_cast<const void*>(this) == dynamic_cast<const void*>(&other));
        }
    };

    // Implements the equals() function using operator==
    template<typename T>
    struct EquatableTypedObject<T, HasEqualityOperator<T>> : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;

        bool equals(const Object& other) const override
        {
            // If other is a T, compare the T portion of this and other:
            if (auto ptr = dynamic_cast<const T*>(&other))
                return (static_cast<const T>(*this) == *ptr);

            // other is not a T, so the objects can't be equal
            else
                return false;
        }
    };

    // The type of the held value. Needed to use the correct member of the enum.
    enum class Type {
        Int,
        Int64,
        Bool,
        Float,
        Double,
        Object
    };

    Type type;

    // The held value, if it's scalar (including enums).
    union
    {
        int intValue;
        juce::int64 int64Value;
        bool boolValue;
        float floatValue;
        double doubleValue;
    } value;

    // The held value, if it's non-scalar.
    std::shared_ptr<Object> objectValue;

    // Constructor for scalar values that aren't handled by any of the specialized public constructors. For example: enums.
    template<typename T>
    any(T&& value, /* is_scalar: */ std::true_type)
    : any(static_cast<juce::int64>(value))
    {}

    // Constructor for non-scalar values
    template<typename T>
    any(T&& value, /* is_scalar: */ std::false_type)
    : type(Type::Object),
      value({}),
      objectValue(std::make_shared<EquatableTypedObject<typename std::decay<T>::type>>(std::forward<T>(value)))
    {}

    // Getter for scalar values (including enums)
    template<typename T>
    inline T _get(/* is_scalar: */ std::true_type) const
    {
        switch (type) {
            case Type::Int:
                return static_cast<T>(value.intValue);
            case Type::Int64:
                // int64 can be converted to int
                return static_cast<T>(value.int64Value);
            case Type::Bool:
                return static_cast<T>(value.boolValue);
            case Type::Float:
                return static_cast<T>(value.floatValue);
            case Type::Double:
                return static_cast<T>(value.doubleValue);

            default: {
                // Type mismatch. Throw error:
                static const std::string RequestedType = typeid(T).name();
                throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + getTypeName() + ".");
            }
        }
    }

    // Getter for non-scalar values
    template<typename T>
    inline T _get(/* is_scalar: */ std::false_type) const
    {
        // Try to cast object value to T
        if (auto ptr = std::dynamic_pointer_cast<const T>(objectValue))
            return *ptr;

        // Type mismatch. Throw error:
        static const std::string RequestedType = typeid(T).name();
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + getTypeName() + ".");
    }

    template<typename T>
    inline bool _is(/* is_scalar: */ std::true_type) const
    {
        return isScalar();
    }

    template<typename T>
    inline bool _is(/* is_scalar: */ std::false_type) const
    {
        return (!isScalar() && std::dynamic_pointer_cast<const T>(objectValue) != nullptr);
    }

    bool isScalar() const;

    std::string getTypeName() const;
};
///@endcond

inline bool operator==(const any& lhs, const any& rhs)
{
    return lhs.equals(rhs);
}
inline bool operator!=(const any& lhs, const any& rhs)
{
    return !(lhs == rhs);
}
}
