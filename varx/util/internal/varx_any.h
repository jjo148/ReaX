#pragma once

namespace detail {
class any
{
public:
    explicit any(int value);
    explicit any(juce::int64 value);
    explicit any(bool value);
    explicit any(float value);
    explicit any(double value);
    explicit any(const char* value);
    any(const any&) = default;
    any(any&&) = default;

    template<typename T>
    using DisableIfAny = typename std::enable_if<!std::is_base_of<any, typename std::decay<T>::type>::value>::type;

    template<typename T>
    using IsScalar = std::is_scalar<typename std::decay<T>::type>;

    /**
     If you use this constructor, make sure that the value you're passing in actually has the type that you try to get() later on. For example, if you have a type T that is implicitly convertible to U, but not a subclass. If you pass in a T here, and then call get<U>(), it won't work. One way to ensure this is to write any(static_cast<U>(myT)).
     */
    template<typename T, typename Enable = DisableIfAny<T>>
    explicit any(T&& value)
    : any(std::forward<T>(value), IsScalar<T>())
    {}

    any& operator=(const any&) = default;

    /**
     Extracts the stored value as a T. Throws an exception if the stored value is not a T.
     */
    template<typename T>
    inline T get() const
    {
        return _get<T>(IsScalar<T>());
    }

    /**
     Compares the stored value to that of another instance.
     
     If the wrapped type is equality-comparable (i.e. has operator==), the values are compared using ==. Otherwise, it returns true if the wrapped values are the same instance.
     
     Scalar types are converted as needed (i.e. float to int).
     */
    bool equals(const any& other) const;

private:
    // Type-erased wrapper
    struct Object
    {
        Object(const std::type_info& typeInfo);
        virtual ~Object();
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
    using HasEqualityOperator = typename std::enable_if<true, decltype(std::declval<const T&>() == std::declval<const T&>(), (void)0)>::type;

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

    // The type of the stored value. Needed to use the correct member of the enum.
    enum class Type {
        Int,
        Int64,
        Bool,
        Float,
        Double,
        Object
    };

    Type type;

    // The stored value, if it's scalar (including enums).
    union
    {
        int intValue;
        juce::int64 int64Value;
        bool boolValue;
        float floatValue;
        double doubleValue;
    } value;

    // The stored value, if it's non-scalar.
    std::shared_ptr<Object> objectValue;

    // Constructor for scalar values that aren't handled by any of the specialized public constructors. For example: enums.
    template<typename T>
    any(T&& value, std::true_type)
    : any(static_cast<juce::int64>(value))
    {}

    // Constructor for non-scalar values
    template<typename T>
    any(T&& value, std::false_type)
    : type(Type::Object),
      value({}),
      objectValue(std::make_shared<EquatableTypedObject<typename std::decay<T>::type>>(std::forward<T>(value)))
    {}

    // Getter for scalar values (including enums)
    template<typename T>
    inline T _get(std::true_type) const
    {
#warning Add better assert
        jassert(type != Type::Object);

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

            default:
                jassertfalse;
                return T();
        }
    }

    // Getter for non-scalar values
    template<typename T>
    inline T _get(std::false_type) const
    {
#warning Replace assert by exception / null check further down
        jassert(type == Type::Object);

        // Try to cast object value to T
        if (auto ptr = std::dynamic_pointer_cast<const T>(objectValue))
            return *ptr;

        // Type mismatch. Determine expected and actual type:
        static const std::string RequestedType = typeid(T).name();
        const std::string actualType = objectValue->typeInfo.name();

        // Throw error
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + actualType + ".");
    }

    bool isNumeric() const;
};

inline bool operator==(const any& lhs, const any& rhs)
{
    return lhs.equals(rhs);
}
inline bool operator!=(const any& lhs, const any& rhs)
{
    return !(lhs == rhs);
}
}
