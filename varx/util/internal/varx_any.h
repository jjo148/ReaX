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

    any& operator=(const any&) = default;

#warning Maybe replace this with static any any::from<T>(const T&), to explicitly specify what should go in
    template<typename T>
    explicit any(const T& value)
    : any(value, std::is_enum<typename std::decay<T>::type>())
    {}

    template<typename T>
    T get() const
    {
        return _get<T>(std::is_enum<typename std::decay<T>::type>());
    }

    bool equals(const any& other) const;

private:
    template<typename T>
    any(T&& value, std::true_type /* is_enum */)
    : any(static_cast<juce::int64>(value))
    {}

    template<typename T>
    any(T&& value, std::false_type /* is_enum */)
    : any(juce::ReferenceCountedObjectPtr<Object>(new EquatableTypedObject<typename std::decay<T>::type>(std::forward<T>(value))))
    {}

    template<typename T>
    T _get(std::true_type /* is_enum */) const
    {
        jassert(type == Type::Int64);

        return static_cast<T>(getPrimitive<juce::int64>());
    }
    
#warning Implement template<typename T> bool is() const;

    template<typename T>
    T _get(std::false_type /* is_enum */) const
    {
#warning Replace assert by exception
        jassert(type == Type::Object);

        if (auto ptr = dynamic_cast<const T*>(objectValue.get()))
            return *ptr;

        // Type mismatch. Determine expected and actual type:
        static const std::string RequestedType = typeid(T).name();
        const std::string actualType = objectValue->typeInfo.name();

        // Throw error
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + actualType + ".");
    }

    enum class Type {
        Int,
        Int64,
        Bool,
        Float,
        Double,
        Object
    };

    Type type;

    union
    {
        int intValue;
        juce::int64 int64Value;
        bool boolValue;
        float floatValue;
        double doubleValue;
    } value;

    struct Object : juce::ReferenceCountedObject
    {
        Object(const std::type_info& typeInfo);

        virtual ~Object();

        virtual bool equals(const Object& other) const = 0;

        const std::type_info& typeInfo;
    };

    template<typename T>
    struct TypedObject : T, Object
    {
        template<typename U>
        TypedObject(U&& value)
        : T(std::forward<U>(value)),
          Object(typeid(U))
        {}
    };

    template<typename T, typename Enable = void>
    struct EquatableTypedObject : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;

        bool equals(const Object& other) const override
        {
            // Compare by address. dynamic_cast<void*> downcasts to the most derived class, so if this and other are the same object, the expression is true.
            return (dynamic_cast<const void*>(this) == dynamic_cast<const void*>(&other));
        }
    };

    template<typename T>
    struct EquatableTypedObject<T, typename std::enable_if<true, decltype(std::declval<const T&>() == std::declval<const T&>(), (void)0)>::type> : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;

        bool equals(const Object& other) const override
        {
            // If other has type T, compare the T portion of this and other:
            if (auto ptr = dynamic_cast<const T*>(&other))
                return (*static_cast<const T*>(this) == *ptr);

            // The types differ, so the objects can't be equal
            else
                return false;
        }
    };

#warning Use std::shared_ptr instead
    juce::ReferenceCountedObjectPtr<Object> objectValue;

    template<typename T>
    T getPrimitive() const
    {
        switch (type) {
            case Type::Int:
                return value.intValue;
            case Type::Int64:
                return static_cast<T>(value.int64Value);
            case Type::Bool:
                return value.boolValue;
            case Type::Float:
                return value.floatValue;
            case Type::Double:
                return value.doubleValue;

            default:
                jassertfalse;
                return 0;
        }
    }

    bool isNumeric() const;

    any(juce::ReferenceCountedObjectPtr<Object>&& object);
};

template<>
inline int any::get<int>() const
{
    jassert(isNumeric());
    return getPrimitive<int>();
}

template<>
inline juce::int64 any::get<juce::int64>() const
{
    jassert(isNumeric());
    return getPrimitive<juce::int64>();
}

template<>
inline bool any::get<bool>() const
{
    jassert(type == Type::Bool);
    return getPrimitive<bool>();
}

template<>
inline float any::get<float>() const
{
    jassert(isNumeric());
    return getPrimitive<float>();
}

template<>
inline double any::get<double>() const
{
    jassert(isNumeric());
    return getPrimitive<double>();
}
}
