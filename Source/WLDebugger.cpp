/*
  ==============================================================================

    wlDebugger.cpp
    Created: 25 Apr 2024 11:15:59am
    Author:  Joe

  ==============================================================================
*/

#include "WLDebugger.h"

WLDebugger* WLDebugger::instance = nullptr;

WLDebugger& WLDebugger::getInstance() {
    if (instance == nullptr) {
        instance = new WLDebugger();
    }
    return *instance;
}

// Constructor implementation with initialization of the base class
WLDebugger::WLDebugger() :  juce::DocumentWindow("WL Debugger",
                            juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
                            juce::DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    //setContentOwned(new juce::Component(), true); // You might want to replace 'new juce::Component()' with your actual content component
    //centreWithSize(400, 300);
    setSize(400, 300);
    setTopLeftPosition(400, 300);
    setResizable(true, true);
    setVisible(true);

    // Set up the text editor for logging
    logDisplay.setMultiLine(true, true);
    logDisplay.setReadOnly(true);
    logDisplay.setCaretVisible(false);
    logDisplay.setScrollbarsShown(true);
    logDisplay.setPopupMenuEnabled(false);
    setContentOwned(&logDisplay, false);
}

WLDebugger::~WLDebugger()
{
}

void WLDebugger::resized()
{
    logDisplay.setBounds(getLocalBounds());  // Ensure the text editor fills the window
}

void WLDebugger::setEnabled(bool enabled)
{
    setVisible(true);
    mIsEnabled = enabled;
}

void WLDebugger::printMessage(juce::String mNameSpace, juce::String functionName, juce::String message)
{
    juce::String timeStamp = (juce::String)juce::Time::getHighResolutionTicks();
    auto functionCalled = mNameSpace + "::" + functionName;
    auto messageToPrint = timeStamp + "\t" + functionCalled + "\t" + message;

    if (mIsEnabled)
    {
        logDisplay.moveCaretToEnd();
        logDisplay.insertTextAtCaret(messageToPrint + "\n");  // Add new message at the end with a newline
    }

}

void WLDebugger::closeButtonPressed()
{
    setVisible(false);
}