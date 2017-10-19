#pragma once

namespace detail {
class any
{
public:
    template<typename T>
    any(const T& t)
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

        const std::type_info& typeInfo;
    };

    template<typename T>
    struct Wrapped : WrappedBase
    {
        Wrapped(const T& t)
        : WrappedBase(typeid(t)),
          t(t)
        {}

        const T t;
    };

    std::shared_ptr<WrappedBase> wrapped;
};
}
