#pragma once

namespace detail {
class any
{
public:
    template<typename T>
    explicit any(const T& t)
    : wrapped(std::make_shared<Wrapped<T>>(t))
    {}

    template<typename T>
    const T& get() const
    {
        if (auto ptr = std::dynamic_pointer_cast<Wrapped<T>>(wrapped))
            return ptr->t;

        // Type mismatch. Determine expected and actual type:
        static const std::string RequestedType = typeid(T).name();
        const std::string actualType = wrapped->typeInfo.name();

        // Throw error
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + actualType + ".");
    }

private:
    struct WrappedBase
    {
        WrappedBase(const std::type_info& typeInfo)
        : typeInfo(typeInfo)
        {}

        virtual ~WrappedBase() {}
        virtual bool equals(const WrappedBase& other) = 0;

        const std::type_info& typeInfo;
    };

    template<typename T>
    struct Wrapped : WrappedBase
    {
        Wrapped(const T& t)
        : WrappedBase(typeid(t)),
          t(t)
        {}

        bool equals(const WrappedBase& other) override
        {
            if (auto otherPtr = dynamic_cast<const Wrapped<T>*>(&other))
                return _equals<T>()(t, otherPtr->t);
            else
                return false;
        }

        template<typename U, typename Enable = void>
        struct _equals
        {
            bool operator()(const U& lhs, const U& rhs)
            {
                return (&lhs == &rhs);
            }
        };

        template<typename U>
        struct _equals<U, typename std::enable_if<true, decltype(std::declval<U&>() == std::declval<U&>(), (void)0)>::type>
        {
            bool operator()(const U& lhs, const U& rhs)
            {
                return (lhs == rhs);
            }
        };

        const T t;
    };

    std::shared_ptr<WrappedBase> wrapped;

    friend bool operator==(const any& lhs, const any& rhs);
};

inline bool operator==(const any& lhs, const any& rhs)
{
    return lhs.wrapped->equals(*rhs.wrapped);
}
inline bool operator!=(const any& lhs, const any& rhs)
{
    return !(lhs == rhs);
}
}
