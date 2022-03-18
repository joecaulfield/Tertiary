/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	// Class containing the oscilloscope which shows 
	// the individual LFO waveforms for viewing
  ==============================================================================
*/

#include "Oscilloscope.h"

// Scroll Pad
ScrollPad::ScrollPad()
{
	// Scroll Pad allows for combined zoom and pan functionality of the oscilloscope.

	// Mouse Drag Up/Down gesture will zoom in/out
	// Mouse Drag Left/Right gesture will pan left/right

	// Dragging or clicking either one of the thumbs will change width of slider,
	// thus changing the zoom and pan

	// Center-Point between the two thumbs defines the focal point of the pan.
	// Distance between the two thumbs defines the zoom

	//slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	//slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

	//slider.addListener(this);
	//slider.addMouseListener(this, true);
	//addAndMakeVisible(slider);
}

ScrollPad::~ScrollPad()
{

}

void ScrollPad::resized()
{
	auto bounds = getLocalBounds();

	point1 = bounds.getCentreX() - currentWidth / 2;
	point2 = bounds.getCentreX() + currentWidth / 2;

	slider.setBounds(bounds);
	slider.setRange(0, bounds.getWidth(), 1);
}

void ScrollPad::paint(juce::Graphics& g)
{
	//g.fillAll(juce::Colours::hotpink);

	// Establish Bounds
	auto bounds = getLocalBounds().toFloat();

	// Draw Slider Track
	juce::Path track;
	track.startNewSubPath(	bounds.getCentreX() - maxWidth / 2.f,		bounds.getCentreY());
	track.lineTo(			bounds.getCentreX() + maxWidth / 2.f,		bounds.getCentreY());
	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	g.strokePath(track, juce::PathStrokeType(	6.f, juce::PathStrokeType::JointStyle::beveled, 
												juce::PathStrokeType::EndCapStyle::rounded));

	// Draw Point-To-Point Range
	juce::Path range;
	range.startNewSubPath(point1, bounds.getCentreY());
	range.lineTo(point2, bounds.getCentreY());
	g.setColour(juce::Colours::purple);
	g.strokePath(range, juce::PathStrokeType(	6.f, juce::PathStrokeType::JointStyle::beveled, 
												juce::PathStrokeType::EndCapStyle::rounded));

	auto size = 10;
	g.setColour(juce::Colours::lightgrey);

	g.setOpacity(0.25f);
	g.fillEllipse(point1 - size / 2, bounds.getCentreY() - size / 2, size, size);
	g.fillEllipse(point2 - size / 2, bounds.getCentreY() - size / 2, size, size);
}

void ScrollPad::mouseDown(const juce::MouseEvent& event)
{
	// Initial Mouse Position
	xDown = event.getMouseDownX();
	yDown = event.getMouseDownY();

	// Historical Mouse Position
	x0 = xDown;
	y0 = yDown;

	// Historical Point1/2 Positions
	p01 = point1;
	p02 = point2;

	// Reset Flags
	shouldUpdatePoint1 = false;
	shouldUpdatePoint2 = false;
	shouldCheckPanOrZoom = false;

	int clickTolerance = 5;

	// Mouse is clicked on or beyond Point1
	if (xDown < point1 + clickTolerance)
		shouldUpdatePoint1 = true;

	// Mouse is clicked in between points
	if (xDown >= point1 + clickTolerance && xDown <= point2 - clickTolerance)
		shouldCheckPanOrZoom = true;

	// Mouse is clicked on or beyond Point2
	if (xDown > point2 - clickTolerance)
		shouldUpdatePoint2 = true;
}

