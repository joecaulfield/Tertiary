/*
  ==============================================================================

	FrequencyResponse.cpp
	Created: 30 Dec 2021 11:38:20am
	Author:  Joe Caulfield

	Class to display and control the frequency response of the crossover

  ==============================================================================
*/

/*

TO DO ==========

[1] Slow down the FFT. Look into smoothing the values.
[2] Pop-Up Menu to Disable RTA

*/

#include "FrequencyResponse.h"

// Constructor
FrequencyResponse::FrequencyResponse(	TertiaryAudioProcessor& p, 
										juce::AudioProcessorValueTreeState& apv, 
										GlobalControls& gc)
	: audioProcessor(p),
	globalControls(gc),
    apvts(apv),
	forwardFFT(audioProcessor.fftOrder),
	window(audioProcessor.fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris)
{

	using namespace Params;
	const auto& params = GetParams();

	auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)    // Bool Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	boolHelper(showFftParameter, Names::Show_FFT);

	mShowFFT = showFftParameter->get();

	// Choice Helper To Attach Choice to Parameter ========
	auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
	{
		param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
		jassert(param != nullptr);
	};

	choiceHelper(fftPickoffParameter, Names::FFT_Pickoff);

	mPickOffID = fftPickoffParameter->getIndex();
	updateToggleStates();

	//switch (pickOffID)
	//{
	//	case 0: togglePickInput.setToggleState(true, false); break;		// Pre
	//	case 1: togglePickOutput.setToggleState(true, false); break;	// Post
	//}

	// Array Maintenance ==========
	fftDrawingPoints.ensureStorageAllocated(audioProcessor.scopeSize);
	fftDrawingPoints.resize(audioProcessor.scopeSize);

	for (int i = 0; i < audioProcessor.scopeSize; i++)
		fftDrawingPoints.setUnchecked(i, 0);

	// Button Options
	buttonOptions.setLookAndFeel(&optionsLookAndFeel);
	buttonOptions.addListener(this);
	buttonOptions.addMouseListener(this, true);
	addAndMakeVisible(buttonOptions);

	toggleShowRTA.addListener(this);
	togglePickInput.addListener(this);
	togglePickOutput.addListener(this);

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
	//addMouseListener(this, false);
}

FrequencyResponse::~FrequencyResponse()
{
	buttonOptions.setLookAndFeel(nullptr);
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

	showFftAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
						audioProcessor.apvts,
						params.at(Names::Show_FFT),
						toggleShowRTA);
}

// Graphics class
void FrequencyResponse::paint(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::FrequencyResponseColors;

	auto bounds = getLocalBounds().toFloat();

	g.setGradientFill(BACKGROUND_GRADIENT(bounds));
	g.fillAll();


	drawGridVertical(g);
	drawGridHorizontal(g);

	if (mShowFFT)
		paintFFT(g, responseArea);

	/* If low-band is hovered, draw low-band on top with highlight */
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

	//fadeInOutComponents(g);
	fadeButtons(g);

	paintBorder(g, juce::Colours::purple, bounds);


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
		g.setOpacity(0.35f);
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

		g.setColour(juce::Colours::lightgrey);
		g.setOpacity(0.35f);
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

	/* Place Show-Menu Button */
	buttonOptions.setSize(100, 25);
	buttonOptions.setTopLeftPosition(4, 4);
}

