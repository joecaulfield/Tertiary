/*
  ==============================================================================

    WaveControls.h
    Created: 1 Jan 2022 9:08:03pm
    Author:  Joe

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST INDIVIDUAL WAVESHAPE PARAMETERS

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "WaveBar.h"
#include "../../../../Source/PluginProcessor.h"
#include "../../../GUI/AllColors.h"

struct WaveControls : juce::Component
{
	WaveControls(juce::AudioProcessorValueTreeState& apv);

	void paint(juce::Graphics& g) override;

	void resized() override;

	void makeAttachments();

	void drawBackgroundImage(juce::Rectangle<int> bounds);

	WaveBar waveBarLow,
			waveBarMid,
			waveBarHigh;

private:
	juce::AudioProcessorValueTreeState& apvts;

    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	waveLowAttachment,
										waveMidAttachment,
										waveHighAttachment,
										depthLowAttachment, 
										depthMidAttachment, 
										depthHighAttachment,
										symmetryLowAttachment,
										symmetryMidAttachment,
										symmetryHighAttachment;

	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	std::unique_ptr<buttonAttachment>	invertLowAttachment,
										invertMidAttachment,
										invertHighAttachment;

	juce::Image background;
};