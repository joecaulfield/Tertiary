/*
  ==============================================================================

	FrequencyResponse.cpp
	Created: 30 Dec 2021 11:38:20am
	Author:  Joe Caulfield

	Class to display and control the frequency response of the crossover

  ==============================================================================
*/

#include "FrequencyResponse.h"

// Constructor
FrequencyResponse::FrequencyResponse(juce::AudioProcessorValueTreeState& apv, GlobalControls& gc)
	: apvts(apv), globalControls(gc)
{
	// Linear Slider from 0 to 1
	sliderLowMidInterface.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderLowMidInterface.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderLowMidInterface.addListener(this);
	sliderLowMidInterface.setRange(0, 1.f, 0.001f);
	sliderLowMidInterface.setAlpha(0.f);
	addAndMakeVisible(sliderLowMidInterface);

	// Linear Slider from 0 to 1
	sliderMidHighInterface.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderMidHighInterface.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderMidHighInterface.addListener(this);
	sliderMidHighInterface.setRange(0, 1.f, 0.001f);
	sliderMidHighInterface.setAlpha(0.f);
	addAndMakeVisible(sliderMidHighInterface);

	// Linear Slider from 20 to 20k
	sliderLowMidCutoff.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderLowMidCutoff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderLowMidCutoff.addListener(this);

	// Linear Slider from 20 to 20k
	sliderMidHighCutoff.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	sliderMidHighCutoff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderMidHighCutoff.addListener(this);

	// ==========================
	sliderLowGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderLowGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderLowGain.addListener(this);
	sliderLowGain.setRange(0, 1.f, 0.001f);
	sliderLowGain.setAlpha(0.f);
	addAndMakeVisible(sliderLowGain);

	// ==========================
	sliderMidGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderMidGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderMidGain.addListener(this);
	sliderMidGain.setRange(0, 1.f, 0.001f);
	sliderMidGain.setAlpha(0.f);
	addAndMakeVisible(sliderMidGain);

	// ==========================
	sliderHighGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	sliderHighGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sliderHighGain.addListener(this);
	sliderHighGain.setRange(0, 1.f, 0.001f);
	sliderHighGain.setAlpha(0.f);
	addAndMakeVisible(sliderHighGain);

	makeAttachments();

	updateResponse();
	startTimerHz(60);
	addMouseListener(this, false);
}

// Make component->parameter attachments
void FrequencyResponse::makeAttachments()
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

	lowMidAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Low_Mid_Crossover_Freq),
						sliderLowMidCutoff);

	lowGainAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Gain_Low_Band),
						sliderLowGain);

	midGainAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Gain_Mid_Band),
						sliderMidGain);

	highGainAttachment =std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						apvts,
						params.at(Names::Gain_High_Band),
						sliderHighGain);
}

