/*
  ==============================================================================

    WindowWrapperFrequency.h
    Created: 13 Oct 2022 11:12:35pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscilloscope.h"

struct WindowWrapperOscilloscope : juce::Component, juce::Timer
{
    WindowWrapperOscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
    ~WindowWrapperOscilloscope();
    
    void resized();
    
    void timerCallback();
    
    void paint(juce::Graphics& g);
    void paintOverChildren(juce::Graphics& g);
    
private:
    /* Reference to the Audio Processor & DSP Parameters */

    TertiaryAudioProcessor& audioProcessor;
    GlobalControls& globalControls;
    
    Oscilloscope oscilloscope{ audioProcessor, globalControls };

    void paintPlayhead(juce::Graphics& g);
    
    float mShowPlayhead{true};
    float mPanOrZoomChanging{false};
    float mPlayheadPositionPixel{0.f};
    juce::Rectangle<int> mLowRegion, mMidRegion, mHighRegion;
};
