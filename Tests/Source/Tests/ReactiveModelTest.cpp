#include "../Other/TestPrefix.h"


TEST_CASE("Reactive<Value> conversion",
          "[Reactive<Value>][ValueExtension]")
{
    Reactive<Value> value;

    IT("supports copy assignment from var-compatible types")
    {
        value = 3;
        value = Array<var>({ 6, 7, 5 });
        value = "Some String";
        REQUIRE(value.getValue() == "Some String");
    }

    IT("can be converted to var")
    {
        value.setValue("Testing");
        var v = value.operator var();
        REQUIRE(v == "Testing");
    }

    IT("supports == and != with var-compatible types")
    {
        value.setValue("Hello!");
        REQUIRE(value == "Hello!");
        REQUIRE(value != "World");
        REQUIRE(value != 3.45);
        REQUIRE(value != 2);
    }
}


TEST_CASE("Reactive<Value> Observable",
          "[Reactive<Value>][ValueExtension]")
{
    auto value = std::make_shared<Reactive<Value>>("Initial");
    Array<var> values;
    Reaction_CollectValues(value->rx.subject, values);

    IT("emits values asynchronously when the Value changes")
    {
        value->setValue("Second");
        Reaction_CheckValues(values, "Initial");
        Reaction_RunDispatchLoopUntil(values.size() == 2);
        Reaction_CheckValues(values, "Initial", "Second");
        value->setValue("Third");
        Reaction_RunDispatchLoopUntil(values.size() == 3)

        Reaction_RequireValues(values, "Initial", "Second", "Third");
    }

    IT("stops emitting values immediately when being destroyed")
    {
        value->setValue("Should not arrive");
        value.reset();
        Reaction_RunDispatchLoop(15);

        Reaction_RequireValues(values, "Initial");
    }
}


class DummyAudioProcessor : public Reactive<AudioProcessor>
{
public:
    // Dummy overrides
    const String getName() const override { return "DummyAudioProcessor"; }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override {}
    double getTailLengthSeconds() const override { return 0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    int getNumPrograms() override { return 2; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return ""; }
    void changeProgramName(int, const String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};


TEST_CASE("Reactive<AudioProcessor>",
          "[Reactive<AudioProcessor>][AudioProcessorExtension]")
{
    DummyAudioProcessor processor;
    Array<Empty> values;
    
    Reaction_CollectValues(processor.rx.processorChanged, values);
    
    IT("does not emit initially")
    {
        REQUIRE(values.isEmpty());
    }
    
    IT("emits asynchronously when setting the latency")
    {
        processor.setLatencySamples(256);
        CHECK(values.isEmpty());
        Reaction_RunDispatchLoopUntil(!values.isEmpty());
        Reaction_RequireValues(values, Empty());
        
        // Set to same value as before, shouldn't emit
        processor.setLatencySamples(256);
        Reaction_RunDispatchLoop(20);
        Reaction_RequireValues(values, Empty());
        
        // Set to different value, should emit
        processor.setLatencySamples(512);
        Reaction_RunDispatchLoopUntil(values.size() == 2);
        Reaction_RequireValues(values, Empty(), Empty());
    }
}


TEST_CASE("Reactive<AudioProcessorValueTreeState>",
          "[Reactive<AudioProcessorValueTreeState>][AudioProcessorValueTreeStateExtension]")
{
    DummyAudioProcessor processor;
    Reactive<AudioProcessorValueTreeState> valueTreeState(processor, nullptr);
    NormalisableRange<float> range(0, 10);
    valueTreeState.createAndAddParameter("foo", "foo", "", range, 2.74f, nullptr, nullptr);
    valueTreeState.createAndAddParameter("bar", "bar", "", range, 8.448f, nullptr, nullptr);
    valueTreeState.state = ValueTree("Test");
    
    Array<var> fooValues;
    Reaction_CollectValues(valueTreeState.rx.parameterValue("foo"), fooValues);
    
    IT("emits an empty var() first")
    {
        Value fooValue = valueTreeState.getParameterAsValue("foo");
        CHECK(fooValue.getValue() == var());
        Reaction_RequireValues(fooValues, var());
        
        IT("emits the default value after a delay")
        {
            // JUCE updates the value tree state asynchronously. Wait for the first update:
            Reaction_RunDispatchLoopUntil(fooValue.getValue() != var());
            
            CHECK(fooValue.getValue() == var(2.74f));
            Reaction_RequireValues(fooValues, var(), var(2.74f));
            
            IT("emits synchronously when setting a new value on the subject")
            {
                valueTreeState.rx.parameterValue("foo").onNext(7.429f);
                Reaction_RequireValues(fooValues, var(), var(2.74f), var(7.429f));
            }
            
            IT("emits after a delay, when setting a new value on the ValueTree")
            {
                valueTreeState.getParameterAsValue("foo").setValue(0.471f);
                Reaction_RunDispatchLoopUntil(fooValues.size() == 4);
                
                // For some reason, JUCE sets the parameter not just to 0.471f, but afterwards to some float value nearby
                Reaction_RequireValues(fooValues, var(), var(2.74f), var(0.471f), var(0.4710000157356262207));
            }
            
            IT("emits after a delay, when setting a new value on the AudioProcessorParameter")
            {
                // setValue expects a value in the range [0..1]
                valueTreeState.getParameter("foo")->setValue(0.98f);
                Reaction_RunDispatchLoopUntil(fooValues.size() == 3);
                
                Reaction_RequireValues(fooValues, var(), var(2.74f), var(9.8f));
            }
            
            IT("does not emit when setting the value of a different parameter")
            {
                valueTreeState.getParameterAsValue("bar").setValue(2.987f);
                Reaction_RunDispatchLoopUntil(fooValues.size() == 2);
                
                CHECK(valueTreeState.rx.parameterValue("bar").getValue() == var(2.987f));
                Reaction_RequireValues(fooValues, var(), var(2.74f));
            }
            
            IT("sets the parameter synchronously when setting a new value on the subject")
            {
                valueTreeState.rx.parameterValue("foo").onNext(3.28f);
                REQUIRE(*valueTreeState.getRawParameterValue("foo") == Approx(3.28));
                REQUIRE(valueTreeState.getParameterAsValue("foo").getValue() == var(3.28f));
                
                // getValue returns a value in the range [0..1]
                REQUIRE(valueTreeState.getParameter("foo")->getValue() == Approx(0.328));
            }
        }
    }
}
