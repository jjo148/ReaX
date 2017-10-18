ValueExtension::ValueExtension(const Value& inputValue)
: subject(inputValue.getValue()),
  value(inputValue)
{
    value.addListener(this);
    subject.takeUntil(deallocated).subscribe([this](const var& newValue) {
        // Only assign a new value if it has actually changed (to avoid problems with AudioProcessorValueTreeState)
        if (newValue != value)
            value = newValue;
    });
}

void ValueExtension::valueChanged(Value&)
{
    // Only emit a new value if it has actually changed
    if (value.getValue() != subject.getLatestItem())
        subject.onNext(value.getValue());
}

AudioProcessorExtension::AudioProcessorExtension(AudioProcessor& parent)
: processorChanged(_processorChanged)
{
    parent.addListener(this);
}

void AudioProcessorExtension::audioProcessorChanged(AudioProcessor* processor)
{
    _processorChanged.onNext(Empty());
}

struct AudioProcessorValueTreeStateExtension::Impl
{
    std::map<String, Reactive<Value>> parameterValues;
};

AudioProcessorValueTreeStateExtension::AudioProcessorValueTreeStateExtension(AudioProcessorValueTreeState& parent)
: impl(new Impl()),
  parent(parent)
{}

AudioProcessorValueTreeStateExtension::~AudioProcessorValueTreeStateExtension() {}

TypedBehaviorSubject<var> AudioProcessorValueTreeStateExtension::parameterValue(const String& parameterID) const
{
    // Create a Reactive<Value> for the parameter, if not already done
    if (impl->parameterValues.find(parameterID) == impl->parameterValues.end())
        impl->parameterValues.emplace(parameterID, parent.getParameterAsValue(parameterID));

    return impl->parameterValues.at(parameterID).rx.subject;
}
