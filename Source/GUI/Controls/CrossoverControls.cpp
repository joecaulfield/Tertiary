/*
  ==============================================================================

    CrossoverControls.cpp
    Created: 1 Jan 2022 9:50:07pm
    Author:  Joe

	CLASS CONTAINING QUADRANT OF CONTROLS WHICH ADJUST CROSSOVER PARAMETERS

  ==============================================================================
*/

#include "CrossoverControls.h"

CrossoverControls::CrossoverControls(juce::AudioProcessorValueTreeState& apv)
	: apvts(apv)
{
	makeAttachments();

	// Initialize Low-Mid Cutoff Slider =======
	sliderLowMidCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	sliderLowMidCutoff.setLookAndFeel(&crossoverLookAndFeel);
	sliderLowMidCutoff.addListener(this);
	addAndMakeVisible(sliderLowMidCutoff);

	// Initialize Mid-High Cutoff Slider =======
	sliderMidHighCutoff.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	sliderMidHighCutoff.setLookAndFeel(&crossoverLookAndFeel);
	sliderMidHighCutoff.addListener(this);
	addAndMakeVisible(sliderMidHighCutoff);

	// Initialize Low-Mid Cutoff Label =======
	sliderValueLM.setEditable(false, true);
	sliderValueLM.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	sliderValueLM.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
	sliderValueLM.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::darkblue);
	sliderValueLM.setJustificationType(juce::Justification::centred);
	sliderValueLM.addListener(this);
	addAndMakeVisible(sliderValueLM);

	// Initialize Mid-High Cutoff Label =======
	sliderValueMH.setEditable(false, true);
	sliderValueMH.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	sliderValueMH.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
	sliderValueMH.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::darkblue);
	sliderValueMH.setJustificationType(juce::Justification::centred);
	sliderValueMH.addListener(this);
	addAndMakeVisible(sliderValueMH);

	startTimerHz(60);

	updateStringText();

	setSize(200, 140);
	imageCrossover = juce::ImageCache::getFromMemory(BinaryData::TitleCrossover_png, BinaryData::TitleCrossover_pngSize);

}

CrossoverControls::~CrossoverControls()
{
	sliderLowMidCutoff.setLookAndFeel(nullptr);
	sliderMidHighCutoff.setLookAndFeel(nullptr);
}

void CrossoverControls::paint(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds();

	g.drawImage(background, bounds.toFloat());
}

void CrossoverControls::resized()
{
	using namespace juce;

	auto bounds = getLocalBounds();

	// Margins For Labels
	int topBarHeight = 25;
	int bottomBarHeight = 20;

	// Bounds In Which FlexBox is Performed
	auto flexBounds = getLocalBounds();
	flexBounds.removeFromTop(topBarHeight);
	flexBounds.removeFromBottom(bottomBarHeight);

	auto spacerV = FlexItem().withFlex(1.f);
	auto sliderHeight = 83.f;
	auto labelHeight = 15.f;

	// COLUMN 1 ======================================================
	FlexBox column1;
	column1.flexDirection = FlexBox::Direction::column;
	column1.flexWrap = FlexBox::Wrap::noWrap;

	column1.items.add(FlexItem(sliderLowMidCutoff).withHeight(sliderHeight));
	column1.items.add(FlexItem(sliderValueLM).withHeight(labelHeight));

	// COLUMN 2 =======================================================
	FlexBox column2;
	column2.flexDirection = FlexBox::Direction::column;
	column2.flexWrap = FlexBox::Wrap::noWrap;

	column2.items.add(FlexItem(sliderMidHighCutoff).withHeight(sliderHeight));
	column2.items.add(FlexItem(sliderValueMH).withHeight(labelHeight));

	// ROW CONTAINING COLUMN 1 AND COLUMN 2 ===========================
	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::row;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	flexBox.items.add(FlexItem(column1).withFlex(1.f));
	flexBox.items.add(FlexItem(column2).withFlex(1.f));

	flexBox.performLayout(flexBounds);

	// Trim the Width of Slider Value Labels
	sliderValueLM.setBounds(sliderValueLM.getBounds().reduced(15, 0));
	sliderValueMH.setBounds(sliderValueMH.getBounds().reduced(15, 0));

	drawBackgroundImage(bounds);
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
	// Means of handling invalid input entries,
	// while allowing some flexibility and auto-interpretation of user-entered values

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

	if (!containsText) // Handle entry as pure numerical
		entryFloat = entryString.getFloatValue();
	else // Handle entry as alphanumeric combination
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

}

void CrossoverControls::drawBackgroundImage(juce::Rectangle<int> bounds)
{
	using namespace juce;

	background = Image(Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	Graphics g(background);

	// Draw the Title Bounds
	juce::Rectangle<int> titleBounds{	bounds.getX(), 
										bounds.getY(), 
										bounds.getWidth(), 
										27};

	// Draw the Title

	g.drawImage(imageCrossover, titleBounds.toFloat());
	g.setColour(juce::Colours::white);
	g.setOpacity(0.5f);

	auto myTypeface = "Helvetica";
	auto titleFont = juce::Font(	myTypeface, 
									titleBounds.getHeight() * 0.85f, 
									juce::Font::FontStyleFlags::bold);

	g.setFont(titleFont);

	// Draw the Label Bounds
	juce::Rectangle<int> labelBounds{	bounds.getX(), 
										sliderValueLM.getBottom() + 8,
										bounds.getWidth(), 
										bounds.getBottom() - sliderValueLM.getBottom() - 6};

	// Set Font Height For Sub-Labels
	g.setFont(16);

	// Draw Parameter Labels: Low-Mid Label ============================
	juce::Rectangle<int> lmLabelBounds{	sliderLowMidCutoff.getX(),
										labelBounds.getY(),
										sliderLowMidCutoff.getWidth(),
										labelBounds.getHeight() };

	g.drawFittedText("LOW-MID", lmLabelBounds, juce::Justification::centred, 1);

	// Draw Parameter Labels: Mid-High Label ============================
	juce::Rectangle<int> mhLabelBounds{	sliderMidHighCutoff.getX(),
										labelBounds.getY(),
										sliderMidHighCutoff.getWidth(),
										labelBounds.getHeight() };

	g.drawFittedText("MID-HIGH", mhLabelBounds, juce::Justification::centred, 1);

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

	auto center = (titleBounds.getBottom() + sliderLowMidCutoff.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

	center = (sliderValueLM.getBottom() + labelBounds.getY()) / 2.f;
	g.drawHorizontalLine(center, bounds.getX(), bounds.getRight());

}