// Graphics class
void FrequencyResponse::paint(juce::Graphics& g)
{
	using namespace juce;
	
	g.fillAll(juce::Colours::black);
	drawGridVertical(g);
	drawGridHorizontal(g);

	// If low-band is hovered, draw low-band on top with highlight
	if (mLowFocus)
	{
		drawHighRegion(g,	responseArea, gainHighPixel,	freq2Pixel,				mHighFocus	? fadeMaxRegion : fadeAlphaRegionHG);
		drawMidRegion(g,	responseArea, gainMidPixel,		freq1Pixel, freq2Pixel, mMidFocus	? fadeMaxRegion : fadeAlphaRegionMG);
		drawLowRegion(g,	responseArea, gainLowPixel,		freq1Pixel,				mLowFocus	? fadeMaxRegion : fadeAlphaRegionLG);
	}

	// If mid-band is hovered, draw mid-band on top with highlight.  Doubles as default view.
	if (mMidFocus || (!mLowFocus && !mMidFocus && !mHighFocus))
	{
		drawHighRegion(g,	responseArea, gainHighPixel,	freq2Pixel,				mHighFocus	? fadeMaxRegion : fadeAlphaRegionHG);
		drawLowRegion(g,	responseArea, gainLowPixel,		freq1Pixel,				mLowFocus	? fadeMaxRegion : fadeAlphaRegionLG);
		drawMidRegion(g,	responseArea, gainMidPixel,		freq1Pixel, freq2Pixel, mMidFocus	? fadeMaxRegion : fadeAlphaRegionMG);
	}

	// If high-band is hovered, draw high-band on top with highlight.  
	if (mHighFocus)
	{
		drawLowRegion(g,	responseArea, gainLowPixel,		freq1Pixel,				mLowFocus	? fadeMaxRegion : fadeAlphaRegionLG);
		drawMidRegion(g,	responseArea, gainMidPixel,		freq1Pixel, freq2Pixel, mMidFocus	? fadeMaxRegion : fadeAlphaRegionMG);
		drawHighRegion(g,	responseArea, gainHighPixel,	freq2Pixel,				mHighFocus	? fadeMaxRegion : fadeAlphaRegionHG);
	}

	// Draw Low-Mid Cursor
	g.setColour(juce::Colours::white);
	g.setOpacity(mLowMidFocus ? 1.f : fadeAlphaCursorLM);
	g.drawLine(cursorLM, 3.f);

	// Draw Mid-High Cursor
	g.setColour(juce::Colours::white);
	g.setOpacity(mMidHighFocus ? 1.f : fadeAlphaCursorMH);
	g.drawLine(cursorMH, 3.f);

	// Draw Low-Gain Cursor
	if (!lowBandMute)
	{
		g.setColour(juce::Colours::white);
		g.setOpacity(fadeAlphaCursorLG);
		g.drawLine(cursorLG, 3.f);
	}

	// Draw Mid-Gain Cursor
	if (!midBandMute)
	{
		g.setColour(juce::Colours::white);
		g.setOpacity(fadeAlphaCursorMG);
		g.drawLine(cursorMG, 3.f);
	}

	// Draw High-Gain Cursor
	if (!highBandMute)
	{
		g.setColour(juce::Colours::white);
		g.setOpacity(fadeAlphaCursorHG);
		g.drawLine(cursorHG, 3.f);
	}

	drawBorder(g);
}

// Draw vertical gridlines and vertical axis labels (gain)
void FrequencyResponse::drawGridVertical(juce::Graphics& g)
{
	using namespace juce;
	const auto textWidth = 30;
	const auto fontHeight = 13;

	// DRAW THE VERTICAL GRIDS ===============
	for (int i = 0; i < freqs.size()-1; i++)
	{
		float x = responseArea.getX() + responseArea.getWidth() * mapLog2(freqs[i]);

		g.setColour(Colours::lightgrey);
		g.drawVerticalLine(x, responseArea.getY(), responseArea.getBottom());

		String strFreq;

		if (freqs[i] > 999.f)
		{
			strFreq << (freqs[i] / 1000.f) << "k";
		}
		else
			strFreq << freqs[i];
		
		g.setFont(fontHeight);
		g.setColour(Colours::lightgrey);

		float xC = x - textWidth / 2;
		g.drawFittedText(strFreq, xC, getHeight()-fontHeight-7, textWidth, fontHeight, Justification::centred, 1, 1);
	}
}

// Draw horizontal gridlines and horizontal axis labels (frequency)
void FrequencyResponse::drawGridHorizontal(juce::Graphics& g)
{
	using namespace juce;
	const auto textWidth = 30;
	const auto fontHeight = 15;

	const auto gMin = gain.getFirst();
	const auto gMax = gain.getLast();

	// DRAW THE HORIZONTAL GRIDS ===============
	for (int i = 1; i < gain.size()-1; i++)
	{
		auto y = responseArea.getY() + jmap(gain[i], gMin, gMax, float(responseArea.getBottom()), responseArea.getY());

		g.setColour(gain[i] == 0.f ? Colours::green : Colours::lightgrey);
		g.drawHorizontalLine(y, responseArea.getX(), responseArea.getRight());

		String strGain;

		if (gain[i] > 0)
			strGain << "+";

		strGain << gain[i];

		float yC = y - fontHeight/2;

		g.setFont(fontHeight);
		g.setColour(Colours::lightgrey);
		g.drawFittedText(strGain, 3, yC, textWidth, fontHeight, juce::Justification::centred, 1, 1);

	}
}

