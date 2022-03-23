/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 19 Jan 2022 4:55:59pm
    Author:  Joe

  ==============================================================================
*/

#include "MyLookAndFeel.h"

// CUSTOM SLIDER COMPONENT =======================================================
CustomSlider::CustomSlider()
{
	// SLIDER ======================================================
	slider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	slider.addListener(this);
	addAndMakeVisible(slider);

	// LABEL ======================================================
	label.setEditable(false, true);
	label.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::white.withAlpha(0.f));
	label.setColour(juce::Label::ColourIds::outlineWhenEditingColourId, juce::Colours::white.withAlpha(0.f));
	label.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::white.withAlpha(0.f));

	label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
	label.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::white);
	label.setColour(juce::Label::ColourIds::backgroundWhenEditingColourId, juce::Colours::purple);

	auto myTypeface = "Helvetica";
	auto labelFont = juce::Font(myTypeface, 16, juce::Font::FontStyleFlags::plain);

	label.setFont(labelFont);
	label.setJustificationType(juce::Justification::centred);

	label.addListener(this);
	addAndMakeVisible(label);
}

CustomSlider::~CustomSlider()
{
	slider.setLookAndFeel(nullptr);
}

void CustomSlider::resized()
{
	auto bounds = getLocalBounds();

	label.setBounds(bounds.getX(), 
					bounds.getY()+1, 
					bounds.getWidth(), 15);

	bounds.reduce(0, 2);

	slider.setBounds(bounds);


}

void CustomSlider::setStyleStandard(juce::String suffix)
{
	slider.setLookAndFeel(&standardSliderLookAndFeel);
	valueSuffix = suffix;
	updateStringText();
}

void CustomSlider::setStyleCenter(juce::String suffix)
{
	slider.setLookAndFeel(&centerSliderLookAndFeel);
	valueSuffix = suffix;
	updateStringText();
}

void CustomSlider::setBandMode(int mode)
{
	standardSliderLookAndFeel.setBandMode(mode);
	centerSliderLookAndFeel.setBandMode(mode);
}

void CustomSlider::labelTextChanged(juce::Label* labelThatHasChanged)
{
	juce::String entryString = labelThatHasChanged->getText();

	// Remove Non-Numerical Text ==================================================

	bool containsNum{ false };

	for (int i = 0; i <= 127; i++)
	{
		// Remove all characters not '0-9' or '-' or '.'
		if ((i < 48 || i > 57) && i !=46 && i != 45)
			entryString = entryString.removeCharacters(juce::String::charToString(char(i)));

		// Entry must contain at least one numeric value
		if ((i >= 48 || i <= 57))
		{
			if (entryString.contains(juce::String::charToString(char(i))))
				containsNum = true;
		}
	}

	if (containsNum)
		slider.setValue((float)entryString.getFloatValue());

	updateStringText();
}

void CustomSlider::sliderValueChanged(juce::Slider* slider)
{
	updateStringText();
}

void CustomSlider::updateStringText()
{
	juce::String sliderValue;

	sliderValue = (juce::String)slider.getValue();
	sliderValue.append(valueSuffix, 5);

	label.setText(sliderValue, juce::NotificationType::dontSendNotification);
}

// GLOBAL LOOK & FEEL FUNCTIONS =============================================================

// Draw the Background Appearance of Option Buttons
void drawOptionButtonBackground(	juce::Graphics& g, 
									juce::Rectangle<int> bounds, 
									bool selected, 
									juce::Slider& slider, 
									int bandMode )
{
	using namespace AllColors::SliderColors;

	auto sliderBounds = slider.getLocalBounds().toFloat();

	// Set Color-Coded Gradient
	if (slider.isEnabled())
	{
		switch (bandMode)
		{
		case 0: {g.setGradientFill(OPTION_SLIDER_GRADIENT_LOW(sliderBounds.toFloat()));	break; }
		case 1: {g.setGradientFill(OPTION_SLIDER_GRADIENT_MID(sliderBounds.toFloat()));	break; }
		case 2: {g.setGradientFill(OPTION_SLIDER_GRADIENT_HIGH(sliderBounds.toFloat())); break; }
		}
	}
	else
		g.setGradientFill(OPTION_SLIDER_GRADIENT_BYPASS(sliderBounds.toFloat()));

	// Fill Rectangle With Color-Coded Gradient
	g.setOpacity(0.65f);
	g.fillRoundedRectangle(bounds.toFloat(), 2);

	// Highlight Icon If Selected
	if (selected && slider.isEnabled())
	{
		g.setColour(juce::Colours::white);
		g.setOpacity(0.65f);
		g.fillRoundedRectangle(bounds.toFloat(), 2);
	}

	g.setOpacity(1.f);
}

