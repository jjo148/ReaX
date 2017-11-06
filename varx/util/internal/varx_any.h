#pragma once

namespace detail {
/**
 A dynamic wrapper that can hold a value of any copy-constructible type.
 
 The type of the held value is erased. So to extract the held value (using `any::get()`), you have to provide the exact type of the held value. No base-class, of it, but the exact type it was constructed from. If in doubt, use `static_cast` before passing the value to the `any` constructor, to ensure that it's stored as a certain type.
 
 Two `any` instances are equality-comparable. If an instance `a` is compared to an instance `b` as in `a == b`, and both hold a scalar value (e.g. int, float, bool), the scalar values are converted and compared. So `var(1.f) == var(1)`. If both hold an object, it casts `b` to the type of `a`. If that succeeds, it compares them using `a`'s `operator==`. If `a` is not equality-comparable, it checks if the addresses of the wrapped values in `a` and `b` are equal. This may be false if both `a` and `b` were contructed from the same value, because the value may have been copied when constructing. Otherwise, `a` and `b` are considered to be non-equal.
 
 This class is used to create a dynamic layer between the type-safe varx::Observable and the type-safe rxcpp::observable.
*/
///@cond INTERNAL
class any
{
public:
    ///@{
    /**
     Creates an instance from an arithmetic value.
     */
    explicit any(int value);
    explicit any(juce::int64 value);
    explicit any(bool value);
    explicit any(float value);
    explicit any(double value);
    ///@}

    template<typename T>
    using is_enum = std::is_enum<typename std::decay<T>::type>;

    template<typename T>
    using is_pointer = std::is_pointer<typename std::decay<T>::type>;

    template<typename T>
    using is_arithmetic = std::is_arithmetic<typename std::decay<T>::type>;

    template<typename T>
    using is_class = std::is_class<typename std::decay<T>::type>;

    template<typename T>
    using is_any = std::is_base_of<any, typename std::decay<T>::type>;


    template<typename T, typename std::enable_if<is_enum<T>::value>::type...>
    explicit any(T&& value)
    : type(Type::Enum)
    {
        this->value.enumValue = static_cast<juce::int64>(value);
    }

    template<typename T, typename std::enable_if<is_pointer<T>::value>::type...>
    explicit any(T&& value)
    : type(Type::RawPointer)
    {
        this->value.rawPointerValue = value;
    }

    /**
     Creates a new instance, wrapping an arbitrary copy- or move-constructible type.
     
     If you use this constructor, make sure that the value you're passing in actually has the type that you try to `get<T>()` later on. One way to ensure this is to use `any(static_cast<T>(myT))`.
     */
    template<typename T, typename std::enable_if<is_class<T>::value && !is_any<T>::value>::type...>
    explicit any(T&& value)
    : type(Type::Object),
      value({}),
      objectValue(std::make_shared<EquatableTypedObject<typename std::decay<T>::type>>(std::forward<T>(value)))
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
        if (is<T>())
            return _get<T>();
        else
            throw typeMismatchError<T>();
    }

    /**
     Checks whether the held value is a T. For class types, it returns true only if the wrapped type is exactly T, not a base class.
     */
    template<typename T>
    inline bool is() const
    {
        return _is<T>();
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
    struct TypedObject : Object
    {
        template<typename U>
        TypedObject(U&& value)
        : Object(typeid(U)),
          t(std::forward<U>(value))
        {}

        T t;
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
            // If other contains a T, compare them:
            if (auto ptr = dynamic_cast<const EquatableTypedObject<T>*>(&other))
                return (TypedObject<T>::t == ptr->t);

            // other does not contain a T, so the objects can't be equal
            else
                return false;
        }
    };

    // The type of the held value. Needed to use the correct member of the union.
    enum class Type {
        Int,
        Int64,
        Bool,
        Float,
        Double,
        RawPointer,
        Enum,
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
        void* rawPointerValue;
        juce::int64 enumValue;
    } value;

    // The held value, if it's non-scalar.
    std::shared_ptr<Object> objectValue;


    template<typename T, typename std::enable_if<is_enum<T>::value>::type...>
    inline T _get() const
    {
        return static_cast<T>(value.enumValue);
    }

    template<typename T, typename std::enable_if<is_pointer<T>::value>::type...>
    inline T _get() const
    {
        return static_cast<T>(value.rawPointerValue);
    }

    template<typename T, typename std::enable_if<is_arithmetic<T>::value>::type...>
    inline T _get() const
    {
        switch (type) {
            case Type::Int:
                return value.intValue;
            case Type::Int64:
                // int64 can be converted to int
                return static_cast<T>(value.int64Value);
            case Type::Bool:
                return value.boolValue;
            case Type::Float:
                return value.floatValue;
            case Type::Double:
                return value.doubleValue;

            default:
                // Type mismatch
                throw typeMismatchError<T>();
        }
    }

    // Getter for object values
    template<typename T, typename std::enable_if<is_class<T>::value>::type...>
    inline T _get() const
    {
        // Type checking was already done in get()
        return getObjectPointer<T>()->t;
    }

    template<typename T>
    inline std::runtime_error typeMismatchError() const
    {
        static const std::string RequestedType = typeid(T).name();
        return std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + getTypeName() + ".");
    }

    template<typename T, typename std::enable_if<is_enum<T>::value>::type...>
    inline bool _is() const
    {
        return (type == Type::Enum);
    }

    template<typename T, typename std::enable_if<is_pointer<T>::value>::type...>
    inline bool _is() const
    {
        return (type == Type::RawPointer);
    }

    template<typename T, typename std::enable_if<is_arithmetic<T>::value>::type...>
    inline bool _is() const
    {
        return isArithmetic();
    }

    template<typename T, typename std::enable_if<is_class<T>::value>::type...>
    inline bool _is() const
    {
        return (getObjectPointer<T>() != nullptr);
    }

    template<typename T>
    inline const TypedObject<T>* getObjectPointer() const
    {
        return std::dynamic_pointer_cast<const TypedObject<T>>(objectValue).get();
    }

    bool isArithmetic() const;

    std::string getTypeName() const;

    JUCE_LEAK_DETECTOR(any)
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
