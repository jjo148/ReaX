using std::placeholders::_1;

ComponentExtension::ComponentExtension(Component& parent)
: colourSubjects(new std::map<int, PublishSubject<juce::Colour>>()),
  parent(parent),
  visible(parent.isVisible()),
  disposeBag(new DisposeBag())
{
    parent.addComponentListener(this);
    visible.subscribe(std::bind(&Component::setVisible, &parent, _1)).disposedBy(*disposeBag);
}

Observer<Colour> ComponentExtension::colour(int colourId) const
{
    // Create subject, if not already done
    if (colourSubjects->find(colourId) == colourSubjects->end())
        colourSubjects->insert(std::make_pair(colourId, PublishSubject<Colour>()));

    // Subscribe
    colourSubjects->at(colourId).subscribe(std::bind(&Component::setColour, &parent, colourId, _1)).disposedBy(*disposeBag);

    // Return as Observer
    return colourSubjects->at(colourId);
}

void ComponentExtension::componentVisibilityChanged(Component& component)
{
    if (component.isVisible() != visible.getLatestItem())
        visible.onNext(component.isVisible());
}


ButtonExtension::ButtonExtension(Button& parent)
: ComponentExtension(parent),
  clicked(_clicked),
  buttonState(parent.getState()),
  toggleState(parent.getToggleState()),
  text(_text),
  tooltip(_tooltip)
{
    parent.addListener(this);

    _text.subscribe(std::bind(&Button::setButtonText, &parent, _1)).disposedBy(disposeBag);
    _tooltip.subscribe(std::bind(&Button::setTooltip, &parent, _1)).disposedBy(disposeBag);
    buttonState.subscribe(std::bind(&Button::setState, &parent, _1)).disposedBy(disposeBag);
    toggleState.subscribe([&parent](bool toggled) {
        parent.setToggleState(toggled, sendNotificationSync);
    }).disposedBy(disposeBag);
}

void ButtonExtension::buttonClicked(Button*)
{
    _clicked.onNext(Empty());
}

void ButtonExtension::buttonStateChanged(Button* button)
{
    if (button->getState() != buttonState.getLatestItem())
        buttonState.onNext(button->getState());

    if (button->getToggleState() != toggleState.getLatestItem())
        toggleState.onNext(button->getToggleState());
}

ImageComponentExtension::ImageComponentExtension(ImageComponent& parent)
: ComponentExtension(parent),
  image(_image),
  imagePlacement(_imagePlacement)
{
    _image.subscribe([&parent](const Image& image) {
        parent.setImage(image);
    }).disposedBy(disposeBag);

    _imagePlacement.subscribe(std::bind(&ImageComponent::setImagePlacement, &parent, _1)).disposedBy(disposeBag);
}

LabelExtension::LabelExtension(Label& parent)
: ComponentExtension(parent),
  _discardChangesWhenHidingEditor(false),
  _textEditor(parent.getCurrentTextEditor()),
  text(parent.getText()),
  showEditor(parent.getCurrentTextEditor() != nullptr),
  discardChangesWhenHidingEditor(_discardChangesWhenHidingEditor),
  font(_font),
  justificationType(_justificationType),
  borderSize(_borderSize),
  attachedComponent(_attachedComponent),
  attachedOnLeft(_attachedOnLeft),
  minimumHorizontalScale(_minimumHorizontalScale),
  keyboardType(_keyboardType),
  editableOnSingleClick(_editableOnSingleClick),
  editableOnDoubleClick(_editableOnDoubleClick),
  lossOfFocusDiscardsChanges(_lossOfFocusDiscardsChanges),
  textEditor(_textEditor.distinctUntilChanged())
{
    parent.addListener(this);

    text.subscribe(std::bind(&Label::setText, &parent, _1, sendNotificationSync)).disposedBy(disposeBag);

    showEditor.withLatestFrom(_discardChangesWhenHidingEditor).subscribe([&parent](const std::pair<bool, bool>& pair) {
        if (pair.first)
            parent.showEditor();
        else
            parent.hideEditor(pair.second);
    }).disposedBy(disposeBag);

    _font.subscribe(std::bind(&Label::setFont, &parent, _1)).disposedBy(disposeBag);
    _justificationType.subscribe(std::bind(&Label::setJustificationType, &parent, _1)).disposedBy(disposeBag);
    _borderSize.subscribe(std::bind(&Label::setBorderSize, &parent, _1)).disposedBy(disposeBag);

    _attachedComponent.subscribe([&parent](const WeakReference<Component>& component) {
        parent.attachToComponent(component, parent.isAttachedOnLeft());
    }).disposedBy(disposeBag);

    _attachedOnLeft.subscribe([&parent](bool attachedOnLeft) {
        parent.attachToComponent(parent.getAttachedComponent(), attachedOnLeft);
    }).disposedBy(disposeBag);

    _minimumHorizontalScale.subscribe(std::bind(&Label::setMinimumHorizontalScale, &parent, _1)).disposedBy(disposeBag);

    _keyboardType.subscribe([&parent](TextInputTarget::VirtualKeyboardType keyboardType) {
        parent.setKeyboardType(keyboardType);

        if (auto editor = parent.getCurrentTextEditor())
            editor->setKeyboardType(keyboardType);
    }).disposedBy(disposeBag);

    // Cannot use combineLatest for these, because changing something on the Slider directly doesn't update the subject
    _editableOnSingleClick.subscribe([&parent](bool editableOnSingleClick) {
        parent.setEditable(editableOnSingleClick, parent.isEditableOnDoubleClick(), parent.doesLossOfFocusDiscardChanges());
    }).disposedBy(disposeBag);
    _editableOnDoubleClick.subscribe([&parent](bool editableOnDoubleClick) {
        parent.setEditable(parent.isEditableOnSingleClick(), editableOnDoubleClick, parent.doesLossOfFocusDiscardChanges());
    }).disposedBy(disposeBag);
    _lossOfFocusDiscardsChanges.subscribe([&parent](bool lossOfFocusDiscardsChanges) {
        parent.setEditable(parent.isEditableOnSingleClick(), parent.isEditableOnDoubleClick(), lossOfFocusDiscardsChanges);
    }).disposedBy(disposeBag);
}