// WAVE SLIDER ===================================================================

void WaveLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
										float sliderPos, float minSliderPos, float maxSliderPos,
										const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
	using namespace juce;

	slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	Rectangle<int> bounds = slider.getLocalBounds();		// Bounding Box
	Rectangle<int> sliderBounds = { x, y, width, height };	// Slider

	auto iconSize = abs((sliderBounds.getX() - bounds.getX()) * 2.f);

	// SET ICON SIZE ================================================================
	wave0.setSize(iconSize, iconSize);
	wave1.setSize(iconSize, iconSize);
	wave2.setSize(iconSize, iconSize);
	wave3.setSize(iconSize, iconSize);
	wave4.setSize(iconSize, iconSize);
	wave5.setSize(iconSize, iconSize);
	wave6.setSize(iconSize, iconSize);

	// SET ICON POSITIONS ============================================================
	auto spacing = sliderBounds.getWidth() / 6.f;

	wave0.setCentre(sliderBounds.getX() + 0 * spacing, sliderBounds.getCentreY());
	wave1.setCentre(sliderBounds.getX() + 1 * spacing, sliderBounds.getCentreY());
	wave2.setCentre(sliderBounds.getX() + 2 * spacing, sliderBounds.getCentreY());
	wave3.setCentre(sliderBounds.getX() + 3 * spacing, sliderBounds.getCentreY());
	wave4.setCentre(sliderBounds.getX() + 4 * spacing, sliderBounds.getCentreY());
	wave5.setCentre(sliderBounds.getX() + 5 * spacing, sliderBounds.getCentreY());
	wave6.setCentre(sliderBounds.getX() + 6 * spacing, sliderBounds.getCentreY());

	// DRAW ICONS ====================================================================
	int select = slider.getValue();
	
	drawWave0(g, wave0, select, slider);
	drawWave1(g, wave1, select, slider);
	drawWave2(g, wave2, select, slider);
	drawWave3(g, wave3, select, slider);
	drawWave4(g, wave4, select, slider);
	drawWave5(g, wave5, select, slider);
	drawWave6(g, wave6, select, slider);
}

void WaveLookAndFeel::drawWave0(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 0) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon ===============================
	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();


	juce::Line<float> arrow;
	arrow.setEnd(xC, y + height-3);
	arrow.setStart(xC, y+3);

	g.setColour(juce::Colours::black);
	g.drawArrow(arrow, 3.f, bounds.getWidth() * 0.65f, bounds.getHeight() * 0.45f);
}

void WaveLookAndFeel::drawWave1(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 1) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();
	
	juce::Line<float> arrow;
	arrow.setStart(xC, y + height-3);
	arrow.setEnd(xC, y+3);
	
	g.setColour(juce::Colours::black);
	g.drawArrow(arrow, 3.f, bounds.getWidth()*0.65f, bounds.getHeight()*0.45f);
}

void WaveLookAndFeel::drawWave2(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 2) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();

	juce::Rectangle<float> square;
	square.setSize(width-8, height-8);
	square.setCentre(xC, yC);
	
	g.setColour(juce::Colours::black);
	g.fillRect(square);
}

