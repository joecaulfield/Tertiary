/*
  ==============================================================================

    TimingBar.cpp
    Created: 5 Jan 2022 1:37:40pm
    Author:  Joe

	CLASS WRAPPED BY TIMING-CONTROLS.  ONE INSTANCE OF HORIZONTAL TIMING-CONTROL BAR

  ==============================================================================
*/

#include "TimingBar.h"

TimingBar::TimingBar()
{
	// RATE SLIDER ===========================================================
	sliderRate.setStyleStandard(" Hz");
	addAndMakeVisible(sliderRate);

	// MULTIPLIER SLIDER =======================================================
	sliderMultuplier.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderMultuplier.setLookAndFeel(&multLookAndFeel);
	addAndMakeVisible(sliderMultuplier);
	sliderMultuplier.addListener(this);

	// PHASE SLIDER ===========================================================
	sliderPhase.setStyleCenter(juce::CharPointer_UTF8("\xc2\xb0"));
	addAndMakeVisible(sliderPhase);

	// SYNC TOGGLE ===========================================================
	toggleSync.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::white);
	toggleSync.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::white);
	addAndMakeVisible(toggleSync);

	// BAND LABEL ===========================================================
	bandLabel.setJustificationType(juce::Justification::centred);
	bandLabel.setFont(juce::Font(20.f, juce::Font::bold));
	bandLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	addAndMakeVisible(bandLabel);

	addMouseListener(this, true);
	toggleSync.addListener(this);
}

TimingBar::~TimingBar()
{
	sliderMultuplier.setLookAndFeel(nullptr);
}

void TimingBar::paint(juce::Graphics& g)
{	
}

void TimingBar::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(15);

	flexBox.items.add(FlexItem().withWidth(5));	// SPACER
	flexBox.items.add(FlexItem(bandLabel).withWidth(25));
	flexBox.items.add(FlexItem().withWidth(5));	// SPACER

	flexBox.items.add(FlexItem(toggleSync).withWidth(25));
	flexBox.items.add(FlexItem().withWidth(10));	// SPACER

	flexBox.items.add(FlexItem(sliderMultuplier).withFlex(1.5f));
	flexBox.items.add(FlexItem().withWidth(15));	// SPACER
	flexBox.items.add(FlexItem(sliderRate).withWidth(115.f));


	flexBox.items.add(FlexItem().withWidth(10));	// SPACER
	flexBox.items.add(FlexItem(sliderPhase).withWidth(90.f));
	flexBox.items.add(FlexItem().withWidth(10));	// SPACER


	flexBox.performLayout(bounds);
}

void TimingBar::mouseEnter(const juce::MouseEvent& event)
{
	hasFocus = true;
}

void TimingBar::mouseExit(const juce::MouseEvent& event)
{
	hasFocus = false;
}

void TimingBar::buttonClicked(juce::Button* button)
{
	// When 'Sync-To-Host' enabled, disable 'Rate' param.
	// When 'Sync-To-Host' disabled, disable 'Multiplier' param.

	if (toggleSync.getToggleState())
	{
		sliderMultuplier.setEnabled(true);
		sliderRate.setEnabled(false);
	}
	else
	{
		sliderMultuplier.setEnabled(false);
		sliderRate.setEnabled(true);
	}

}

void TimingBar::setMode(juce::String bandMode)
{
	if (bandMode == "low")	mode = 0;

	if (bandMode == "mid") mode = 1;

	if (bandMode == "high") mode = 2;

	sliderPhase.setBandMode(mode);
	sliderRate.setBandMode(mode);
	multLookAndFeel.setBandMode(mode);

	switch (mode)
	{
	case 0: {bandLabel.setText("L", juce::NotificationType::dontSendNotification); break; }
	case 1: {bandLabel.setText("M", juce::NotificationType::dontSendNotification); break; }
	case 2: {bandLabel.setText("H", juce::NotificationType::dontSendNotification); break; }
	}
}