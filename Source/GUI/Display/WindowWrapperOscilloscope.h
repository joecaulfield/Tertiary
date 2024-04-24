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
                                    juce::AudioProcessorValueTreeState::Listener
{
    WindowWrapperOscilloscope(TertiaryAudioProcessor& p);
    ~WindowWrapperOscilloscope();
    
    void resized() override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    void paint(juce::Graphics& g) override;
    //void paintOverChildren(juce::Graphics& g) override;

    // Pass Scope Channels up to Plugin Editor
    ScopeChannel& getOscilloscopeLow() {return oscilloscope.getLowScope(); };
    ScopeChannel& getOscilloscopeMid() {return oscilloscope.getMidScope(); };
    ScopeChannel& getOscilloscopeHigh() {return oscilloscope.getHighScope(); };
    
private:
    /* Reference to the Audio Processor & DSP Parameters */

    TertiaryAudioProcessor& audioProcessor;
    Oscilloscope oscilloscope{ audioProcessor };

    void buildOptionsMenuParameters();
    void updateOptionsParameters();

    OptionsMenu optionsMenu;
    juce::AudioParameterBool* showLowBandParam{ nullptr };
    juce::AudioParameterBool* showMidBandParam{ nullptr };
    juce::AudioParameterBool* showHighBandParam{ nullptr };
    juce::AudioParameterBool* stackBandsParam{ nullptr };
    //juce::AudioParameterBool* showCursorParam{ nullptr };
    //juce::AudioParameterBool* showPlayheadParam{ nullptr };
    
    juce::Rectangle<int> mLowRegion, mMidRegion, mHighRegion;
};