// Draw Low-Frequency Response Trapezoid
void FrequencyResponse::drawLowRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float alpha)
{
	using namespace AllColors::FrequencyResponseColors;
	auto slope = 10;

	bounds.reduce(2, 0);

	// LOW REGION ========================================================
	juce::Path lowRegion;

	lowRegion.startNewSubPath(	bounds.getX(),	bounds.getBottom());
	lowRegion.lineTo(			bounds.getX(),	gainPixel);
	lowRegion.lineTo(			freq1Pixel,		gainPixel);

	if (bounds.getRight() - freq1Pixel > slope)
		lowRegion.lineTo(freq1Pixel + slope, bounds.getBottom());
	else
		lowRegion.lineTo(bounds.getRight(), bounds.getBottom());

	if (!lowBandMute)
	{
		g.setGradientFill	(	lowBandBypass ? 
								BYPASS_REGION_GRADIENT(bounds.toFloat()) :
								LOW_REGION_GRADIENT(bounds.toFloat()) );

		g.setOpacity(alpha);
		g.fillPath(lowRegion.createPathWithRoundedCorners(10));

		g.setColour		(	lowBandBypass ? REGION_BORDER_COLOR_BYPASS() :
							REGION_BORDER_COLOR_LOW());

		g.strokePath(lowRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
	}

}

// Draw Mid-Frequency Response Trapezoid
void FrequencyResponse::drawMidRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float freq2Pixel, float alpha)
{
	using namespace AllColors::FrequencyResponseColors;
	auto slope = 10;

	bounds.reduce(2, 0);

	juce::Path midRegion;
	
	if (freq1Pixel - bounds.getX() > slope)
		midRegion.startNewSubPath(freq1Pixel - slope, bounds.getBottom());
	else
		midRegion.startNewSubPath(bounds.getX(), bounds.getBottom());

	midRegion.lineTo(freq1Pixel, gainPixel);
	midRegion.lineTo(freq2Pixel, gainPixel);

	if (bounds.getRight() - freq2Pixel > slope)
		midRegion.lineTo(freq2Pixel + slope, bounds.getBottom());
	else
		midRegion.lineTo(bounds.getRight(), bounds.getBottom());

	if (!midBandMute)
	{
		g.setGradientFill	(	midBandBypass ? 
								BYPASS_REGION_GRADIENT(bounds.toFloat()) :
								MID_REGION_GRADIENT(bounds.toFloat()) );

		g.setOpacity(alpha);
		g.fillPath(midRegion.createPathWithRoundedCorners(10));

		g.setColour		(	midBandBypass ? REGION_BORDER_COLOR_BYPASS() :
							REGION_BORDER_COLOR_MID());

		g.strokePath(midRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
	}

}

// Draw High-Frequency Response Trapezoid
void FrequencyResponse::drawHighRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq2Pixel, float alpha)
{
	using namespace AllColors::FrequencyResponseColors;
	auto slope = 10;

	bounds.reduce(2, 0);

	juce::Path highRegion;

	if (freq2Pixel - bounds.getX() > slope)
		highRegion.startNewSubPath(freq2Pixel - slope, bounds.getBottom());
	else
		highRegion.startNewSubPath(bounds.getX(), bounds.getBottom());

	highRegion.lineTo(freq2Pixel, gainPixel);
	highRegion.lineTo(bounds.getRight(), gainPixel);
	highRegion.lineTo(bounds.getRight(), bounds.getBottom());

	if (!highBandMute)
	{
		g.setGradientFill	(	highBandBypass ? 
								BYPASS_REGION_GRADIENT(bounds.toFloat()) :
								HIGH_REGION_GRADIENT(bounds.toFloat()) );

		g.setOpacity(alpha);
		g.fillPath(highRegion.createPathWithRoundedCorners(10));

		g.setColour		(	highBandBypass ? REGION_BORDER_COLOR_BYPASS() :
							REGION_BORDER_COLOR_HIGH());

		g.strokePath(highRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
	}
}

// Draw master border around window
void FrequencyResponse::drawBorder(juce::Graphics& g)
{
	// DRAW BORDER ====================
	auto bounds = getLocalBounds();

	juce::Rectangle<float> border;
	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y,
				bounds.getWidth(), bounds.getHeight());

	g.setColour(juce::Colours::white);
	g.drawRoundedRectangle(border, 3.f, 3.f);
	// =====

	bounds.reduce(1, 1);
	bounds.setCentre(getWidth() / 2, getHeight() / 2);

	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y,
		bounds.getWidth(), bounds.getHeight());

	g.setColour(juce::Colours::lightgrey);
	g.drawRoundedRectangle(border, 2.f, 2.f);
	// =====

	bounds.reduce(1, 1);
	bounds.setCentre(getWidth() / 2, getHeight() / 2);

	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y,
		bounds.getWidth(), bounds.getHeight());

	g.setColour(juce::Colours::grey);
	g.drawRoundedRectangle(border, 2.f, 2.f);
	// =====

	bounds.reduce(1, 1);
	bounds.setCentre(getWidth() / 2, getHeight() / 2);

	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y,
		bounds.getWidth(), bounds.getHeight());

	g.setColour(juce::Colours::darkgrey);
	g.drawRoundedRectangle(border, 2.f, 2.f);
	// =====

}