void ScrollPad::mouseDrag(const juce::MouseEvent& event)
{
	auto bounds = getLocalBounds();

	// Immediate Mouse Position
	x1 = event.getPosition().getX();
	y1 = event.getPosition().getY();

	int edgeTolerance = bounds.getX() + bounds.getCentreX() - maxWidth / 2.f;

	// Based On click, we know to update single-point only
	// Or to apply further insection to determine zoom or pan intentions

	// Mouse is clicked on or beyond left thumb
	if (shouldUpdatePoint1)
	{
		point1 = x1;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point1 = point2 - minWidth;
	}

	// Mouse is clicked on or beyond right thumb
	if (shouldUpdatePoint2)
	{
		point2 = x1;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point2 = point1 + minWidth;
	}

	//// Update Width After Single-Point Adjustment
	//currentWidth = point2 - point1;

	// Mouse is clicked in between thumbs
	if (shouldCheckPanOrZoom)
	{
		// Distance moved since last 'Starting Point'
		float xAbs = (x1 - x0); // Delta X after breaking distance threshold
		float yAbs = (y1 - y0);	// Delta Y after breaking distance threshold
		float hAbs = sqrt(pow(xAbs, 2) + pow(yAbs, 2)); // Absolute Distance

		float ang = 0.f;			// Angle of mouse-move every 3 pixels
		bool shouldZoom = false;	// Flagged if vertical mouse movement
		bool shouldPan = false;		// Flagged if horizontal mouse movement
		float distThreshold = 1;	// Distance before resetting 'Current Position'

		// Once distance-threshold is met, calculate mouse-direction and determine Pan or Zoom
		if (abs(hAbs) > distThreshold)
		{
			ang = abs(juce::radiansToDegrees(asin(yAbs / hAbs)));

			if (ang < 60)
				shouldPan = true;	// Boundary Checking On Pan Engaged
			else
				shouldZoom = true;	// Boundary Checking On Zoom Engaged
		}

		// Apply Zoom... Larger Upward Motion = Larger Zoom Increment

		if (shouldZoom)
		{
			zoomScale = juce::jmap(ang, 0.f, 90.f, 0.f, zoomIncrement);

			// When Zooming In... Shrink 'Width' until Min
			if (yAbs < 0.f)
			{
				currentWidth -= zoomScale;

				if (currentWidth < minWidth)
					currentWidth = minWidth;
			}

			// When Zooming Out... Expand 'Width' until Max
			if (yAbs > 0.f)
			{
				currentWidth += zoomScale;

				if (currentWidth > maxWidth)
					currentWidth = maxWidth;
			}
		}

		int c0 = (p01 + p02) / 2.f; // Actual Center of Points at time of Downclick
		int offset = xDown - c0;	// Distance between Mouse Down and Points-Center

		center = x1 - offset;		// Center of points as a function of mouse position

		// Prevent Panning Beyond Left Edge
		if (center < bounds.getX() + edgeTolerance + currentWidth / 2.f)
			center = bounds.getX() + edgeTolerance + currentWidth / 2.f;

		// Prevent Panning Beyond Right Edge
		if (center > bounds.getRight() - edgeTolerance - currentWidth / 2.f)
			center = bounds.getRight() - edgeTolerance - currentWidth / 2.f;

		// Update Points
		point1 = center - currentWidth / 2.f;
		point2 = center + currentWidth / 2.f;

		// 'Current' Mouse Position become 'Old' position
		x0 = x1;
		y0 = y1;
	}

	// Prevent Point1 from Protruding Left-Side
	if (point1 < bounds.getX() + edgeTolerance)
		point1 = bounds.getX() + edgeTolerance;

	// Prevent Point2 from Protruding Right-Side
	if (point2 > bounds.getRight() - edgeTolerance)
		point2 = bounds.getRight() - edgeTolerance;

	calculateZoomFactor();
}

void ScrollPad::mouseDoubleClick(const juce::MouseEvent& event)
{
	auto bounds = getLocalBounds();

	center = bounds.getCentreX();
	point1 = center - defaultWidth / 2;
	point2 = center + defaultWidth / 2;

	calculateZoomFactor();
}

void ScrollPad::calculateZoomFactor()
{

	// Define Default Width.  This equates to zoom of 1x or 100%

	// At min width, zoom should be max zoom factor.

	// At max width, zoom sohuld be min zoom factor.

	// Update Width After Single-Point Adjustment
	currentWidth = point2 - point1;

	float minZoomFactor = 0.5f;
	float maxZoomFactor = 10.f;

	zoomFactor = juce::jmap((float)currentWidth, (float)minWidth, (float)maxWidth, minZoomFactor, maxZoomFactor);

	//float roundToTheNearest = 0.5f;

	//zoomFactor = zoomFactor - fmod(zoomFactor, roundToTheNearest);
}

