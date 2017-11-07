#include "../Other/TestPrefix.h"

TEST_CASE("Reactive<Component>",
          "[Reactive<Component>][ComponentExtension]")
{
    Array<bool> values;
    Reactive<Component> component;
    varxCollectValues(component.rx.visible, values);

    IT("initially has the same value as the getter")
    {
        REQUIRE(component.isVisible() == component.rx.visible.getValue());
    }

    IT("emits when visibility is changed through setter")
    {
        for (bool visible : { false, false, true, true, false })
            component.setVisible(visible);

        varxRequireValues(values, false, true, false);
    }

    IT("changes visiblility when pushing values")
    {
        for (bool visible : { false, false, true, true, false }) {
            component.rx.visible.onNext(visible);

            REQUIRE(component.isVisible() == visible);
        }
    }
}


TEST_CASE("Reactive<ImageComponent>",
          "[Reactive<ImageComponent>][ImageComponentExtension]")
{
    Reactive<ImageComponent> imageComponent;
    const Image image1(Image::RGB, 17, 47, false);
    const Image image2(Image::RGB, 32, 12, false);
    const RectanglePlacement placement(RectanglePlacement::onlyReduceInSize | RectanglePlacement::yBottom);

    IT("allows setting a new image and placement")
    {
        imageComponent.rx.image.onNext(image1);

        CHECK(imageComponent.getImage().getWidth() == 17);
        CHECK(imageComponent.getImage().getHeight() == 47);

        imageComponent.rx.imagePlacement.onNext(placement);

        CHECK(imageComponent.getImagePlacement() == placement);

        imageComponent.rx.image.onNext(image2);

        REQUIRE(imageComponent.getImage().getWidth() == 32);
        REQUIRE(imageComponent.getImage().getHeight() == 12);

        REQUIRE(imageComponent.getImagePlacement() == placement);
    }
}


TEST_CASE("Reactive<Button>",
          "[Reactive<Button>][ButtonExtension]")
{
    Reactive<TextButton> button("Click Here");

    CONTEXT("clicked")
    {
        Array<Empty> values;
        varxCollectValues(button.rx.clicked, values);

        IT("doesn't emit a value on subscribe")
        {
            REQUIRE(values.isEmpty());
        }

        IT("emits void vars asynchronously when the Button is clicked")
        {
            button.triggerClick();
            varxRunDispatchLoopUntil(!values.isEmpty());

            varxCheckValues(values, Empty());

            button.triggerClick();
            button.triggerClick();
            varxRunDispatchLoopUntil(values.size() == 3);

            varxRequireValues(values, Empty(), Empty(), Empty());
        }
    }

    CONTEXT("buttonState")
    {
        Array<Button::ButtonState> values;
        varxCollectValues(button.rx.buttonState, values);

        IT("emits the normal state on subscribe")
        {
            varxRequireValues(values, Button::ButtonState::buttonNormal);
        }

        IT("emits values synchronously when the Button state changes")
        {
            button.setState(Button::ButtonState::buttonDown);

            varxCheckValues(values,
                           Button::ButtonState::buttonNormal,
                           Button::ButtonState::buttonDown);

            button.setState(Button::ButtonState::buttonNormal);
            button.setState(Button::ButtonState::buttonOver);

            varxRequireValues(values,
                             Button::ButtonState::buttonNormal,
                             Button::ButtonState::buttonDown,
                             Button::ButtonState::buttonNormal,
                             Button::ButtonState::buttonOver);
        }
    }

    CONTEXT("toggleState")
    {
        Array<bool> values;
        varxCollectValues(button.rx.toggleState, values);

        IT("emits false on subscribe")
        {
            varxRequireValues(values, false);
        }

        IT("emits values when calling the JUCE setter")
        {
            button.setToggleState(true, sendNotificationSync);
            button.setToggleState(false, sendNotificationSync);
            button.setToggleState(false, sendNotificationSync);
            button.setToggleState(true, sendNotificationSync);

            varxRequireValues(values, false, true, false, true);
        }

        IT("sets the value when pushing values")
        {
            for (auto toggled : { false, true, true, false, true }) {
                button.rx.toggleState.onNext(toggled);
                REQUIRE(button.getToggleState() == toggled);
            }
        }

        IT("emits values when clicking, if clicking toggles state")
        {
            button.setClickingTogglesState(true);

            button.triggerClick();
            varxRunDispatchLoopUntil(button.rx.toggleState.getValue() == true);

            button.triggerClick();
            button.triggerClick();
            varxRunDispatchLoop(20);
            CHECK(button.rx.toggleState.getValue() == true);

            button.triggerClick();
            varxRunDispatchLoopUntil(button.rx.toggleState.getValue() == false);

            varxRequireValues(values, false, true, false, true, false);
        }
    }

    CONTEXT("text")
    {
        IT("sets the button text when pushing a String")
        {
            button.rx.text.onNext("Hello!");
            CHECK(button.getButtonText() == "Hello!");
            button.rx.text.onNext("How Are You?");
            REQUIRE(button.getButtonText() == "How Are You?");
        }
    }

    CONTEXT("tooltip")
    {
        IT("sets the tooltip when pushing a String")
        {
            button.rx.tooltip.onNext("Hello!");
            CHECK(button.getTooltip() == "Hello!");
            button.rx.tooltip.onNext("Click me!");
            REQUIRE(button.getTooltip() == "Click me!");
        }
    }

    CONTEXT("colour()")
    {
        IT("sets the colours for different colour ids")
        {
            for (auto colourId : { TextButton::buttonColourId, TextButton::buttonOnColourId, TextButton::textColourOffId }) {
                auto observer = button.rx.colour(colourId);

                for (auto colour : { Colours::red, Colour::fromFloatRGBA(0.3f, 0.47f, 0.11f, 0.575f), Colours::white }) {
                    observer.onNext(colour);
                    REQUIRE(button.isColourSpecified(colourId));
                    REQUIRE(button.findColour(colourId) == colour);
                }
            }
        }
    }
}