// Methods to call on a timed-basis
void FrequencyResponse::timerCallback()
{
	checkMousePosition();

	updateResponse();
	//fadeInOutComponents();
	fadeCursor();
	fadeRegions();
	checkExternalFocus();
	repaint();

	// Check for new FFT information
	if (audioProcessor.nextFFTBlockReady)
	{
		drawNextFrameOfSpectrum();
		audioProcessor.nextFFTBlockReady = false;
	}
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

	// Set Low-Mid Crossover Cursor Bounds
	cursorLM.setStart(freq1Pixel, responseArea.getY() + 1);
	cursorLM.setEnd(freq1Pixel, responseArea.getBottom() - 1);

	cursorMH.setStart(freq2Pixel, responseArea.getY() + 1);
	cursorMH.setEnd(freq2Pixel, responseArea.getBottom() - 1);

	// Set Mid-High Crossover Cursor Bounds
	mLowGain = *apvts.getRawParameterValue(params.at(Names::Gain_Low_Band));
	mMidGain = *apvts.getRawParameterValue(params.at(Names::Gain_Mid_Band));
	mHighGain = *apvts.getRawParameterValue(params.at(Names::Gain_High_Band));

	// Convert band gain values to relative pixel values
	gainLowPixel = responseArea.getY() + juce::jmap(mLowGain,	-30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
	gainMidPixel = responseArea.getY() + juce::jmap(mMidGain,	-30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
	gainHighPixel = responseArea.getY() + juce::jmap(mHighGain, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());

	// Set Cursor Widths. Cursors A Fraction of Band Width
	auto lowWidth = (freq1Pixel - responseArea.getX()) * 0.75f;
	auto midWidth = (freq2Pixel - freq1Pixel) * 0.75f;
	auto highWidth = (responseArea.getRight() - freq2Pixel) * 0.75f;

	// Set Low-Gain Cursor Bounds
	auto center = (responseArea.getX() + freq1Pixel) / 2.f;

	cursorLG.setStart	(center - lowWidth / 2.f, gainLowPixel);
	cursorLG.setEnd		(center + lowWidth / 2.f, gainLowPixel);

	// Set Mid-Gain Cursor Bounds
	center = (freq1Pixel + freq2Pixel) / 2.f;
	
	cursorMG.setStart	(center - midWidth / 2.f, gainMidPixel);
	cursorMG.setEnd		(center + midWidth / 2.f, gainMidPixel);

	// Set High-Gain Cursor Bounds
	center = (freq2Pixel + responseArea.getRight()) / 2.f;

	cursorHG.setStart	(center - highWidth / 2.f, gainHighPixel);
	cursorHG.setEnd		(center + highWidth / 2.f, gainHighPixel);

	// Update Band Bypass States
	lowBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
	midBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
	highBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));

	// Update Band Solo States
	lowBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Low_Band));
	midBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Mid_Band));
	highBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_High_Band));

	// Update Band Mute States
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
	if (!checkMenuFocus(event))
		checkCursorFocus(event);
}

// Called on Mouse Enter
void FrequencyResponse::mouseEnter(const juce::MouseEvent& event)
{
	if (!checkMenuFocus(event))
		checkCursorFocus(event);
}

// Called on Mouse Exit
void FrequencyResponse::mouseExit(const juce::MouseEvent& event)
{
	if (!checkMenuFocus(event))
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

	// Prevent Control of Params if Cursor is within Shown Options Menu

	//bool	lmCursorIsInMenu{ false },
	//		mhCursorIsInMenu{ false },
	//		lgCursorIsInMenu{ false },
	//		mgCursorIsInMenu{ false },
	//		hgCursorIsInMenu{ false };

	//// Check if LM Cursor is in Menu
	//if (cursorLM.getStartX() < buttonBounds.getRight())
	//	lmCursorIsInMenu = true;

	//// Check if MH Cursor is in Menu
	//if (cursorMH.getStartX() < buttonBounds.getRight())
	//	mhCursorIsInMenu = true;

	//// Check if LG Cursor is in Menu
	//auto lgCenter = cursorLG.getStartX() + cursorLG.getEndX() / 2.f;

	//if (lgCenter < buttonBounds.getRight() && cursorLG.getStartY() < buttonBounds.getBottom())
	//	lgCursorIsInMenu = true;

	//// Check if MG Cursor is in Menu
	//auto mgCenter = cursorMG.getStartX() + cursorMG.getEndX() / 2.f;

	//if (mgCenter < buttonBounds.getRight() && cursorMG.getStartY() < buttonBounds.getBottom())
	//	mgCursorIsInMenu = true;

	//// Check if HG Cursor is in Menu
	//auto hgCenter = cursorHG.getStartX() + cursorHG.getEndX() / 2.f;

	//if (hgCenter < buttonBounds.getRight() && cursorHG.getStartY() < buttonBounds.getBottom())
	//	hgCursorIsInMenu = true;

	//if (lmCursorIsInMenu && showMenu)
	//	fadeInCursorLM = false;

	//if (mhCursorIsInMenu && showMenu)
	//	fadeInCursorMH = false;


	// Mouse is out of window, kill all fades
	if (xM < x0 || xM > x1 || yM < y0 || yM > y1)
	{
		fadeRegionLG = false;
		fadeRegionMG = false;
		fadeRegionHG = false;
		fadeInCursorLM = false;
		fadeInCursorMH = false;
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

bool FrequencyResponse::checkMenuFocus(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	bool mouseIsInMenu{ false };

	if (x > buttonBounds.getX() && x < buttonBounds.getRight() &&
		y > buttonBounds.getY() && y < buttonBounds.getBottom())
		mouseIsInMenu = true;

	if (buttonOptions.isMouseOverOrDragging() || mouseIsInMenu)
	{
		fadeInButton = true;
		fadeInCursorLM = false;
		fadeInCursorMH = false;
		fadeInCursorLG = false;
		fadeInCursorMG = false;
		fadeInCursorHG = false;
		return true;
	}
	else
	{
		fadeInButton = false;
		return false;
	}		
}

// Fade Components on Mouse Enter
void FrequencyResponse::fadeButtons(juce::Graphics& g)
{
	if (fadeInButton || showMenu) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaButton < fadeMaxButton)
			fadeAlphaButton += fadeStepUpButton;

		if (fadeAlphaButton > fadeMaxButton)
			fadeAlphaButton = fadeMaxButton;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaButton > fadeMinButton)
			fadeAlphaButton -= fadeStepDownButton;

		if (fadeAlphaButton < fadeMinButton)
			fadeAlphaButton = fadeMinButton;
	}

	g.setColour(juce::Colours::white);
	g.setOpacity(0.9f);
	g.fillRoundedRectangle(	buttonBounds.getX(),
							buttonBounds.getY(),
							buttonBounds.getWidth(),
							buttonBounds.getHeight(),
							2.f);

	buttonOptions.setAlpha(fadeAlphaButton);
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
		fadeInButton = false;
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

