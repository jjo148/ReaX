#pragma once

#include <sstream>

#define VARX_DEFINE_PRINT_FUNCTION(__type, __body) \
    namespace varx { \
    namespace printfunctions { \
        inline juce::String print(const __type& value) __body \
    } \
    } \
    namespace juce { \
    inline std::ostream& operator<<(std::ostream& os, const __type& value) \
    { \
        os << varx::printfunctions::print(value); \
        return os; \
    } \
    }

VARX_DEFINE_PRINT_FUNCTION(juce::var, {
    if (value.isBool())
        return (value ? "true" : "false");
    else if (value.isVoid())
        return "void";
    else if (value.isUndefined())
        return "undefined";
    else if (value.isString())
        return "\"" + value.toString() + "\"";
    else if (value.isArray())
        return varx::printfunctions::print(*value.getArray());
    else
        return value.toString();
})

VARX_DEFINE_PRINT_FUNCTION(juce::StringArray, {
    return "{" + value.joinIntoString(", ") + "}";
})

namespace varx {
    namespace printfunctions {
        template<typename T>
        using IsOstreamable = typename std::enable_if<true, decltype(std::declval<std::ostream>() << std::declval<const T&>(), std::declval<T>())>::type;
        
        template<typename T>
        inline juce::String print(const IsOstreamable<T>& value)
        {
            std::stringstream stream;
            stream << value;
            return String(stream.str());
        }
    }
}

// Print function for all Array<T>s where T is printable
namespace varx {
namespace printfunctions {
    template<typename T>
    using IsPrintable = typename std::enable_if<true, decltype(varx::printfunctions::print(std::declval<const T&>()), std::declval<T>())>::type;

    template<typename T>
    inline juce::String print(const juce::Array<IsPrintable<T>>& value)
    {
        juce::StringArray strings;
        for (const T& v : value)
            strings.add(varx::printfunctions::print(v));

        return varx::printfunctions::print(strings);
    }
}
}
namespace juce {
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const juce::Array<varx::printfunctions::IsPrintable<T>>& value)
{
    os << varx::printfunctions::print(value);
    return os;
}
}


#undef VARX_DEFINE_PRINT_FUNCTION
