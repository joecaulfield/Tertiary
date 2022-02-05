/*
  ==============================================================================

    GainBar.h
    Created: 5 Jan 2022 1:37:47pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../../DSP/Params.h"
#include "../../MyLookAndFeel.h"

struct GainBar : juce::Component, juce::Timer, juce::MouseListener, juce::Button::Listener
{
	GainBar();
	~GainBar() override;

	void paint(juce::Graphics& g) override;

	void resized() override;

	void timerCallback() override;
	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void fadeInOut();

	void buttonClicked(juce::Button* button);

	juce::ToggleButton toggleBypass, toggleSolo, toggleMute;
	//juce::Slider sliderGain;
	CustomSlider sliderGain;

	bool hasFocus{ false };

	StandardSliderLookAndFeel gainSliderLookAndFeel;

	void setMode(juce::String bandMode); // "low", "mid", "high"
	int mode{ 0 };

private:
	bool fadeIn{ false };
	float fadeAlpha{ 1.f };

	float fadeMax = 1.f;
	float fadeMin = 0.45;
	float fadeStepUp = 0.05f;
	float fadeStepDown = 0.05f;
};