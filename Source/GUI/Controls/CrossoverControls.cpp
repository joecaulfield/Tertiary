/*
  ==============================================================================

    CrossoverControls.cpp
    Created: 1 Jan 2022 9:50:07pm
    Author:  Joe

  ==============================================================================
*/

#include "CrossoverControls.h"

CrossoverControls::CrossoverControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{
	//drawLabels();
	makeAttachments();

	sliderLowMidCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	sliderLowMidCutoff.setLookAndFeel(&crossoverLookAndFeel);
	sliderLowMidCutoff.addListener(this);

	sliderMidHighCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	sliderMidHighCutoff.setLookAndFeel(&crossoverLookAndFeel);
	sliderMidHighCutoff.addListener(this);

	addAndMakeVisible(sliderLowMidCutoff);
	addAndMakeVisible(sliderMidHighCutoff);

	sliderValueLM.setEditable(false, true);
	sliderValueLM.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
	sliderValueLM.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
	sliderValueLM.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::darkblue);
	sliderValueLM.setJustificationType(juce::Justification::centred);
	sliderValueLM.addListener(this);
	addAndMakeVisible(sliderValueLM);

	sliderValueMH.setEditable(false, true);
	sliderValueMH.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white);
	sliderValueMH.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
	sliderValueMH.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::darkblue);
	sliderValueMH.setJustificationType(juce::Justification::centred);
	sliderValueMH.addListener(this);
	addAndMakeVisible(sliderValueMH);

	startTimerHz(60);

	updateStringText();

	setSize(200, 140);
}

CrossoverControls::~CrossoverControls()
{
	sliderLowMidCutoff.setLookAndFeel(nullptr);
	sliderMidHighCutoff.setLookAndFeel(nullptr);
}

void CrossoverControls::paint(juce::Graphics& g)
{
	drawBorder(g);

	auto bounds = getLocalBounds();

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{ bounds.getX(), bounds.getY(), bounds.getWidth(), topBarHeight };
	g.setColour(juce::Colours::white);
	g.setOpacity(0.40f);
	g.fillRect(titleBounds);

	g.setColour(juce::Colours::black);
	g.drawFittedText("CROSSOVER", titleBounds, juce::Justification::centred, 1);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{ bounds.getX(), bounds.getBottom() - bottomBarHeight, bounds.getWidth(), bottomBarHeight };
	g.setColour(juce::Colours::darkgrey);
	g.setOpacity(0.25f);
	g.fillRect(labelBounds);

	// SET LABEL FONT
	g.setColour(juce::Colours::white);

	// LOW-MID LABEL ===========================================================
	juce::Rectangle<int> lmLabelBounds{	sliderLowMidCutoff.getX(),
										labelBounds.getY(),
										sliderLowMidCutoff.getWidth(),
										labelBounds.getHeight() };

	g.drawFittedText("LOW-MID", lmLabelBounds, juce::Justification::centred, 1);

	// LOW-MID LABEL ===========================================================
	juce::Rectangle<int> mhLabelBounds{	sliderMidHighCutoff.getX(),
										labelBounds.getY(),
										sliderMidHighCutoff.getWidth(),
										labelBounds.getHeight() };

	g.drawFittedText("MID-HIGH", mhLabelBounds, juce::Justification::centred, 1);
}

void CrossoverControls::drawBorder(juce::Graphics& g)
{
	using namespace AllColors::CrossoverControlsColors;

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

void CrossoverControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	bounds.removeFromTop(topBarHeight);
	bounds.removeFromBottom(bottomBarHeight);

	auto spacerV = FlexItem().withHeight(3);

	// COLUMN 1 ======================================================
	FlexBox column1;
	column1.flexDirection = FlexBox::Direction::column;
	column1.flexWrap = FlexBox::Wrap::noWrap;

	//column1.items.add(FlexItem().withHeight(1));
	column1.items.add(FlexItem(sliderLowMidCutoff).withFlex(1.f));
	column1.items.add(FlexItem(sliderValueLM).withHeight(20));
	column1.items.add(FlexItem().withHeight(5));

	// COLUMN 2 =======================================================
	FlexBox column2;
	column2.flexDirection = FlexBox::Direction::column;
	column2.flexWrap = FlexBox::Wrap::noWrap;

	column2.items.add(spacerV);
	column2.items.add(FlexItem(sliderMidHighCutoff).withFlex(1.f));
	column2.items.add(FlexItem(sliderValueMH).withHeight(20));
	column2.items.add(spacerV);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacerH = FlexItem().withWidth(10);

	flexBox.items.add(FlexItem(column1).withFlex(1.f));
	flexBox.items.add(FlexItem(column2).withFlex(1.f));

	flexBox.performLayout(bounds);

	sliderValueLM.setBounds(sliderValueLM.getBounds().reduced(15, 0));
	sliderValueMH.setBounds(sliderValueMH.getBounds().reduced(15, 0));

}