// Called on Component Resize
void FrequencyResponse::resized()
{
	// =============================
	responseArea = getLocalBounds().toFloat();
	responseArea.reduce(2, 2);
	responseArea.removeFromBottom(20);
	responseArea.removeFromLeft(32);
	responseArea.removeFromRight(6);

	// =============================
	sliderLowMidCutoff.setBounds(responseArea.getX(), responseArea.getY()+80, 25, 25);
	sliderMidHighCutoff.setBounds(responseArea.getX(), responseArea.getY()+90, 50, 25);

	// =============================
	sliderLowMidInterface.setBounds(responseArea.getX() - 10, 25, responseArea.getWidth() + 20, 10);
	sliderMidHighInterface.setBounds(responseArea.getX() - 10, 50, responseArea.getWidth() + 20, 10);

	sliderMidGain.setBounds(responseArea.getX()+25, responseArea.getY(), 20, responseArea.getHeight());
	sliderHighGain.setBounds(responseArea.getX()+50, responseArea.getY(), 20, responseArea.getHeight());

	sliderLowGain.setBounds(responseArea.getX(), responseArea.getY(), 1, responseArea.getHeight());
	sliderMidGain.setBounds(responseArea.getX(), responseArea.getY(), 1, responseArea.getHeight());
	sliderHighGain.setBounds(responseArea.getX(), responseArea.getY(), 1, responseArea.getHeight());
}

// Methods to call on a timed-basis
void FrequencyResponse::timerCallback()
{
	checkMousePosition();

	updateResponse();
	fadeInOutComponents();
	fadeCursor();
	fadeRegions();
	checkExternalFocus();
	repaint();
}

// Check for external band hovering.  Refactor for encapsulation.
void FrequencyResponse::checkExternalFocus()
{
	using namespace Params;
	const auto& params = GetParams();

	mLowFocus = (	globalControls.mTimingControls.timingBarLow.hasFocus || 
					globalControls.mGainControls.gainBarLow.hasFocus ||
					globalControls.mWaveControls.waveBarLow.hasFocus) || fadeRegionLG;

	mMidFocus = (	globalControls.mTimingControls.timingBarMid.hasFocus || 
					globalControls.mGainControls.gainBarMid.hasFocus ||
					globalControls.mWaveControls.waveBarMid.hasFocus) || fadeRegionMG;

	mHighFocus = (	globalControls.mTimingControls.timingBarHigh.hasFocus || 
					globalControls.mGainControls.gainBarHigh.hasFocus ||
					globalControls.mWaveControls.waveBarHigh.hasFocus) || fadeRegionHG;

	mLowMidFocus =  globalControls.mXoverControls.lowMidFocus;
	mMidHighFocus = globalControls.mXoverControls.midHighFocus;
}