void ScrollPad::sliderValueChanged(juce::Slider* slider)
{
	// Change in slider value implies panning

	//x1 = slider->getValue();
}

float ScrollPad::getCenter()
{
	// Center should represent a percentage of the total waveform.
	auto bounds = getLocalBounds().toFloat();
	auto centerScaled = juce::jmap(	(float)center, 
									bounds.getCentreX() - maxWidth / 2.f, 
									bounds.getCentreX() + maxWidth / 2.f,
									+0.5f, -0.5f);

	return centerScaled;
}

// Constructor
Oscilloscope::Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc)
	: audioProcessor(p), 
	lowLFO(p.lowLFO), midLFO(p.midLFO), highLFO(p.highLFO),
	globalControls(gc)
{

	using namespace Params;             // Create a Local Reference to Parameter Mapping
	const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

	auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)    // Bool Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	boolHelper(showLowBand, Names::Show_Low_Scope);
	boolHelper(showMidBand, Names::Show_Mid_Scope);
	boolHelper(showHighBand, Names::Show_High_Scope);
	boolHelper(stackBands, Names::Stack_Bands_Scope);

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	// Cursor Position.  Rename scopeScrollParam
	floatHelper(scopeScrollParam, Names::Cursor_Position);
	dragX = scopeScrollParam->get();


	// Scroll-Bar Position
	//floatHelper(displayPhase, Names::Scope_Scroll);



	//sliderScroll.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	//sliderScroll.setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
	//sliderScroll.setLookAndFeel(&scrollLookAndFeel);
	addAndMakeVisible(sliderScroll);

	addMouseListener(this, true);

	sampleRate = audioProcessor.getSampleRate();
	makeAttachments();

	updatePreferences();

	startTimerHz(30);

	// Initialize WaveTable Arrays
	waveTableLow.resize(sampleRate);	// Initialize Wavetable
	waveTableLow.clearQuick();			// Initialize Wavetable

	waveTableMid.resize(sampleRate);	// Initialize Wavetable
	waveTableMid.clearQuick();			// Initialize Wavetable

	waveTableHigh.resize(sampleRate);	// Initialize Wavetable
	waveTableHigh.clearQuick();			// Initialize Wavetable
}

// Destructor
Oscilloscope::~Oscilloscope()
{
	//sliderScroll.setLookAndFeel(nullptr);
}

// Called on component resize
void Oscilloscope::resized()
{
	drawToggles();

	auto bounds = getLocalBounds();
	bounds.reduce(4, 0);		// To account for in-set border
	bounds.removeFromBottom(5); // To account for in-set border

	sliderScroll.setSize(bounds.getWidth(), 10);
	sliderScroll.setTopLeftPosition(bounds.getX(), bounds.getBottom() - sliderScroll.getHeight());
}

