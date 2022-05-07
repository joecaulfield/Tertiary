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
	Oscilloscope oscilloscope{ audioProcessor, globalControls};
	FrequencyResponse frequencyResponse{ audioProcessor, audioProcessor.apvts, globalControls };

	juce::Label companyTitle;
	juce::Image imageTitleHeader;

	juce::String version = "Beta 0.1";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessorEditor)
};