// Update the response prior to painting.
void FrequencyResponse::updateResponse()
{
	using namespace Params;
	const auto& params = GetParams();

	// Get cutoff frequency parameter values
	mLowMidCutoff = *apvts.getRawParameterValue(params.at(Names::Low_Mid_Crossover_Freq));
	mMidHighCutoff = *apvts.getRawParameterValue(params.at(Names::Mid_High_Crossover_Freq));

	// Convert cutoff values to relative pixel values
	freq1Pixel = responseArea.getX() + mapLog2(mLowMidCutoff) * responseArea.getWidth();
	freq2Pixel = responseArea.getX() + mapLog2(mMidHighCutoff) * responseArea.getWidth();

	// Use pixel values as path bounds
	cursorLM.setStart(freq1Pixel, responseArea.getY() + 1);
	cursorLM.setEnd(freq1Pixel, responseArea.getBottom() - 1);

	cursorMH.setStart(freq2Pixel, responseArea.getY() + 1);
	cursorMH.setEnd(freq2Pixel, responseArea.getBottom() - 1);

	// Get band gain parameter values
	mLowGain = *apvts.getRawParameterValue(params.at(Names::Gain_Low_Band));
	mMidGain = *apvts.getRawParameterValue(params.at(Names::Gain_Mid_Band));
	mHighGain = *apvts.getRawParameterValue(params.at(Names::Gain_High_Band));

	// Convert band gain values to relative pixel values
	gainLowPixel = responseArea.getY() + juce::jmap(mLowGain,	-30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
	gainMidPixel = responseArea.getY() + juce::jmap(mMidGain,	-30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
	gainHighPixel = responseArea.getY() + juce::jmap(mHighGain, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());

	// Use pixel values as path bounds
	auto lowWidth = freq1Pixel * 0.25f;
	auto midWidth = (freq2Pixel - freq1Pixel) * 0.25f;
	auto highWidth = (responseArea.getRight() - freq2Pixel) * 0.25f;

	cursorLG.setStart(responseArea.getX() + lowWidth, gainLowPixel);
	cursorLG.setEnd(freq1Pixel - lowWidth, gainLowPixel);

	cursorMG.setStart(freq1Pixel + midWidth, gainMidPixel);
	cursorMG.setEnd(freq2Pixel - midWidth, gainMidPixel);

	cursorHG.setStart(freq2Pixel + highWidth, gainHighPixel);
	cursorHG.setEnd(responseArea.getRight() - highWidth, gainHighPixel);

	// Update band bypass/solo/mute states
	lowBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
	midBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
	highBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));

	lowBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Low_Band));
	midBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Mid_Band));
	highBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_High_Band));

	lowBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_Low_Band));
	midBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_Mid_Band));
	highBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_High_Band));

	checkSolos();
}

// Check if bands are soloed.  Negate mute if so.
void FrequencyResponse::checkSolos()
{
	auto x = lowBandSolo * 100;
	auto y = midBandSolo * 10;
	auto z = highBandSolo;
	auto s = x + y + z;

	switch (s)
	{
	case 1:   {lowBandMute = true; midBandMute = true;  break; }
	case 10:  {lowBandMute = true; highBandMute = true; break; }
	case 11:  {lowBandMute = true;						break; }
	case 100: {midBandMute = true; highBandMute = true; break; }
	case 101: {midBandMute = true;						break; }
	case 110: {highBandMute = true;						break; }
	}
}