// Graphics class
void Oscilloscope::paint(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::OscilloscopeColors;

	auto bounds = getLocalBounds().toFloat();

	// Set Gradient Fill
	g.setColour(juce::Colours::black);
	g.fillAll();

	// DRAW REGION OUTLINES ============

	if (!mShowLowBand && !mShowMidBand && !mShowHighBand)
	{
		g.setGradientFill(BACKGROUND_GRADIENT(scopeRegion.toFloat()));
		g.fillRoundedRectangle(scopeRegion.toFloat(), 5.f);
	}

	if (mShowLowBand)
	{
		g.setGradientFill(BACKGROUND_GRADIENT(lowRegion.toFloat()));
		g.fillRoundedRectangle(lowRegion.toFloat(), 5.f);
	}

	if (mShowMidBand)
	{
		g.setGradientFill(BACKGROUND_GRADIENT(midRegion.toFloat()));
		g.fillRoundedRectangle(midRegion.toFloat(), 5.f);
	}

	if (mShowHighBand)
	{
		g.setGradientFill(BACKGROUND_GRADIENT(highRegion.toFloat()));
		g.fillRoundedRectangle(highRegion.toFloat(), 5.f);
	}

	// DRAW AXIS IF NO BANDS SHOWN =========
	if (!mShowLowBand && !mShowMidBand && !mShowHighBand)
		drawAxis(g, scopeRegion);	// Grid when nothing shown

	// DRAW GRIDS IF LOW-BAND SHOWN
	if (mShowLowBand)
		drawAxis(g, lowRegion);

	// DRAW GRIDS IF MID-BAND SHOWN
	if (mShowMidBand)
		drawAxis(g, midRegion);

	// DRAW GRIDS IF HIGH-BAND SHOWN
	if (mShowHighBand)
		drawAxis(g, highRegion);

	// DRAW & FILL LOW REGION
	g.setGradientFill(mLowBypass ? WAVEFORM_BYPASS_GRADIENT(lowRegion.toFloat()) : WAVEFORM_LOW_GRADIENT(lowRegion.toFloat()));
	g.setOpacity(mLowFocus ? 0.95 : 0.85);
	g.fillPath(lowPathFill);

	g.setColour(mLowBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_LOW());
	g.setOpacity(mLowFocus ? 1.f : 0.85f);
	g.strokePath(lowPath, mLowFocus?	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW & FILL MID REGION
	g.setGradientFill(mMidBypass ? WAVEFORM_BYPASS_GRADIENT(midRegion.toFloat()) : WAVEFORM_MID_GRADIENT(midRegion.toFloat()));
	g.setOpacity(mMidFocus ? 0.95 : 0.85);
	g.fillPath(midPathFill);

	fadeInOutComponents(g);
}

// Graphics class which covers components
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	using namespace AllColors::OscilloscopeColors;

	auto bounds = getLocalBounds().toFloat();

	g.setColour(mMidBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_MID());
	g.setOpacity(mMidFocus ? 1.f : 0.85f);
	g.strokePath(midPath, mMidFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW & FILL HIGH REGION
	g.setGradientFill(mHighBypass ? WAVEFORM_BYPASS_GRADIENT(highRegion.toFloat()) : WAVEFORM_HIGH_GRADIENT(highRegion.toFloat()));
	g.setOpacity(mHighFocus ? 0.95 : 0.85);
	g.fillPath(highPathFill);

	g.setColour(mHighBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_HIGH());
	g.setOpacity(mHighFocus ? 1.f : 0.85f);
	g.strokePath(highPath, mHighFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW REGION OUTLINES
	g.setColour(juce::Colour(0xff252323));
	g.setOpacity(1.f);

	float width = 2.f; // Line Width

	if (!mShowLowBand && !mShowMidBand && !mShowHighBand)
		g.drawRoundedRectangle(scopeRegion.toFloat(), 5.f, width);

	if (mShowLowBand)
		g.drawRoundedRectangle(lowRegion.toFloat(),  5.f, width);

	if (mShowMidBand)
		g.drawRoundedRectangle(midRegion.toFloat(),  5.f, width);

	if (mShowHighBand)
		g.drawRoundedRectangle(highRegion.toFloat(), 5.f, width);

	drawAndFadeCursor(g, getLocalBounds());

	// Draw the Moving Playhead
	g.setColour(juce::Colours::darkgrey);
	g.drawVerticalLine(playHeadPositionPixel, lowRegion.getY(), lowRegion.getBottom());
	g.drawVerticalLine(playHeadPositionPixel, midRegion.getY(), midRegion.getBottom());
	g.drawVerticalLine(playHeadPositionPixel, highRegion.getY(), highRegion.getBottom());

	paintBorder(g, juce::Colours::purple, bounds);
}

// Draw the gridlines within the corresponding region
void Oscilloscope::drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
	using namespace AllColors::OscilloscopeColors;

	// DRAW VERTICAL LINES =============================
	juce::Line<float> verticalAxis;
	auto numLines = 2;

	g.setColour(juce::Colours::lightgrey);
	g.setOpacity(0.35f);

	for (int i = 1; i <= numLines; i++)
	{
		verticalAxis.setStart(bounds.getX() + bounds.getWidth() * (float)i/(numLines+1) , bounds.getY());
		verticalAxis.setEnd(bounds.getX() + bounds.getWidth() * (float) i/(numLines + 1), bounds.getY() + bounds.getHeight());
		g.drawLine(verticalAxis, 1.f);
	}

	// DRAW HORIZONTAL LINES =============================
	for (int i = 1; i < numDepthLines; i++)
	{
		auto y = bounds.getY() + i * (bounds.getHeight() / numDepthLines);
		g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
	}
}

// Draw the cursor.  Fade based on mouse-focus.
void Oscilloscope::drawAndFadeCursor(juce::Graphics& g, juce::Rectangle<int> bounds)
{
	using namespace AllColors::OscilloscopeColors;

	// FADE FUNCTIONS FOR MOUSE-ENTER -> TOGGLES AND SLIDERS
	if (fadeInCursor) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursor < fadeMaxCursor)
			fadeAlphaCursor += fadeStepUpCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursor > fadeMinCursor)
			fadeAlphaCursor -= fadeStepDownCursor;
	}

	if (fadeAlphaCursor > fadeMaxCursor)
		fadeAlphaCursor = fadeMaxCursor;

	if (fadeAlphaCursor < fadeMinCursor)
		fadeAlphaCursor = fadeMinCursor;

	cursor.setStart(bounds.getX() + dragX*getWidth(), scopeRegion.getY()+1);
	cursor.setEnd(bounds.getX() + dragX * getWidth(), scopeRegion.getBottom()-1);

	g.setColour(CURSOR_LINE);
	g.setOpacity(fadeAlphaCursor);
	g.drawLine(cursor, 3.f);
}

