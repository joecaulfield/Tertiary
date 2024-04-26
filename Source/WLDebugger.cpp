#include "WLDebugger.h"

WLDebugger::WLDebugger() {
    // Optionally set up the TextEditor and the window
    
    logDisplay.setMultiLine(true, true);
    logDisplay.setReadOnly(false);
    logDisplay.setReturnKeyStartsNewLine(true);
    logDisplay.setCaretVisible(false);
    logDisplay.setScrollbarsShown(true);
    logDisplay.setPopupMenuEnabled(true);
    logDisplay.setBounds(100, 100, 500, 800);

    window = std::make_unique<juce::DocumentWindow>("WL Debugger", juce::Colours::black, juce::DocumentWindow::allButtons);

    window->setUsingNativeTitleBar(true);
    window->setBounds(100, 100, 500, 800);
    window->setContentOwned(&logDisplay, true);
    window->setResizable(true, false);

}

WLDebugger* WLDebugger::instance = nullptr;

WLDebugger& WLDebugger::getInstance() {
    if (instance == nullptr) {
        instance = new WLDebugger();
    }
    return *instance;
}

void WLDebugger::logMessage(const juce::String& message) {

    auto timeStamp = (juce::String)juce::Time::getHighResolutionTicks();

    juce::String formattedMessage = timeStamp + "\t" + message;
    juce::Logger::writeToLog(formattedMessage);
    if (mIsEnabled) {
        const juce::ScopedLock lock(messageLock);
        juce::MessageManager::callAsync([this, formattedMessage] {
            logDisplay.moveCaretToEnd();
            logDisplay.insertTextAtCaret(formattedMessage + "\n");
            });
    }
}

void WLDebugger::setEnabled(bool enabled) {
    mIsEnabled = enabled;
}

void WLDebugger::openWindow() {
    if (window) {
        window->setVisible(true);
        mIsEnabled = true;
    }
}

void WLDebugger::closeWindow() {
    if (window) {
        window->setVisible(false);
        logDisplay.clear();
        mIsEnabled = false;
    }
}

//void WLDebugger::closeButtonPressed()
//{
//    delete this;
//}

void WLDebugger::printMessage(const juce::String& mNameSpace, const juce::String& functionName, const juce::String& message) {

    juce::String fullMessage = mNameSpace + "::" + functionName;

    if (message != "")
        fullMessage = fullMessage + "\t" + message;

    logMessage(fullMessage);
}

WLDebugger::~WLDebugger() {
    window->clearContentComponent();
    juce::Logger::setCurrentLogger(nullptr);
}





















///*
//  ==============================================================================
//
//    wlDebugger.cpp
//    Created: 25 Apr 2024 11:15:59am
//    Author:  Joe
//
//  ==============================================================================
//*/
//
//#include "WLDebugger.h"
//
//WLDebugger* WLDebugger::instance = nullptr;
//
//WLDebugger& WLDebugger::getInstance() {
//    if (instance == nullptr) {
//        instance = new WLDebugger();
//    }
//    return *instance;
//}
//
//// Constructor implementation with initialization of the base class
//WLDebugger::WLDebugger() :  juce::DocumentWindow("WL Debugger",
//                            juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
//                            juce::DocumentWindow::allButtons)
//{
//    setUsingNativeTitleBar(true);
//    setSize(500, 800);
//    setTopLeftPosition(400, 300);
//    setResizable(true, true);
//    setVisible(true);
//
//    // Set up the text editor for logging
//    logDisplay.setMultiLine(true, true);
//    logDisplay.setReadOnly(false);
//    logDisplay.setReturnKeyStartsNewLine(true);
//    logDisplay.setCaretVisible(false);
//    logDisplay.setScrollbarsShown(true);
//    logDisplay.setPopupMenuEnabled(true);
//    setContentOwned(&logDisplay, false);
//
//    addMouseListener(this, true);
//}
//
//WLDebugger::~WLDebugger()
//{
//}
//
//void WLDebugger::resized()
//{
//    logDisplay.setBounds(0, 0, getWidth(), getHeight());  // Ensure the text editor fills the window
//}
//
//void WLDebugger::setEnabled(bool enabled)
//{
//    mIsEnabled = enabled;
//}
//
//void WLDebugger::printMessage(juce::String mNameSpace, juce::String functionName, juce::String message)
//{
//    juce::String timeStamp = (juce::String)juce::Time::getHighResolutionTicks();
//    auto functionCalled = mNameSpace + "::" + functionName;
//    auto messageToPrint = timeStamp + "\t" + functionCalled + "\t" + message;
//
//    if (mIsEnabled)
//    {
//        logDisplay.moveCaretToEnd();
//        logDisplay.insertTextAtCaret(messageToPrint + "\n");  // Add new message at the end with a newline
//    }
//
//}
//
//void WLDebugger::closeButtonPressed()
//{
//    setVisible(false);
//}
//
//void WLDebugger::timerCallback()
//{
//    if (mIsEnabled)
//    {
//        logDisplay.moveCaretToEnd();
//        logDisplay.insertTextAtCaret("\n\n\n");  // Add new message at the end with a newline
//    }
//}
//
//void WLDebugger::openWindow()
//{
//    setVisible(true);
//    setEnabled(true);
//}
//
//void WLDebugger::closeWindow()
//{
//    setVisible(false);
//    setEnabled(false);
//}
//
//void WLDebugger::mouseDown(const juce::MouseEvent& event)
//{
//    if (event.eventComponent == &logDisplay && event.mods.isRightButtonDown())
//    {
//        buildMenuOptions();
//    }
//}
//
//void WLDebugger::buildMenuOptions()
//{
//
//}