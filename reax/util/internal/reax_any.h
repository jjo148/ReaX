#pragma once

namespace detail {
/**
 A dynamic wrapper that can hold a value of any copy-constructible type.
 
 The type of the held value is erased. So to extract the held value (using `any::get()`), you have to provide the exact type of the held value. No base-class, of it, but the exact type it was constructed from. If in doubt, use `static_cast` before passing the value to the `any` constructor, to ensure that it's stored as a certain type.
 
 This class is used to create a dynamic layer between the type-safe `reax::Observable` and the type-safe `rxcpp::observable`.
*/
///@cond INTERNAL
class any final
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
    using is_enum = std::is_enum<decay_t<T>>;

    template<typename T>
    using is_pointer = std::is_pointer<decay_t<T>>;

    template<typename T>
    using is_arithmetic = std::is_arithmetic<decay_t<T>>;

    template<typename T>
    using is_class = std::is_class<decay_t<T>>;

    template<typename T>
    using is_any = std::is_base_of<any, decay_t<T>>;


    template<typename T>
    explicit any(T&& value, enable_if_t<is_enum<T>::value>* = 0)
    : type(Type::Enum),
      enumValue(static_cast<juce::int64>(value))
    {}

    template<typename T>
    explicit any(T&& value, enable_if_t<is_pointer<T>::value>* = 0)
    : type(Type::RawPointer),
      rawPointerValue(value)
    {}

    /**
     Creates a new instance, wrapping an arbitrary copy- or move-constructible type.
     
     If you use this constructor, make sure that the value you're passing in actually has the type that you try to `get<T>()` later on. One way to ensure this is to use `any(static_cast<T>(myT))`.
     */
    template<typename T>
    explicit any(T&& value, enable_if_t<is_class<T>::value && !is_any<T>::value>* = 0)
    : type(Type::Object),
      objectValue(std::make_shared<TypedObject<decay_t<T>>>(std::forward<T>(value)))
    {}

    /// Default move constructor
    any(any&&) = default;

    /// Default copy constructor. If the wrapped value is scalar, it is copied. Otherwise, it is shared by reference.
    any(const any&) = default;

    /// Default copy assignment operator. If the wrapped value is scalar, it is copied. Otherwise, it is shared by reference.
    any& operator=(const any&) = default;

    ///@{
    /**
     Extracts the held value as a T. Throws an exception if the held value is not a T.
     
     Objects are returned by `const &`, so no copy is made.
     */
    template<typename T>
    T get(enable_if_t<is_enum<T>::value>* = 0) const
    {
        if (!is<T>())
            throw typeMismatchError<T>();

        return static_cast<T>(enumValue);
    }

    template<typename T>
    T get(enable_if_t<is_pointer<T>::value>* = 0) const
    {
        if (!is<T>())
            throw typeMismatchError<T>();

        return static_cast<T>(rawPointerValue);
    }

    template<typename T>
    T get(enable_if_t<is_arithmetic<T>::value>* = 0) const
    {
        if (!is<T>())
            throw typeMismatchError<T>();

        switch (type) {
            case Type::Int:
                return static_cast<T>(intValue);
            case Type::Int64:
                // int64 can be converted to int
                return static_cast<T>(int64Value);
            case Type::Bool:
                return static_cast<T>(boolValue);
            case Type::Float:
                return static_cast<T>(floatValue);
            case Type::Double:
                // double can be converted to float
                return static_cast<T>(doubleValue);

            default:
                // Type mismatch
                throw typeMismatchError<T>();
        }
    }

    template<typename T>
    const T& get(enable_if_t<is_class<T>::value>* = 0) const
    {
        if (!is<T>())
            throw typeMismatchError<T>();

        return getObjectPointer<T>()->t;
    }
    ///@}

    /**
     Checks whether the held value is a T. For class types, it returns true only if the wrapped type is exactly T, not a base class.
     */
    template<typename T>
    bool is(enable_if_t<is_enum<T>::value>* = 0) const
    {
        return (type == Type::Enum);
    }

    template<typename T>
    bool is(enable_if_t<is_pointer<T>::value>* = 0) const
    {
        return (type == Type::RawPointer);
    }

    template<typename T>
    bool is(enable_if_t<is_arithmetic<T>::value>* = 0) const
    {
        return isArithmetic();
    }

    template<typename T>
    bool is(enable_if_t<is_class<T>::value>* = 0) const
    {
        return (getObjectPointer<T>() != nullptr);
    }

private:
    // Type-erased wrapper
    struct Object
    {
        Object(const std::type_info& typeInfo);
        virtual ~Object() {}

        const std::type_info& typeInfo;
    };

    // Object subclass that holds a T.
    template<typename T>
    struct TypedObject : Object
    {
        template<typename U>
        TypedObject(U&& value)
        : Object(typeid(U)),
          t(std::forward<U>(value))
        {}

        const T t;
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
    };

    // The held value, if it's non-scalar.
    std::shared_ptr<const Object> objectValue;

    template<typename T>
    std::runtime_error typeMismatchError() const
    {
        static const std::string RequestedType = typeid(T).name();
        return std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + getTypeName() + ".");
    }

    template<typename T>
    const TypedObject<T>* getObjectPointer() const
    {
        return std::dynamic_pointer_cast<const TypedObject<T>>(objectValue).get();
    }

    bool isArithmetic() const;

    std::string getTypeName() const;
    
    JUCE_LEAK_DETECTOR(any)
};
///@endcond
}
