#pragma once

/**
    Adds reactive extensions to a juce::Component.
 */
class ComponentExtension : public ExtensionBase, private juce::ComponentListener
{
    juce::Component& parent;
    std::function<void(const TypedPublishSubject<juce::Colour>&)> storeSubject;

public:
    /** Creates a new instance for a given juce::Component */
    ComponentExtension(juce::Component& parent);

    /** Controls the visibility of the Component, and emits an item whenever it changes.​ */
    const TypedBehaviorSubject<bool> visible;

    /** Returns an Observer that controls the colour for the given colourId.​ */
    TypedObserver<juce::Colour> colour(int colourId) const;

private:
    void componentVisibilityChanged(juce::Component& component) override;
};

/**
    Adds reactive extensions to a juce::Button.
 */
class ButtonExtension : public ComponentExtension, private juce::Button::Listener
{
    const TypedPublishSubject<Empty> _clicked;
    const TypedPublishSubject<juce::String> _text;
    const TypedPublishSubject<juce::String> _tooltip;

public:
    /** Creates a new instance for a given Button. */
    ButtonExtension(juce::Button& parent);

    /** Emits an item whenever the Button is clicked.​ */
    const TypedObservable<Empty> clicked;

    /** Controls the ButtonState.​ */
    const TypedBehaviorSubject<juce::Button::ButtonState> buttonState;

    /** Controls the toggle state.​ */
    const TypedBehaviorSubject<bool> toggleState;

    /** Controls the button text.​ */
    const TypedObserver<juce::String> text;

    /** Controls the tooltip.​ */
    const TypedObserver<juce::String> tooltip;

private:
    void buttonClicked(juce::Button*) override;
    void buttonStateChanged(juce::Button*) override;
};

/**
    Adds reactive extensions to a juce::ImageComponent.
 */
class ImageComponentExtension : public ComponentExtension
{
    const TypedPublishSubject<juce::Image> _image;
    const TypedPublishSubject<juce::RectanglePlacement> _imagePlacement;

public:
    /** Creates a new instance for a given ImageComponent. */
    ImageComponentExtension(juce::ImageComponent& parent);

    /** Controls the displayed image.​ */
    const TypedObserver<juce::Image> image;

    /** Controls the placement of the image.​ */
    const TypedObserver<juce::RectanglePlacement> imagePlacement;
};

/**
    Adds reactive extensions to a juce::Label.
 */
class LabelExtension : public ComponentExtension, private juce::Label::Listener
{
    const TypedBehaviorSubject<bool> _discardChangesWhenHidingEditor;
    const TypedPublishSubject<juce::Font> _font;
    const TypedPublishSubject<juce::Justification> _justificationType;
    const TypedPublishSubject<juce::BorderSize<int>> _borderSize;
    const TypedPublishSubject<juce::WeakReference<juce::Component>> _attachedComponent;
    const TypedPublishSubject<bool> _attachedOnLeft;
    const TypedPublishSubject<float> _minimumHorizontalScale;
    const TypedPublishSubject<juce::TextInputTarget::VirtualKeyboardType> _keyboardType;
    const TypedPublishSubject<bool> _editableOnSingleClick;
    const TypedPublishSubject<bool> _editableOnDoubleClick;
    const TypedPublishSubject<bool> _lossOfFocusDiscardsChanges;
    const TypedBehaviorSubject<juce::WeakReference<juce::Component>> _textEditor;

public:
    /** Creates a new instance for a given Label. */
    LabelExtension(juce::Label& parent);

    /** Controls the Label's text. Setting a new string notifies all Label::Listeners.​ */
    const TypedBehaviorSubject<juce::String> text;

    /** Controls whether the Label is showing a TextEditor.​ **Type: bool** */
    const TypedBehaviorSubject<bool> showEditor;

    /** Controls whether changes are discarded when hiding the TextEditor. The default is false.​ */
    const TypedObserver<bool> discardChangesWhenHidingEditor;

    /** Controls the Label's font. */
    const TypedObserver<juce::Font> font;

    /** Controls the Label's justification.​ **Type: Justification** */
    const TypedObserver<juce::Justification> justificationType;

    /** Controls the Label's border size.​ **Type: BorderSize<int>** */
    const TypedObserver<juce::BorderSize<int>> borderSize;

    /** Attaches the Label to another Component.​ */
    const TypedObserver<juce::WeakReference<juce::Component>> attachedComponent;

    /** Controls whether the attachedComponent should be attached on the left.​ */
    const TypedObserver<bool> attachedOnLeft;

