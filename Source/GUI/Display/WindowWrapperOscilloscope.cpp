/*
  ==============================================================================

    WindowWrapperFrequency.cpp
    Created: 13 Oct 2022 11:12:35pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "WindowWrapperOscilloscope.h"


WindowWrapperOscilloscope::WindowWrapperOscilloscope( TertiaryAudioProcessor& p, GlobalControls& gc)
                                                
: audioProcessor(p),
globalControls(gc)
{
    addAndMakeVisible(oscilloscope);
    startTimerHz(60);
}

WindowWrapperOscilloscope::~WindowWrapperOscilloscope()
{
    
}

void WindowWrapperOscilloscope::resized()
{
    oscilloscope.setSize(getWidth(), getHeight() );
}

void WindowWrapperOscilloscope::timerCallback()
{
    /* Do the FFT Calculation here */
    
    mShowPlayhead = oscilloscope.getShowPlayhead();
    mPanOrZoomChanging = oscilloscope.getPanOrZoomChanging();
    mPlayheadPositionPixel = oscilloscope.getPlayheadPositionPixel();
    
    mLowRegion = oscilloscope.getLowRegion();
    mMidRegion = oscilloscope.getMidRegion();
    mHighRegion = oscilloscope.getHighRegion();
    
    repaint();
}

void WindowWrapperOscilloscope::paint(juce::Graphics& g) {}

void WindowWrapperOscilloscope::paintOverChildren(juce::Graphics& g)
{
    paintPlayhead(g);
}

void WindowWrapperOscilloscope::paintPlayhead(juce::Graphics& g)
{
    if (mShowPlayhead && ! mPanOrZoomChanging)
    {
        g.setColour(juce::Colours::darkgrey);
        
        float buttonBoundsRight = oscilloscope.getMenuButtonBounds().getRight();
        float buttonBoundsBottom = oscilloscope.getMenuButtonBounds().getBottom();
        
        if (oscilloscope.isMenuOpen())
        {
            if (mPlayheadPositionPixel > buttonBoundsRight)
            {
                g.drawVerticalLine(mPlayheadPositionPixel, mLowRegion.getY(), mLowRegion.getBottom());
                g.drawVerticalLine(mPlayheadPositionPixel, mMidRegion.getY(), mMidRegion.getBottom());
                g.drawVerticalLine(mPlayheadPositionPixel, mHighRegion.getY(), mHighRegion.getBottom());
            }
            else
                g.drawVerticalLine(mPlayheadPositionPixel, buttonBoundsBottom, mHighRegion.getBottom());
        }
        else
        {
            g.drawVerticalLine(mPlayheadPositionPixel, mLowRegion.getY(), mLowRegion.getBottom());
            g.drawVerticalLine(mPlayheadPositionPixel, mMidRegion.getY(), mMidRegion.getBottom());
            g.drawVerticalLine(mPlayheadPositionPixel, mHighRegion.getY(), mHighRegion.getBottom());
        }
    }
}
