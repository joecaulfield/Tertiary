/*
  ==============================================================================

    TimingControls.h
    Created: 1 Jan 2022 9:49:47pm
    Author:  Joe

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
	void drawBorder(juce::Graphics& g);
	void drawLabels() {};

	void resized() override;

	void makeAttachments();

	//void timerCallback() override;
	//void mouseEnter(const juce::MouseEvent& event) override;
	//void mouseExit(const juce::MouseEvent& event) override;
	//void fadeInOut(juce::Graphics& g);

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
	int bottomBarHeight{ 17 };
};
