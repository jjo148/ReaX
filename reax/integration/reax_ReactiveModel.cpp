Reactive<Value>::Reactive()
: rx(Value()) {}

Reactive<Value>::Reactive(const Value& other)
: rx(other) {}

Reactive<Value>::Reactive(const var& initialValue)
: rx(Value(initialValue)) {}

Reactive<Value>::operator juce::Value() const
{
    return rx.getValue();
}

var Reactive<Value>::getValue() const
{
    return rx.getValue().getValue();
}

Reactive<Value>::operator var() const
{
    return rx.getValue().operator var();
}

void Reactive<Value>::setValue(const var& newValue)
{
    rx.getValue().setValue(newValue);
}

Reactive<Value>& Reactive<Value>::operator=(const var& newValue)
{
    rx.getValue() = newValue; // Causes Observable to emit
    return *this;
}

void Reactive<Value>::referTo(const Value& valueToReferTo)
{
    rx.getValue().referTo(valueToReferTo);
}

Reactive<Value>& Reactive<Value>::operator=(const Value& newValue)
{
    referTo(newValue);
    return *this;
}

Reactive<juce::AudioProcessorValueTreeState>::Reactive(AudioProcessor& processorToConnectTo, UndoManager* undoManagerToUse)
: AudioProcessorValueTreeState(processorToConnectTo, undoManagerToUse),
  rx(*this)
{}
