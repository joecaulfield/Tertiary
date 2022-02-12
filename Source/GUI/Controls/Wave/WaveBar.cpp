/*
  ==============================================================================

	WaveBar.cpp
	Created: 1 Jan 2022 10:17:22pm
	Author:  Joe

	CLASS WRAPPED BY WAVE-CONTROLS.  ONE INSTANCE OF HORIZONTAL WAVE-CONTROL BAR

  ==============================================================================
*/

#include "WaveBar.h"

WaveBar::WaveBar()
{
	// WAVE-SHAPE SLIDER ====================================================
	sliderWave.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderWave.setLookAndFeel(&waveLookAndFeel);
	addAndMakeVisible(sliderWave);
	sliderWave.addListener(this);

	// SYMMETRY SLIDER =======================================================
	sliderSymmetry.setStyleCenter("%");
	addAndMakeVisible(sliderSymmetry);

	// DEPTH SLIDER ==========================================================
	sliderDepth.setStyleStandard("%");
	addAndMakeVisible(sliderDepth);

	// INVERT TOGGLE ========================================================
	toggleInvert.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white);
	toggleInvert.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
	addAndMakeVisible(toggleInvert);

	// BAND LABEL =============================================================
	bandLabel.setJustificationType(juce::Justification::centred);
	bandLabel.setFont(juce::Font(20.f, juce::Font::bold));
	bandLabel.setColour(juce::Label::textColourId, juce::Colours::white);	
	addAndMakeVisible(bandLabel);

	addMouseListener(this, true);
}

WaveBar::~WaveBar()
{
	sliderWave.setLookAndFeel(nullptr);
}

void WaveBar::paint(juce::Graphics& g)
{
}

void WaveBar::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	// CREATE WAVEBAR LAYOUT
	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(10);
	
	flexBox.items.add(FlexItem().withWidth(5));	// SPACER
	flexBox.items.add(FlexItem(bandLabel).withWidth(25));
	flexBox.items.add(FlexItem().withWidth(5));	// SPACER

	flexBox.items.add(FlexItem(toggleInvert).withWidth(25));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(sliderWave).withFlex(1.5f));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(sliderSymmetry).withWidth(90));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(sliderDepth).withWidth(90));
	flexBox.items.add(spacer);

	flexBox.performLayout(bounds);
}

void WaveBar::mouseEnter(const juce::MouseEvent& event)
{
	hasFocus = true;
}

void WaveBar::mouseExit(const juce::MouseEvent& event)
{
	hasFocus = false;
}

void WaveBar::setMode(juce::String bandMode)
{
	if (bandMode == "low") mode = 0;
	if (bandMode == "mid") mode = 1;
	if (bandMode == "high") mode = 2;

	waveLookAndFeel.setBandMode(mode);
	sliderDepth.setBandMode(mode);
	sliderSymmetry.setBandMode(mode);

	switch (mode)
	{
	case 0: {bandLabel.setText("L", juce::NotificationType::dontSendNotification); break; }
	case 1: {bandLabel.setText("M", juce::NotificationType::dontSendNotification); break; }
	case 2: {bandLabel.setText("H", juce::NotificationType::dontSendNotification); break; }
	}
}