TEST_CASE("Reactive<Button> with custom TextButton subclass",
          "[Reactive<Button>][ButtonExtension]")
{
    class MyButton : public TextButton
    {
    public:
        void hoverAcrossButton()
        {
            setState(buttonOver);
            MessageManager::getInstance()->callAsync([this]() {
                setState(buttonNormal);
            });
        }
    };

    Reactive<MyButton> button;
    Array<Button::ButtonState> values;
    varxCollectValues(button.rx.buttonState, values);

    IT("initially has the normal state")
    {
        varxRequireValues(values, Button::ButtonState::buttonNormal);
    }

    IT("changes states when calling the method in the custom subclass")
    {
        button.hoverAcrossButton();
        varxCheckValues(values,
                       Button::ButtonState::buttonNormal,
                       Button::ButtonState::buttonOver);
        varxRunDispatchLoopUntil(values.size() == 3);

        varxRequireValues(values,
                         Button::ButtonState::buttonNormal,
                         Button::ButtonState::buttonOver,
                         Button::ButtonState::buttonNormal);
    }
}


TEST_CASE("Reactive<Label>",
          "[Reactive<Label>][LabelExtension]")
{
    Reactive<Label> label;

    CONTEXT("text")
    {
        Array<String> values;
        varxCollectValues(label.rx.text, values);

        IT("initially emits the empty String")
        {
            CHECK(label.getText().isEmpty());

            varxRequireValues(values, label.getText());
        }

        IT("emits values when the Label changes its text")
        {
            label.setText("Foo", sendNotificationSync);
            label.setText("Bar", sendNotificationSync);

            varxRequireValues(values, "", "Foo", "Bar");
        }

        IT("changes the Label text synchronously when calling onNext")
        {
            for (auto text : { "Hello", "World!" }) {
                label.rx.text.onNext(text);
                REQUIRE(label.getText() == text);
            }

            varxRequireValues(values, "", "Hello", "World!");
        }
    }

    CONTEXT("showEditor, discardChangesWhenHidingEditor and textEditor")
    {
        Array<bool> values;
        DisposeBag disposeBag;
        varxCollectValues(label.rx.showEditor, values);

        Array<Component *> editors;
        label.rx.textEditor.subscribe([&](WeakReference<Component> editor) { editors.add(editor); }).disposedBy(disposeBag);

        // The label must be on the screen to show an editor (asserts otherwise)
        TestWindow::getInstance().addAndMakeVisible(label);

        IT("is is initially false/nullptr")
        {
            CHECK(label.getCurrentTextEditor() == nullptr);
            CHECK(editors == Array<Component *>({ nullptr }));

            varxRequireValues(values, false);
        }

        IT("remains false if the discard setting is changed")
        {
            label.rx.discardChangesWhenHidingEditor.onNext(true);
            CHECK(label.getCurrentTextEditor() == nullptr);
            varxCheckValues(values, false);
            CHECK(editors == Array<Component *>({ nullptr }));

            label.rx.discardChangesWhenHidingEditor.onNext(false);
            REQUIRE(label.getCurrentTextEditor() == nullptr);
            CHECK(editors == Array<Component *>({ nullptr }));

            varxRequireValues(values, false);
        }

        IT("becomes true/non-null when calling Label::showEditor")
        {
            label.showEditor();
            CHECK(label.getCurrentTextEditor() != nullptr);
            CHECK(editors.size() == 2);
            CHECK(editors.getLast() != nullptr);

            varxRequireValues(values, false, true);

            IT("becomes false/nullptr when calling Label::hideEditor")
            {
                label.hideEditor(true);
                CHECK(label.getCurrentTextEditor() == nullptr);
                CHECK(editors.size() == 3);
                CHECK(editors.getLast() == nullptr);

                varxRequireValues(values, false, true, false);
            }
        }

        IT("shows the editor when pushing true")
        {
            label.rx.showEditor.onNext(true);
            varxCheckValues(values, false, true);
            CHECK(editors.size() == 2);
            CHECK(editors.getLast() != nullptr);

            REQUIRE(label.getCurrentTextEditor() != nullptr);

            IT("keeps showing the editor even if the discard setting changes")
            {
                label.rx.discardChangesWhenHidingEditor.onNext(false);
                label.rx.discardChangesWhenHidingEditor.onNext(true);
                varxCheckValues(values, false, true);
                CHECK(editors.size() == 2);
                CHECK(editors.getLast() != nullptr);

                REQUIRE(label.getCurrentTextEditor() != nullptr);
            }

            IT("hides the editor when pushing false")
            {
                label.rx.showEditor.onNext(false);
                varxCheckValues(values, false, true, false);
                CHECK(editors.size() == 3);
                CHECK(editors.getLast() == nullptr);

                REQUIRE(label.getCurrentTextEditor() == nullptr);
            }
        }
    }

    CONTEXT("font")
    {
        const Font font1(18.43f, Font::bold | Font::underlined);
        const Font font2(4.3f, Font::italic);

        IT("changes the Label font when pushing values")
        {
            label.rx.font.onNext(font1);
            CHECK(label.getFont() == font1);
            label.rx.font.onNext(font2);

            REQUIRE(label.getFont() == font2);
        }
    }

    CONTEXT("justificationType")
    {
        const Justification justification1(Justification::horizontallyJustified | Justification::top);
        const Justification justification2(Justification::bottom | Justification::left);

        IT("changes the justification type when pushing values")
        {
            label.rx.justificationType.onNext(justification1);
            CHECK(label.getJustificationType() == justification1);
            label.rx.justificationType.onNext(justification2);

            REQUIRE(label.getJustificationType() == justification2);
        }
    }

    CONTEXT("borderSize")
    {
        const BorderSize<int> borderSize1(1, 5, 8, 2);
        const BorderSize<int> borderSize2(33, 108, 47, 0);

        IT("changes the border size when pushing values")
        {
            label.rx.borderSize.onNext(borderSize1);
            CHECK(label.getBorderSize() == borderSize1);
            label.rx.borderSize.onNext(borderSize2);

            REQUIRE(label.getBorderSize() == borderSize2);
        }
    }

    CONTEXT("attachedComponent")
    {
        label.attachToComponent(nullptr, false);
        CHECK(label.getAttachedComponent() == nullptr);
        CHECK(label.isAttachedOnLeft() == false);

        IT("attaches to another Component")
        {
            Component other;
            label.rx.attachedComponent.onNext(WeakReference<Component>(&other));

            CHECK(label.isAttachedOnLeft() == false);
            REQUIRE(label.getAttachedComponent() == &other);

            IT("can change the attachment side and keeps the Component")
            {
                label.rx.attachedOnLeft.onNext(true);
                label.rx.attachedOnLeft.onNext(false);
                label.rx.attachedOnLeft.onNext(true);

                CHECK(label.isAttachedOnLeft() == true);
                REQUIRE(label.getAttachedComponent() == &other);
            }

            IT("can remove the attachment again via nullptr")
            {
                label.rx.attachedComponent.onNext(nullptr);

                REQUIRE(label.getAttachedComponent() == nullptr);
            }

            IT("can remove the attachment again via an empty weak reference")
            {
                label.rx.attachedComponent.onNext(WeakReference<Component>());

                REQUIRE(label.getAttachedComponent() == nullptr);
            }
        }

        IT("loses the attachment when the other component is destroyed")
        {
            auto other = std::make_shared<Component>();
            label.rx.attachedComponent.onNext(other.get());
            CHECK(label.getAttachedComponent() == other.get());

            other.reset();

            REQUIRE(label.getAttachedComponent() == nullptr);
        }
    }

    CONTEXT("minimumHorizontalScale")
    {
        IT("changes the scale when pushing values")
        {
            CHECK(label.getMinimumHorizontalScale() == 0);

            for (auto scale : { 0.f, 15.f, 0.33f, 0.f, 4.24f }) {
                label.rx.minimumHorizontalScale.onNext(scale);
                REQUIRE(label.getMinimumHorizontalScale() == scale);
            }
        }
    }

    CONTEXT("keyboardType")
    {
        // The label must be on the screen to show an editor (asserts otherwise)
        TestWindow::getInstance().addAndMakeVisible(label);

        IT("changes the keyboard type when pushing values")
        {
            label.rx.keyboardType.onNext(TextInputTarget::emailAddressKeyboard);
            label.showEditor();
            CHECK(label.getCurrentTextEditor() != nullptr);

            REQUIRE(label.getCurrentTextEditor()->getKeyboardType() == TextInputTarget::emailAddressKeyboard);

            IT("can change the type while an editor is showing")
            {
                label.rx.keyboardType.onNext(TextInputTarget::decimalKeyboard);

                REQUIRE(label.getCurrentTextEditor()->getKeyboardType() == TextInputTarget::decimalKeyboard);
            }

            IT("remembers the type when hiding and showing again")
            {
                label.hideEditor(true);
                CHECK(label.getCurrentTextEditor() == nullptr);
                label.showEditor();

                REQUIRE(label.getCurrentTextEditor()->getKeyboardType() == TextInputTarget::emailAddressKeyboard);
            }
        }
    }

    CONTEXT("editable")
    {
        CHECK(!label.isEditableOnSingleClick());
        CHECK(!label.isEditableOnDoubleClick());
        CHECK(!label.doesLossOfFocusDiscardChanges());

        IT("changes only single click when pushing to single click")
        {
            label.rx.editableOnSingleClick.onNext(true);
            REQUIRE(label.isEditableOnSingleClick());

            CHECK(!label.isEditableOnDoubleClick());
            CHECK(!label.doesLossOfFocusDiscardChanges());

            label.rx.editableOnSingleClick.onNext(false);
            REQUIRE(!label.isEditableOnSingleClick());
        }

        IT("changes only double click when pushing to double click")
        {
            label.rx.editableOnDoubleClick.onNext(true);
            REQUIRE(label.isEditableOnDoubleClick());

            CHECK(!label.isEditableOnSingleClick());
            CHECK(!label.doesLossOfFocusDiscardChanges());

            label.rx.editableOnDoubleClick.onNext(false);
            REQUIRE(!label.isEditableOnDoubleClick());
        }

        IT("changes only the discard setting when pushing to it")
        {
            label.rx.lossOfFocusDiscardsChanges.onNext(true);
            REQUIRE(label.doesLossOfFocusDiscardChanges());

            CHECK(!label.isEditableOnSingleClick());
            CHECK(!label.isEditableOnDoubleClick());

            label.rx.lossOfFocusDiscardsChanges.onNext(false);
            REQUIRE(!label.doesLossOfFocusDiscardChanges());
        }
    }
}