void WaveLookAndFeel::drawWave3(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 3) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();

	juce::Path triangle;
	triangle.startNewSubPath(xC, y+4);
	triangle.lineTo(x + width - 4, y + height - 4);
	triangle.lineTo(x + 4, y + height - 4);
	triangle.closeSubPath();

	g.setColour(juce::Colours::black);
	g.fillPath(triangle);
}

void WaveLookAndFeel::drawWave4(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 4) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();


	juce::Path p;

	// Second Half Of Sine Wave
	p.addArc(	0, 0, 
				20, 20, 
				-1.f * juce::MathConstants<float>::pi / 2.f,
				juce::MathConstants<float>::pi / 2.f, true);

	p.applyTransform(juce::AffineTransform().rotated(juce::MathConstants<float>::pi, 20, 10));

	// First Half Of Sine Wave
	p.addArc(	0, 0, 
				20, 20, 
				-1.f * juce::MathConstants<float>::pi / 2.f, 
				juce::MathConstants<float>::pi / 2.f, true);

	p.scaleToFit(x+4, y+4, width-8, height-8, false);

	g.setColour(juce::Colours::black);
	g.strokePath(p, juce::PathStrokeType(2.f));
}

void WaveLookAndFeel::drawWave5(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 5) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();

	g.setColour(juce::Colours::black);

	// Add Arc
	juce::Path p;
	p.addCentredArc(	xC, 
						y + height, 
						width, 
						height, 
						0.f, 
						-1.f * juce::MathConstants<float>::pi / 2.f, 
						juce::MathConstants<float>::pi / 2.f, 
						true);

	p.scaleToFit(x+5, y+5, width-10, height- 10, false);
	g.strokePath(p, juce::PathStrokeType(2.f));
}

void WaveLookAndFeel::drawWave6(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 6) 
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	auto x =		bounds.getX();
	auto y =		bounds.getY();
	auto width =	bounds.getWidth();
	auto height =	bounds.getHeight();
	auto xC =		bounds.getCentreX();
	auto yC =		bounds.getCentreY();

	g.setColour(juce::Colours::black);

	// Add Arc
	juce::Path p;
	p.addCentredArc(	xC, 
						y + height, 
						width, 
						height,
						juce::MathConstants<float>::pi, 
						-1.f * juce::MathConstants<float>::pi / 2.f, 
						juce::MathConstants<float>::pi / 2.f, 
						true);

	p.scaleToFit(x+5, y+5, width-10, height- 10, false);
	g.strokePath(p, juce::PathStrokeType(2.f));
}

void WaveLookAndFeel::setBandMode(int bandMode)
{
	mode = bandMode;
}

// MULT SLIDER ===================================================================

MultLookAndFeel::MultLookAndFeel()
{
	imageMultIcon0 = juce::ImageCache::getFromMemory(BinaryData::Mult0_png, BinaryData::Mult0_pngSize);
	imageMultIcon1 = juce::ImageCache::getFromMemory(BinaryData::Mult1_png, BinaryData::Mult1_pngSize);
	imageMultIcon2 = juce::ImageCache::getFromMemory(BinaryData::Mult2_png, BinaryData::Mult2_pngSize);
	imageMultIcon3 = juce::ImageCache::getFromMemory(BinaryData::Mult3_png, BinaryData::Mult3_pngSize);
	imageMultIcon4 = juce::ImageCache::getFromMemory(BinaryData::Mult4_png, BinaryData::Mult4_pngSize);
	imageMultIcon5 = juce::ImageCache::getFromMemory(BinaryData::Mult5_png, BinaryData::Mult5_pngSize);
}

void MultLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
										float sliderPos, float minSliderPos, float maxSliderPos,
										const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
	using namespace juce;

	slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	Rectangle<int> bounds = slider.getLocalBounds();		// Bounding Box
	Rectangle<int> sliderBounds = { x, y, width, height };	// Slider

	auto iconSize = abs((sliderBounds.getX() - bounds.getX()) * 2.f);

	// SET ICON SIZE ================================================================
	mult0.setSize(iconSize, iconSize);
	mult1.setSize(iconSize, iconSize);
	mult2.setSize(iconSize, iconSize);
	mult3.setSize(iconSize, iconSize);
	mult4.setSize(iconSize, iconSize);
	mult5.setSize(iconSize, iconSize);

	// SET ICON POSITIONS ============================================================
	auto spacing = sliderBounds.getWidth() / 5.f;

	mult0.setCentre(sliderBounds.getX() + 0 * spacing, sliderBounds.getCentreY());
	mult1.setCentre(sliderBounds.getX() + 1 * spacing, sliderBounds.getCentreY());
	mult2.setCentre(sliderBounds.getX() + 2 * spacing, sliderBounds.getCentreY());
	mult3.setCentre(sliderBounds.getX() + 3 * spacing, sliderBounds.getCentreY());
	mult4.setCentre(sliderBounds.getX() + 4 * spacing, sliderBounds.getCentreY());
	mult5.setCentre(sliderBounds.getX() + 5 * spacing, sliderBounds.getCentreY());

	// DRAW ICONS ====================================================================
	int select = slider.getValue();
	
	g.setOpacity(1.f);
	drawMult0(g, mult0, select, slider);
	drawMult1(g, mult1, select, slider);
	drawMult2(g, mult2, select, slider);
	drawMult3(g, mult3, select, slider);
	drawMult4(g, mult4, select, slider);
	drawMult5(g, mult5, select, slider);
}

void MultLookAndFeel::drawMult0(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 0)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	g.drawImage(imageMultIcon0, bounds.toFloat());
}

void MultLookAndFeel::drawMult1(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 1)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	g.drawImage(imageMultIcon1, bounds.toFloat());
}

void MultLookAndFeel::drawMult2(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 2)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	g.drawImage(imageMultIcon2, bounds.toFloat());
}

void MultLookAndFeel::drawMult3(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 3)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	g.drawImage(imageMultIcon3, bounds.toFloat());
}

void MultLookAndFeel::drawMult4(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 4)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);

	// Draw Icon
	g.drawImage(imageMultIcon4, bounds.toFloat());
}

void MultLookAndFeel::drawMult5(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider)
{
	bool selected{ false };

	if (selection == 5)
		selected = true;

	// Draw Icon Background
	drawOptionButtonBackground(g, bounds, selected, slider, mode);
	
	// Draw Icon
	g.drawImage(imageMultIcon5, bounds.toFloat());
}

void MultLookAndFeel::setBandMode(int bandMode)
{
	mode = bandMode;
}

// STANDARD SLIDER ==================================================================

void StandardSliderLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
													float sliderPos, float minSliderPos, float maxSliderPos,
													const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
	using namespace AllColors::SliderColors;

	// Set Textbox Style
	slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.f));

	// Establish Bounds Of Container (Slider & Label)
	juce::Rectangle<float> bounds = { (float)x, (float)y, (float)width, (float)height };

	// Establish Bounds of Slider
	juce::Rectangle<int> valueBounds = bounds.removeFromBottom(10).toNearestInt();

	// Pixel Position of Parameter State
	auto valuePosition = juce::jmap((sliderPos / bounds.getWidth()), bounds.getX(), bounds.getRight() - 1);

	// Fill Background
	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	g.fillRect(valueBounds);
	// Draw Value Bar
	juce::Rectangle<float> valueBar = { (float)valueBounds.getX(),
										(float)valueBounds.getY(),
										(float)valuePosition - x,
										(float)valueBounds.getHeight() };
	if (slider.isEnabled())
	{
		switch (mode)
		{
		case 0: {g.setGradientFill(BASIC_SLIDER_GRADIENT_LOW(bounds));	break; }
		case 1: {g.setGradientFill(BASIC_SLIDER_GRADIENT_MID(bounds));	break; }
		case 2: {g.setGradientFill(BASIC_SLIDER_GRADIENT_HIGH(bounds));	break; }
		}
	}
	else
		g.setGradientFill(BASIC_SLIDER_GRADIENT_BYPASS(bounds));

	g.fillRect(valueBar);
}

