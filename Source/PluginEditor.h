/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI\Controls\GlobalControls.h"
#include "GUI\Display\Oscilloscope.h"
#include "GUI\Display\FrequencyResponse.h"
#include "GUI\AllColors.h"

//==============================================================================
/**
*/
class TertiaryAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TertiaryAudioProcessorEditor (TertiaryAudioProcessor&);
    ~TertiaryAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TertiaryAudioProcessor& audioProcessor;

	GlobalControls globalControls{ audioProcessor };
	Oscilloscope oscilloscope{ audioProcessor, globalControls};						// Instance of Oscilloscope Panel
	FrequencyResponse frequencyResponse{ audioProcessor.apvts, globalControls };	// Instance of Frequency Response Panel

	juce::Label companyTitle;
	juce::Image imageTitleHeader;

	juce::String version = "1.5";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessorEditor)
};
