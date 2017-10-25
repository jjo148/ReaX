#pragma once

/**
 Adds reactive extensions to a juce::Value.
 
 Instead of creating a juce::Value, create an instance of this as follows:
 
 Reactive<juce::Value> myValue;
 
 It inherits from juce::Value, so you can use it as a drop-in replacement. And you can access `myValue.rx.subject` to subscribe to changes, etc.:
 
 myValue.rx.subject.map(...).filter(...).subscribe(...);
 
 And you can subscribe this value to some Observable, to change the value whenever the Observable emits an item:
 
 someObservable.subscribe(myValue.rx.subject);
 */
template<>
class Reactive<juce::Value> : public juce::Value
{
public:
    /** Creates a new instance. Has the same behavior as the juce::Value equivalent. */
    ///@{
    Reactive();
    Reactive(const juce::Value& other);
    explicit Reactive(const juce::var& initialValue);
    ///@}

    /** Sets a new value. This is the same as calling Reactive<Value>::setValue. */
    Reactive& operator=(const juce::var& newValue);

    /** The reactive extension object. */
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
    /** Creates a new instance. @see juce::AudioProcessor::AudioProcessor. */
    template<typename... Args>
    Reactive(Args&&... args)
    : juce::AudioProcessor(std::forward<Args>(args)...),
      rx(*this)
    {}

    /** The reactive extension object. */
    const AudioProcessorExtension rx;
};


/**
 Adds reactive extensions to a juce::AudioProcessorValueTreeState.
 */
template<>
class Reactive<juce::AudioProcessorValueTreeState> : public juce::AudioProcessorValueTreeState
{
public:
    /** Creates a new instance. @see juce::AudioProcessorValueTreeState::AudioProcessorValueTreeState. */
    Reactive(juce::AudioProcessor& processorToConnectTo, juce::UndoManager* undoManagerToUse);

    /** The reactive extension object. */
    const AudioProcessorValueTreeStateExtension rx;
};