void StandardSliderLookAndFeel::setBandMode(int bandMode)
{
	mode = bandMode;
}

// CENTER SLIDER ==================================================================

void CenterSliderLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
												float sliderPos, float minSliderPos, float maxSliderPos,
												const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
	using namespace AllColors::SliderColors;

	// Set Textbox Style
	slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.f));

	// Establish Bounds Of Container (Slider & Label)
	juce::Rectangle<float> bounds = { (float)x, (float)y, (float)width, (float)height };

	// Establish Bounds of Slider
	juce::Rectangle<int> valueBounds = bounds.removeFromBottom(10).toNearestInt();

	// Pixel Position of Parameter State
	auto valuePosition = juce::jmap((sliderPos / bounds.getWidth()), bounds.getX(), bounds.getRight() - 1);

	// Fill Background
	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	g.fillRect(valueBounds);

	// Establish Value Bar
	juce::Path valueBar;
	valueBar.startNewSubPath(valueBounds.getCentreX(), valueBounds.getY());
	valueBar.lineTo(valueBounds.getCentreX(), valueBounds.getBottom());
	valueBar.lineTo(valuePosition, valueBounds.getBottom());
	valueBar.lineTo(valuePosition, valueBounds.getY());
	valueBar.closeSubPath();

	// Fill Value Bar
	switch (mode)
	{
	case 0: {g.setGradientFill(CENTER_SLIDER_GRADIENT_LOW(bounds));	break; }
	case 1: {g.setGradientFill(CENTER_SLIDER_GRADIENT_MID(bounds));	break; }
	case 2: {g.setGradientFill(CENTER_SLIDER_GRADIENT_HIGH(bounds)); break; }
	}

	if (slider.isEnabled())
		g.fillPath(valueBar);

	// Draw Value Line
	g.setColour(juce::Colours::white);
	g.drawVerticalLine(valuePosition, valueBounds.getY(), valueBounds.getBottom());
}

void CenterSliderLookAndFeel::setBandMode(int bandMode)
{
	mode = bandMode;
}

// SCROLL SLIDER =================================================================