// Methods to call on a timed-basis
void Oscilloscope::timerCallback()
{
	timerCounter++;

	if (timerCounter = 10)
	{
		getWavesForDisplay();
		timerCounter = 0;
	}



	checkMousePosition();
	updateRegions();
	updatePreferences();
	checkFocus();
	repaint();
	getPlayheadPosition();

}

// Grab WaveShapes From Processor For Display
void Oscilloscope::getWavesForDisplay()
{
	// Pull Waves And Scale Amplitude
	waveTableLow = scaleWaveAmplitude(lowLFO.getWaveShapeDisplay(), lowLFO);
	waveTableMid = scaleWaveAmplitude(midLFO.getWaveShapeDisplay(), midLFO);
	waveTableHigh = scaleWaveAmplitude(highLFO.getWaveShapeDisplay(), highLFO);

	// Time-Shifting
	//waveTableLow = addWavePhaseShift(waveTableLow, lowLFO);
	//waveTableMid = addWavePhaseShift(waveTableMid, midLFO);
	//waveTableHigh = addWavePhaseShift(waveTableHigh, highLFO);

	// Time-Scaling
	//waveTableLow = scaleWaveTime(waveTableLow, lowLFO);
	//waveTableMid = scaleWaveTime(waveTableMid, midLFO);
	//waveTableHigh = scaleWaveTime(waveTableHigh, highLFO);
}

// Maps incoming WaveTable amplitude from [0, -1] to [-0.5, +0.5]
juce::Array<float> Oscilloscope::scaleWaveAmplitude(juce::Array<float> waveTable, LFO lfo)
{
	float mDepth = lfo.getWaveDepth();

	float min = 2.0f;		// Used to calculate min value of LFO, for scaling
	float max = -1.0f;		// Used to calculate max value of LFO, for scaling

	for (int sample = 0; sample < waveTable.size(); sample++)
	{
		// Get Min and Max
		if (waveTable[sample] < min)
			min = waveTable[sample];

		if (waveTable[sample] > max)
			max = waveTable[sample];
	}

	// MAP AMPLITUDE TO CENTER AROUND 0 ===================
	for (int i = 0; i < waveTable.size(); i++)
	{
		float value = juce::jmap<float>(waveTable[i], min, max, -0.5f * mDepth, 0.5f * mDepth);
		waveTable.set(fmod(i, waveTable.size()), value);
	}

	return waveTable;
}

