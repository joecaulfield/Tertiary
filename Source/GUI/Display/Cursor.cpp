/*
  ==============================================================================

    Cursor.cpp
    Created: 21 Apr 2024 8:58:30pm
    Author:  Joe

  ==============================================================================
*/

#include "Cursor.h"

using namespace juce;

Cursor::Cursor()
{

    setSize(5, 25);
    startTimerHz(30);
}

Cursor::~Cursor()
{
}

void Cursor::resized()
{

    auto bounds = getLocalBounds();

    // Establish Horizontal Cursor Bounds
    if (mIsVertical)
    {
        auto x = bounds.getCentreX();

        auto startY = bounds.getY();
        auto endY = bounds.getBottom();

        mCursor.setStart(x, startY);
        mCursor.setEnd(x, endY);
    }
    else
    {
        auto y = bounds.getCentreY();

        auto startX = bounds.getX();
        auto endX = bounds.getRight();

        mCursor.setStart(startX, y);
        mCursor.setEnd(endX, y);
    }

}

void Cursor::timerCallback()
{
    // If this cursor has focus, timerCounter increases until at its max.  
    // If no focus, timerCounter decreases until at its minimum.

    if ((timerCounter > timerCounterMin) || (timerCounter < timerCounterMax))
    {
        if (mHasFocus || mForceFocus)
        {
            if (timerCounter < timerCounterMax)
            {
                timerCounter++;
                repaint();
            }
        }
        else
        {
            if (timerCounter > timerCounterMin)
            {
                timerCounter--;
                repaint();
            }  
        }
    }

    
}

void Cursor::paint(juce::Graphics& g)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    auto cursorWidth = 3.f;

    fadeValue = juce::jmap((float)timerCounter, (float)timerCounterMin, (float)timerCounterMax, fadeValueMin, fadeValueMax);

    if (mHasFocus)
        cursorWidth = 4.f;

    g.setColour(juce::Colours::white);
    g.setOpacity(fadeValue);

    g.drawLine(mCursor, cursorWidth);
}

void Cursor::setHorizontalOrientation()
{

    mIsVertical = false;
}

void Cursor::setFocus(bool hasFocus)
{

    mHasFocus = hasFocus;

    juce::String bFocus;

    if (hasFocus)
        bFocus = "TRUE";
    else
        bFocus = "FALSE";

    sendBroadcast("cFOCUS", bFocus);

    repaint();
}


//
// ===========================================================================================
void Cursor::actionListenerCallback(const juce::String& message)
{

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

/* Send Broadcast Message */
// ===========================================================================================
void Cursor::sendBroadcast(juce::String parameterName, juce::String parameterValue)
{

    juce::String delimiter = ":::::";

    juce::String bandName = "xxxxx";

    auto message = bandName + delimiter + parameterName.paddedLeft('x', 10) + delimiter + parameterValue.paddedLeft('x', 10);

    sendActionMessage(message);
}