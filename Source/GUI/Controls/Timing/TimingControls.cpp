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
	imageWaveTiming = juce::ImageCache::getFromMemory(BinaryData::TitleWaveTiming_png, BinaryData::TitleWaveTiming_pngSize);

}

void TimingControls::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();

	g.drawImage(background, bounds.toFloat());
}

void TimingControls::drawBackgroundImage(juce::Rectangle<int> bounds)
{
	using namespace juce;

	background = Image(Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	Graphics g(background);

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{	bounds.getX(), 
										bounds.getY(),
										bounds.getWidth(), 
										25};

	// Draw the Title

	g.drawImage(imageWaveTiming, titleBounds.toFloat());
	g.setColour(juce::Colours::white);
	g.setOpacity(0.5f);

	auto myTypeface = "Helvetica";
	auto titleFont = juce::Font(	myTypeface, 
									titleBounds.getHeight() * 0.85f, 
									juce::Font::FontStyleFlags::bold);

	g.setFont(titleFont);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{	bounds.getX(), 
										timingBarHigh.getBottom() + 3, 
										bounds.getWidth(), 
										bounds.getBottom() - timingBarHigh.getBottom()};

	// Set Font Height For Sub-Labels
	g.setFont(16);

	//g.setColour(juce::Colours::white);

	// Draw Parameter Labels: Sync ======================
	juce::Rectangle<int> syncLabelBounds {	timingBarHigh.toggleSync.getX()-3,
											labelBounds.getY(),
											timingBarHigh.toggleSync.getWidth()+6,
											labelBounds.getHeight()};


	g.drawFittedText("SYNC", syncLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Multiplier AKA Rhythm ======================
	juce::Rectangle<int> multLabelBounds{	timingBarHigh.sliderMultuplier.getX(),
											labelBounds.getY(),
											timingBarHigh.sliderMultuplier.getWidth(),
											labelBounds.getHeight() };

	// LABEL
	g.drawFittedText("RHYTHM", multLabelBounds, juce::Justification::centred, 1);

	// HORIZONTAL LINES
	g.setOpacity(0.25f);

	g.drawHorizontalLine(	labelBounds.getCentreY(),
							timingBarLow.sliderMultuplier.getX(),
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) - 40);

	g.drawHorizontalLine(	labelBounds.getCentreY(),
							timingBarLow.sliderMultuplier.getX() + (timingBarLow.sliderMultuplier.getWidth() / 2.f) + 40,
							timingBarLow.sliderMultuplier.getRight());

	g.setOpacity(0.5f);

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

	// Setup Gradient For Division Lines ======================
	float p1 = 0.2f;
	float p2 = 0.8f;

	auto gradient = ColourGradient(	juce::Colours::black,
									bounds.getBottomLeft().toFloat(),
									juce::Colours::black,
									bounds.getBottomRight().toFloat(), false);

	gradient.addColour(p1, juce::Colours::white);
	gradient.addColour(p2, juce::Colours::white);

	g.setGradientFill(gradient);
	g.setOpacity(0.25f);

	// Draw Division Lines ======================

	auto center = (titleBounds.getBottom() + timingBarLow.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (timingBarLow.getBottom() + timingBarMid.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (timingBarMid.getBottom() + timingBarHigh.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (timingBarHigh.getBottom() + labelBounds.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());
}

void TimingControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	// Margins For Labels
	int topBarHeight = 25;
	int bottomBarHeight = 20;

	// Bounds In Which FlexBox is Performed
	auto flexBounds = bounds;
	flexBounds.removeFromTop(topBarHeight);
	flexBounds.removeFromBottom(bottomBarHeight);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withFlex(1.f);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(timingBarLow).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(timingBarMid).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(timingBarHigh).withHeight(30));
	flexBox.items.add(spacer);

	flexBox.performLayout(flexBounds);

	// Draw Background Image, Control Labels
	drawBackgroundImage(bounds);
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