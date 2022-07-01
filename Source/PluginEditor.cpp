/*
	The Plugin Editor is the top-level component which contains all child classes related to the UI.

	Oscilloscope: Top-Left; Child Class representing the time-domain response of LFO parameters
	Frequency Response: Top-Right; Child Class representing the crossover parameters and displays a FFT
	Global Controls: Lower-Half; Child Class containing all user parameters.

	Plugin Editor also contains company title, plug-in name, version information.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TertiaryAudioProcessorEditor::TertiaryAudioProcessorEditor (TertiaryAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	/* Provides GPU acceleration */
	openGLContext.attachTo(*getTopLevelComponent());

	/* Container class for all parameter controls */
	addAndMakeVisible(globalControls);

	/* Frequency-Domain & Crossover Display */
	addAndMakeVisible(frequencyResponse);

	/* Time-Domain Display */
	addAndMakeVisible(oscilloscope);

	/* Registers this class as a listener for mouse-related callbacks */
	//addMouseListener(this, true); // Do i need this?

	/* Sets window size */
	setSize(800, 625);

	/* Graphics Asset, Top-Center Plugin-Name "Tertiary" */
	imageTitleHeader = juce::ImageCache::getFromMemory(BinaryData::TitleHeader_png, BinaryData::TitleHeader_pngSize);

	/* Format Company & Plug-in information */
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
	/* Set the Gradient */
	g.setGradientFill(AllColors::PluginEditorColors::BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
	g.fillAll();

	/* Place the top-banner plug-in title "Tertiary" */
	juce::Rectangle<float> titleBounds{ 0, 0, 800, 50 };
	g.drawImage(imageTitleHeader, titleBounds);
}

void TertiaryAudioProcessorEditor::resized()
{

	using namespace juce;

	auto bounds = getLocalBounds();

	/* Padding at top for title area */
	bounds.removeFromTop(50);

	/* Left & Right Window Margin */
	bounds.removeFromLeft(5);
	bounds.removeFromRight(5);

	/* Position the company & plug-in info in top-right corner */
	companyTitle.setSize(250, 50);
	companyTitle.setTopRightPosition(bounds.getRight(), 0);

	/* Create a horizontal FlexBox container to arrange the Oscilloscope and Frequency Response */
	juce::FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withWidth(5);	// Gap between O-Scope and Freq Resp

	flexBox.items.add(FlexItem(oscilloscope).withFlex(1.f));		// Insert O-Scope
	flexBox.items.add(spacer);										// Insert Spacer
	flexBox.items.add(FlexItem(frequencyResponse).withFlex(1.f));	// Insert Freq Response
	flexBox.performLayout(bounds.removeFromTop(getHeight() * 0.4));	// Do The Layout

	/* Set the boundaries for the remainder of the parameters */
	bounds.removeFromTop(5);
	bounds.removeFromBottom(5);
	globalControls.setBounds(bounds);
}