#pragma once

namespace detail {
template<typename T>
using IsSimpleComponent = enable_if_t<std::is_base_of<juce::Component, T>::value && !std::is_base_of<juce::ImageComponent, T>::value && !std::is_base_of<juce::Button, T>::value && !std::is_base_of<juce::Label, T>::value && !std::is_base_of<juce::Slider, T>::value>;

template<typename T>
using IsImageComponent = enable_if_t<std::is_base_of<juce::ImageComponent, T>::value>;

template<typename T>
using IsButton = enable_if_t<std::is_base_of<juce::Button, T>::value>;

template<typename T>
using IsLabel = enable_if_t<std::is_base_of<juce::Label, T>::value>;

template<typename T>
using IsSlider = enable_if_t<std::is_base_of<juce::Slider, T>::value>;
}

/**
    Adds reactive extensions to a `Component` (or subclass).
 */
template<typename ComponentType>
class Reactive<ComponentType, detail::IsSimpleComponent<ComponentType>> : public ComponentType
{
public:
    /// Creates a new instance. @see `juce::Component::Component`.
    using ComponentType::ComponentType;

    /// The reactive extension object.
    const ComponentExtension rx{ *this };
};

/**
    Adds reactive extensions to an `ImageComponent` (or subclass).
 */
template<typename ImageComponentType>
class Reactive<ImageComponentType, detail::IsImageComponent<ImageComponentType>> : public ImageComponentType
{
public:
    /// Creates a new instance. @see `juce::ImageComponent::ImageComponent`.
    using ImageComponentType::ImageComponentType;

    /// The reactive extension object.
    const ImageComponentExtension rx{ *this };
};

/**
    Adds reactive extensions to a `Button` (or subclass).
 */
template<typename ButtonType>
class Reactive<ButtonType, detail::IsButton<ButtonType>> : public ButtonType
{
public:
    /// Creates a new instance. @see `juce::Button::Button`.
    using ButtonType::ButtonType;

    /// The reactive extension object.
    const ButtonExtension rx{ *this };
};

/**
    Adds reactive extensions to a `Label` (or subclass).
 */
template<typename LabelType>
class Reactive<LabelType, detail::IsLabel<LabelType>> : public LabelType
{
public:
    /// Creates a new instance. @see `juce::Label::Label`.
    using LabelType::LabelType;

    /// The reactive extension object.
    const LabelExtension rx{ *this };
};

/**
    Adds reactive extensions to a `Slider` (or subclass).
 */
template<typename SliderType>
class Reactive<SliderType, detail::IsSlider<SliderType>> : public SliderType
{
public:
    /// Creates a new instance. @see `juce::Slider::Slider`.
    using SliderType::SliderType;

    /// The reactive extension object.
    const SliderExtension rx{ *this };

    ///@cond INTERNAL
    double getValueFromText(const juce::String& text) override
    {
        const auto& getValueFromText = rx.getValueFromText.getValue();
        if (getValueFromText)
            return getValueFromText(text);
        else
            return SliderType::getValueFromText(text);
    }

    juce::String getTextFromValue(double value) override
    {
        const auto& getTextFromValue = rx.getTextFromValue.getValue();
        if (getTextFromValue)
            return getTextFromValue(value);
        else
            return SliderType::getTextFromValue(value);
    }

    double snapValue(double attemptedValue, juce::Slider::DragMode dragMode) override
    {
        const auto& snapValue = rx.snapValue.getValue();
        if (snapValue)
            return snapValue(attemptedValue, dragMode);
        else
            return SliderType::snapValue(attemptedValue, dragMode);
    }
    ///@endcond
};
