/*
  ==============================================================================

    CrossoverControls.h
    Created: 1 Jan 2022 9:50:07pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../PluginProcessor.h"
#include "../../GUI/AllColors.h"
#include "../MyLookAndFeel.h"

struct CrossoverControls :	juce::Component, 
							juce::Slider::Listener, 
							juce::Label::Listener,
							juce::Timer
{
	CrossoverControls(juce::AudioProcessorValueTreeState& apv);
	~CrossoverControls() override;

	void paint(juce::Graphics& g) override;
	void drawBorder(juce::Graphics& g);
	//void drawLabels() {};

	void resized() override;

	//void drawSliders() {};
	void sliderValueChanged(juce::Slider* slider) override;

	void makeAttachments();

	void timerCallback() override;

	void updateStringText();

	bool lowMidFocus{ false };
	bool midHighFocus{ false };

	void labelTextChanged(juce::Label* labelThatHasChanged) override;

private:
	juce::AudioProcessorValueTreeState& apvts;

	juce::Slider	sliderLowMidCutoff, 
					sliderMidHighCutoff;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	lowMidAttachment, 
										midHighAttachment;

	//juce::Label labelLowMid, labelMidHigh, labelTitle;

	CrossoverLookAndFeel crossoverLookAndFeel;

	juce::Label sliderValueLM, sliderValueMH;

	int topBarHeight{ 25 };
	int bottomBarHeight{ 17 };

};
