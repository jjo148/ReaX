#pragma once

/**
 Connects a `juce::Value` with a `BehaviorSubject`.
 
 Whenever the `Value` changes, the `BehaviorSubject` is changed, and vice versa.
 */
class ValueExtension : private juce::Value::Listener
{
public:
    /// Creates a new instance with a given Value. The subject will refers to the **`ValueSource`** of `inputValue`.
    ValueExtension(const juce::Value& inputValue);
    
    /// The subject that's connected to the Value's `ValueSource`. This changes whenever the Value changes, and vice versa.
    const BehaviorSubject<juce::var> subject;
    
private:
    juce::Value value;
    DisposeBag disposeBag;
    
    void valueChanged(juce::Value&) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueExtension);
};

/**
 Adds reactive extensions to an `AudioProcessor`.
 
 If you use this directly (instead of `Reactive<AudioProcessor>`), you **must** make sure that the `AudioProcessor` has a longer lifetime than this `AudioProcessorExtension`!
 */
class AudioProcessorExtension : private juce::AudioProcessorListener
{
    LockFreeSource<Empty> _processorChanged;
public:
    /// Creates a new instance for a given AudioProcessor. 
    AudioProcessorExtension(juce::AudioProcessor& parent);

    /**
     Emits when something (apart from a parameter value) has changed, for example the latency.
     
     Emits asynchronously on the JUCE message thread.
     */
    const Observable<Empty> processorChanged;

private:
    DisposeBag disposeBag;
    
    void audioProcessorParameterChanged(juce::AudioProcessor*, int, float) override {}
    void audioProcessorChanged(juce::AudioProcessor*) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorExtension);
};

/**
 Adds reactive extensions to an `AudioProcessorValueTreeState`.
 
 If you use this directly (instead of `Reactive<AudioProcessorValueTreeState>`), you **must** make sure that the `AudioProcessorValueTreeState` has a longer lifetime than this `AudioProcessorValueTreeStateExtension`!
 */
class AudioProcessorValueTreeStateExtension {
public:
    /// Creates a new instance for a given AudioProcessorValueTreeState. 
    AudioProcessorValueTreeStateExtension(juce::AudioProcessorValueTreeState& parent);
    
    ~AudioProcessorValueTreeStateExtension();
    
    /**
     Returns a subject to control the value of the parameter with the given ID.
     
     If this is called early in the app lifecycle, the subject contains `var()` for a short amount of time, and not the parameter's default value. This is because JUCE updates the ValueTree asynchronously.
     
     Parameter values can be changed from the audio thread; in this case the subject's Observable side emits asynchronously.
     */
    BehaviorSubject<juce::var> parameterValue(const juce::String& parameterID) const;
    
private:
    struct Impl;
    const juce::ScopedPointer<Impl> impl;
    juce::AudioProcessorValueTreeState& parent;
    DisposeBag disposeBag;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorValueTreeStateExtension);
};
