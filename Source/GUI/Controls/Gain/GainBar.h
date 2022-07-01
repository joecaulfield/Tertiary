/*
  ==============================================================================

    GainBar.h
    Created: 5 Jan 2022 1:37:47pm
    Author:  Joe

	CLASS WRAPPED BY GAIN-CONTROLS.  ONE INSTANCE OF HORIZONTAL GAIN-CONTROL BAR

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../../DSP/Params.h"
#include "../../MyLookAndFeel.h"

struct GainBar : juce::Component, juce::Timer, juce::Button::Listener
{
	GainBar();
	~GainBar() override;

	void paint(juce::Graphics& g) override;

	void resized() override;

	void timerCallback() override {};
	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

	void buttonClicked(juce::Button* button);

	juce::ToggleButton toggleBypass, toggleSolo, toggleMute;

	CustomSlider sliderGain;

	bool hasFocus{ false };

	StandardSliderLookAndFeel gainSliderLookAndFeel;

	void setMode(juce::String bandMode); // "low", "mid", "high"
	int mode{ 0 };

private:

};
