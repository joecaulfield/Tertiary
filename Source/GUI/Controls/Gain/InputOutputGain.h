/*
  ==============================================================================

    InputOutputGain.h
    Created: 10 Jan 2022 8:19:16pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "../../../PluginProcessor.h"
#include "../../MyLookAndFeel.h"

struct InputOutputGain : juce::Component, juce::Timer
{
	InputOutputGain(TertiaryAudioProcessor& p);
	~InputOutputGain() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void timerCallback() override;

	void setPickOffPoint(juce::String pickoff);

	TertiaryAudioProcessor& audioProcessor;
	juce::Slider sliderGain;

	InOutLookAndFeel inOutLookAndFeel;

	void drawGrill(juce::Rectangle<float> bounds);

	void getLevels();

	juce::ColourGradient InputOutputGain::makeMeterGradient(juce::Rectangle<float> bounds, float brightness);

private:

	float leftLevel = -60.f;
	float rightLevel = -60.f;
	float leftLevelPixel;
	float rightLevelPixel;

	// Grill Drawing Parameters
	int margin = 0;			// Amount of Pixels removed from Top/Bottom before grill
	int spacing = 5;			// Vertical Distance between LEDs
	float ledWidth = 18;
	float ledHeight = 3;
	juce::Array<float> ledThresholds;

	juce::Rectangle<float> leftBounds, rightBounds;

	juce::String pickOffPoint = "NA"; // 0 = INPUT, 1 = OUTPUT

	juce::ColourGradient ledOffGradient, ledOnGradient{};

	juce::Image grill;
};