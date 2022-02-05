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

	addAndMakeVisible(globalControls);		// Make GC Visible
	addAndMakeVisible(frequencyResponse);	// Make FR Visible
	addAndMakeVisible(oscilloscope);		// Make O-Scope Visible

	addMouseListener(this, true);
    setSize (800, 580);
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

	bounds.removeFromTop(35);			// Title Area
	bounds.removeFromLeft(5);			// Left Padding
	bounds.removeFromRight(5);			// Right Padding

	// Flexbox to Wrap O-Scope and FrequencyResponse
	juce::FlexBox flexBox;	
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(5);	// Gap between O-Scope and Freq Resp
	 
	flexBox.items.add(FlexItem(oscilloscope).withFlex(1.f));		// Insert O-Scope
	flexBox.items.add(spacer);										// Insert Gap
	flexBox.items.add(FlexItem(frequencyResponse).withFlex(1.f));	// Insert Freq Resp
	flexBox.performLayout(bounds.removeFromTop(getHeight() * 0.4));	// Do the Layout

	bounds.removeFromTop(5);			// Gap
	bounds.removeFromBottom(5);			// Bottom Padding

	globalControls.setBounds(bounds);	// Set Boundaries for the Global Controls Panel
}