#pragma once

/**
    Adds reactive extensions to a juce::Component.
 */
class ComponentExtension : public ExtensionBase, private juce::ComponentListener
{
    juce::Component& parent;
    std::function<void(const PublishSubject<juce::Colour>&)> storeSubject;

public:
    /** Creates a new instance for a given juce::Component */
    ComponentExtension(juce::Component& parent);

    /** Controls the visibility of the Component, and emits an item whenever it changes.​ */
    const BehaviorSubject<bool> visible;

    /** Returns an Observer that controls the colour for the given colourId.​ */
    Observer<juce::Colour> colour(int colourId) const;

private:
    void componentVisibilityChanged(juce::Component& component) override;
};

/**
    Adds reactive extensions to a juce::Button.
 */
class ButtonExtension : public ComponentExtension, private juce::Button::Listener
{
    const PublishSubject<Empty> _clicked;
    const PublishSubject<juce::String> _text;
    const PublishSubject<juce::String> _tooltip;

public:
    /** Creates a new instance for a given Button. */
    ButtonExtension(juce::Button& parent);

    /** Emits an item whenever the Button is clicked.​ */
    const Observable<Empty> clicked;

    /** Controls the ButtonState.​ */
    const BehaviorSubject<juce::Button::ButtonState> buttonState;

    /** Controls the toggle state.​ */
    const BehaviorSubject<bool> toggleState;

    /** Controls the button text.​ */
    const Observer<juce::String> text;

    /** Controls the tooltip.​ */
    const Observer<juce::String> tooltip;

private:
    void buttonClicked(juce::Button*) override;
    void buttonStateChanged(juce::Button*) override;
};

/**
    Adds reactive extensions to a juce::ImageComponent.
 */
class ImageComponentExtension : public ComponentExtension
{
    const PublishSubject<juce::Image> _image;
    const PublishSubject<juce::RectanglePlacement> _imagePlacement;

public:
    /** Creates a new instance for a given ImageComponent. */
    ImageComponentExtension(juce::ImageComponent& parent);

    /** Controls the displayed image.​ */
    const Observer<juce::Image> image;

    /** Controls the placement of the image.​ */
    const Observer<juce::RectanglePlacement> imagePlacement;
};

/**
    Adds reactive extensions to a juce::Label.
 */
class LabelExtension : public ComponentExtension, private juce::Label::Listener
{
    const BehaviorSubject<bool> _discardChangesWhenHidingEditor;
    const PublishSubject<juce::Font> _font;
    const PublishSubject<juce::Justification> _justificationType;
    const PublishSubject<juce::BorderSize<int>> _borderSize;
    const PublishSubject<juce::WeakReference<juce::Component>> _attachedComponent;
    const PublishSubject<bool> _attachedOnLeft;
    const PublishSubject<float> _minimumHorizontalScale;
    const PublishSubject<juce::TextInputTarget::VirtualKeyboardType> _keyboardType;
    const PublishSubject<bool> _editableOnSingleClick;
    const PublishSubject<bool> _editableOnDoubleClick;
    const PublishSubject<bool> _lossOfFocusDiscardsChanges;
    const BehaviorSubject<juce::WeakReference<juce::Component>> _textEditor;

public:
    /** Creates a new instance for a given Label. */
    LabelExtension(juce::Label& parent);

    /** Controls the Label's text. Setting a new string notifies all Label::Listeners.​ */
    const BehaviorSubject<juce::String> text;

    /** Controls whether the Label is showing a TextEditor.​ **Type: bool** */
    const BehaviorSubject<bool> showEditor;

    /** Controls whether changes are discarded when hiding the TextEditor. The default is false.​ */
    const Observer<bool> discardChangesWhenHidingEditor;

    /** Controls the Label's font. */
    const Observer<juce::Font> font;

    /** Controls the Label's justification.​ **Type: Justification** */
    const Observer<juce::Justification> justificationType;

    /** Controls the Label's border size.​ **Type: BorderSize<int>** */
    const Observer<juce::BorderSize<int>> borderSize;

