/*
  ==============================================================================

    GlobalControls.cpp
    Created: 30 Dec 2021 11:38:20am
    Author:  Joe

  ==============================================================================
*/

#include "GlobalControls.h"

// GLOBAL CONTROLS =============================================================

GlobalControls::GlobalControls(TertiaryAudioProcessor& p)
	: audioProcessor(p), apvts(p.apvts)
{
	addAndMakeVisible(mWaveControls);
	addAndMakeVisible(mTimingControls);
	addAndMakeVisible(mGainControls);
	addAndMakeVisible(mXoverControls);
	addAndMakeVisible(inputGain);
	addAndMakeVisible(outputGain);

	// Determine the Label-Meters' Pickoff Point
	inputGain.setPickOffPoint("INPUT");
	outputGain.setPickOffPoint("OUTPUT");

	makeAttachments();

	setSize(250, 140);
}

void GlobalControls::paint(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::GlobalControlsColors;

	auto bounds = getLocalBounds().toFloat();

	g.setGradientFill(BACKGROUND_GRADIENT(bounds));
	g.fillAll();
}

void GlobalControls::paintOverChildren(juce::Graphics& g)
{
	// Input Gain Border
	paintBorder(g, juce::Colours::purple, border1);

	// Wave-Controls Border
	paintBorder(g, juce::Colours::purple, border2.toFloat().removeFromTop(border2.getHeight() / 2.f));

	// Timing-Controls Border
	paintBorder(g, juce::Colours::purple, border2.toFloat().removeFromBottom(border2.getHeight() / 2.f));

	// Gain-Controls Border
	paintBorder(g, juce::Colours::purple, border3.toFloat().removeFromTop(border2.getHeight() / 2.f));

	// Crossover-Controls Border
	paintBorder(g, juce::Colours::purple, border3.toFloat().removeFromBottom(border2.getHeight() / 2.f));

	// Output Gain Border
	paintBorder(g, juce::Colours::purple, border4);
}

void GlobalControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	auto meterWidth = 40;
	auto controlWidth1 = 500;
	auto controlWidth2 = 210;

	// Borders 1-4 represent the 4 columns of controls
	border1.setBounds(	bounds.getX(),					bounds.getY(), meterWidth,		bounds.getHeight());
	border2.setBounds(	border1.getRight(),				bounds.getY(), controlWidth1,	bounds.getHeight());
	border3.setBounds(	border2.getRight(),				bounds.getY(), controlWidth2,	bounds.getHeight());
	border4.setBounds(	bounds.getRight() - meterWidth, bounds.getY(), meterWidth,		bounds.getHeight());

	auto spacerHeight = FlexItem().withHeight(4);

	// FIRST COLUMN - INPUT GAIN ====================
	FlexBox column1;
	column1.flexDirection = FlexBox::Direction::column;
	column1.flexWrap = FlexBox::Wrap::noWrap;

	column1.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column1.items.add(FlexItem(inputGain).withFlex(1.f));
	column1.items.add(spacerHeight); // Offset for Inward-Drawn Border

	column1.performLayout(border1);

	// SECOND COLUMN - WAVE AND TIMING CONTROLS ====================
	FlexBox column2;
	column2.flexDirection = FlexBox::Direction::column;
	column2.flexWrap = FlexBox::Wrap::noWrap;

	column2.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column2.items.add(FlexItem(mWaveControls).withFlex(1.f));
	column2.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column2.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column2.items.add(FlexItem(mTimingControls).withFlex(1.f));
	column2.items.add(spacerHeight); // Offset for Inward-Drawn Border

	column2.performLayout(border2);

	// THIRD COLUMN - GAIN AND CROSSOVER CONTROLS ====================
	FlexBox column3;
	column3.flexDirection = FlexBox::Direction::column;
	column3.flexWrap = FlexBox::Wrap::noWrap;

	column3.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column3.items.add(FlexItem(mGainControls).withFlex(1.f));
	column3.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column3.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column3.items.add(FlexItem(mXoverControls).withFlex(1.f));
	column3.items.add(spacerHeight); // Offset for Inward-Drawn Border

	column3.performLayout(border3);

	// FOURTH COLUMN - OUTPUT GAIN ====================
	FlexBox column4;
	column4.flexDirection = FlexBox::Direction::column;
	column4.flexWrap = FlexBox::Wrap::noWrap;

	column4.items.add(spacerHeight); // Offset for Inward-Drawn Border
	column4.items.add(FlexItem(outputGain).withFlex(1.f));
	column4.items.add(spacerHeight); // Offset for Inward-Drawn Border

	column4.performLayout(border4);
}

void GlobalControls::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	inputGainAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Input_Gain),
							inputGain.sliderGain);

	outputGainAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Output_Gain),
							outputGain.sliderGain);
}