void LabelExtension::labelTextChanged(Label* parent)
{
    if (parent->getText() != text.getLatestItem())
        text.onNext(parent->getText());
}

void LabelExtension::editorShown(Label* parent, TextEditor&)
{
    if (!showEditor.getLatestItem())
        showEditor.onNext(true);

    _textEditor.onNext(parent->getCurrentTextEditor());
}

void LabelExtension::editorHidden(Label* parent, TextEditor&)
{
    if (showEditor.getLatestItem())
        showEditor.onNext(false);

    _textEditor.onNext(nullptr);
}


SliderExtension::SliderExtension(juce::Slider& parent, const Observer<std::function<double(const juce::String&)>>& getValueFromText, const Observer<std::function<juce::String(double)>>& getTextFromValue)
: ComponentExtension(parent),
  _dragging(false),
  _discardChangesWhenHidingTextBox(false),
  value(parent.getValue()),
  minimum(_minimum),
  maximum(_maximum),
  minValue(hasMultipleThumbs(parent) ? parent.getMinValue() : parent.getValue()),
  maxValue(hasMultipleThumbs(parent) ? parent.getMaxValue() : parent.getValue()),
  doubleClickReturnValue(_doubleClickReturnValue),
  interval(_interval),
  skewFactorMidPoint(_skewFactorMidPoint),
  dragging(_dragging.distinctUntilChanged()),
  thumbBeingDragged(dragging.map([&parent](bool) { return parent.getThumbBeingDragged(); })),
  showTextBox(_showTextBox),
  textBoxIsEditable(_textBoxIsEditable),
  discardChangesWhenHidingTextBox(_discardChangesWhenHidingTextBox),
  getValueFromText(getValueFromText),
  getTextFromValue(getTextFromValue)
{
    parent.addListener(this);

    value.subscribe([&parent](double value) {
        parent.setValue(value, sendNotificationSync);
    }).disposedBy(disposeBag);

    // Cannot use combineLatest for these, because changing something on the Slider directly doesn't update the subject
    _minimum.subscribe([&parent](double minimum) {
        parent.setRange(minimum, parent.getMaximum(), parent.getInterval());
    }).disposedBy(disposeBag);
    _maximum.subscribe([&parent](double maximum) {
        parent.setRange(parent.getMinimum(), maximum, parent.getInterval());
    }).disposedBy(disposeBag);
    _interval.subscribe([&parent](double interval) {
        parent.setRange(parent.getMinimum(), parent.getMaximum(), interval);
    }).disposedBy(disposeBag);

    minValue.skip(1).subscribe([&parent](double minValue) {
        parent.setMinValue(minValue, sendNotificationSync, true);
    }).disposedBy(disposeBag);

    maxValue.skip(1).subscribe([&parent](double maxValue) {
        parent.setMaxValue(maxValue, sendNotificationSync, true);
    }).disposedBy(disposeBag);

    _doubleClickReturnValue.subscribe([&parent](double value) {
        parent.setDoubleClickReturnValue(value != DBL_MAX, value);
    }).disposedBy(disposeBag);

    _skewFactorMidPoint.subscribe(std::bind(&Slider::setSkewFactorFromMidPoint, &parent, _1)).disposedBy(disposeBag);

    _showTextBox.withLatestFrom(_discardChangesWhenHidingTextBox).subscribe([&parent](const std::pair<bool, bool>& pair) {
        if (pair.first)
            parent.showTextBox();
        else
            parent.hideTextBox(pair.second);
    }).disposedBy(disposeBag);

    _textBoxIsEditable.subscribe(std::bind(&Slider::setTextBoxIsEditable, &parent, _1)).disposedBy(disposeBag);
}

void SliderExtension::sliderValueChanged(Slider* slider)
{
    if (slider->getValue() != value.getLatestItem())
        value.onNext(slider->getValue());

    if (hasMultipleThumbs(*slider) && slider->getMinValue() != minValue.getLatestItem())
        minValue.onNext(slider->getMinValue());

    if (hasMultipleThumbs(*slider) && slider->getMaxValue() != maxValue.getLatestItem())
        maxValue.onNext(slider->getMaxValue());
}

void SliderExtension::sliderDragStarted(Slider*)
{
    _dragging.onNext(true);
}

void SliderExtension::sliderDragEnded(Slider*)
{
    _dragging.onNext(false);
}

bool SliderExtension::hasMultipleThumbs(const juce::Slider& parent)
{
    switch (parent.getSliderStyle()) {
        case Slider::TwoValueHorizontal:
        case Slider::TwoValueVertical:
        case Slider::ThreeValueHorizontal:
        case Slider::ThreeValueVertical:
            return true;

        default:
            return false;
    }
}
