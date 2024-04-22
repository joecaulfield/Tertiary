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
}

Cursor::~Cursor()
{
}

void Cursor::resized()
{

    auto bounds = getLocalBounds();

    // Establish Horizontal Cursor Bounds
    if (isVertical)
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

void Cursor::paint(juce::Graphics& g)
{

    auto cursorWidth = 3.f;
    auto cursorOpacity = 0.75f;

    if (mHasFocus)
    {
        cursorWidth = 4.f;
        cursorOpacity = 1.f;
    }
    else {}

    g.setColour(juce::Colours::white);
    g.setOpacity(cursorOpacity);
    g.drawLine(mCursor, cursorWidth);
}

void Cursor::setHorizontalOrientation()
{
    isVertical = false;
}


void Cursor::setFocus(bool hasFocus)
{
    mHasFocus = hasFocus;
    repaint();
}