//void ScrollLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
//											float sliderPos, float minSliderPos, float maxSliderPos,
//											const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
//{
//	// Set Textbox Style
//	slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
//	slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black.withAlpha(0.f));
//
//	// Establish Bounds
//	juce::Rectangle<float> bounds = { (float)x, (float)y, (float)width, (float)height };
//	//bounds.reduce(0, 6);
//
//	auto numLines = 100;
//	auto wheelHeight = height;
//	auto spacing = bounds.getWidth() / numLines;
//
//	// RESTRICT THE RANGE OF SCROLLING
//	float z = (sliderPos - x) / width;
//	auto scrollRange = juce::jmap(z, bounds.getX() + 20, bounds.getRight() - 20);
//
//	// Fill Rounded Background
//	g.setGradientFill(AllColors::OscilloscopeColors::SCROLL_GRADIENT(bounds.toFloat(), z));
//	g.fillPath(getOutline(g, bounds, wheelHeight));
//
//	// DRAW BARS
//	for (int i = 1; i < numLines; i++)
//	{
//		// BARS TO THE LEFT
//		auto linePos = scrollRange - i * spacing;
//		auto top = evaluateHeight(linePos, bounds);
//		auto thickness = 1.f;
//
//		if (i % 2 == 0)
//		{
//			g.setColour(juce::Colours::darkgrey);
//			thickness = 2.f;
//		}
//		else
//		{
//			g.setColour(juce::Colours::lightgrey);
//			thickness = 1.f;
//		}
//			
//		if (linePos > x)
//			g.drawLine(linePos, top, linePos, top + wheelHeight, thickness);
//
//		// BARS TO THE RIGHT
//		linePos = scrollRange + i * spacing;
//		top = evaluateHeight(linePos, bounds);
//
//		if (linePos < x + width)
//			g.drawLine(linePos, top, linePos, top + wheelHeight, thickness);
//	}
//
//	// DRAW CENTER BAR
//	auto top = evaluateHeight(scrollRange, bounds);
//
//	g.setColour(juce::Colours::black);
//	g.drawLine(scrollRange, top, scrollRange, top + wheelHeight, 8.f);
//		
//	g.setColour(juce::Colours::silver);
//	g.drawLine(scrollRange, top, scrollRange, top + wheelHeight, 2.f);
//
//	// DRAW ROUNDED CORNER
//	g.setColour(juce::Colours::darkgrey);
//	g.strokePath(getOutline(g, bounds, wheelHeight), juce::PathStrokeType(2.f));
//}
//
//
//float ScrollLookAndFeel::evaluateHeight(int position, juce::Rectangle<float> bounds)
//{
//	using namespace juce;
//
//	auto distFromCenter = bounds.getX() + abs(position - bounds.getCentreX());
//	auto heightScalar = distFromCenter / (bounds.getWidth() / 2.f);
//
//	auto wheelOffset = juce::jmap(heightScalar*heightScalar, bounds.getHeight()/4, 0.f);
//
//	auto top = bounds.getY() + wheelOffset;
//
//	return top;
//}
//
//juce::Path ScrollLookAndFeel::getOutline(juce::Graphics& g, juce::Rectangle<float> bounds, int wheelHeight)
//{
//	using namespace juce;
//
//	Path p;
//
//	for (int i = bounds.getX(); i <= bounds.getRight(); i++)
//	{
//		auto top = evaluateHeight(i, bounds);
//
//		if (i == bounds.getX())
//			p.startNewSubPath(i, top);
//
//		p.lineTo(i, top);
//	}
//
//	for (int i = bounds.getRight(); i >= bounds.getX(); i--)
//	{
//		auto bottom = evaluateHeight(i, bounds) + wheelHeight;
//
//		p.lineTo(i, bottom);
//
//		if (i == bounds.getX())
//			p.closeSubPath();
//	}
//
//	return p;
//}

void ButtonOptionsLookAndFeel::drawButtonBackground(juce::Graphics& g,
													juce::Button& button,
													const juce::Colour& backgroundColour,
													bool shouldDrawButtonAsHighlighted,
													bool shouldDrawButtonAsDown)
{
	auto bounds = button.getLocalBounds();

	if (!shouldDrawButtonAsDown)
	{
		g.fillAll(juce::Colours::white);

		g.setColour(juce::Colours::darkgrey);
		g.drawRect(bounds);
	}
	else
	{
		g.fillAll(juce::Colours::whitesmoke);

		g.setColour(juce::Colours::darkgrey);
		g.drawRect(bounds);

		bounds.reduce(1, 1);
		g.setColour(juce::Colours::darkslategrey);
		g.drawRect(bounds);
	}
		


}

void ButtonOptionsLookAndFeel::drawButtonText(	juce::Graphics& g, 
												juce::TextButton& button,
												bool shouldDrawButtonAsHighlighted, 
												bool shouldDrawButtonAsDown)
{
	auto bounds = button.getLocalBounds();

	auto myTypeface = "Helvetica";
	auto buttonFont = juce::Font(	myTypeface, 
									bounds.getHeight() * 0.75f, 
									juce::Font::FontStyleFlags::bold);

	g.setFont(buttonFont);

	g.drawText("Options", bounds, juce::Justification::centred, false);
}

void ScrollLookAndFeel::drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
											float sliderPos, float minSliderPos, float maxSliderPos,
											const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{

}

// IN/OUT METER SLIDER ===========================================================

void InOutLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
	float sliderPos, float minSliderPos, float maxSliderPos,
	const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider)
{
	// Set Textbox Style
	slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
	slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::hotpink.withAlpha(0.f));

	// Establish Bounds
	juce::Rectangle<float> bounds = { (float)x, (float)y, (float)width, (float)height };

	// Draw Track & Fader
	auto faderWidth = 30;
	auto faderHeight = 15;

	auto sliderPosScaled = (sliderPos - bounds.getY()) / bounds.getHeight();

	auto faderRange = juce::jmap(sliderPosScaled, bounds.getY() + 10, bounds.getBottom() - 10);

	juce::Path track;
	track.startNewSubPath(bounds.getCentreX(), bounds.getY() + 10);
	track.lineTo(bounds.getCentreX(), bounds.getBottom() - 10);
	g.setColour(juce::Colours::black);
	g.strokePath(track, juce::PathStrokeType(	6.f, juce::PathStrokeType::JointStyle::beveled, 
												juce::PathStrokeType::EndCapStyle::rounded));

	juce::Rectangle<float> fader;
	fader.setBounds(	bounds.getCentreX() - faderWidth/2.f, 
						faderRange - faderHeight / 2.f,
						faderWidth, faderHeight);
	
	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(1.5f));
	g.setOpacity(0.85f);
	g.fillRoundedRectangle(fader, 3.f);

	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(0.5f));
	g.drawRoundedRectangle(fader, 3.f, 1.f);

	// Build dB Value String

	float value;
	juce::String valueString;

	value = juce::jmap(sliderPosScaled, 24.f, -24.f);

	if (value > 0.f)
		valueString = "+";

	valueString = valueString << juce::String(value);
		
	g.setColour(juce::Colours::white);
	g.setFont(15);
	g.drawFittedText(valueString, fader.reduced(1,3).toNearestInt(), juce::Justification::centred, 1);
}

// CROSSOVER SLIDER ==============================================================

CrossoverLookAndFeel::CrossoverLookAndFeel()
{
	
}

void CrossoverLookAndFeel::drawRotarySlider(	juce::Graphics& g, int x, int y, int width, int height,
												float sliderPosProportional, float rotaryStartAngle,
												float rotaryEndAngle, juce::Slider& slider)
{
	using namespace juce;

	slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);

	auto bounds = Rectangle<float>(x, y, width, height).toFloat();
	bounds.removeFromTop(10);
	bounds.removeFromBottom(5);

	// Use parent bounds as bounds of slider.  Square off and center.
	juce::Rectangle<float> ellipseBounds = { bounds.getX(), bounds.getY(), bounds.getHeight(), bounds.getHeight() };
	ellipseBounds.setCentre(bounds.getCentreX(), ellipseBounds.getCentreY());

	// Draw Outer Ring (Fixed Background)
	Path p1;

	p1.addArc(	ellipseBounds.getX(), 
				ellipseBounds.getY(), 
				ellipseBounds.getWidth(), 
				ellipseBounds.getHeight(), 
				rotaryStartAngle, rotaryEndAngle,
				true);

	g.setColour(juce::Colours::grey);
	g.strokePath(p1, juce::PathStrokeType(	4.f, 
											juce::PathStrokeType::JointStyle::beveled, 
											juce::PathStrokeType::EndCapStyle::rounded ) );

	// Draw Outer Ring (Value-Based)
	Path p2;

	// Current Angle of Parameter Selection
	auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

	p2.addArc(	ellipseBounds.getX(),
				ellipseBounds.getY(), 
				ellipseBounds.getWidth(), 
				ellipseBounds.getHeight(), 
				rotaryStartAngle, sliderAngRad,
				true);

	g.setColour(juce::Colour(0xff2530bd));
	g.strokePath(p2, juce::PathStrokeType(	5.f,
											juce::PathStrokeType::JointStyle::beveled, 
											juce::PathStrokeType::EndCapStyle::rounded ) );



	//g.setColour(juce::Colours::black);
	//g.drawEllipse(ellipseBounds, 2.f);

	//auto center = ellipseBounds.getCentre();
	//Path p;

	//Rectangle<float> r;
	//r.setLeft(center.getX() - 2);
	//r.setRight(center.getX() + 2);
	//r.setTop(bounds.getY());
	//r.setBottom(center.getY());

	//p.addRoundedRectangle(r, 2.f);

	//auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

	//p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

	//g.setColour(juce::Colours::white);
	//g.fillPath(p);
}