namespace util {
    juce::String arrayDesc(const juce::Array<juce::var>& array)
    {
        juce::StringArray strings;
        for (juce::var v : array)
            strings.add(varx::util::desc(v));

        return varx::util::desc(strings);
    }
}