// Introduces Relative Phase Shift + Scroll-Bar Display Phase Shift
juce::Array<float> Oscilloscope::addWavePhaseShift(juce::Array<float> waveTable, LFO lfo)
{
	juce::Array<float> waveTableShifted;

	waveTableShifted.resize(waveTable.size());
	waveTableShifted.clearQuick();

	// Calculate Relative Phase Shift
	float mRelativePhase = lfo.getRelativePhase();

	// Phase Shift = Number of Samples of WaveTable to Offset
	//mDisplayPhase = sliderScroll.getCenter() / 360.f;

	// Calculate Display Phase Shift
	// Refactor so that mDisplayPhase is not the value of one slider thumb, 
	// but the difference between the two.
	
	//float mDisplayPhase = displayPhase->get() / 360.f;

	float mDisplayPhase = sliderScroll.getCenter();

	if (mDisplayPhase <= 0)
		mDisplayPhase = abs(mDisplayPhase) * waveTable.size();
	else
		mDisplayPhase = waveTable.size() * (1 - mDisplayPhase);

	// Add Phase Shifts
	for (int i = 0; i < waveTable.size(); i++)
	{
		auto revolvingIndex = fmod(i + mRelativePhase + mDisplayPhase, waveTable.size());
		waveTableShifted.set(i, waveTable[revolvingIndex]);
	}

	return waveTableShifted;
}

// Scales incoming WaveTable Frequency based on Rate, Rhythm and Display Params
juce::Array<float> Oscilloscope::scaleWaveTime(juce::Array<float> waveTable, LFO lfo)
{
	juce::Array<float> waveTableScaled;

	waveTableScaled.resize(waveTable.size());
	waveTableScaled.clearQuick();

	int numCycles = 2;

	auto sync = lfo.isSyncedToHost();

	float zoomFactor = sliderScroll.getZoom();

	//float scalar = numCycles * sliderScroll.getZoom();

	//zoomFactor = 1.f;

	// Scale WaveTable by Multiplier, or by Rate, but not Both
	float scalar;

	if (sync)
		scalar = lfo.getWaveMultiplier() * zoomFactor;
	else
		scalar = lfo.getWaveRate() * zoomFactor;

	// Scale By Multiplier if Applicable
	for (int i = 0; i < waveTable.size(); i++)
	{
		waveTableScaled.set(i, waveTable[fmod(i * scalar * numCycles, waveTable.size())]);
	}

	return waveTableScaled;
}

// Checks Host Playhead for Current Position
void Oscilloscope::getPlayheadPosition()
{
	auto numBeats = 8;

	// Retrieve Play Position From Processor
	
	auto position = fmod(audioProcessor.getPlayPosition(), numBeats) / numBeats;
	playHeadPositionPixel = position * scopeRegion.getWidth();
}

// Update the scope-regions based on the configured viewing scheme
void Oscilloscope::updateRegions()
{
	scopeRegion = getLocalBounds();
	scopeRegion.reduce(0, 6);
	scopeRegion.removeFromBottom(10);	// Make room for scroll slider

	lowRegion = scopeRegion;
	midRegion = scopeRegion;
	highRegion = scopeRegion;

	if (mStackAllBands)
		drawStackedScope();

	//drawLowLFO(lowRegion);
	//drawMidLFO(midRegion);
	//drawHighLFO(highRegion);

	drawLFO(lowRegion, lowPath, lowPathFill, waveTableLow, mShowLowBand, lowLFO);
	drawLFO(midRegion, midPath, midPathFill, waveTableMid, mShowMidBand, midLFO);
	drawLFO(highRegion, highPath, highPathFill, waveTableHigh, mShowHighBand, highLFO);
}