    /** Attaches the Label to another Component.​ */
    const Observer<juce::WeakReference<juce::Component>> attachedComponent;

    /** Controls whether the attachedComponent should be attached on the left.​ */
    const Observer<bool> attachedOnLeft;

    /** Controls  the minimum amount that the Label font can be squashed horizontally before it starts using ellipsis.​ **Type: float** */
    const Observer<float> minimumHorizontalScale;

    /** Controls the keyboard type to use in the TextEditor. If the editor is currently open, the type is changed for the open editor.​ */
    const Observer<juce::TextInputTarget::VirtualKeyboardType> keyboardType;

    /** Controls whether clicking the Label opens a TextEditor.​ */
    const Observer<bool> editableOnSingleClick;

    /** Controls whether double-clicking the Label opens a TextEditor.​ */
    const Observer<bool> editableOnDoubleClick;

    /** Controls whether unfocussing the TextEditor discards changes.​ */
    const Observer<bool> lossOfFocusDiscardsChanges;

    /** The currently visible TextEditor.​ */
    const Observable<juce::WeakReference<juce::Component>> textEditor;

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
    PublishSubject<double> _minimum;
    PublishSubject<double> _maximum;
    PublishSubject<double> _doubleClickReturnValue;
    PublishSubject<double> _interval;
    PublishSubject<double> _skewFactorMidPoint;
    BehaviorSubject<bool> _dragging;
    BehaviorSubject<bool> _discardChangesWhenHidingTextBox;
    PublishSubject<bool> _showTextBox;
    PublishSubject<bool> _textBoxIsEditable;

public:
    /** Creates a new instance for a given Slider. */
    SliderExtension(juce::Slider& parent, const Observer<std::function<double(const juce::String&)>>& getValueFromText, const Observer<std::function<juce::String(double)>>& getTextFromValue);

    /** Controls the Slider value.​ */
    const BehaviorSubject<double> value;

    /** Controls the minimum Slider value.​ */
    const Observer<double> minimum;

    /** Controls the maximum Slider value.​ */
    const Observer<double> maximum;

    /** Control the lowest value in a Slider with multiple thumbs. **Do not push items if the Slider has just one thumb.**​ */
    const BehaviorSubject<double> minValue;

    /** Control the highest value in a Slider with multiple thumbs.​ **Do not push items if the Slider has just one thumb.**​ */
    const BehaviorSubject<double> maxValue;

#warning This should be Maybe<double>
    /** Controls the default value of the slider.​ Push std::numeric_limits<double>::max() to prevent double-clicking from resetting the slider. */
    const Observer<double> doubleClickReturnValue;

    /** Controls the step size for values.​ */
    const Observer<double> interval;

    /** Sets the mid point for the Slider's skew factor.​ */
    const Observer<double> skewFactorMidPoint;

    /** Whether the Slider is currently being dragged.​ */
    const Observable<bool> dragging;

#warning This should be Maybe<int>
    /** The thumb that is currently being dragged.​ 0 is the main thumb, 1 is the minValue thumb, 2 is the maxValue thumb. */
    const Observable<int> thumbBeingDragged;

    /** Controls whether the text-box is visible.​ */
    const Observer<bool> showTextBox;

    /** Controls whether the text-box is editable.​ */
    const Observer<bool> textBoxIsEditable;

    /** Controls whether changes are discarded when hiding the text-box. The default is false.​ */
    const Observer<bool> discardChangesWhenHidingTextBox;

    /** Controls how a String that has been entered into the text-box is converted to a Slider value.​ If you don't use this, the slider will use its getValueFromText member function. */
    const Observer<std::function<double(const juce::String&)>> getValueFromText;

    /** Controls how a Slider value is displayed as a String.​ If you don't use this, the slider will use its getTextFromValue member function. */
    const Observer<std::function<juce::String(double)>> getTextFromValue;

private:
    void sliderValueChanged(juce::Slider*) override;
    void sliderDragStarted(juce::Slider*) override;
    void sliderDragEnded(juce::Slider*) override;

    static bool hasMultipleThumbs(const juce::Slider& parent);
};
