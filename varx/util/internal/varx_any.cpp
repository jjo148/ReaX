
class any2
{
public:
    any2(int value)
    : type(Type::Int),
      value({ .intValue = value })
    {}

    any2(juce::int64 value)
    : type(Type::Int64),
      value({ .int64Value = value })
    {}

    any2(bool value)
    : type(Type::Bool),
      value({ .boolValue = value })
    {}

    any2(float value)
    : type(Type::Float),
      value({ .floatValue = value })
    {}

    any2(double value)
    : type(Type::Double),
      value({ .doubleValue = value })
    {}

    any2(const char* cString)
    : any2(String(cString))
    {}

    template<typename T>
    any2(T&& value)
    : any2(juce::ReferenceCountedObjectPtr<Object>(new EquatableTypedObject<T>(std::forward<T>(value))))
    {}

    template<typename T>
    T get() const
    {
        jassert(type == Type::Object);

        if (auto ptr = dynamic_cast<const TypedObject<T>*>(objectValue.get()))
            return ptr->value;

        // Type mismatch. Determine expected and actual type:
        static const std::string RequestedType = typeid(T).name();
        const std::string actualType = objectValue->typeInfo.name();

        // Throw error
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + actualType + ".");
    }

    bool equals(const any2& other) const;

private:
    enum class Type {
        Int,
        Int64,
        Bool,
        Float,
        Double,
        Object
    };

    const Type type;

    const union
    {
        int intValue;
        int64 int64Value;
        bool boolValue;
        float floatValue;
        double doubleValue;
    } value;

    struct Object : juce::ReferenceCountedObject
    {
        Object(const std::type_info& typeInfo)
        : typeInfo(typeInfo)
        {}

        virtual ~Object() {}
        
        virtual bool equals(const Object& other) const = 0;

        const std::type_info& typeInfo;
    };

    template<typename T>
    struct TypedObject : Object
    {
        template<typename U>
        TypedObject(U&& value)
        : Object(typeid(U)),
          value(std::forward<U>(value))
        {}

        const T value;
    };
    
    template<typename T, typename Enable = void>
    struct EquatableTypedObject : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;
        
        bool equals(const Object& other) const override
        {
            return (this == &other);
        }
    };
    
    template<typename T>
    struct EquatableTypedObject<T, typename std::enable_if<true, decltype(std::declval<const T&>() == std::declval<const T&>(), (void)0)>::type> : TypedObject<T>
    {
        using TypedObject<T>::TypedObject;
        
        bool equals(const Object& other) const override
        {
            if (auto ptr = dynamic_cast<const TypedObject<T>*>(&other))
                return (TypedObject<T>::value == ptr->value);
            else
                return false;
        }
    };

    juce::ReferenceCountedObjectPtr<Object> objectValue;

    any2(juce::ReferenceCountedObjectPtr<Object>&& object)
    : type(Type::Object),
      value({}),
      objectValue(object)
    {}

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
                        
    bool isNumeric() const
    {
        return (type != Type::Bool && type != Type::Object);
    }
};

template<>
int any2::get<int>() const
{
    jassert(isNumeric());
    return getPrimitive<int>();
}

template<>
juce::int64 any2::get<juce::int64>() const
{
    jassert(isNumeric());
    return getPrimitive<juce::int64>();
}

template<>
bool any2::get<bool>() const
{
    jassert(type == Type::Bool);
    return getPrimitive<bool>();
}

template<>
float any2::get<float>() const
{
    jassert(isNumeric());
    return getPrimitive<float>();
}

template<>
double any2::get<double>() const
{
    jassert(isNumeric());
    return getPrimitive<double>();
}


// .cpp file

bool any2::equals(const any2& other) const
{
    switch (type) {
        case Type::Int:
            return (other.isNumeric() && get<int>() == other.get<int>());
        case Type::Int64:
            return (other.isNumeric() && get<juce::int64>() == other.get<juce::int64>());
        case Type::Bool:
            return (other.type == Type::Bool && get<bool>() == other.get<bool>());
        case Type::Float:
            return (other.isNumeric() && get<float>() == other.get<float>());
        case Type::Double:
            return (other.isNumeric() && get<double>() == other.get<double>());
        case Type::Object:
            return (other.type == Type::Object && objectValue->equals(*other.objectValue));
    }
}

inline bool operator==(const any2& lhs, const any2& rhs)
{
    return lhs.equals(rhs);
}
inline bool operator!=(const any2& lhs, const any2& rhs)
{
    return !(lhs == rhs);
}

namespace {
void foo()
{
    any2 test1(3);
    test1.get<int>();
    test1.get<String>();

    any2 test2("Foo");
    any2 test3(String("Bar"));
    any2 test4(3.4f);
    any2 test5(3.4);
    any2 test6(false);
    any2 test7(juce::int64(4));
    any2 test8(juce::Point<int>(2, 4));
    any2 test9(any2(String("Inception")));

    test1.get<double>();

    test8.get<juce::Point<int>>();
}
}
