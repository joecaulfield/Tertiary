/*
  ==============================================================================

    InputOutputGain.cpp
    Created: 10 Jan 2022 8:19:16pm
    Author:  Joe

  ==============================================================================
*/

#include "InputOutputGain.h"

InputOutputGain::InputOutputGain(TertiaryAudioProcessor& p) :
	audioProcessor(p)
{
	setSize(40, 150);

	sliderGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderGain.setLookAndFeel(&inOutLookAndFeel);
	addAndMakeVisible(sliderGain);

	startTimerHz(60);
}

InputOutputGain::~InputOutputGain()
{
	sliderGain.setLookAndFeel(nullptr);
}

void InputOutputGain::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds().toFloat();
	bounds.reduce(4, 0);

	g.setColour(juce::Colours::black);
	g.fillRect(bounds);

	// DRAW THE LEFT METER
	g.setGradientFill(gradient);
	g.fillRect(leftBounds);

	// DRAW THE RIGHT METER
	g.setGradientFill(gradient);
	g.fillRect(rightBounds);

	// Draw Left & Right Grills
	juce::Rectangle<float> grillBounds = {	bounds.getX(), bounds.getY(),
											bounds.getWidth(), bounds.getHeight() };

	g.drawImage(grill, grillBounds);
					
}

void InputOutputGain::resized()
{
	auto bounds = getLocalBounds().toFloat();

	// Place The Slider ===========================================================
	sliderGain.setSize(getWidth() * 0.5f, getHeight() * 0.9f);
	sliderGain.setCentreRelative(0.5f, 0.5f);

	// Draw Level Gradient ========================================================
	gradient = juce::ColourGradient{juce::Colours::green, bounds.getBottomLeft(),
									 juce::Colours::red, bounds.getTopLeft(), false };

	gradient.addColour(0.6f, juce::Colours::yellow);

	// Draw The Grill Image =======================================================
	drawGrill(bounds);
}

void InputOutputGain::timerCallback()
{
	getLevels();

	repaint();
}

void InputOutputGain::getLevels()
{
	// Pull current audio-level values from processor

	if (pickOffPoint == "INPUT")
	{
		leftLevel = audioProcessor.getRmsValue(0, 0);
		rightLevel = audioProcessor.getRmsValue(0, 1);
	}

	if (pickOffPoint == "OUTPUT")
	{
		leftLevel = audioProcessor.getRmsValue(1, 0);
		rightLevel = audioProcessor.getRmsValue(1, 1);
	}

	if (pickOffPoint == "NA")
	{
		leftLevel = 0.f;
		rightLevel = 0.f;
	}

	auto bounds = getLocalBounds().toFloat();

	leftLevelPixel = bounds.getHeight() * juce::jmap(leftLevel, -60.f, 6.f, 0.f, 1.f);
	rightLevelPixel = bounds.getHeight() * juce::jmap(rightLevel, -60.f, 6.f, 0.f, 1.f);

	for (int i = ledThresholds.size()-1; i >= 0; i--)
	{
		if (leftLevelPixel < ledThresholds[i] && leftLevelPixel > ledThresholds[i - 1])
			leftLevelPixel = ledThresholds[i - 1];

		if (rightLevelPixel < ledThresholds[i] && rightLevelPixel > ledThresholds[i - 1])
			rightLevelPixel = ledThresholds[i - 1];

	}

	leftBounds.setBounds(	bounds.getX(), 
							bounds.getBottom() - leftLevelPixel, 
							bounds.getWidth() / 2, 
							leftLevelPixel);

	rightBounds.setBounds(	bounds.getX() + bounds.getWidth()/2.f, 
							bounds.getBottom() - rightLevelPixel,
							bounds.getWidth() / 2,
							rightLevelPixel);
}

void InputOutputGain::setPickOffPoint(juce::String pickoff)
{
	pickOffPoint = pickoff;
}

void InputOutputGain::drawGrill(juce::Rectangle<float> bounds)
{
	using namespace juce;
	using namespace AllColors::InputOutputMeterColors;

	ledThresholds.clear();

	grill = Image(Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	Graphics g(grill);

	g.setGradientFill(GRILL_GRADIENT(bounds));
	g.fillRect(bounds);

	for (int x = 0; x < 2; x++)	// Iterate Loop Twice for Left/Right Grills
	{

		float xPos;

		if (x == 0) xPos = -0.25f;	// Left-LED Positioning
		if (x == 1) xPos = 0.25f;	// Right-LED Positioning

		for (int y = 0; y < 2; y++)	// Iterate Loop Twice.  Begin LED's at center and move Up, Down
		{
			float dir;

			if (y == 0) dir = 1;  // Draw Downward
			if (y == 1) dir = -1; // Draw Upward

			for (int z = 0; z < bounds.getCentreY() - margin; z += spacing)
			{
				// Define LED bound to draw
				juce::Rectangle<float> ledArea = {  bounds.getCentreX() + bounds.getWidth() * xPos - ledWidth / 2.f,
													bounds.getCentreY() + dir * z - ledHeight / 2,
													ledWidth, ledHeight };

				// Store LED's Y value in an array, as to quantize (discretize) the meter levels later on
				ledThresholds.add(ledArea.getBottom());

				// Provide transparency in the image
				grill.clear(ledArea.toNearestInt(), juce::Colours::hotpink.withAlpha(0.f));

				// Draw bound around the grill
				g.setColour(juce::Colours::darkgrey);
				g.drawRect(ledArea, 1);

				// Draw bound around the grill
				ledArea.expand(1, 1);
				g.setColour(juce::Colours::grey);
				g.drawRect(ledArea, 1);
			}


		}
	}

	// Sort LED Y values in Ascending Fashion
	ledThresholds.sort();

	// Remove Duplicates
	for (int i = 0; i < ledThresholds.size(); i++)
	{
		if (ledThresholds[i] == ledThresholds[i + 1])
			ledThresholds.remove(i + 1);
	}
}