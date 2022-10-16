/*
  ==============================================================================

    GainBar.cpp
    Created: 5 Jan 2022 1:37:47pm
    Author:  Joe

	CLASS WRAPPED BY GAIN-CONTROLS.  ONE INSTANCE OF HORIZONTAL GAIN-CONTROL BAR

  ==============================================================================
*/

#include "GainBar.h"

GainBar::GainBar()
{
	// BYPASS TOGGLE ===================================================================================
	toggleBypass.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white);
	toggleBypass.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
    toggleBypass.addListener(this);
	addAndMakeVisible(toggleBypass);

	// SOLO TOGGLE ===================================================================================
	toggleSolo.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white);
	toggleSolo.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
	toggleSolo.addListener(this);
	addAndMakeVisible(toggleSolo);

	// MUTE TOGGLE ===================================================================================
	toggleMute.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white);
	toggleMute.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
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

void GainBar::mouseEnter(const juce::MouseEvent& event)
{
	hasFocus = true;
}

void GainBar::mouseExit(const juce::MouseEvent& event)
{
	hasFocus = false;
}

void GainBar::buttonClicked(juce::Button* button)
{
	// Prevent both 'Mute' and 'Solo' from being engaged simultaneously

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
    
    mBsmChanged = true;
}

void GainBar::setMode(juce::String bandMode)
{
	if (bandMode == "low") mode = 0;

	if (bandMode == "mid") mode = 1;

	if (bandMode == "high") mode = 2;

	sliderGain.setBandMode(mode);
}