// Draw the scope boundaries based on the configured viewing scheme
void Oscilloscope::drawStackedScope()
{
	int count = 1;

	// Convert Show-Choices to Binary Counter
	int x{ 0 }, y{ 0 }, z{ 0 };

	if (mShowLowBand) x = 1;
	if (mShowMidBand) y = 1;
	if (mShowHighBand) z = 1;
	int sum = x*100 + y*10 + z;

	switch (sum)	// Determines the total number of bands to be displayed
	{
		case 0: count = 1; numDepthLines = 10;  break;
		case 1: count = 1; numDepthLines = 10;  break;
		case 10: count = 1; numDepthLines = 10; break;
		case 11: count = 2; numDepthLines = 6; break;
		case 100: count = 1; numDepthLines = 10; break;
		case 101: count = 2; numDepthLines = 6; break;
		case 110: count = 2; numDepthLines = 6; break;
		case 111: count = 3; numDepthLines = 4; break;
	}

	auto width = scopeRegion.getWidth();
	auto height = scopeRegion.getHeight() / count;

	lowRegion.setBounds(scopeRegion.getX(), scopeRegion.getY(), width, height * x);
	midRegion.setBounds(scopeRegion.getX(), scopeRegion.getY() + lowRegion.getHeight(), width, height * y);
	highRegion.setBounds(scopeRegion.getX(), scopeRegion.getY() + lowRegion.getHeight() + midRegion.getHeight(), width, height * z);

	scopeRegion.reduce(0, 3);
	lowRegion.reduce(0, 3);
	midRegion.reduce(0, 3);
	highRegion.reduce(0, 3);
}

void Oscilloscope::drawLFO(	juce::Rectangle<int> bounds,
							juce::Path &lfoStrokePath, 
							juce::Path &lfoFillPath,
							juce::Array<float> &waveTable,
							bool showBand,
							LFO lfo)
{
	using namespace juce;

	float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

	bounds.reduce(3, 3);

	lfoFillPath.clear();
	lfoStrokePath.clear();

	if (showBand)
	{
		int dataSize = 10 * bounds.getWidth();

		
		// Build Stroke Path
		for (int i = 0; i <= 2 * dataSize; i++)
		{
			// Get Zoom Factor ===========================================

			int numCycles = 2;	// Hard-Coded Base Number of Cycles
			auto sync = lfo.isSyncedToHost();	// Check if Synced
			float zoomFactor = sliderScroll.getZoom();	// Display Zoom Factor

			// Scale WaveTable by Multiplier, or by Rate, but not Both
			float scalar;

			if (sync)
				scalar = lfo.getWaveMultiplier() * zoomFactor;
			else
				scalar = lfo.getWaveRate() * zoomFactor;

			// Get Wave's Relative Phase Shift ==================
			auto mRelativePhase = lfo.getRelativePhase();

			// Get Display's Scroll Phase Shift ==================
			float mDisplayPhase = sliderScroll.getCenter() * bounds.getWidth();

			// Get WaveTable Value and Convert to Pixel Value Y Coordinate
			auto increment = scalar * waveTable.size() / (bounds.getWidth());
			float index = fmod(i * increment + mRelativePhase, waveTable.size());
			float y = midY + waveTable[index] * (float)bounds.getHeight();

			// Start New Subpath
			if (i == 0)
			{
				lfoStrokePath.startNewSubPath(bounds.getX() + mDisplayPhase - dataSize, y);
				
				lfoFillPath.startNewSubPath(bounds.getX() + mDisplayPhase - dataSize, midY);
				lfoFillPath.lineTo(bounds.getX() + mDisplayPhase - dataSize, y);
			}
			else 
			{
				lfoStrokePath.lineTo(bounds.getX() + mDisplayPhase + i - dataSize, y);
				lfoFillPath.lineTo(bounds.getX() + mDisplayPhase + i - dataSize, y);
			}
		}
	}
}

// Check for external hover focus.  Refactor for encapsulation.
void Oscilloscope::checkFocus()
{
	mLowFocus = (	globalControls.mTimingControls.timingBarLow.hasFocus || 
					globalControls.mGainControls.gainBarLow.hasFocus ||
					globalControls.mWaveControls.waveBarLow.hasFocus );

	mMidFocus = (	globalControls.mTimingControls.timingBarMid.hasFocus || 
					globalControls.mGainControls.gainBarMid.hasFocus ||
					globalControls.mWaveControls.waveBarMid.hasFocus );

	mHighFocus = (	globalControls.mTimingControls.timingBarHigh.hasFocus || 
					globalControls.mGainControls.gainBarHigh.hasFocus ||
					globalControls.mWaveControls.waveBarHigh.hasFocus );
}

