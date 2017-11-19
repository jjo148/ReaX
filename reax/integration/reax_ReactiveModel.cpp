Reactive<Value>::Reactive() {}

Reactive<Value>::Reactive(const Value& other)
: value(other) {}

Reactive<Value>::Reactive(const var& initialValue)
: value(initialValue) {}

Reactive<Value>::operator juce::Value() const
{
    return value;
}

var Reactive<Value>::getValue() const
{
    return value.getValue();
}

Reactive<Value>::operator var() const
{
    return value.operator var();
}

void Reactive<Value>::setValue(const var& newValue)
{
    value.setValue(newValue);
}

Reactive<Value>& Reactive<Value>::operator=(const var& newValue)
{
    value = newValue; // Causes Observable to emit
    return *this;
}
