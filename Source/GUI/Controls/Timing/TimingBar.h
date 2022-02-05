/*
  ==============================================================================

    TimingBar.h
    Created: 5 Jan 2022 1:37:40pm
    Author:  Joe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../../../DSP/Params.h"
#include "../../MyLookAndFeel.h"

struct TimingBar : juce::Component, juce::Timer, juce::MouseListener, juce::Slider::Listener, juce::Button::Listener
{
	TimingBar();
	~TimingBar() override;

	void paint(juce::Graphics& g) override;

	void resized() override;

	void timerCallback() override {};
	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void fadeInOut() {};

	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) {};

	juce::ToggleButton toggleSync;
	juce::Slider sliderMultuplier;
	CustomSlider sliderRate, sliderPhase;

	juce::Label bandLabel;

	bool hasFocus{ false };

	MultLookAndFeel multLookAndFeel;

	void setMode(juce::String bandMode); // "low", "mid", "high"
	int mode{ 0 };

private:

};