TEST_CASE("Reactive<Slider>",
          "[Reactive<Slider>][SliderExtension]")
{
    Reactive<Slider> slider;
    slider.setValue(10, sendNotificationSync);

    CONTEXT("value")
    {
        Array<double> values;
        varxCollectValues(slider.rx.value, values);

        IT("initially has the Slider value")
        {
            varxRequireValues(values, 10);
        }

        IT("emits values when the Slider value changes")
        {
            slider.setValue(3, sendNotificationSync);
            slider.setValue(7.45, sendNotificationSync);

            varxRequireValues(values, 10.0, 3.0, 7.45);
        }
    }

    CONTEXT("dragging")
    {
        Array<bool> values;
        varxCollectValues(slider.rx.dragging, values);

        IT("is initially false")
        {
            varxRequireValues(values, false);
        }
    }

    CONTEXT("minimum, maximum")
    {
        IT("sets the minimum")
        {
            slider.rx.minimum.onNext(11);

            REQUIRE(slider.getMinimum() == 11);
            CHECK(slider.getValue() == 11);
        }

        IT("sets the maximum")
        {
            slider.rx.maximum.onNext(5.43);

            REQUIRE(slider.getMaximum() == 5.43);
            CHECK(slider.getValue() == 5.43);
        }

        IT("does not overwrite the interval")
        {
            slider.rx.interval.onNext(1.445);
            slider.rx.maximum.onNext(13.23);
            slider.rx.minimum.onNext(1.2);

            REQUIRE(slider.getInterval() == 1.445);
        }
    }

    IT("sets the skew factor")
    {
        CHECK(slider.getSkewFactor() == 1);
        slider.rx.skewFactorMidPoint.onNext(7.5);
        REQUIRE(slider.getSkewFactor() == Approx(2.4094208397));
    }

    IT("sets the interval")
    {
        CHECK(slider.getInterval() == 0.0);
        slider.rx.interval.onNext(2.565);
        REQUIRE(slider.getInterval() == 2.565);
    }

    CONTEXT("minValue / maxValue")
    {
        slider.setValue(5, sendNotificationSync);
        slider.setSliderStyle(Slider::ThreeValueHorizontal);
        slider.setMinValue(1, sendNotificationSync);
        slider.setMaxValue(8.45, sendNotificationSync);

        Array<double> minValues;
        varxCollectValues(slider.rx.minValue, minValues);
        Array<double> maxValues;
        varxCollectValues(slider.rx.maxValue, maxValues);

        IT("initially has the values set on the slider")
        {
            REQUIRE(slider.rx.minValue.getValue() == 1);
            REQUIRE(slider.rx.maxValue.getValue() == 8.45);
        }

        IT("emits values when calling the JUCE setters")
        {
            slider.setMinAndMaxValues(0.3, 6.77, sendNotificationSync);
            slider.setMinValue(1.344, sendNotificationSync);
            slider.setMaxValue(8, sendNotificationSync);
            slider.setValue(6, sendNotificationSync);

            varxRequireValues(minValues, 1.0, 0.3, 1.344);
            varxRequireValues(maxValues, 8.45, 6.77, 8.0);
        }

        IT("calls setMinValue when pushing min values")
        {
            slider.setValue(10);
            for (auto value : { 5.6, 4.25, 7.4 }) {
                slider.rx.minValue.onNext(value);

                REQUIRE(slider.getMinValue() == value);
            }
        }

        IT("calls setMaxValue when pushing max values")
        {
            slider.setValue(0);
            for (auto value : { 5.6, 4.25, 7.4 }) {
                slider.rx.maxValue.onNext(value);

                REQUIRE(slider.getMaxValue() == value);
            }
        }
    }

    CONTEXT("double click return value")
    {
        IT("sets the value to a double (if provided)")
        {
            slider.setDoubleClickReturnValue(false, 0);
            CHECK_FALSE(slider.isDoubleClickReturnEnabled());

            slider.rx.doubleClickReturnValue.onNext(1.323);

            CHECK(slider.isDoubleClickReturnEnabled());
            REQUIRE(slider.getDoubleClickReturnValue() == 1.323);
        }

        IT("disables the double click return value when pushing numeric_limits<double>::max()")
        {
            slider.setDoubleClickReturnValue(true, 4.2);
            CHECK(slider.isDoubleClickReturnEnabled());

            slider.rx.doubleClickReturnValue.onNext(std::numeric_limits<double>::max());

            REQUIRE_FALSE(slider.isDoubleClickReturnEnabled());
        }
    }

    CONTEXT("getValueFromText")
    {
        IT("initially parses a string as a number")
        {
            REQUIRE(slider.getValueFromText("10.33") == 10.33);
        }

        IT("parses a value using the pushed function")
        {
            std::function<double(String)> function = [](String s) {
                if (s == "4.464")
                    return 4.464;
                else if (s == "3")
                    return 3.0;
                else
                    return 0.1;
            };

            slider.rx.getValueFromText.onNext(function);
        }
    }

    CONTEXT("getTextFromValue")
    {
        IT("initially stringifies its value as just a number")
        {
            REQUIRE(slider.getTextFromValue(slider.getValue()) == "10");
        }

        IT("stringifies its value using the pushed function")
        {
            std::function<String(double)> function = [](double value) {
                return (value > 5 ? "BIG!" : "small");
            };
            slider.rx.getTextFromValue.onNext(function);

            REQUIRE(slider.getTextFromValue(2) == "small");
            REQUIRE(slider.getTextFromValue(8.4) == "BIG!");
        }
    }
}


