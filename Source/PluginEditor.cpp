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

	setSize(800, 625);

	imageTitleHeader = juce::ImageCache::getFromMemory(BinaryData::TitleHeader_png, BinaryData::TitleHeader_pngSize);

	// TITLE LABEL =============================================================

	companyTitle.setJustificationType(juce::Justification::centredRight);
	companyTitle.setFont(juce::Font(17.f, juce::Font::bold));
	companyTitle.setColour(juce::Label::textColourId, juce::Colours::white);
	companyTitle.setAlpha(0.5f);
	companyTitle.setText("Caulfield Engineering \n Version " + version, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(companyTitle);
}

TertiaryAudioProcessorEditor::~TertiaryAudioProcessorEditor()
{
	
}

//==============================================================================
void TertiaryAudioProcessorEditor::paint(juce::Graphics& g)
{
	using namespace AllColors::PluginEditorColors;

	g.setGradientFill(BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
	g.fillAll();

	juce::Rectangle<float> titleBounds{ 0, 0, 800, 50 };
	g.drawImage(imageTitleHeader, titleBounds);
}

void TertiaryAudioProcessorEditor::resized()
{

	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(50);	// Title Area
	bounds.removeFromLeft(5);	// Left Padding
	bounds.removeFromRight(5);	// Right Padding

	companyTitle.setSize(250, 50);
	companyTitle.setTopRightPosition(bounds.getRight(), 0);

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