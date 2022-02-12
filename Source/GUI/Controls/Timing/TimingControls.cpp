/*
  ==============================================================================

    TimingControls.cpp
    Created: 1 Jan 2022 9:49:48pm
    Author:  Joe

  ==============================================================================
*/

#include "TimingControls.h"

TimingControls::TimingControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{

	// Color-Coding and Defining Label-Text of Each Bar Class
	timingBarLow.setMode("low");
	timingBarMid.setMode("mid");
	timingBarHigh.setMode("high");

	addAndMakeVisible(timingBarLow);
	addAndMakeVisible(timingBarMid);
	addAndMakeVisible(timingBarHigh);

	makeAttachments();

	setSize(500, 140);
}

void TimingControls::paint(juce::Graphics& g)
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
	g.drawFittedText("WAVE TIMING", titleBounds, juce::Justification::centred, 1);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom() - bottomBarHeight, bounds.getWidth(), bottomBarHeight };

	// Set Font Height For Sub-Labels
	g.setFont(16);

	//g.setColour(juce::Colours::white);


	// Draw Parameter Labels: Sync ======================
	juce::Rectangle<int> syncLabelBounds {	timingBarHigh.toggleSync.getX(),
											labelBounds.getY(),
											timingBarHigh.toggleSync.getWidth(),
											labelBounds.getHeight()};


	g.drawFittedText("STH", syncLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Multiplier AKA Rhythm ======================
	juce::Rectangle<int> multLabelBounds{	timingBarHigh.sliderMultuplier.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderMultuplier.getWidth(),
											labelBounds.getHeight() };

	// VERTICAL LINE
	g.drawVerticalLine(	timingBarLow.sliderMultuplier.getX(),
						labelBounds.getY(),
						labelBounds.getCentreY());

	// HORIZONTAL LINE
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							timingBarLow.sliderMultuplier.getX(),
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) - 40);
	
	// LABEL
	g.drawFittedText("RHYTHM", multLabelBounds, juce::Justification::centred, 1);

	// HORIZONTAL LINE
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) + 40,
							timingBarLow.sliderMultuplier.getRight());

	// VERTICAL LINE
	g.drawVerticalLine(	timingBarLow.sliderMultuplier.getRight(),
						labelBounds.getY(),
						labelBounds.getCentreY());


	// Draw Parameter Labels: Rate ======================
	juce::Rectangle<int> rateLabelBounds{	timingBarHigh.sliderRate.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderRate.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("RATE", rateLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Phase ======================
	juce::Rectangle<int> phaseLabelBounds{	timingBarHigh.sliderPhase.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderPhase.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("PHASE", phaseLabelBounds, juce::Justification::centred, 1);
}

void TimingControls::resized()
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
	flexBox.items.add(FlexItem(timingBarLow).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(timingBarMid).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(timingBarHigh).withHeight(30));
	flexBox.items.add(spacer);

	flexBox.performLayout(bounds);
}

void TimingControls::makeAttachments()
{

	using namespace Params;
	const auto& params = GetParams();

	// SYNC TO HOST ================
	syncLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Sync_Low_LFO),
							timingBarLow.toggleSync);

	syncMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Sync_Mid_LFO),
							timingBarMid.toggleSync);

	syncHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Sync_High_LFO),
							timingBarHigh.toggleSync);

	// RATE ================================
	rateLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Rate_Low_LFO),
							timingBarLow.sliderRate.slider);

	rateMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Rate_Mid_LFO),
							timingBarMid.sliderRate.slider);

	rateHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Rate_High_LFO),
							timingBarHigh.sliderRate.slider);

	// MULTIPLIER AKA RHYTHM ================================
	multLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Multiplier_Low_LFO),
							timingBarLow.sliderMultuplier);

	multMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Multiplier_Mid_LFO),
							timingBarMid.sliderMultuplier);

	multHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Multiplier_High_LFO),
							timingBarHigh.sliderMultuplier);

	// PHASE ================================
	phaseLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Relative_Phase_Low_LFO),
							timingBarLow.sliderPhase.slider);

	phaseMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Relative_Phase_Mid_LFO),
							timingBarMid.sliderPhase.slider);

	phaseHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Relative_Phase_High_LFO),
							timingBarHigh.sliderPhase.slider);
}