void FrequencyResponse::paintFFT(juce::Graphics& g, juce::Rectangle<float> bounds)
{
	// Wrap in Toggle Button

	for (int i = 1; i < audioProcessor.scopeSize; ++i)
	{
		float width = bounds.getWidth();
		float height = bounds.getHeight();

		float startX = bounds.getX() + (float)juce::jmap(	(float)i - 1, 
															0.f,
															(float)audioProcessor.scopeSize - 1.f,
															0.f, 
															width);

		float startY = bounds.getY() + juce::jmap(	audioProcessor.scopeData[i - 1], 
													0.0f,
													1.0f, 
													height, 
													0.0f);

		float endX = bounds.getX() + juce::jmap(	(float)i, 
													0.f,
													(float)audioProcessor.scopeSize - 1.f,
													0.f,
													width);
		
		float endY = bounds.getY() + juce::jmap(	audioProcessor.scopeData[i],		
													0.0f,
													1.0f, 
													height, 
													0.0f);

		fftDrawingPoints.set(i - 1, startY);
	}

	juce::Path f;

    f.startNewSubPath(bounds.getX() + 2, bounds.getBottom());

	int curve = 3;

	float x0{ 0.f }, x1{ 0.f }, x2{ 0.f };

    for (int i = 0; i < fftDrawingPoints.size()-curve-1; i++)
    {
		if (i % curve == 0 && i > 0)
		{
			float x0 = juce::jmap(	(float)i-curve, 
									0.f, 
									(float)(fftDrawingPoints.size()) - 1.f,
									2.f, 
									bounds.getWidth()-2 );

			float x1 = juce::jmap(	(float)i, 
									0.f, 
									(float)(fftDrawingPoints.size()) - 1.f,
									2.f, 
									bounds.getWidth()-2 );

			float x2 = juce::jmap(	(float)i+ curve,
									0.f, 
									(float)(fftDrawingPoints.size()) - 1.f,
									2.f, 
									bounds.getWidth()-2 );

			juce::Point<float> point0 = { bounds.getX() + x0, fftDrawingPoints[i - curve] - 2 };
			juce::Point<float> point1 = { bounds.getX() + x1, fftDrawingPoints[i] - 2 };
			juce::Point<float> point2 = { bounds.getX() + x2, fftDrawingPoints[i + curve] - 2 };

			f.cubicTo(point0, point1, point2);
		}

    }

	f.lineTo(bounds.getRight(), bounds.getBottom());
	f.closeSubPath();

	// Fill FFT Background
	float p1 = 0.25f;
	float p2 = 0.5f;

	auto gradient = juce::ColourGradient(	juce::Colours::grey,
											bounds.getBottomLeft(),
											juce::Colours::lightgrey,
											bounds.getTopRight(), false);

	gradient.addColour(p1, juce::Colours::white.withBrightness(1.25f));
	gradient.addColour(p2, juce::Colours::whitesmoke.withBrightness(1.25f));

	g.setGradientFill(gradient);
	g.setOpacity(0.75f);
	g.fillPath(f);

	// Fill FFT Outline 
    g.setColour(juce::Colours::darkgrey);
    g.setOpacity(0.75f);
    g.strokePath(f, juce::PathStrokeType(0.5f));

}

