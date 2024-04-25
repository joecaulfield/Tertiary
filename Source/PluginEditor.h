/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Controls/GlobalControls.h"
#include "GUI/Display/WindowWrapperFrequency.h"
#include "GUI/Display/WindowWrapperOscilloscope.h"
#include "GUI/Display/TopBanner.h"
#include "Utility/UtilityFunctions.h"

//#include "ActivatorWindow.h"

//==============================================================================
/**
*/
class TertiaryAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer, juce::Button::Listener//, juce::MouseListener
{
public:
    
    TertiaryAudioProcessorEditor (TertiaryAudioProcessor&);
    ~TertiaryAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    


    void timerCallback() override {};
    void buttonClicked(juce::Button* button) override {};
    void mouseDoubleClick (const juce::MouseEvent &event) override {};
    
private:
    
    juce::String mNameSpace{ "PluginEditor" };

    /* Handles opening & closing of the license activator window */
    ////void checkIfShouldShowTrialTitle();
    ////void openLicenseWindow();
    ////void closeLicenseWindow();
    
    ////ActivatorWindow activatorWindow;
    ////bool showTrialTitle{false};
    ////int counterFade = 30;
    ////int counterWait = 0;
    
    /* Actions used to close the Activator pop-up window */
    ////bool buttonAction{false};
    ////bool doNotWait{false};
    
    /* Reference to the Audio Processor & DSP Parameters */
    TertiaryAudioProcessor& audioProcessor;

    /* Contains Lower-Half of Parameters */
	GlobalControls globalControls{ audioProcessor };
    
    /* Time-Domain Display */
    WindowWrapperOscilloscope wrapperOscilloscope{ audioProcessor };

    /* Frequency-Domain & Crossover Display */
    WindowWrapperFrequency wrapperFrequency{ audioProcessor, audioProcessor.apvts };
    
    /* Company & Plugin Title Banner */
    //void buildTopBanner(juce::Graphics& g);
    TopBanner topBanner;
    
    /* Header displayed in top corner */
	juce::Label companyTitle;

    void buildFlexboxLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessorEditor)

};
 