// Map linear slider value to true octave-logarithmic value
float FrequencyResponse::mapLog2(float freq)
{
	auto logMin = std::log2(20.f);
	auto logMax = std::log2(20000.f);

	return (std::log2(freq) - logMin) / (logMax - logMin);
}

// Called on Mouse Move
void FrequencyResponse::mouseMove(const juce::MouseEvent& event)
{
	checkCursorFocus(event);
}

// Called on Mouse Enter
void FrequencyResponse::mouseEnter(const juce::MouseEvent& event)
{
	fadeIn = true;
	checkCursorFocus(event);
}

// Called on Mouse Exit
void FrequencyResponse::mouseExit(const juce::MouseEvent& event)
{
	fadeIn = false;

	auto xM = event.getPosition().getX();
	auto yM = event.getPosition().getY();

	checkCursorFocus(event);
}

// Check to see if mouse is focusing on any cursor for parameter change
void FrequencyResponse::checkCursorFocus(const juce::MouseEvent& event)
{
	auto yM = event.getPosition().getY();
	auto xM = event.getPosition().getX();

	auto xLM = cursorLM.getStartX();
	auto xMH = cursorMH.getStartX();

	auto yLG = cursorLG.getStartY();
	auto yMG = cursorMG.getStartY();
	auto yHG = cursorHG.getStartY();

	auto xMargin = 10;
	auto yMargin = 10;

	auto x0 = responseArea.getX();
	auto x1 = responseArea.getRight();
	auto y0 = responseArea.getY();
	auto y1 = responseArea.getBottom();

	// Mouse is out of window, kill all fades
	if (xM < x0 || xM > x1 || yM < y0 || yM > y1)
	{
		fadeRegionLG = false;
		fadeRegionMG = false;
		fadeRegionHG = false;
		fadeInCursorLM = false;
		fadeInCursorMH = false;
	}

	// Mouse is on Low-Mid Cursor
	if (abs(xM - xLM) < xMargin)
		fadeInCursorLM = true;
	else fadeInCursorLM = false;

	// Mouse is on Mid-High Cursor
	if (abs((xM - xMH)) < xMargin)
		fadeInCursorMH = true;
	else fadeInCursorMH = false;

	// Mouse is within Low Gain Region
	if (xM < freq1Pixel - xMargin)
	{
		fadeRegionLG = true;

		// Mouse is on Low Gain Cursor
		if (abs(yM - yLG) < yMargin)
		{
			fadeInCursorLG = true;
			fadeInCursorLM = false;
			fadeInCursorMH = false;
		} 
		else fadeInCursorLG = false;
	}
	else
	{
		fadeRegionLG = false;
		fadeInCursorLG = false;
	}

	// Mouse is within Mid Gain Region
	if (xM > freq1Pixel + xMargin && xM < freq2Pixel - xMargin)
	{
		fadeRegionMG = true;

		// Mouse is on Mid Gain Cursor
		if (abs(yM - yMG) < yMargin)
		{
			fadeInCursorMG = true;
			fadeInCursorLM = false;
			fadeInCursorMH = false;
		} 
		else fadeInCursorMG = false;
	}
	else
	{
		fadeRegionMG = false;
		fadeInCursorMG = false;
	}

	// Mouse is within High Gain Region
	if (xM > freq2Pixel + xMargin)
	{
		fadeRegionHG = true;

		// Mouse is on High Gain Cursor
		if (abs(yM - yHG) < yMargin)
		{
			fadeInCursorHG = true;
			fadeInCursorLM = false;
			fadeInCursorMH = false;
		} else fadeInCursorHG = false;
	}
	else
	{
		fadeRegionHG = false;
		fadeInCursorHG = false;
	}
	
	// Establish top-bottom bounds of Dummy Sliders
	auto sliderTop = responseArea.getY() + juce::jmap(24.f, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY())-5;
	auto sliderBot = responseArea.getY() + juce::jmap(-24.f, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY())+5;

	// Mouse is on Low-Mid Cursor, place dummy slider underneath mouse
	if (fadeInCursorLM)
		sliderLowMidInterface.setBounds(responseArea.getX(), yM, responseArea.getWidth(), 1);

	// Mouse is on Mid-High Cursor, place dummy slider underneath mouse
	if (fadeInCursorMH)
		sliderMidHighInterface.setBounds(responseArea.getX(), yM, responseArea.getWidth(), 1);

	// Mouse is on Low Gain Cursor, place dummy slider underneath mouse
	if (fadeInCursorLG)
		sliderLowGain.setBounds(xM, sliderTop, 1, (sliderBot-sliderTop));

	// Mouse is on Mid Gain Cursor, place dummy slider underneath mouse
	if (fadeInCursorMG)
		sliderMidGain.setBounds(xM, sliderTop, 1, (sliderBot - sliderTop));

	// Mouse is on High Gain Cursor, place dummy slider underneath mouse
	if (fadeInCursorHG)
		sliderHighGain.setBounds(xM, sliderTop, 1, (sliderBot - sliderTop));

}

