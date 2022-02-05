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
}

void WaveControls::paint(juce::Graphics& g)
{
	drawBorder(g);

	auto bounds = getLocalBounds();

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{ bounds.getX(), bounds.getY(), bounds.getWidth(), topBarHeight };
	g.setColour(juce::Colours::white);
	g.setOpacity(0.40f);
	g.fillRect(titleBounds);

	// Draw the Title
	g.setColour(juce::Colours::black);
	g.drawFittedText("WAVE SHAPE", titleBounds, juce::Justification::centred, 1);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom() - bottomBarHeight, bounds.getWidth(), bottomBarHeight };
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.25f);
	g.fillRect(labelBounds);

	g.setColour(juce::Colours::white);

	// Draw Parameter Labels: Invert ======================
	juce::Rectangle<int> invertLabelBounds{ waveBarHigh.toggleInvert.getX(),
												labelBounds.getY(),
												waveBarHigh.toggleInvert.getWidth(),
												labelBounds.getHeight() };

	g.drawFittedText("INV", invertLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Waveshape ======================
	juce::Rectangle<int> waveLabelBounds {	waveBarHigh.sliderWave.getX(),
											labelBounds.getY(),
											waveBarHigh.sliderWave.getWidth(),
											labelBounds.getHeight()};
	// VERTICAL LINE
	g.drawVerticalLine(	waveBarLow.sliderWave.getX(),
						labelBounds.getY(),
						labelBounds.getCentreY());

	// HORIZONTAL LINE
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							waveBarLow.sliderWave.getX(),
							waveBarLow.sliderWave.getX() + (waveBarLow.sliderWave.getWidth() / 2.f) - 25);
	
	// LABEL
	g.drawFittedText("SHAPE", waveLabelBounds, juce::Justification::centred, 1);

	// HORIZONTAL LINE
	g.drawHorizontalLine(	labelBounds.getCentreY(),
							waveBarLow.sliderWave.getX() + (waveBarLow.sliderWave.getWidth() / 2.f) + 25,
							waveBarLow.sliderWave.getRight());

	// VERTICAL LINE
	g.drawVerticalLine(	waveBarLow.sliderWave.getRight(),
						labelBounds.getY(),
						labelBounds.getCentreY());

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
					
}

void WaveControls::drawBorder(juce::Graphics& g)
{
	using namespace AllColors::WaveControlsColors;

	auto border = getLocalBounds();

	g.setGradientFill(BACKGROUND_GRADIENT(border.toFloat()));
	g.setOpacity(BACKGROUND_OPACITY());
	g.fillRoundedRectangle(border.toFloat(), 5.f);

	g.setColour(BORDER_OUTLINE_COLOR());
	g.drawRoundedRectangle(border.toFloat(), 5.f, BORDER_OUTLINE_THICKNESS());
}

void WaveControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(topBarHeight);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withHeight(2);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(waveBarLow).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(waveBarMid).withHeight(30));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(waveBarHigh).withHeight(30));
	flexBox.items.add(spacer);

	flexBox.performLayout(bounds);
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
