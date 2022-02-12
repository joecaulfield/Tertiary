/*
  ==============================================================================

    GlobalControls.h
    Created: 30 Dec 2021 11:38:20am
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Wave\WaveControls.h"
#include "Timing\TimingControls.h"
#include "Gain\GainControls.h"
#include "Gain\InputOutputGain.h"
#include "CrossoverControls.h"
#include "../../../Source/PluginProcessor.h"
#include "../../GUI/AllColors.h"
#include "../UtilityFunctions.h"

struct GlobalControls : juce::Component
{
	GlobalControls(TertiaryAudioProcessor& p);

	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g);
	void resized() override;

	void makeAttachments();

	juce::AudioProcessorValueTreeState& apvts;
	TertiaryAudioProcessor& audioProcessor;

	WaveControls mWaveControls{ apvts};
	TimingControls mTimingControls{ apvts };
	GainControls mGainControls{ apvts };
	CrossoverControls mXoverControls{ apvts };

	InputOutputGain inputGain{ audioProcessor};
	InputOutputGain outputGain{ audioProcessor };

private:

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	inputGainAttachment, outputGainAttachment;

	juce::Rectangle<float> border1, border2, border3, border4;
};