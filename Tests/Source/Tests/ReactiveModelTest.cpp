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
    Array<var> items;
    varxCollectItems(value->rx.subject, items);

    IT("emits items asynchronously when the Value changes")
    {
        value->setValue("Second");
        varxCheckItems(items, "Initial");
        varxRunDispatchLoop();
        varxCheckItems(items, "Initial", "Second");
        value->setValue("Third");
        varxRunDispatchLoop();

        varxRequireItems(items, "Initial", "Second", "Third");
    }

    IT("stops emitting items immediately when being destroyed")
    {
        value->setValue("Should not arrive");
        value.reset();
        varxRunDispatchLoop();

        varxRequireItems(items, "Initial");
    }
}


TEST_CASE("Reactive<AudioProcessor>",
          "[Reactive<AudioProcessor>][AudioProcessorExtension]")
{
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
    
    DummyAudioProcessor processor;
    Array<var> items;
    
    varxCollectItems(processor.rx.processorChanged, items);
    
    IT("does not emit initially")
    {
        REQUIRE(items.isEmpty());
    }
    
    IT("emits when setting the latency")
    {
        processor.setLatencySamples(256);
        varxRequireItems(items, var::undefined());
        
        // Set to same value as before, shouldn't emit
        processor.setLatencySamples(256);
        varxRequireItems(items, var::undefined());
        
        // Set to different value, should emit
        processor.setLatencySamples(512);
        varxRequireItems(items, var::undefined(), var::undefined());
    }
}
