 /* ==============================================================================

    WaveBar.h
    Created: 1 Jan 2022 10:17:22pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../../DSP/Params.h"
#include "../../MyLookAndFeel.h"

struct WaveBar : juce::Component, juce::Timer, juce::MouseListener, juce::Slider::Listener
{
	WaveBar();
	~WaveBar() override;

	void paint(juce::Graphics& g) override;

	void resized() override;

	void timerCallback() override {};

	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

	void sliderValueChanged(juce::Slider* slider) {};

	juce::ToggleButton toggleInvert;
	juce::Slider sliderWave;
	CustomSlider sliderSymmetry, sliderDepth;

	juce::Label bandLabel;

	bool hasFocus{ false };

	WaveLookAndFeel waveLookAndFeel;

	//StandardSliderLookAndFeel depthSliderLookAndFeel;
	//CenterSliderLookAndFeel symmetrySliderLookAndFeel;

	void setMode(juce::String bandMode); // "low", "mid", "high"
	int mode{ 0 };

private:

	bool fadeIn{ true };
	float fadeAlpha{ 1.f };

	float fadeMax = 1.f;
	float fadeMin = 0.45;
	float fadeStepUp = 0.05f;
	float fadeStepDown = 0.05f;
};