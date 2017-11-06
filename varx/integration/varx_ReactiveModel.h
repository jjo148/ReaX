#pragma once

/**
 Adds reactive extensions to a juce::Value.
 
 Instead of creating a juce::Value, create an instance of this as follows:
 
 Reactive<juce::Value> myValue;
 
 It's implicitly convertible from and to juce::Value. And you can access `myValue.rx.subject` to subscribe to changes, etc.:
 
 myValue.rx.subject.map(...).filter(...).subscribe(...);
 
 And you can subscribe this value to some Observable, to change the value whenever the Observable emits an item:
 
 someObservable.subscribe(myValue.rx.subject);
 */
template<>
class Reactive<juce::Value>
{
    juce::Value value;

public:
    /// Creates a new instance. Has the same behavior as the juce::Value equivalent.
    ///@{
    Reactive();
    Reactive(const juce::Value& other);
    explicit Reactive(const juce::var& initialValue);
    ///@}

    /// Converts the Reactive<juce::Value> to a juce::Value
    operator juce::Value() const;

    /// Returns the current value.
    juce::var getValue() const;

    /// Returns the current value.
    operator juce::var() const;

    /// Sets a new value.
    void setValue(const juce::var& newValue);

    /// Sets a new value. This is the same as calling Reactive<Value>::setValue.
    Reactive& operator=(const juce::var& newValue);

    /// The reactive extension object.
    const ValueExtension rx;

private:
    Reactive& operator=(const Reactive&) = delete;
};

/**
 Adds reactive extensions to a juce::AudioProcessor.
 */
template<>
class Reactive<juce::AudioProcessor> : public juce::AudioProcessor
{
public:
    /// Creates a new instance. @see juce::AudioProcessor::AudioProcessor.
    template<typename... Args>
    Reactive(Args&&... args)
    : juce::AudioProcessor(std::forward<Args>(args)...),
      rx(*this)
    {}

    /// The reactive extension object.
    const AudioProcessorExtension rx;
};


/**
 Adds reactive extensions to a juce::AudioProcessorValueTreeState.
 */
template<>
class Reactive<juce::AudioProcessorValueTreeState> : public juce::AudioProcessorValueTreeState
{
public:
    /// Creates a new instance. @see juce::AudioProcessorValueTreeState::AudioProcessorValueTreeState.
    Reactive(juce::AudioProcessor& processorToConnectTo, juce::UndoManager* undoManagerToUse);

    /// The reactive extension object.
    const AudioProcessorValueTreeStateExtension rx;
};