    /** Controls  the minimum amount that the Label font can be squashed horizontally before it starts using ellipsis.​ **Type: float** */
    const TypedObserver<float> minimumHorizontalScale;

    /** Controls the keyboard type to use in the TextEditor. If the editor is currently open, the type is changed for the open editor.​ */
    const TypedObserver<juce::TextInputTarget::VirtualKeyboardType> keyboardType;

    /** Controls whether clicking the Label opens a TextEditor.​ */
    const TypedObserver<bool> editableOnSingleClick;

    /** Controls whether double-clicking the Label opens a TextEditor.​ */
    const TypedObserver<bool> editableOnDoubleClick;

    /** Controls whether unfocussing the TextEditor discards changes.​ */
    const TypedObserver<bool> lossOfFocusDiscardsChanges;

    /** The currently visible TextEditor.​ */
    const TypedObservable<juce::WeakReference<juce::Component>> textEditor;

private:
    void labelTextChanged(juce::Label*) override;
    void editorShown(juce::Label*, juce::TextEditor&) override;
    void editorHidden(juce::Label*, juce::TextEditor&) override;
};


/**
    Adds reactive extensions to a juce::Slider.
 */
class SliderExtension : public ComponentExtension, private juce::Slider::Listener
{
    TypedPublishSubject<double> _minimum;
    TypedPublishSubject<double> _maximum;
    TypedPublishSubject<double> _doubleClickReturnValue;
    TypedPublishSubject<double> _interval;
    TypedPublishSubject<double> _skewFactorMidPoint;
    TypedBehaviorSubject<bool> _dragging;
    TypedBehaviorSubject<bool> _discardChangesWhenHidingTextBox;
    TypedPublishSubject<bool> _showTextBox;
    TypedPublishSubject<bool> _textBoxIsEditable;

public:
    /** Creates a new instance for a given Slider. */
    SliderExtension(juce::Slider& parent, const TypedObserver<std::function<double(const juce::String&)>>& getValueFromText, const TypedObserver<std::function<juce::String(double)>>& getTextFromValue);

    /** Controls the Slider value.​ */
    const TypedBehaviorSubject<double> value;

    /** Controls the minimum Slider value.​ */
    const TypedObserver<double> minimum;

    /** Controls the maximum Slider value.​ */
    const TypedObserver<double> maximum;

    /** Control the lowest value in a Slider with multiple thumbs. **Do not push items if the Slider has just one thumb.**​ */
    const TypedBehaviorSubject<double> minValue;

    /** Control the highest value in a Slider with multiple thumbs.​ **Do not push items if the Slider has just one thumb.**​ */
    const TypedBehaviorSubject<double> maxValue;

#warning This should be Maybe<double>
    /** Controls the default value of the slider.​ Push std::numeric_limits<double>::max() to prevent double-clicking from resetting the slider. */
    const TypedObserver<double> doubleClickReturnValue;

    /** Controls the step size for values.​ */
    const TypedObserver<double> interval;

    /** Sets the mid point for the Slider's skew factor.​ */
    const TypedObserver<double> skewFactorMidPoint;

    /** Whether the Slider is currently being dragged.​ */
    const TypedObservable<bool> dragging;

#warning This should be Maybe<int>
    /** The thumb that is currently being dragged.​ 0 is the main thumb, 1 is the minValue thumb, 2 is the maxValue thumb. */
    const TypedObservable<int> thumbBeingDragged;

    /** Controls whether the text-box is visible.​ */
    const TypedObserver<bool> showTextBox;

    /** Controls whether the text-box is editable.​ */
    const TypedObserver<bool> textBoxIsEditable;

    /** Controls whether changes are discarded when hiding the text-box. The default is false.​ */
    const TypedObserver<bool> discardChangesWhenHidingTextBox;

    /** Controls how a String that has been entered into the text-box is converted to a Slider value.​ If you don't use this, the slider will use its getValueFromText member function. */
    const TypedObserver<std::function<double(const juce::String&)>> getValueFromText;

    /** Controls how a Slider value is displayed as a String.​ If you don't use this, the slider will use its getTextFromValue member function. */
    const TypedObserver<std::function<juce::String(double)>> getTextFromValue;

private:
    void sliderValueChanged(juce::Slider*) override;
    void sliderDragStarted(juce::Slider*) override;
    void sliderDragEnded(juce::Slider*) override;

    static bool hasMultipleThumbs(const juce::Slider& parent);
};
