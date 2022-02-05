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

	inputGain.setPickOffPoint("INPUT");
	outputGain.setPickOffPoint("OUTPUT");

	makeAttachments();

	setSize(250, 140);
}

void GlobalControls::paint(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::GlobalControlsColors;

	g.setGradientFill(BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
	g.fillAll();

	paintBorder(g, border2);
	paintBorder(g, border3);
}

void GlobalControls::paintOverChildren(juce::Graphics& g)
{
	paintBorder(g, border1);
	paintBorder(g, border4);

}

void GlobalControls::resized()
{
	using namespace juce;

	// DRAW FLEXBOX WITH SECTIONS =======================================================================
	auto bounds = getLocalBounds();

	auto meterWidth = 40;
	auto controlWidth1 = 500;
	auto controlWidth2 = 210;

	auto reduced = 10;

	border1.setBounds(bounds.getX(), bounds.getY(), meterWidth, bounds.getHeight());
	border2.setBounds(border1.getRight(), bounds.getY(), controlWidth1, bounds.getHeight());
	border3.setBounds(border2.getRight(), bounds.getY(), controlWidth2, bounds.getHeight());
	border4.setBounds(bounds.getRight() - meterWidth, bounds.getY(), meterWidth, bounds.getHeight());

	auto spacerHeight = FlexItem().withHeight(4);

	// FIRST COLUMN - INPUT GAIN
	FlexBox column1;
	column1.flexDirection = FlexBox::Direction::column;
	column1.flexWrap = FlexBox::Wrap::noWrap;

	column1.items.add(FlexItem(inputGain).withFlex(1.f));

	column1.performLayout(border1);

	// SECOND COLUMN - WAVE AND TIMING CONTROLS
	FlexBox column2;
	column2.flexDirection = FlexBox::Direction::column;
	column2.flexWrap = FlexBox::Wrap::noWrap;

	column2.items.add(FlexItem(mWaveControls).withFlex(1.f));
	column2.items.add(spacerHeight);
	column2.items.add(FlexItem(mTimingControls).withFlex(1.f));

	column2.performLayout(border2.reduced(reduced, reduced));

	// THIRD COLUMN - GAIN AND CROSSOVER CONTROLS
	FlexBox column3;
	column3.flexDirection = FlexBox::Direction::column;
	column3.flexWrap = FlexBox::Wrap::noWrap;

	column3.items.add(FlexItem(mGainControls).withFlex(1.f));
	column3.items.add(spacerHeight);
	column3.items.add(FlexItem(mXoverControls).withFlex(1.f));

	column3.performLayout(border3.reduced(reduced, reduced));

	// FOURTH COLUMN - OUTPUT GAIN
	FlexBox column4;
	column4.flexDirection = FlexBox::Direction::column;
	column4.flexWrap = FlexBox::Wrap::noWrap;

	column4.items.add(FlexItem(outputGain).withFlex(1.f));

	column4.performLayout(border4);

	// SET UP FILL GRADIENT ================================================================
	//fillGradient = juce::ColourGradient{ juce::Colours::darkgrey,	bounds.getBottomLeft().toFloat(),
	//									 juce::Colours::grey,		bounds.getTopRight().toFloat(),		true };

	//fillGradient.addColour(0.5f, juce::Colours::slategrey);
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

void GlobalControls::paintBorder(juce::Graphics& g, juce::Rectangle<float> bounds)
{
	// DRAW BORDER 1 ====================
	//auto bounds = getLocalBounds();
	juce::Rectangle<float> border;
	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y, bounds.getWidth(), bounds.getHeight());
	g.setColour(juce::Colours::white);
	g.drawRoundedRectangle(border, 1.f, 2.f);

	// DRAW BORDER 2 =====
	border.reduce(1, 1);
	g.setColour(juce::Colours::lightgrey);
	g.drawRoundedRectangle(border, 2.f, 2.f);

	// DRAW BORDER 3 =====
	border.reduce(1, 1);
	g.setColour(juce::Colours::grey);
	g.drawRoundedRectangle(border, 2.f, 2.f);

	// DRAW BORDER 4 =====
	border.reduce(1, 1);
	g.setColour(juce::Colours::darkgrey);
	g.drawRoundedRectangle(border, 2.f, 2.f);
}