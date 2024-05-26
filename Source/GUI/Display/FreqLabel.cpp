/*
  ==============================================================================

    FreqLabel.cpp
    Created: 22 Apr 2024 10:21:12pm
    Author:  Joe

  ==============================================================================
*/

#include "FreqLabel.h"

FreqLabel::FreqLabel()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    setSize(65, 25);

    // Initialize Low-Mid Cutoff Label =======
    mLabel.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.f));
    mLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    mLabel.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::antiquewhite);
    mLabel.setJustificationType(juce::Justification::centred);
    mLabel.setEditable(false, true);
    mLabel.addListener(this);
    mLabel.addMouseListener(this, true);
    addAndMakeVisible(mLabel);

    startTimerHz(30);
}

FreqLabel::~FreqLabel()
{
}

void FreqLabel::resized()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    mLabel.setBounds(getBounds());
}

void FreqLabel::paint(juce::Graphics& g)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    // Fill Rectangle
    g.setColour(juce::Colours::black);
    g.setOpacity(mLabel.isBeingEdited() ? 1.f : fadeValue);
    g.fillRoundedRectangle(mLabel.getBounds().toFloat(), 3.f);

    // Draw Outline
    g.setColour(juce::Colours::darkgrey);
    g.setOpacity(mLabel.isBeingEdited() ? 1.f : fadeValue);
    g.drawRoundedRectangle(mLabel.getBounds().toFloat(), 3.f, 1.f);
}

/* Entry-Validation & Update the Slider Value after Label Change */
// ===========================================================================================
void FreqLabel::labelTextChanged(juce::Label* labelThatHasChanged)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    // Means of handling invalid input entries,
    // while allowing some flexibility and auto-interpretation of user-entered values

    juce::String original = labelThatHasChanged->getText();

    juce::String entryString = labelThatHasChanged->getText();
    float entryFloat{ 0.f };

    // Simplify Text
    entryString = entryString.toLowerCase();            // Lower Case
    entryString = entryString.removeCharacters(" ");    // Remove Spaces
    entryString = entryString.removeCharacters("hz");    // Remove Units

    // Check for Non-Numerical Text ==================================================
    bool containsText = false;
    bool containsNum = false;

    for (int i = 0; i < entryString.length(); i++)
    {
        if ((int)entryString[i] < 48 || (int)entryString[i] > 57)
            containsText = true;

        if ((int)entryString[i] >= 48 && (int)entryString[i] <= 57)
            containsNum = true;
    }

    if (!containsText) // Handle entry as pure numerical
        entryFloat = entryString.getFloatValue();
    else // Handle entry as alphanumeric combination
    {
        if (entryString.containsChar('k'))
        {
            entryString.removeCharacters("k");
            entryFloat = entryString.getFloatValue() * 1000.f;
        }
        else // Unaccepted entry, truncate the decimal
            entryFloat = juce::roundToInt(entryString.getFloatValue());
    }

    if (!containsNum)
    {
        labelThatHasChanged->setText("20 Hz", juce::NotificationType::sendNotification);

        if (getName() == "LOW")
            sendBroadcast(getName(), "20.0");
        if (getName() == "HIGH")
            sendBroadcast(getName(), "20000.0");
    }
    else
    {
        sendBroadcast(getName(), (juce::String)entryFloat);
    }




}

/* Update the Label Value after Slider Change */
// ===========================================================================================
void FreqLabel::setLabelValue(float newValue)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    juce::String string;

    // Abbreviate Values In The Kilohertz
    if (newValue <= 999.f)
        string = (juce::String)newValue + " Hz";
    else
    {
        auto num = newValue;
        num /= 10.f;
        num = juce::roundFloatToInt(num);
        num /= 100.f;
        string = (juce::String)(num)+" kHz";
    }

    mLabel.setText(string, juce::NotificationType::dontSendNotification);

}

/* Send Broadcast Message */
// ===========================================================================================
void FreqLabel::sendBroadcast(juce::String parameterName, juce::String parameterValue)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    juce::String delimiter = ":::::";

    juce::String bandName = "xxxxx";

    auto message = bandName + delimiter + parameterName.paddedLeft('x', 10) + delimiter + parameterValue.paddedLeft('x', 10);

    sendActionMessage(message);
}

/* */
// ===========================================================================================
void FreqLabel::timerCallback()
{

    // If this cursor has focus, timerCounter increases until at its max.  
    // If no focus, timerCounter decreases until at its minimum.

    if ((timerCounter > timerCounterMin) || (timerCounter < timerCounterMax))
    {
        if (mHasFocus || mForceFocus)
        {
            if (timerCounter < timerCounterMax)
                timerCounter++;
        }
        else
        {
            if (timerCounter > timerCounterMin)
                timerCounter--;
        }

        fadeValue = juce::jmap((float)timerCounter, (float)timerCounterMin, (float)timerCounterMax, fadeValueMin, fadeValueMax);
        setAlpha(fadeValue);
    }
}

/* */
// ===========================================================================================
void FreqLabel::mouseEnter(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    if (!isMouseButtonDown())
    {
        mHasFocus = true;
        sendBroadcast("cFOCUS", "TRUE");
    }
}

/* */
// ===========================================================================================
void FreqLabel::mouseExit(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    if (!isMouseOver(true))
    {
        mHasFocus = false;
        sendBroadcast("cFOCUS", "FALSE");
    }
}

//
// ===========================================================================================
void FreqLabel::actionListenerCallback(const juce::String& message)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto paramName = message.replaceSection(0, 10, "");
    paramName = paramName.replaceSection(10, 25, "");
    paramName = paramName.removeCharacters("x");

    juce::String paramValue = message.replaceSection(0, 25, "");
    paramValue = paramValue.removeCharacters("x");

    if (paramName == "cFOCUS")
    {
        if (paramValue == "TRUE")
            mForceFocus = true;
        else
            mForceFocus = false;
    }
}