template<typename T1, typename T2>
using isSame = typename std::is_same<typename std::decay<T1>::type, T2>;

TEST_CASE("Template ambiguities")
{
    IT("chooses the correct template for a juce::Component")
    {
        Reactive<Component> myComponent;
        static_assert(isSame<decltype(myComponent.rx), ComponentExtension>::value, "rx Member has wrong type.");
    }

    IT("chooses the correct template for a Component subclass")
    {
        class MyCustomComponent : public Component
        {};
        Reactive<MyCustomComponent> myCustomComponent;
        static_assert(isSame<decltype(myCustomComponent.rx), ComponentExtension>::value, "rx Member has wrong type.");
    }

    IT("chooses the correct template for a Button subclass")
    {
        class MyButton : public Button
        {
        public:
            MyButton()
            : Button("") {}
            void paintButton(Graphics &, bool, bool) override {}
        };
        Reactive<MyButton> myButton;
        static_assert(isSame<decltype(myButton.rx), ButtonExtension>::value, "rx Member has wrong type.");
    }

    IT("chooses the correct template for a juce::ImageComponent")
    {
        Reactive<ImageComponent> myImageComponent;
        static_assert(isSame<decltype(myImageComponent.rx), ImageComponentExtension>::value, "rx Member has wrong type.");
    }
}
