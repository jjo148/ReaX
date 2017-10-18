#pragma once

class any
{
public:
    template<typename T>
    explicit any(T&& t)
    : wrapped(std::make_shared<Wrapped<T>>(std::forward<T>(t)))
    {}
    
    template<typename T>
    T get() const
    {
        if (auto ptr = std::dynamic_pointer_cast<Wrapped<T>>(wrapped))
            return ptr->t;
        
        // Type mismatch. Determine expected and actual type:
        static const std::string RequestedType = typeid(Wrapped<T>).name();
        const std::string actualType = typeid(*wrapped).name();
        
        // Throw error
        throw std::runtime_error("Error getting type from any. Requested: " + RequestedType + ". Actual: " + actualType + ".");
    }
    
private:
    struct WrappedBase
    {
        virtual ~WrappedBase() {}
    };
    
    template<typename T>
    struct Wrapped : WrappedBase
    {
        template<typename U>
        Wrapped(U&& u)
        : t(std::forward<U>(u))
        {}
        
        const T t;
    };
    
    std::shared_ptr<WrappedBase> wrapped;
};
