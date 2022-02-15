/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TertiaryAudioProcessorEditor::TertiaryAudioProcessorEditor (TertiaryAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	addAndMakeVisible(globalControls);		// Container Class For All Parameter Controls
	addAndMakeVisible(frequencyResponse);	// Frequency-Domain Response Display
	addAndMakeVisible(oscilloscope);		// Time-Domain LFO Display

	addMouseListener(this, true);
    //setSize (800, 580);
	setSize(800, 625);

	// TITLE LABEL =============================================================
	pluginTitle.setJustificationType(juce::Justification::centred);
	pluginTitle.setFont(juce::Font(24.f, juce::Font::bold));
	pluginTitle.setColour(juce::Label::textColourId, juce::Colours::white);
	pluginTitle.setText("TERTIARY v0.1", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(pluginTitle);
}

TertiaryAudioProcessorEditor::~TertiaryAudioProcessorEditor()
{
	
}

//==============================================================================
void TertiaryAudioProcessorEditor::paint (juce::Graphics& g)
{
	using namespace AllColors::PluginEditorColors;

	g.setGradientFill(BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
	g.fillAll();
}

void TertiaryAudioProcessorEditor::resized()
{

	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(50);	// Title Area
	bounds.removeFromLeft(5);	// Left Padding
	bounds.removeFromRight(5);	// Right Padding

	pluginTitle.setSize(250, 50);
	pluginTitle.setCentrePosition(bounds.getCentreX(), bounds.getY() / 2.f);

	// Flexbox to Wrap Oscilloscope and FrequencyResponse
	juce::FlexBox flexBox;	
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(5);	// Gap between O-Scope and Freq Resp
	 
	flexBox.items.add(FlexItem(oscilloscope).withFlex(1.f));		// Insert O-Scope
	flexBox.items.add(spacer);										// Insert Spacer
	flexBox.items.add(FlexItem(frequencyResponse).withFlex(1.f));	// Insert Freq Response
	flexBox.performLayout(bounds.removeFromTop(getHeight() * 0.4));	// Do The Layout

	bounds.removeFromTop(5);			// Gap
	bounds.removeFromBottom(5);			// Bottom Padding

	globalControls.setBounds(bounds);	// Set Boundaries for the Global Controls Panel
}