// Fade Components on Mouse Enter
void FrequencyResponse::fadeInOutComponents()
{
	if (fadeIn) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlpha < fadeMax)
			fadeAlpha += fadeStepUp;

		if (fadeAlpha > fadeMax)
			fadeAlpha = fadeMax;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlpha > fadeMin)
			fadeAlpha -= fadeStepDown;

		if (fadeAlpha < fadeMin)
			fadeAlpha = fadeMin;
	}
}

// Fade Cursor when within bounds
void FrequencyResponse::fadeCursor()
{

	// LOW-MID ========================================
	if (fadeInCursorLM) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursorLM < fadeMaxCursor)
			fadeAlphaCursorLM += fadeStepUpCursor;

		if (fadeAlphaCursorLM > fadeMaxCursor)
			fadeAlphaCursorLM = fadeMaxCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursorLM > fadeMinCursor)
			fadeAlphaCursorLM -= fadeStepDownCursor;

		if (fadeAlphaCursorLM < fadeMinCursor)
			fadeAlphaCursorLM = fadeMinCursor;
	}

	// MID-HIGH ========================================
	if (fadeInCursorMH) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursorMH < fadeMaxCursor)
			fadeAlphaCursorMH += fadeStepUpCursor;

		if (fadeAlphaCursorMH > fadeMaxCursor)
			fadeAlphaCursorMH = fadeMaxCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursorMH > fadeMinCursor)
			fadeAlphaCursorMH -= fadeStepDownCursor;

		if (fadeAlphaCursorMH < fadeMinCursor)
			fadeAlphaCursorMH = fadeMinCursor;
	}

	// LOW-GAIN ========================================
	if (fadeInCursorLG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursorLG < fadeMaxCursor)
			fadeAlphaCursorLG += fadeStepUpCursor;

		if (fadeAlphaCursorLG > fadeMaxCursor)
			fadeAlphaCursorLG = fadeMaxCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursorLG > fadeMinCursor)
			fadeAlphaCursorLG -= fadeStepDownCursor;

		if (fadeAlphaCursorLG < fadeMinCursor)
			fadeAlphaCursorLG = fadeMinCursor;
	}

	// MID-GAIN ========================================
	if (fadeInCursorMG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursorMG < fadeMaxCursor)
			fadeAlphaCursorMG += fadeStepUpCursor;

		if (fadeAlphaCursorMG > fadeMaxCursor)
			fadeAlphaCursorMG = fadeMaxCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursorMG > fadeMinCursor)
			fadeAlphaCursorMG -= fadeStepDownCursor;

		if (fadeAlphaCursorMG < fadeMinCursor)
			fadeAlphaCursorMG = fadeMinCursor;
	}

	// HIGH-GAIN ========================================
	if (fadeInCursorHG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursorHG < fadeMaxCursor)
			fadeAlphaCursorHG += fadeStepUpCursor;

		if (fadeAlphaCursorHG > fadeMaxCursor)
			fadeAlphaCursorHG = fadeMaxCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursorHG > fadeMinCursor)
			fadeAlphaCursorHG -= fadeStepDownCursor;

		if (fadeAlphaCursorHG < fadeMinCursor)
			fadeAlphaCursorHG = fadeMinCursor;
	}

}

