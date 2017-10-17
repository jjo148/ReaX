#pragma once

namespace varx {
namespace detail {
    template<typename T>
    using IsEnum = typename std::enable_if<std::is_enum<T>::value>::type;
    
    template<typename T>
    using IsCopyConstructible = typename std::enable_if<!std::is_enum<T>::value && std::is_copy_constructible<T>::value>::type;
    
    // If you get an error here, it probably means that you're using VARX_DEFINE_VARIANT_CONVERTER for a type that is not an enum and not copy-constructible.
    template<typename T, typename Enable = void>
    struct VariantConverter;
    
    // Specialization for enums. Just converts enum values to int64 and back.
    template<typename T>
    struct VariantConverter<T, IsEnum<T>> {
        static T fromVar(const juce::var& v)
        {
            return static_cast<T>(v.operator juce::int64());
        }
        
        static juce::var toVar(const T& enumValue)
        {
            return juce::var(enumValue);
        }
    };
    
    // Specialization for copy-constructible types. Wraps values in ReferenceCountedObjects, and stores those in the var.
    template<typename T>
    struct VariantConverter<T, IsCopyConstructible<T>>
    {
    public:
        static T fromVar(const juce::var& v)
        {
            juce::ReferenceCountedObjectPtr<juce::ReferenceCountedObject> ptr(v.getObject());
            
            if (auto wrapper = dynamic_cast<Wrapped*>(ptr.get()))
                return wrapper->t;
            
            // Type mismatch. Determine expected and actual type:
            static const std::string ExpectedType = typeid(Wrapped).name();
            std::string actualType;
            
            if (auto pointer = ptr.get()) {
                actualType = typeid(*pointer).name();
            }
            else {
                actualType = "nullptr";
            }
            
            // Throw error
            throw std::runtime_error("Error unwrapping type from var. Expected: " + ExpectedType + ". Actual: " + actualType + ".");
        }
        
        // This has a different signature than juce::VariantConverter (it uses a universal reference).
        template<typename U>
        static juce::var toVar(U&& u)
        {
            return new Wrapped(std::forward<U>(u));
        }
        
    private:
        // Wraps a copy- or move-constructible type as a ReferenceCountedObject, so it can be stored in a juce var
        struct Wrapped : public juce::ReferenceCountedObject
        {
            template<typename U>
            Wrapped(U&& u)
            : t(std::forward<U>(u))
            {}
            
            const T t;
        };
    };
}

/**
    Unwraps a type from a juce::var. It's a shorthand for juce::VariantConverter<T>::fromVar.
 */
template<typename T>
inline T fromVar(const juce::var& v)
{
    return juce::VariantConverter<T>::fromVar(v);
}

/**
    Wraps a type into a juce::var. It's a shorthand for juce::VariantConverter<T>::toVar.
 */
template<typename T>
inline juce::var toVar(T&& t)
{
    return juce::VariantConverter<typename std::decay<T>::type>::toVar(std::forward<T>(t));
}
}

/**
 Defines a juce::VariantConverter for a given type.
 
 If the type is an enum, it just converts the enum values to ints, and stores those in a var.
 
 If the type is copy-constructible, it wraps the type as a ReferenceCountedObject, and stores that in a var. If the type is also move-constructible, toVar() will prefer moving instead of copying.
 
 Example:
 
    struct Circle
    {
        float x, y, radius;
    };
    
    // This must be placed outside of functions
    VARX_DEFINE_VARIANT_CONVERTER(Circle)
    
    // Create an instance, wrap it into a var
    Circle myCircle;
    var wrapped = toVar(myCircle);
    
    // Do something with the var, for example:
    auto observable = Observable::just(wrapped);
 
    // Unwrap the Circle from a var
    observable.subscribe([](var item) {
        Circle unwrapped = fromVar<Circle>(item);
        // ...
    });
 
 **If you use Observable::distinctUntilChanged(), make sure to provide a custom comparison function, which actually compares your type in a meaningful way.**
 */
#define VARX_DEFINE_VARIANT_CONVERTER(__t) template<> struct juce::VariantConverter<__t> : varx::detail::VariantConverter<__t> {};


// Define VariantConverters for common types
VARX_DEFINE_VARIANT_CONVERTER(juce::Button::ButtonState)
VARX_DEFINE_VARIANT_CONVERTER(juce::TextInputTarget::VirtualKeyboardType)
VARX_DEFINE_VARIANT_CONVERTER(juce::Image)
VARX_DEFINE_VARIANT_CONVERTER(juce::Point<int>)
VARX_DEFINE_VARIANT_CONVERTER(juce::Point<float>)
VARX_DEFINE_VARIANT_CONVERTER(juce::Point<double>)
VARX_DEFINE_VARIANT_CONVERTER(juce::Justification)
VARX_DEFINE_VARIANT_CONVERTER(juce::RectanglePlacement)
VARX_DEFINE_VARIANT_CONVERTER(juce::BorderSize<int>)
VARX_DEFINE_VARIANT_CONVERTER(juce::Font)
VARX_DEFINE_VARIANT_CONVERTER(juce::Colour)

// Define a VariantConverter that converts from/to any std::function
template<typename ReturnType, typename... Args>
struct juce::VariantConverter<std::function<ReturnType(Args...)>> : varx::detail::VariantConverter<std::function<typename std::decay<ReturnType>::type(typename std::decay<Args>::type...)>>
{
};

// Define a VariantConverter for WeakReferences
template<typename T>
struct juce::VariantConverter<juce::WeakReference<T>>
{
    static juce::WeakReference<T> fromVar(const var& v)
    {
        if (v.isUndefined() || v.isVoid())
            return nullptr;
        else
            return varx::detail::VariantConverter<juce::WeakReference<T>>::fromVar(v);
    }
    
    static juce::var toVar(const juce::WeakReference<T>& weakReference)
    {
        return varx::detail::VariantConverter<juce::WeakReference<T>>::toVar(weakReference);
    }
};