void FrequencyResponse::drawNextFrameOfSpectrum()
{
	// Apply Window Function to Data
	window.multiplyWithWindowingTable(audioProcessor.fftData, audioProcessor.fftSize);

	// Render FFT Data
	forwardFFT.performFrequencyOnlyForwardTransform(audioProcessor.fftData);

	auto mindB = -60.f;
	auto maxdB = 0.0f;

	for (int i = 0; i < audioProcessor.scopeSize; ++i)
	{
		//auto skewedProportionX = 1.0f - pow(2, std::log2(1.0f - (float)i / (float)processor.scopeSize * 0.2f));
		//auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)processor.scopeSize) * 0.2f);
		//auto linearScale = i / float(processor.scopeSize) * 0.835f;

		auto linearScale = i / float(audioProcessor.scopeSize) * 9.699f;
		auto skewedProportionX = (20 * pow(2, linearScale)) / 20000;

        // 
        auto fftDataIndex = juce::jlimit	(   0,                                                          // Lower
												audioProcessor.fftSize / 2,                                      // Upper Limit
												(int)(skewedProportionX * (float)audioProcessor.fftSize * 0.5f)  // Value to Constrain
											);

        // Clamps the value to within specified dB range
        auto limit = juce::jlimit	(	mindB,      // Lower Limit
										maxdB,      // Upper Limit
										juce::Decibels::gainToDecibels(audioProcessor.fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)audioProcessor.fftSize)  // Value to Constrain
									);


        auto level = juce::jmap (	limit,  // Source Value
									mindB,  // Source Range Min
									maxdB,  // Source Range Max
									0.0f,   // Target Range Min
									1.0f    // Target Range Max
								);

        audioProcessor.scopeData[i] = level;                                   // [4]
	}


}

void FrequencyResponse::drawToggles(bool showMenu)
{
	using namespace juce;

	/* Toggle Menu Visibility Based On Display Params */
	if (showMenu)
		buttonBounds.setBounds(buttonOptions.getX(), buttonOptions.getBottom(), buttonOptions.getWidth(), 75);
	else
		buttonBounds.setBounds(0, 0, 0, 0);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto margin = FlexItem().withHeight(5);
	auto spacer = FlexItem().withHeight(2.5);
	auto height = (150.f - 2.f * 5.f - 5.f * 2.5f) / 6.f;

	flexBox.items.add(margin);
	flexBox.items.add(FlexItem(toggleShowRTA).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(togglePickInput).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(togglePickOutput).withHeight(height));
	flexBox.items.add(margin);

	flexBox.performLayout(buttonBounds);

	toggleShowRTA.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	togglePickInput.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	togglePickOutput.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

	toggleShowRTA.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	togglePickInput.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	togglePickOutput.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	
	toggleShowRTA.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
	togglePickInput.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
	togglePickOutput.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);

	toggleShowRTA.setButtonText("Show FFT");
	togglePickInput.setButtonText("FFT In");
	togglePickOutput.setButtonText("FFT Out");

	togglePickInput.setRadioGroupId(1);
	togglePickOutput.setRadioGroupId(1);

	addAndMakeVisible(toggleShowRTA);
	addAndMakeVisible(togglePickInput);
	addAndMakeVisible(togglePickOutput);
}

void FrequencyResponse::buttonClicked(juce::Button* button)
{
	if (button == &buttonOptions)
	{
		showMenu = !showMenu;
		drawToggles(showMenu);
	}
		

	if (button == &toggleShowRTA)
	{
		mShowFFT = toggleShowRTA.getToggleState();
	}
		
	if (button == &togglePickInput || button == &togglePickOutput)
	{
		mPickOffID = togglePickOutput.getToggleState();

		updateToggleStates();
	}
	
}

void FrequencyResponse::updateToggleStates()
{
	// When Pickoff ID Changes,
	// Update Toggle States
	// Set Parameter

	switch (mPickOffID)
	{
		case 0: // Pre
		{
			togglePickInput.setToggleState(true, false);
			togglePickOutput.setToggleState(false, false); break;
		}
		case 1: // Post
		{
			togglePickOutput.setToggleState(true, false);
			togglePickInput.setToggleState(false, false); break;
		}
	}

	fftPickoffParameter->setValueNotifyingHost(mPickOffID);

	audioProcessor.setFftPickoffPoint(mPickOffID);
}
