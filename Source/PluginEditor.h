/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Controls/GlobalControls.h"
#include "GUI/Display/Oscilloscope.h"
#include "GUI/Display/FrequencyResponse.h"
#include "GUI/AllColors.h"

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
    
    /* Reference to the Audio Processor & DSP Parameters */
    TertiaryAudioProcessor& audioProcessor;

    /* Contains Lower-Half of Parameters */
	GlobalControls globalControls{ audioProcessor };
    
    /* Time-Domain Display */
	Oscilloscope oscilloscope{ audioProcessor, globalControls};

    /* Frequency-Domain & Crossover Display */
	FrequencyResponse frequencyResponse{ audioProcessor, audioProcessor.apvts, globalControls };

    /* Header displayed in top corner */
	juce::Label companyTitle;

    /* Graphics Asset, Top-Center Plugin-Name "Tertiary" */
	juce::Image imageTitleHeader;

    /* Plugin Version Number */
    juce::String version = "Beta 0.5";

    /* GPU Graphics Accerlation */
    juce::OpenGLContext openGLContext;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessorEditor)
};
 
