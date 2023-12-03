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

#include "OptionsMenu.h"

struct WindowWrapperOscilloscope :  juce::Component,
                                    juce::Timer,
                                    juce::AudioProcessorValueTreeState::Listener
{
    WindowWrapperOscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
    ~WindowWrapperOscilloscope();
    
    void resized() override;
    
    void timerCallback() override;
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;

private:
    /* Reference to the Audio Processor & DSP Parameters */

    TertiaryAudioProcessor& audioProcessor;
    GlobalControls& globalControls;
    Oscilloscope oscilloscope{ audioProcessor, globalControls };

    void buildOptionsMenuParameters();
    void updateOptionsParameters();

    OptionsMenu optionsMenu;
    juce::AudioParameterBool* showLowBandParam{ nullptr };
    juce::AudioParameterBool* showMidBandParam{ nullptr };
    juce::AudioParameterBool* showHighBandParam{ nullptr };
    juce::AudioParameterBool* stackBandsParam{ nullptr };
    juce::AudioParameterBool* showCursorParam{ nullptr };
    juce::AudioParameterBool* showPlayheadParam{ nullptr };
    
    void paintPlayhead(juce::Graphics& g);
    bool mShowPlayhead;
    float mPanOrZoomChanging{false};
    float mPlayheadPositionPixel{0.f};
    juce::Rectangle<int> mLowRegion, mMidRegion, mHighRegion;
};