void CrossoverControls::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	lowMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Low_Mid_Crossover_Freq),
						sliderLowMidCutoff);

	midHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Mid_High_Crossover_Freq),
						sliderMidHighCutoff);
	}

void CrossoverControls::sliderValueChanged(juce::Slider* slider)
{
	// Prevent Overlapping of Cutoff Frequencies

	if (slider == &sliderLowMidCutoff)
	{
		if (slider->getValue() > sliderMidHighCutoff.getValue())
			sliderMidHighCutoff.setValue(slider->getValue());
	}

	if (slider == &sliderMidHighCutoff)
	{
		if (slider->getValue() < sliderLowMidCutoff.getValue())
			sliderLowMidCutoff.setValue(slider->getValue());
	}

	updateStringText();
}

void CrossoverControls::updateStringText()
{
	juce::String stringLM;
	juce::String stringMH;

	// Abbreviate Values In The Kilohertz
	if (sliderLowMidCutoff.getValue() <= 999.f)
		stringLM = (juce::String)sliderLowMidCutoff.getValue() << " Hz";
	else
	{
		auto num = sliderLowMidCutoff.getValue();
		num /= 10.f;
		num = juce::roundFloatToInt(num);
		num /= 100.f;
		stringLM = (juce::String)(num) << " kHz";
	}

	
	if (sliderMidHighCutoff.getValue() <= 999.f)
		stringMH = (juce::String)sliderMidHighCutoff.getValue() << " Hz";
	else
	{
		auto num = sliderMidHighCutoff.getValue();
		num /= 10.f;
		num = juce::roundFloatToInt(num);
		num /= 100.f;
		stringMH = (juce::String)(num) << " kHz";
	}


	sliderValueLM.setText(stringLM, juce::NotificationType::dontSendNotification);
	sliderValueMH.setText(stringMH, juce::NotificationType::dontSendNotification);
}

void CrossoverControls::timerCallback()
{
	lowMidFocus = 0;	midHighFocus = 0;

	if (sliderLowMidCutoff.isMouseOverOrDragging())
	{
		lowMidFocus = 1;	midHighFocus = 0;
	}

	if (sliderMidHighCutoff.isMouseOverOrDragging())
	{
		lowMidFocus = 0;	midHighFocus = 1;
	}
}

void CrossoverControls::labelTextChanged(juce::Label* labelThatHasChanged)
{
	juce::String original = labelThatHasChanged->getText();

	juce::String entryString = labelThatHasChanged->getText();
	float entryFloat{ 0.f };

	// Simplify Text
	entryString = entryString.toLowerCase();			// Lower Case
	entryString = entryString.removeCharacters(" ");	// Remove Spaces
	entryString = entryString.removeCharacters("hz");	// Remove Units

	// Check for Non-Numerical Text ==================================================
	bool containsText = false;
	bool containsNum = false;

	for (int i = 0; i < entryString.length(); i++)
	{
		if ((int)entryString[i] < 48 || (int)entryString[i] > 57)
			containsText = true;

		if ((int)entryString[i] >= 48 && (int)entryString[i] <= 57)
			containsNum = true;
	}

	if (!containsText) // Handle entry as pure numerical ==================================================
		entryFloat = entryString.getFloatValue();
	else // Handle entry as alphanumeric combination ==================================================
	{
		if (entryString.containsChar('k'))
		{
			entryString.removeCharacters("k");
			entryFloat = entryString.getFloatValue() * 1000.f;
		}
		else // Unaccepted entry, truncate the decimal
			entryFloat = juce::roundToInt(entryString.getFloatValue());
	}

	if (labelThatHasChanged == &sliderValueLM)
	{
		if (!containsNum)
		{
			labelThatHasChanged->setText("20 Hz", juce::NotificationType::sendNotification);
			sliderLowMidCutoff.setValue(20.f);
		}
		else
			sliderLowMidCutoff.setValue(entryFloat);
	}


	if (labelThatHasChanged == &sliderValueMH)
	{
		if (!containsNum)
		{
			labelThatHasChanged->setText("20 kHz", juce::NotificationType::dontSendNotification);
			sliderMidHighCutoff.setValue(20000);
		}
		else
			sliderMidHighCutoff.setValue(entryFloat);
	}


	// If It Only Contains Letters ===========================


}