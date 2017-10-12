using std::placeholders::_1;

ValueExtension::ValueExtension(const Value& inputValue)
: subject(inputValue.getValue()),
  value(inputValue)
{
    value.addListener(this);
    subject.takeUntil(deallocated).subscribe(std::bind(&Value::setValue, value, _1));
}

void ValueExtension::valueChanged(Value&)
{
    if (value.getValue() != subject.getLatestItem()) {
        subject.onNext(value.getValue());
    }
}

AudioProcessorExtension::AudioProcessorExtension(juce::AudioProcessor& parent)
: processorChanged(_processorChanged)
{
    parent.addListener(this);
}

void AudioProcessorExtension::audioProcessorChanged(AudioProcessor* processor)
{
    _processorChanged.onNext(var::undefined());
}
