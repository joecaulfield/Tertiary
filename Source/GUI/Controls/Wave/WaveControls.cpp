/*
  ==============================================================================

    WaveControls.cpp
    Created: 1 Jan 2022 9:08:03pm
    Author:  Joe

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST INDIVIDUAL WAVE TIMING PARAMETERS

  ==============================================================================
*/

#include "WaveControls.h"

WaveControls::WaveControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{

	// Color-Coding and Defining Label-Text of Each Bar Class
	waveBarLow.setMode("low");
	waveBarMid.setMode("mid");
	waveBarHigh.setMode("high");

	addAndMakeVisible(waveBarLow);
	addAndMakeVisible(waveBarMid);
	addAndMakeVisible(waveBarHigh);

	makeAttachments();

	setSize(500, 140);

	imageWaveShape = juce::ImageCache::getFromMemory(BinaryData::TitleWaveShape_png, BinaryData::TitleWaveShape_pngSize);

}

void WaveControls::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds();

	g.drawImage(background, bounds.toFloat());



}

void WaveControls::drawBackgroundImage(juce::Rectangle<int> bounds)
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
	g.drawImage(imageWaveShape, titleBounds.toFloat());
	g.setColour(juce::Colours::white);
	g.setOpacity(0.5f);

	auto myTypeface = "Helvetica";
	auto titleFont = juce::Font(	myTypeface, 
									titleBounds.getHeight() * 0.85f, 
									juce::Font::FontStyleFlags::bold);

	g.setFont(titleFont);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{	bounds.getX(), 
										waveBarHigh.getBottom() + 3, 
										bounds.getWidth(), 
										bounds.getBottom() - waveBarHigh.getBottom()};

	// Set Font Height For Sub-Labels
	g.setFont(16);

	// Draw Parameter Labels: Invert ======================
	juce::Rectangle<int> invertLabelBounds{		waveBarHigh.toggleInvert.getX(),
												labelBounds.getY(),
												waveBarHigh.toggleInvert.getWidth(),
												labelBounds.getHeight() };

	g.drawFittedText("INV", invertLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Waveshape ======================
	juce::Rectangle<int> waveLabelBounds {	waveBarHigh.sliderWave.getX(),
											labelBounds.getY(),
											waveBarHigh.sliderWave.getWidth(),
											labelBounds.getHeight()};

	// LABEL
	g.drawFittedText("SHAPE", waveLabelBounds, juce::Justification::centred, 1);

	// HORIZONTAL LINES

	g.setOpacity(0.25f);

	g.drawHorizontalLine(	labelBounds.getCentreY(),
							waveBarLow.sliderWave.getX(),
							waveBarLow.sliderWave.getX() + (waveBarLow.sliderWave.getWidth() / 2.f) - 35);
	
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							waveBarLow.sliderWave.getX() + (waveBarLow.sliderWave.getWidth() / 2.f) + 35,
							waveBarLow.sliderWave.getRight());

	g.setOpacity(0.5f);

	// Draw Parameter Labels: Symmetry AKA Skew ======================
	juce::Rectangle<int> symLabelBounds{ waveBarHigh.sliderSymmetry.getX(),
											labelBounds.getY(),
											waveBarHigh.sliderSymmetry.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("SKEW", symLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Depth ======================
	juce::Rectangle<int> depthLabelBounds {	waveBarHigh.sliderDepth.getX(),
											labelBounds.getY(),
											waveBarHigh.sliderDepth.getWidth(),
											labelBounds.getHeight()};

	g.drawFittedText("DEPTH", depthLabelBounds, juce::Justification::centred, 1);

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

	auto center = (titleBounds.getBottom() + waveBarLow.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (waveBarLow.getBottom() + waveBarMid.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (waveBarMid.getBottom() + waveBarHigh.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (waveBarHigh.getBottom() + labelBounds.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());
}

void WaveControls::resized()
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
	flexBox.items.add(FlexItem(waveBarLow).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(waveBarMid).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(waveBarHigh).withHeight(30));
	flexBox.items.add(spacer);

	flexBox.performLayout(flexBounds);

	// Draw Background Image, Control Labels
	drawBackgroundImage(bounds);
}

void WaveControls::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	// WAVESHAPE ===================
	waveLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Wave_Low_LFO),
							waveBarLow.sliderWave);

	waveMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Wave_Mid_LFO),
							waveBarMid.sliderWave);

	waveHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Wave_High_LFO),
							waveBarHigh.sliderWave);

	// DEPTH ===================
	depthLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Depth_Low_LFO),
							waveBarLow.sliderDepth.slider);

	depthLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Depth_Low_LFO),
							waveBarLow.sliderDepth.slider);

	depthMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Depth_Mid_LFO),
							waveBarMid.sliderDepth.slider);

	depthHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Depth_High_LFO),
							waveBarHigh.sliderDepth.slider);

	// SYMMETRY AKA SKEW ===================
	symmetryLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Symmetry_Low_LFO),
							waveBarLow.sliderSymmetry.slider);

	symmetryMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Symmetry_Mid_LFO),
							waveBarMid.sliderSymmetry.slider);

	symmetryHighAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							apvts,
							params.at(Names::Symmetry_High_LFO),
							waveBarHigh.sliderSymmetry.slider);

	// INVERT ===================
	invertLowAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Invert_Low_LFO),
							waveBarLow.toggleInvert);

	invertMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Invert_Mid_LFO),
							waveBarMid.toggleInvert);

	invertHighAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							apvts,
							params.at(Names::Invert_High_LFO),
							waveBarHigh.toggleInvert);
}
