/*
  ==============================================================================

    GainControls.cpp
    Created: 1 Jan 2022 9:49:55pm
    Author:  Joe Caulfield

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST INDIVIDUAL BAND GAIN

  ==============================================================================
*/

#include "GainControls.h"

GainControls::GainControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{

	// Color-Coding and Defining Label-Text of Each Bar Class
	gainBarLow.setMode("low");	
	gainBarMid.setMode("mid");
	gainBarHigh.setMode("high");

	addAndMakeVisible(gainBarLow);
	addAndMakeVisible(gainBarMid);
	addAndMakeVisible(gainBarHigh);

	makeAttachments();

	setSize(200, 140);
}

void GainControls::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{ bounds.getX(), bounds.getY(), bounds.getWidth(), topBarHeight };

	// Draw the Title
	g.setColour(juce::Colours::white);

	auto myTypeface = "Helvetica";
	auto titleFont = juce::Font(myTypeface, topBarHeight * 0.75f, juce::Font::FontStyleFlags::bold);

	g.setFont(titleFont);

	g.setColour(juce::Colours::white);
	g.drawFittedText("GAIN CONTROL", titleBounds, juce::Justification::centred, 1);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom()- bottomBarHeight, bounds.getWidth(), bottomBarHeight };

	// Set Font Height For Sub-Labels
	g.setFont(16);

	//g.setColour(juce::Colours::white);
	
	// Draw Parameter Labels: Gain ======================
	juce::Rectangle<int> gainLabelBounds{	gainBarHigh.sliderGain.getX(), 
											labelBounds.getY(), 
											gainBarHigh.sliderGain.getWidth(),
											labelBounds.getHeight()};

	g.drawFittedText("BAND GAIN", gainLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Bypass ======================
	juce::Rectangle<int> bypLabelBounds{	gainBarHigh.toggleBypass.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleBypass.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("B", bypLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Mute ======================
	juce::Rectangle<int> muteLabelBounds{	gainBarHigh.toggleMute.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleMute.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("M", muteLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Solo ======================
	juce::Rectangle<int> soloLabelBounds{	gainBarHigh.toggleSolo.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleSolo.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("S", soloLabelBounds, juce::Justification::centred, 1);
}

void GainControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(topBarHeight);
	bounds.removeFromBottom(bottomBarHeight);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withHeight(2);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(gainBarLow).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(gainBarMid).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(gainBarHigh).withHeight(30));
	flexBox.items.add(spacer);

	flexBox.performLayout(bounds);
}

void GainControls::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	// GAIN =====================
	gainLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Gain_Low_Band),
							gainBarLow.sliderGain.slider);

	gainMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Gain_Mid_Band),
							gainBarMid.sliderGain.slider);

	gainHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Gain_High_Band),
							gainBarHigh.sliderGain.slider);

	// SOLO =====================
	soloLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Solo_Low_Band),
							gainBarLow.toggleSolo);

	soloMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Solo_Mid_Band),
							gainBarMid.toggleSolo);

	soloHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Solo_High_Band),
							gainBarHigh.toggleSolo);

	// BYPASS =====================
	bypassLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Bypass_Low_Band),
							gainBarLow.toggleBypass);

	bypassMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Bypass_Mid_Band),
							gainBarMid.toggleBypass);

	bypassHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Bypass_High_Band),
							gainBarHigh.toggleBypass);

	// MUTE =====================
	muteLowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Mute_Low_Band),
							gainBarLow.toggleMute);

	muteMidAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Mute_Mid_Band),
							gainBarMid.toggleMute);

	muteHighAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Mute_High_Band),
							gainBarHigh.toggleMute);
}