#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "JuceHeader.h"

REAX_ENABLE_EXTRA_WARNINGS

using namespace juce;

class TestRunnerApplication : public JUCEApplication
{
public:
    void initialise(const String&) override
    {
        // Show debug output on windows (Visual Studio):
#if JUCE_DEBUG && JUCE_WINDOWS
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif
        Catch::ConfigData config;
        //		config.shouldDebugBreak = true;

        Catch::Session session;
        session.useConfigData(config);
        session.run();

        // Keep debug output window open on exit (Visual Studio):
#if JUCE_DEBUG && JUCE_WINDOWS
		system("pause");
#endif

        quit();
    }

    void shutdown() override
    {}

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return false; }
};


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
START_JUCE_APPLICATION(TestRunnerApplication)
#pragma clang diagnostic pop
