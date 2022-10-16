/*
  ==============================================================================

    GainControls.h
    Created: 1 Jan 2022 9:49:55pm
    Author:  Joe Caulfield

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST INDIVIDUAL BAND GAIN

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "GainBar.h"
#include "../../../../Source/PluginProcessor.h"
#include "../../../GUI/AllColors.h"

struct GainControls : juce::Component
{
	GainControls(juce::AudioProcessorValueTreeState& apv);

	void paint(juce::Graphics& g) override;

	void drawBackgroundImage(juce::Rectangle<int> bounds);

	void resized() override;

	void makeAttachments();



	GainBar gainBarLow,
			gainBarMid,
			gainBarHigh;

private:
	juce::AudioProcessorValueTreeState& apvts;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	gainLowAttachment,
										gainMidAttachment,
										gainHighAttachment;

	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	std::unique_ptr<buttonAttachment>	soloLowAttachment,
										bypassLowAttachment,
										muteLowAttachment,
										soloMidAttachment,
										bypassMidAttachment,
										muteMidAttachment,
										soloHighAttachment,
										bypassHighAttachment,
										muteHighAttachment;

	//int topBarHeight{ 25 };
	//int bottomBarHeight{ 25 };

	juce::Image background;
	juce::Image imageGainControls;
    

};
