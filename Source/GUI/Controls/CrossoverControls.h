/*
  ==============================================================================

    CrossoverControls.h
    Created: 1 Jan 2022 9:50:07pm
    Author:  Joe

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST CROSSOVER PARAMETERS

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
	void resized() override;

	void drawBackgroundImage(juce::Rectangle<int> bounds);

	void sliderValueChanged(juce::Slider* slider) override;
	void labelTextChanged(juce::Label* labelThatHasChanged) override;
	void makeAttachments();
	void timerCallback() override;
	
	void updateStringText();

	bool lowMidFocus{ false };
	bool midHighFocus{ false };

private:
	juce::AudioProcessorValueTreeState& apvts;

	juce::Slider	sliderLowMidCutoff, 
					sliderMidHighCutoff;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	lowMidAttachment, 
										midHighAttachment;

	CrossoverLookAndFeel crossoverLookAndFeel;

	juce::Label sliderValueLM, sliderValueMH;

	juce::Image background;
	juce::Image imageCrossover;
};
