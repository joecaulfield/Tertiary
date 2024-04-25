#pragma once
#include <JuceHeader.h>

struct WLDebugger : juce::DocumentWindow
{

public:

    static WLDebugger& getInstance();                   // Singleton access

    WLDebugger(WLDebugger const&) = delete;             // Delete copy constructor
    void operator=(WLDebugger const&) = delete;         // Delete assignment operator

    void resized();

    void setEnabled(bool enabled);
    void printMessage(juce::String mNameSpace, juce::String functionName, juce::String message);

    void closeButtonPressed() override;

private:

    static WLDebugger* instance;    // Singleton instance

    WLDebugger();                   // Private constructor
    ~WLDebugger();                  // Destructor

    juce::TextEditor logDisplay;

    bool mIsEnabled{ false };
};