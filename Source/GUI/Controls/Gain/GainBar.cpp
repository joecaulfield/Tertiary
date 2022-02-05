/*
  ==============================================================================

    GainBar.cpp
    Created: 5 Jan 2022 1:37:47pm
    Author:  Joe

  ==============================================================================
*/

#include "GainBar.h"

GainBar::GainBar()
{
	// BYPASS TOGGLE ===================================================================================
	toggleBypass.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleBypass.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	addAndMakeVisible(toggleBypass);

	// SOLO TOGGLE ===================================================================================
	toggleSolo.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleSolo.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleSolo.addListener(this);
	addAndMakeVisible(toggleSolo);

	// MUTE TOGGLE ===================================================================================
	toggleMute.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleMute.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleMute.addListener(this);
	addAndMakeVisible(toggleMute);

	// GAIN SLIDER ===================================================================================
	sliderGain.setStyleStandard(" dB");
	addAndMakeVisible(sliderGain);

	addMouseListener(this, true);
}

GainBar::~GainBar()
{
}

void GainBar::paint(juce::Graphics& g)
{
}

void GainBar::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(10);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(sliderGain).withWidth(100));
	flexBox.items.add(FlexItem().withWidth(5));
	flexBox.items.add(FlexItem(toggleBypass).withFlex(1.f));
	flexBox.items.add(FlexItem(toggleMute).withFlex(1.f));
	flexBox.items.add(FlexItem(toggleSolo).withFlex(1.f));
	flexBox.items.add(spacer);

	flexBox.performLayout(bounds);
}

void GainBar::timerCallback()
{
	//fadeInOut();
	//repaint();
}

void GainBar::mouseEnter(const juce::MouseEvent& event)
{
	fadeIn = true;
	hasFocus = true;
}

void GainBar::mouseExit(const juce::MouseEvent& event)
{
	fadeIn = false;
	hasFocus = false;
}

void GainBar::fadeInOut()
{
	if (fadeIn) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlpha < fadeMax)
		{
			fadeAlpha += fadeStepUp;

			if (fadeAlpha > fadeMax)
				fadeAlpha = fadeMax;
		}
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlpha > fadeMin)
		{
			fadeAlpha -= fadeStepDown;

			if (fadeAlpha < fadeMin)
				fadeAlpha = fadeMin;
		}
	}

	setAlpha(fadeAlpha);
}

void GainBar::buttonClicked(juce::Button* button)
{
	if (button == &toggleSolo)
	{
		if (button->getToggleState())
			toggleMute.setToggleState(false, true);
	}

	if (button == &toggleMute)
	{
		if (button->getToggleState())
			toggleSolo.setToggleState(false, true);
	}
}

void GainBar::setMode(juce::String bandMode)
{
	if (bandMode == "low") mode = 0;

	if (bandMode == "mid") mode = 1;

	if (bandMode == "high") mode = 2;

	sliderGain.setBandMode(mode);
}