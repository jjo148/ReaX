ValueExtension::ValueExtension(const Value& inputValue)
: subject(inputValue.getValue()),
  value(inputValue)
{
    // value is a member variable, so no need to call removeListener in destructor
    value.addListener(this);
    
    subject.subscribe([this](const var& newValue) {
               // Only assign a new value if it has actually changed (to avoid problems with AudioProcessorValueTreeState)
               if (newValue != value)
                   value = newValue;
           })
        .disposedBy(disposeBag);
}

void ValueExtension::valueChanged(Value&)
{
    // Only emit a new value if it has actually changed
    if (value.getValue() != subject.getLatestItem())
        subject.onNext(value.getValue());
}

AudioProcessorExtension::AudioProcessorExtension(AudioProcessor& parent)
: parent(parent),
  _processorChanged(1),
  processorChanged(_processorChanged)
{
    parent.addListener(this);
}

AudioProcessorExtension::~AudioProcessorExtension()
{
    parent.removeListener(this);
}

void AudioProcessorExtension::audioProcessorChanged(AudioProcessor*)
{
    // If there's already an item in the queue, it will be emitted soon. So there's no need to add another one.
    _processorChanged.onNext(Empty(), CongestionPolicy::DropNewest);
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

BehaviorSubject<var> AudioProcessorValueTreeStateExtension::parameterValue(const String& parameterID) const
{
    // Create a Reactive<Value> for the parameter, if not already done
    if (impl->parameterValues.find(parameterID) == impl->parameterValues.end())
        impl->parameterValues.emplace(parameterID, parent.getParameterAsValue(parameterID));

    return impl->parameterValues.at(parameterID).rx.subject;
}
