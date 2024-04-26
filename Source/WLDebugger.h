

// WLDebugger.h
#pragma once

#include <JuceHeader.h>

class WLDebugger {
public:
    static WLDebugger& getInstance();                   // Singleton access
    
    WLDebugger(WLDebugger const&) = delete;             // Delete copy constructor
    void operator=(WLDebugger const&) = delete;         // Delete assignment operator

    void logMessage(const juce::String& message);

    void setEnabled(bool enabled);
    void openWindow();
    void closeWindow();
    void printMessage(const juce::String& mNameSpace, const juce::String& functionName, const juce::String& message);

private:

    WLDebugger();
    ~WLDebugger();

    static WLDebugger* instance;    // Singleton instance
    juce::CriticalSection messageLock; // To ensure thread safety
    juce::TextEditor logDisplay; // The TextEditor where logs will be displayed
    std::unique_ptr<juce::DocumentWindow> window; // Optionally holds the window

    bool mIsEnabled{ false };

};

















//#pragma once
//#include <JuceHeader.h>
//
//
//struct WLDebugger : juce::DocumentWindow, juce::Timer, juce::MouseListener
//{
//
//public:
//
//    static WLDebugger& getInstance();                   // Singleton access
//
//    WLDebugger(WLDebugger const&) = delete;             // Delete copy constructor
//    void operator=(WLDebugger const&) = delete;         // Delete assignment operator
//
//    void resized();
//
//    void setEnabled(bool enabled);
//    
//    void openWindow();
//    void closeWindow();
//
//    bool isWindowOpen() { return isVisible(); };
//
//    void printMessage(juce::String mNameSpace, juce::String functionName, juce::String message);
//
//    void closeButtonPressed() override;
//
//    void timerCallback() override;
//    
//    void mouseDown(const juce::MouseEvent& event);
//
//    //void addPopupMenuItems(juce::PopupMenu& menuToAddTo, const juce::MouseEvent* mouseClickEvent) override {};
//
//private:
//
//    static WLDebugger* instance;    // Singleton instance
//
//    void buildMenuOptions();
//
//    WLDebugger();                   // Private constructor
//    ~WLDebugger();                  // Destructor
//
//    juce::TextEditor logDisplay;
//    juce::TextButton mButtonClear;
//    juce::TextButton mButtonSpacer;

//    bool mIsEnabled{ false };
//};



