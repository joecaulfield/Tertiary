/*
  ==============================================================================

    TimingControls.h
    Created: 1 Jan 2022 9:49:47pm
    Author:  Joe

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST INDIVIDUAL WAVE TIMING PARAMETERS

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "TimingBar.h"
#include "../../../../Source/PluginProcessor.h"
#include "../../../GUI/AllColors.h"

struct TimingControls : juce::Component/*, juce::Timer, juce::MouseListener*/
{
	TimingControls(juce::AudioProcessorValueTreeState& apv);

	void paint(juce::Graphics& g) override;

	void resized() override;

	void makeAttachments();

	TimingBar	timingBarLow,
				timingBarMid,
				timingBarHigh;

private:
	juce::AudioProcessorValueTreeState& apvts;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	rateLowAttachment,
										rateMidAttachment,
										rateHighAttachment,
										multLowAttachment,
										multMidAttachment,
										multHighAttachment,
										phaseLowAttachment,
										phaseMidAttachment,
										phaseHighAttachment;

	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	std::unique_ptr<buttonAttachment>	syncLowAttachment,
										syncMidAttachment,
										syncHighAttachment;
	
	int topBarHeight{ 25 };
	int bottomBarHeight{ 25 };
};
