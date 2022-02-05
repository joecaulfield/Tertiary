/*
  ==============================================================================

    GainControls.cpp
    Created: 1 Jan 2022 9:49:55pm
    Author:  Joe

  ==============================================================================
*/

#include "GainControls.h"

GainControls::GainControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{

	gainBarLow.setMode("low");
	gainBarMid.setMode("mid");
	gainBarHigh.setMode("high");

	addAndMakeVisible(gainBarLow);
	addAndMakeVisible(gainBarMid);
	addAndMakeVisible(gainBarHigh);

	drawLabels();

	makeAttachments();

	setSize(200, 140);
}

void GainControls::paint(juce::Graphics& g)
{
	drawBorder(g);

	auto bounds = getLocalBounds();

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{ bounds.getX(), bounds.getY(), bounds.getWidth(), topBarHeight };
	g.setColour(juce::Colours::white);
	g.setOpacity(0.40f);
	g.fillRect(titleBounds);

	g.setColour(juce::Colours::black);
	g.drawFittedText("GAIN CONTROL", titleBounds, juce::Justification::centred, 1);


	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom()- bottomBarHeight, bounds.getWidth(), bottomBarHeight };
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.25f);
	g.fillRect(labelBounds);

	// SET LABEL FONT
	g.setColour(juce::Colours::white);

	// GAIN LABEL ===================================================
	juce::Rectangle<int> gainLabelBounds{	gainBarHigh.sliderGain.getX(), 
											labelBounds.getY(), 
											gainBarHigh.sliderGain.getWidth(),
											labelBounds.getHeight()};

	g.drawFittedText("BAND GAIN", gainLabelBounds, juce::Justification::centred, 1);

	// BYPASS LABEL ===================================================
	juce::Rectangle<int> bypLabelBounds{	gainBarHigh.toggleBypass.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleBypass.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("B", bypLabelBounds, juce::Justification::centred, 1);

	// MUTE LABEL ===================================================
	juce::Rectangle<int> muteLabelBounds{	gainBarHigh.toggleMute.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleMute.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("M", muteLabelBounds, juce::Justification::centred, 1);

	// SOLO LABEL ===================================================
	juce::Rectangle<int> soloLabelBounds{	gainBarHigh.toggleSolo.getX(),
											labelBounds.getY(),
											gainBarHigh.toggleSolo.getWidth(),
											labelBounds.getHeight() };

	g.drawFittedText("S", soloLabelBounds, juce::Justification::centred, 1);
}

void GainControls::drawBorder(juce::Graphics& g)
{
	using namespace AllColors::GainControlsColors;

	auto border = getLocalBounds();

	g.setGradientFill(BACKGROUND_GRADIENT(border.toFloat()));
	g.fillRoundedRectangle(border.toFloat(), 5.f);

	g.setColour(BORDER_OUTLINE_COLOR());
	g.drawRoundedRectangle(border.toFloat(), 5.f, BORDER_OUTLINE_THICKNESS());

	//// DRAW TITLE BACKGROUND ====================
	//juce::Rectangle<float> titleBounds;
	//titleBounds.setBounds(labelTitle.getX(), labelTitle.getY(), labelTitle.getWidth(), labelTitle.getHeight());

	//g.setColour(TEXT_BACKGROUND_FILL());
	//g.fillRoundedRectangle(titleBounds, 5.f);

	//g.setColour(BORDER_OUTLINE_COLOR());
	//g.drawRoundedRectangle(titleBounds, 5.f, BORDER_OUTLINE_THICKNESS());
}

void GainControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	//labelTitle.setSize(bounds.getWidth(), 25);
	//labelTitle.setCentrePosition(bounds.getCentreX(), bounds.getY() + labelTitle.getHeight() / 2);

	bounds.removeFromTop(topBarHeight);

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

	// ALIGN THE LABELS
	//labelGain.setCentrePosition(	gainBarLow.getX() + gainBarLow.sliderGain.getX() + gainBarLow.sliderGain.getWidth()/2,
	//								gainBarHigh.getBottom() + labelGain.getHeight()/2);

	//labelBypass.setCentrePosition(	gainBarLow.getX() + gainBarLow.toggleBypass.getX() + gainBarLow.toggleBypass.getWidth()/2,
	//								labelGain.getY() + labelGain.getHeight() /2);

	//labelSolo.setCentrePosition(	gainBarLow.getX() + gainBarLow.toggleSolo.getX() + gainBarLow.toggleSolo.getWidth()/2,
	//								labelGain.getY() + labelGain.getHeight() /2);

	//labelMute.setCentrePosition(	gainBarLow.getX() + gainBarLow.toggleMute.getX() + gainBarLow.toggleMute.getWidth()/2,
	//								labelGain.getY() + labelGain.getHeight() /2);
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

void GainControls::drawLabels()
{
	//gainBarLow.bandLabel.setText("LOW", juce::NotificationType::dontSendNotification);
	//waveBarMid.bandLabel.setText("MID", juce::NotificationType::dontSendNotification);
	//waveBarHigh.bandLabel.setText("HIGH", juce::NotificationType::dontSendNotification);

	//auto controlsFont = juce::Font(12.f);
	//auto labelWidth = 100;
	//auto labelHeight = 20;

	//labelGain.setJustificationType(juce::Justification::centred);
	//labelGain.setFont(controlsFont);
	//labelGain.setColour(juce::Label::textColourId, juce::Colours::black);
	//labelGain.setText("BAND GAIN", juce::NotificationType::dontSendNotification);
	//labelGain.setSize(labelWidth, labelHeight);
	//addAndMakeVisible(labelGain);

	//labelBypass.setJustificationType(juce::Justification::centred);
	//labelBypass.setFont(controlsFont);
	//labelBypass.setColour(juce::Label::textColourId, juce::Colours::black);
	//labelBypass.setText("B", juce::NotificationType::dontSendNotification);
	//labelBypass.setSize(labelWidth, labelHeight);
	//addAndMakeVisible(labelBypass);

	//labelSolo.setJustificationType(juce::Justification::centred);
	//labelSolo.setFont(controlsFont);
	//labelSolo.setColour(juce::Label::textColourId, juce::Colours::black);
	//labelSolo.setText("S", juce::NotificationType::dontSendNotification);
	//labelSolo.setSize(labelWidth, labelHeight);
	//addAndMakeVisible(labelSolo);

	//labelMute.setJustificationType(juce::Justification::centred);
	//labelMute.setFont(controlsFont);
	//labelMute.setColour(juce::Label::textColourId, juce::Colours::black);
	//labelMute.setText("M", juce::NotificationType::dontSendNotification);
	//labelMute.setSize(labelWidth, labelHeight);
	//addAndMakeVisible(labelMute);

	//// TITLE
	//labelTitle.setJustificationType(juce::Justification::centred);
	//labelTitle.setFont(juce::Font(22.f, juce::Font::bold));
	//labelTitle.setColour(juce::Label::textColourId, juce::Colours::black);
	//labelTitle.setText("GAIN CONTROLS", juce::NotificationType::dontSendNotification);
	//addAndMakeVisible(labelTitle);
}