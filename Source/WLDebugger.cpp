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


void WLDebugger::printMessage(const juce::String& mNameSpace, const juce::String& functionName, const juce::String& message) {

    juce::String fullMessage = mNameSpace + "::" + functionName;

    if (message != "")
        fullMessage = fullMessage + "\t" + message;

    logMessage(fullMessage);
}

WLDebugger::~WLDebugger() {
    window->clearContentComponent();
    window = nullptr; // Ensure the window is properly deleted
    juce::Logger::setCurrentLogger(nullptr);
    DBG("CLOSED");
}

void WLDebugger::cleanupInstance() {
    instance = nullptr;
    delete instance;
}