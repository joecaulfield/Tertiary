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
	timingBarLow.setMode("low");
	timingBarMid.setMode("mid");
	timingBarHigh.setMode("high");

	addAndMakeVisible(timingBarLow);
	addAndMakeVisible(timingBarMid);
	addAndMakeVisible(timingBarHigh);

	drawLabels();

	makeAttachments();

	setSize(500, 140);
}

void TimingControls::paint(juce::Graphics& g)
{
	drawBorder(g);

	auto bounds = getLocalBounds();

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{ bounds.getX(), bounds.getY(), bounds.getWidth(), topBarHeight };
	g.setColour(juce::Colours::white);
	g.setOpacity(0.40f);
	g.fillRect(titleBounds);

	g.setColour(juce::Colours::black);
	g.drawFittedText("WAVE TIMING", titleBounds, juce::Justification::centred, 1);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom() - bottomBarHeight, bounds.getWidth(), bottomBarHeight };
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.25f);
	g.fillRect(labelBounds);

	// SET LABEL FONT
	g.setColour(juce::Colours::white);

	// SYNC LABEL ===========================================================
	juce::Rectangle<int> syncLabelBounds {	timingBarHigh.toggleSync.getX(),
											labelBounds.getY(),
											timingBarHigh.toggleSync.getWidth(),
											labelBounds.getHeight()};

	g.drawFittedText("SYNC", syncLabelBounds, juce::Justification::centred, 1);

	// MUTIPLIER LABEL ===========================================================
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
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) - 25);
	
	// LABEL
	g.drawFittedText("RHYTHM", multLabelBounds, juce::Justification::centred, 1);

	// HORIZONTAL LINE
		// HORIZONTAL LINE
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) + 25,
							timingBarLow.sliderMultuplier.getRight());

	// VERTICAL LINE
	g.drawVerticalLine(	timingBarLow.sliderMultuplier.getRight(),
						labelBounds.getY(),
						labelBounds.getCentreY());


	// RATE LABEL ===========================================================
	juce::Rectangle<int> rateLabelBounds{	timingBarHigh.sliderRate.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderRate.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("RATE", rateLabelBounds, juce::Justification::centred, 1);

	// PHASE LABEL ===========================================================
	juce::Rectangle<int> phaseLabelBounds{	timingBarHigh.sliderPhase.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderPhase.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("PHASE", phaseLabelBounds, juce::Justification::centred, 1);
}

void TimingControls::drawBorder(juce::Graphics& g)
{
	using namespace AllColors::TimingControlsColors;

	auto border = getLocalBounds();

	g.setGradientFill(BACKGROUND_GRADIENT(border.toFloat()));
	g.fillRoundedRectangle(border.toFloat(), 5.f);

	g.setColour(BORDER_OUTLINE_COLOR());
	g.drawRoundedRectangle(border.toFloat(), 5.f, BORDER_OUTLINE_THICKNESS());
}

void TimingControls::resized()
{
	drawLabels();

	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(topBarHeight);

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

	// MULTIPLIER ================================

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

	// RELATIVE PHASE ================================

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