// Fade Band regions when within bounds
void FrequencyResponse::fadeRegions()
{

	// LOW-GAIN ========================================
	if (fadeRegionLG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaRegionLG < fadeMaxRegion)
			fadeAlphaRegionLG += fadeStepUpRegion;

		if (fadeAlphaRegionLG > fadeMaxRegion)
			fadeAlphaRegionLG = fadeMaxRegion;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaRegionLG > fadeMinRegion)
			fadeAlphaRegionLG -= fadeStepDownRegion;

		if (fadeAlphaRegionLG < fadeMinRegion)
			fadeAlphaRegionLG = fadeMinRegion;
	}

	// MID-GAIN ========================================
	if (fadeRegionMG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaRegionMG < fadeMaxRegion)
			fadeAlphaRegionMG += fadeStepUpRegion;

		if (fadeAlphaRegionMG > fadeMaxRegion)
			fadeAlphaRegionMG = fadeMaxRegion;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaRegionMG > fadeMinRegion)
			fadeAlphaRegionMG -= fadeStepDownRegion;

		if (fadeAlphaRegionMG < fadeMinRegion)
			fadeAlphaRegionMG = fadeMinRegion;
	}

	// HIGH-GAIN ========================================
	if (fadeRegionHG) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaRegionHG < fadeMaxRegion)
			fadeAlphaRegionHG += fadeStepUpRegion;

		if (fadeAlphaRegionHG > fadeMaxRegion)
			fadeAlphaRegionHG = fadeMaxRegion;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaRegionHG > fadeMinRegion)
			fadeAlphaRegionHG -= fadeStepDownRegion;

		if (fadeAlphaRegionHG < fadeMinRegion)
			fadeAlphaRegionHG = fadeMinRegion;
	}
}

// Check mouse position on timed-basis
void FrequencyResponse::checkMousePosition()
{
	// If none apply, kill all fades

	if (!isMouseOverOrDragging(true))
	{
		fadeInCursorLM = false;
		fadeInCursorMH = false;
		fadeRegionLG = false;
		fadeRegionMG = false;
		fadeRegionHG = false;
		fadeIn = false;
	}
}

// Called on slider value change
void FrequencyResponse::sliderValueChanged(juce::Slider* slider)
{
	// 'Interface' slider is linear from 0 to 1.  
	// Changed by user by dragging Cutoff Cursors
	// Slider is converted from [0 to 1] to [20 to 20k] to store frequency

	if (slider == &sliderLowMidInterface || slider == &sliderMidHighInterface)
	{
		if (slider == &sliderLowMidInterface)
		{
			// Convert [0 to 1] (Interface) value to [20 to 20k] (Cutoff) value.
			// Send to Cutoff slider to update params.

			auto y = slider->getValue();
			auto f = 20 * pow(2, std::log2(1000.f) * y);
			sliderLowMidCutoff.setValue(f);
		}

		if (slider == &sliderMidHighInterface)
		{
			// Convert [0 to 1] (Interface) value to [20 to 20k] (Cutoff) value.
			// Send to Cutoff slider to update params.

			auto y = slider->getValue();
			auto f = 20 * pow(2, std::log2(1000.f) * y);
			sliderMidHighCutoff.setValue(f);
		}

	}

	// 'Cutoff' slider is linear from 20 to 20k.
	// Changed by user via attachment to external sliders.
	// Slider is converted from [20 to 20k] to [0 to 1] for pixel representation

	if (slider == &sliderLowMidCutoff || slider == &sliderMidHighCutoff)
	{
		sliderMidHighInterface.setValue(mapLog2(sliderMidHighCutoff.getValue()));
		sliderLowMidInterface.setValue(mapLog2(sliderLowMidCutoff.getValue()));
	}
}