// Update parameter values
void Oscilloscope::updatePreferences()
{
	using namespace Params;
	const auto& params = GetParams();

	mShowLowBand = showLowBand->get();
	mShowMidBand = showMidBand->get();
	mShowHighBand = showHighBand->get();
	mStackAllBands = stackBands->get();

	mLowBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
	mMidBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
	mHighBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
}

// Make component->parameter attachments
void Oscilloscope::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	showLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Low_Scope),
							toggleShowLow);

	showMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Mid_Scope),
							toggleShowMid);

	showHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_High_Scope),
							toggleShowHigh);

	stackBandsAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Stack_Bands_Scope),
							toggleStackBands);

	//scrollAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
	//						audioProcessor.apvts,
	//						params.at(Names::Scope_Scroll),
	//						sliderScroll);
}

// Initialize and place toggle buttons.  Refactor later.
void Oscilloscope::drawToggles()
{
	using namespace juce;

	buttonBounds.setBounds(scopeRegion.getX()+5, scopeRegion.getY()+5, 65, 70);
	buttonBounds.reduce(3, 3);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withHeight(2.5);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowLow).withHeight(20));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowMid).withHeight(20));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowHigh).withHeight(20));
	flexBox.items.add(spacer);

	flexBox.performLayout(buttonBounds);
	
	toggleShowLow.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);

	toggleShowLow.setButtonText("Low");
	toggleShowMid.setButtonText("Mid");
	toggleShowHigh.setButtonText("High");
	toggleStackBands.setButtonText("Stack");

	addAndMakeVisible(toggleShowLow);
	addAndMakeVisible(toggleShowMid);
	addAndMakeVisible(toggleShowHigh);
}

// Function to fade in view-menu.  Should optimize view-menu to its own class.
void Oscilloscope::fadeInOutComponents(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::OscilloscopeColors;

	// FADE FUNCTIONS FOR MOUSE-ENTER -> TOGGLES AND SLIDERS
	if (fadeIn && !fadeInCursor) // If mouse entered... fade Toggles Alpha up
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

	g.setColour(BUTTON_BACKGROUND_FILL);
	g.setOpacity(fadeAlpha*0.55f);
	g.fillRoundedRectangle(buttonBounds, 5.f);

	toggleShowLow.setAlpha(fadeAlpha);
	toggleShowMid.setAlpha(fadeAlpha);
	toggleShowHigh.setAlpha(fadeAlpha);
	toggleStackBands.setAlpha(fadeAlpha);
}

// On mouse-enter, fade in view-menu unless dragging cursor

// On mouse exit, fade out view-menu
void Oscilloscope::mouseExit(const juce::MouseEvent& event)
{
	fadeIn = false;
}

// If mouse-down within range of cursor, begin moving cursor
void Oscilloscope::mouseDown(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10 && !sliderScroll.isMouseOverOrDragging())
		cursorDrag = true;
}

// A mouse-up gesture indicates the cursor is done being dragged
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	cursorDrag = false;
	scopeScrollParam->setValueNotifyingHost(dragX);
}

// Move the cursor when it is dragged
void Oscilloscope::mouseDrag(const juce::MouseEvent& event)
{
	auto margin = 7;

	if (cursorDrag)
	{
		dragX = event.getPosition().getX() / (float)getWidth();

		if (event.getPosition().getX() < margin)
			dragX = margin / (float)getWidth();

		if (event.getPosition().getX() > (float)getWidth() - margin)
			dragX = ((float)getWidth() - margin) / getWidth();
	}
}

// Fade-In the cursor when mouse is within 10 pixels of it
void Oscilloscope::mouseMove(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10 && !sliderScroll.isMouseOverOrDragging())
	{
		fadeInCursor = true;
	}
	else fadeInCursor = false;
}

// Reset scroll-view position on double-click
void Oscilloscope::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
	{
		dragX = 0.5f;
		cursorDrag = false;
		scopeScrollParam->setValueNotifyingHost(dragX);
	}
}

// Fade out the view-menu if mouse has left focus
void Oscilloscope::checkMousePosition()
{
	if (!isMouseOverOrDragging(true))
	{
		fadeInCursor = false;
		fadeIn = false;
	}
}