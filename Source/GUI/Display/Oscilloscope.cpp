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
}

ScrollPad::~ScrollPad()
{
}

void ScrollPad::resized()
{
}

// Takes In Saved Points For Initialization
void ScrollPad::initializePoints(float newP1, float newP2)
{
	// Prevent Crossing Points via Parameters List
	if (point2 - point1 >= minWidth)
	{
		point1 = newP1;
		point2 = newP2;

		currentCenter = float(point1 + point2) / 2.f;
		currentWidth = point2 - point1;

		calculateZoomFactor();
	}
	else
		makeDefaultPoints();
}

// Change Points & Calculate P/Z Upon Movement
void ScrollPad::updatePoints(int x1, int y1)
{
	auto bounds = getLocalBounds();

	int edgeTolerance = bounds.getX() + bounds.getCentreX() - maxWidth / 2.f;

	// Mouse is clicked on or beyond left thumb
	if (shouldUpdatePoint1)
	{
		point1 = x1;

		if (point1 < bounds.getCentreX() - (float)maxWidth / 2.f)
			point1 = bounds.getCentreX() - (float)maxWidth / 2.f;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point1 = point2 - minWidth;

		currentCenter = (float)(point1 + point2) / 2.f;
		currentWidth = point2 - point1;
	}

	// Mouse is clicked on or beyond right thumb
	if (shouldUpdatePoint2)
	{
		point2 = x1;

		if (point2 > bounds.getCentreX() + (float)maxWidth / 2.f)
			point2 = bounds.getCentreX() + (float)maxWidth / 2.f;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point2 = point1 + minWidth;

		currentCenter = (float)(point1 + point2) / 2.f;
		currentWidth = point2 - point1;
	}

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

		currentCenter = x1 - offset;		// Center of points as a function of mouse position

		//// Disable Panning Function
		//currentCenter = bounds.getCentreX();

		// Prevent Panning Beyond Left Edge
		if (currentCenter < bounds.getX() + edgeTolerance + currentWidth / 2.f)
			currentCenter = bounds.getX() + edgeTolerance + currentWidth / 2.f;

		// Prevent Panning Beyond Right Edge
		if (currentCenter > bounds.getRight() - edgeTolerance - currentWidth / 2.f)
			currentCenter = bounds.getRight() - edgeTolerance - currentWidth / 2.f;

		// Update Points
		point1 = currentCenter - currentWidth / 2.f;
		point2 = currentCenter + currentWidth / 2.f;

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

void ScrollPad::paint(juce::Graphics& g)
{
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
}

// Reset Pan & Zoom to Default
void ScrollPad::makeDefaultPoints()
{
	auto bounds = getLocalBounds();

	currentCenter = bounds.getCentreX();
	point1 = currentCenter - defaultWidth / 2;
	point2 = currentCenter + defaultWidth / 2;

	calculateZoomFactor();
}

// Record Mouse-Down Positions
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

// Mouse Drag Callback
void ScrollPad::mouseDrag(const juce::MouseEvent& event)
{
	auto bounds = getLocalBounds();

	// Immediate Mouse Position
	x1 = event.getPosition().getX();
	y1 = event.getPosition().getY();

	updatePoints(x1, y1);
}

// Mouse Double-Click Callback
void ScrollPad::mouseDoubleClick(const juce::MouseEvent& event)
{
	makeDefaultPoints();
}

// Take Width and Derive Zoom Factor
void ScrollPad::calculateZoomFactor()
{
	currentWidth = point2 - point1;
	currentZoomFactor = juce::jmap((float)currentWidth, (float)minWidth, (float)maxWidth, minZoomFactor, maxZoomFactor);
}

// Returns Current Center Position
float ScrollPad::getCenter()
{
	// Center should represent a percentage of the total waveform.
	auto bounds = getLocalBounds().toFloat();
	auto centerScaled = juce::jmap(	(float)currentCenter,
									bounds.getCentreX() - maxWidth / 2.f, 
									bounds.getCentreX() + maxWidth / 2.f,
									+0.5f, -0.5f);

	return centerScaled;
}

// Returns Current Zoom Value
float ScrollPad::getZoom()
{
	return currentZoomFactor;
}

// Constructor
Oscilloscope::Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc)
	: audioProcessor(p), 
	lowLFO(p.lowLFO), midLFO(p.midLFO), highLFO(p.highLFO),
	globalControls(gc)
{

	//openGLContext.attachTo(*this);

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
	boolHelper(showCursor, Names::Show_Cursor_Scope);
	boolHelper(showPlayhead, Names::Show_Playhead_Scope);

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	// Cursor Position.  Rename scopeScrollParam
	floatHelper(scopeCursorParam, Names::Cursor_Position);
	floatHelper(scopePoint1Param, Names::Scope_Point1);
	floatHelper(scopePoint2Param, Names::Scope_Point2);

	sliderScroll.initializePoints(scopePoint1Param->get(), scopePoint2Param->get());
	

	dragX = scopeCursorParam->get();

	// Update Scroll Points
	
	buttonOptions.setLookAndFeel(&optionsLookAndFeel);
	buttonOptions.addListener(this);
	//buttonOptions.addMouseListener(this, true);
	addAndMakeVisible(buttonOptions);
	//buttonOptions.setButtonText("Options");

	addAndMakeVisible(sliderScroll);

	toggleShowLow.addListener(this);
	toggleShowMid.addListener(this);
	toggleShowHigh.addListener(this);
	toggleStackBands.addListener(this);

	addMouseListener(this, true);

	sliderScroll.addMouseListener(this, true);

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

void Oscilloscope::getPanZoom()
{
}

// Destructor
Oscilloscope::~Oscilloscope()
{
	//sliderScroll.setLookAndFeel(nullptr);
	buttonOptions.setLookAndFeel(nullptr);
}

// Called on component resize
void Oscilloscope::resized()
{
	auto bounds = getLocalBounds();
	bounds.reduce(4, 0);		// To account for in-set border
	bounds.removeFromBottom(5); // To account for in-set border

	sliderScroll.setSize(bounds.getWidth(), 10);
	sliderScroll.setTopLeftPosition(bounds.getX(), bounds.getBottom() - sliderScroll.getHeight());
	
	int x = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;
	float p1 = scopePoint1Param->get() / 100.f * (float)sliderScroll.getMaxWidth();
	float p2 = scopePoint2Param->get() / 100.f * (float)sliderScroll.getMaxWidth();

	/* Place Show-Menu Button */
	buttonOptions.setSize(100, 25);
	buttonOptions.setTopLeftPosition(4,4);

	/* Update ScrollPad with Loaded Points */
	sliderScroll.initializePoints(x + p1, x + p2);

	/* Update Band Display Regions Based on Params */
	updateRegions();
}

void Oscilloscope::buttonClicked(juce::Button* button)
{
	if (button == &buttonOptions)
		showMenu = !showMenu;

	drawToggles(showMenu);

	updateLfoDisplay = true;
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

	// Draw in order so that focus is on top

	
	/* Draw & Fill Low-Region */
	if (mShowLowBand)
	{
		g.setGradientFill(mLowBypass ? WAVEFORM_BYPASS_GRADIENT(lowRegion.toFloat()) : WAVEFORM_LOW_GRADIENT(lowRegion.toFloat()));
		g.setOpacity(mLowFocus ? 0.95f : 0.85f);
		g.fillPath(lowPathFill);

		g.setColour(mLowBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_LOW());
		g.setOpacity(mLowFocus ? 1.f : 0.85f);
		g.strokePath(lowPath, mLowFocus?	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
											juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
	}

	/* Draw & Fill Mid-Region */
	if (mShowMidBand)
	{
		g.setGradientFill(mMidBypass ? WAVEFORM_BYPASS_GRADIENT(midRegion.toFloat()) : WAVEFORM_MID_GRADIENT(midRegion.toFloat()));
		g.setOpacity(mMidFocus ? 0.95f : 0.85f);
		g.fillPath(midPathFill);

		g.setColour(mMidBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_MID());
		g.setOpacity(mMidFocus ? 1.f : 0.85f);
		g.strokePath(midPath, mMidFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
											juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));	
	}

	/* Draw & Fill High-Region */
	if (mShowHighBand)
	{
		g.setGradientFill(mHighBypass ? WAVEFORM_BYPASS_GRADIENT(highRegion.toFloat()) : WAVEFORM_HIGH_GRADIENT(highRegion.toFloat()));
		g.setOpacity(mHighFocus ? 0.95f : 0.85f);
		g.fillPath(highPathFill);

		g.setColour(mHighBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_HIGH());
		g.setOpacity(mHighFocus ? 1.f : 0.85f);
		g.strokePath(highPath, mHighFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
											juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
	}







	// DRAW THE MOVING PLAYHEAD
	if (mShowPlayhead && !panOrZoomChanging)
	{
		g.setColour(juce::Colours::darkgrey);
		g.drawVerticalLine(playHeadPositionPixel, lowRegion.getY(), lowRegion.getBottom());
		g.drawVerticalLine(playHeadPositionPixel, midRegion.getY(), midRegion.getBottom());
		g.drawVerticalLine(playHeadPositionPixel, highRegion.getY(), highRegion.getBottom());
	}

	// DRAW THE CURSOR
	drawAndFadeCursor(g, getLocalBounds());

	// DRAW THE OPTIONS MENU
	fadeInOutComponents(g);
}

/* Paint on top of child components */
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds().toFloat();

	/* Draw window border */
	paintBorder(g, juce::Colours::purple, bounds);
}

/* Draws the axes and grid-lines for corresponding LFO, based on tempo and zoom/pan scaling */
void Oscilloscope::drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
	using namespace AllColors::OscilloscopeColors;

	/* Used to store current beat spacing, to detect a change based on new beat spacing */
	float oldBeatSpacing = beatSpacing;

	// DRAW VERTICAL LINES =============================
	juce::Line<float> verticalAxis;

	// Convert Zoom Factor to Int
	// Represents Number of Full Quarter-Notes to be Shown
	float zoomFactor = sliderScroll.getZoom();

	g.setColour(juce::Colours::lightgrey);

	/* If all bands are stacked (overlayed), apply a transparency */
	if (mStackAllBands)
		g.setOpacity(0.35f);
	else
		g.setOpacity(0.1f);

	if (bounds.getWidth() > 0)
		beatSpacing = bounds.getWidth() / (zoomFactor);
	else
		beatSpacing = 1.f;

	// Allows Grid to Split Center
	float mDisplayPhase = sliderScroll.getCenter() * bounds.getWidth();

	auto c = bounds.getCentreX() + mDisplayPhase;
	int num = 0;

	float leftGridLine;
	float rightGridLine;

	// Draw Gridlines
	for (int i = bounds.getCentreX(); i <= 2 * bounds.getRight(); i++)
	{

		// Draw Markers at Single-Beat Intervals
		if (i % (int)beatSpacing == 0)
		{
			g.setColour(juce::Colours::darkgrey);

			// Highlight Even Markers
			if (num % 2 == 0)
				g.setColour(juce::Colours::lightgrey);

			// Highlight Origin
			if (num == 0)
				g.setColour(juce::Colours::slategrey);

			if (mStackAllBands)
				g.setOpacity(0.35f);
			else
				g.setOpacity(0.1f);

			// Draw Twice as Many Gridlines to Account for Panning
			rightGridLine = c + num * beatSpacing;

			verticalAxis.setStart(rightGridLine, bounds.getY());
			verticalAxis.setEnd(rightGridLine, bounds.getBottom());
			g.drawLine(verticalAxis, 1.f);

			leftGridLine = c - num * beatSpacing;
			verticalAxis.setStart(leftGridLine, bounds.getY());
			verticalAxis.setEnd(leftGridLine, bounds.getBottom());
			g.drawLine(verticalAxis, 1.f);

			// Calculate Left-Most Onscreen Valid (Even) Marker

			if (leftGridLine >= bounds.getX())
				leastOnscreenLeftGridLine = leftGridLine;

			// Calculate Right-Most Onscreen Valid (Even) Marker
			if (rightGridLine <= bounds.getRight())
				greatestOnscreenRightGridLine = rightGridLine;

			num++;
		}

		// Calulate Beat Information

		playBackNumBeats = 2 * num;
		playBackWidth = playBackNumBeats * beatSpacing;

		// Number of Onscreen Beats Present
		onScreenBeatWidth = greatestOnscreenRightGridLine - leastOnscreenLeftGridLine;
		onScreenNumBeats = onScreenBeatWidth / beatSpacing;

		onScreenNumBeatsLeftFromCenter = (c - leastOnscreenLeftGridLine) / beatSpacing;
		onScreenNumBeatsRightFromCenter = (greatestOnscreenRightGridLine - c) / beatSpacing;


		// If zoom changes, update the LFO display
		if (oldBeatSpacing != beatSpacing)
			updateLfoDisplay = true;
	}

	g.setColour(juce::Colours::lightgrey);

	if (mStackAllBands)
		g.setOpacity(0.35f);
	else
		g.setOpacity(0.1f);

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
	if (mShowCursor)
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

		cursor.setStart(bounds.getX() + dragX * getWidth(), scopeRegion.getY() + 1);
		cursor.setEnd(bounds.getX() + dragX * getWidth(), scopeRegion.getBottom() - 1);

		g.setColour(CURSOR_LINE);
		g.setOpacity(fadeAlphaCursor);
		g.drawLine(cursor, 3.f);
	}
}

// Methods to call on a timed-basis
void Oscilloscope::timerCallback()
{
	updatePreferences();
	updateRegions();

	repaint();

	if (checkIfLfoHasChanged(lowLFO) || 
		checkIfLfoHasChanged(midLFO) ||
		checkIfLfoHasChanged(highLFO) ||
		updateLfoDisplay)
	{

		waveTableLow = scaleWaveAmplitude(lowLFO.getWaveShapeDisplay(), lowLFO);
		generateLFOPathForDrawing(lowRegion, lowPath, lowPathFill, waveTableLow, mShowLowBand, lowLFO);

		waveTableMid = scaleWaveAmplitude(midLFO.getWaveShapeDisplay(), midLFO);
		generateLFOPathForDrawing(midRegion, midPath, midPathFill, waveTableMid, mShowMidBand, midLFO);

		waveTableHigh = scaleWaveAmplitude(highLFO.getWaveShapeDisplay(), highLFO);
		generateLFOPathForDrawing(highRegion, highPath, highPathFill, waveTableHigh, mShowHighBand, highLFO);

		updateLfoDisplay = false;
	}

	checkMousePosition();
	checkFocus();
	getPlayheadPosition();
}

bool Oscilloscope::checkIfLfoHasChanged(LFO& lfo)
{
	// Update LFO Display On...
	// Invert, Shape, Skew, Depth, Sync, Rhythm, Rate, Phase

	auto index = 0;

	if (&lfo == &midLFO)
		index = 1;

	if (&lfo == &highLFO)
		index = 2;

	// Get Parameters
	auto newInvert = lfo.getWaveInvert();
	auto newShape = lfo.getWaveform();
	auto newSkew = lfo.getWaveSkew();
	auto newDepth = lfo.getWaveDepth();
	auto newSync = lfo.isSyncedToHost();
	auto newRhythm = lfo.getWaveMultiplier();
	auto newRate = lfo.getWaveRate();
	auto newPhase = lfo.getRelativePhase();

	bool hasChanged = false;

 	// Invert
	if (oldInvert[index] != newInvert)
	{
		oldInvert[index] = newInvert;
		hasChanged = true;
	}

	// Shape
	if (oldShape[index] != newShape)
	{
		oldShape[index] = newShape;
		hasChanged = true;
	}

	// Skew
	if (oldSkew[index] != newSkew)
	{
		oldSkew[index] = newSkew;
		hasChanged = true;
	}

	// Depth
	if (oldDepth[index] != newDepth)
	{
		oldDepth[index] = newDepth;
		hasChanged = true;
	}

	// Sync
	if (oldSync[index] != newSync)
	{
		oldSync[index] = newSync;
		hasChanged = true;
	}

	// Rhythm
	if (oldRhythm[index] != newRhythm)
	{
		oldRhythm[index] = newRhythm;
		hasChanged = true;
	}

	// Rate
	if (oldRate[index] != newRate)
	{
		oldRate[index] = newRate;
		hasChanged = true;
	}

	// Phase
	if (oldPhase[index] != newPhase)
	{
		oldPhase[index] = newPhase;
		hasChanged = true;
	}

	if (hasChanged)
		return true;
	else
		return false;
		
}

// Grab WaveShapes From Processor For Display
void Oscilloscope::getWavesForDisplay()
{
	//// Pull Waves And Scale Amplitude
	//waveTableLow = scaleWaveAmplitude(lowLFO.getWaveShapeDisplay(), lowLFO);
	//waveTableMid = scaleWaveAmplitude(midLFO.getWaveShapeDisplay(), midLFO);
	//waveTableHigh = scaleWaveAmplitude(highLFO.getWaveShapeDisplay(), highLFO);
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
		float value = juce::jmap<float>(waveTable[i], min, max, 0.5f * mDepth, -0.5f * mDepth);
		waveTable.set(fmod(i, waveTable.size()), value);
	}

	return waveTable;
}

// Checks Host Playhead for Current Position
void Oscilloscope::getPlayheadPosition()
{
	/* Determine current condition of display and onscreen beats and make a decision as to how to loop playhead. 
	
	[0] To be a valid starting point, a cursor must be an even multiple.  Representing a downbeat.
	
	[1] If a valid starting/ending point is within X percent of window edge, use that.

	[2] If nearest valid starting point is too far out of tolerance, add two beats to the left/offscreen.

	[3] If nearest starting point to edge is invalid, add one beat to left/offscreen.

	[4] Given calculated starting point, distance to ending point in beats must be even.

	[5] If even number of beats, use Right-Most Ending-Point, if RMEP is within tolerance of edge.

	[6] If not within tolerance of edge, add two beats to the right/offscreen. 	*/

	auto bounds = getLocalBounds();

	// Shift to Determine Playhead Downbeat Starting Point
	float playHeadStart = bounds.getX();
	float playHeadStop = bounds.getRight();

	// If a valid starting point is within X % of beatspacing from X-Origin, use it.
	float onScreenTolerance = 0.25f;

	// Determine Left Starting Loop Point =============================================

	if ((int)onScreenNumBeatsLeftFromCenter % 2 == 0)
	{
		if (leastOnscreenLeftGridLine < beatSpacing * onScreenTolerance)
			playHeadStart = leastOnscreenLeftGridLine;
		else
			playHeadStart = leastOnscreenLeftGridLine - 2 * beatSpacing;
	}
	else
		playHeadStart = leastOnscreenLeftGridLine - beatSpacing;

	// Determine Right Ending Loop Point =============================================

	auto resultantBeatsWithNewStart = round((greatestOnscreenRightGridLine - playHeadStart) / beatSpacing);

	if ((int)resultantBeatsWithNewStart % 2 == 0)
	{
		if (bounds.getRight() - greatestOnscreenRightGridLine < beatSpacing * onScreenTolerance)
			playHeadStop = greatestOnscreenRightGridLine;
		else
			playHeadStop = greatestOnscreenRightGridLine + 2 * beatSpacing;
	}
	else
		playHeadStop = greatestOnscreenRightGridLine + beatSpacing;

	// Calculations =============================================
	auto playHeadLoopWidth = playHeadStop - playHeadStart;
	auto playHeadNumBeats = playHeadLoopWidth / beatSpacing;
	auto position = fmod((float)audioProcessor.getPlayPosition(), playHeadNumBeats) / (playHeadNumBeats);

	playHeadPositionPixel = bounds.getX() + playHeadStart + position * (playHeadLoopWidth);
}

// Update the scope-regions based on the configured viewing scheme
void Oscilloscope::updateRegions()
{
	bool update = false;

	if (oldRegions[0] != mShowLowBand)
	{
		oldRegions[0] = mShowLowBand;
		update = true;
	}

	if (oldRegions[1] != mShowMidBand)
	{
		oldRegions[1] = mShowMidBand;
		update = true;
	}

	if (oldRegions[2] != mShowHighBand)
	{
		oldRegions[2] = mShowHighBand;
		update = true;
	}

	if (oldRegions[3] != mStackAllBands)
	{
		oldRegions[3] = mStackAllBands;
		update = true;
	}

	if (update)
	{
		scopeRegion = getLocalBounds();
		scopeRegion.reduce(0, 6);
		scopeRegion.removeFromBottom(10);	// Make room for scroll slider

		lowRegion = scopeRegion;
		midRegion = scopeRegion;
		highRegion = scopeRegion;

		// Stacking Bands Displays the Individual Waveforms Separately
		if (mStackAllBands)
			drawStackedScope();
		else
		{
			lowRegion.reduce(0, 3);
			midRegion.reduce(0, 3);
			highRegion.reduce(0, 3);
		}

		updateLfoDisplay = true;
	}
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

	lowRegion.reduce(0, 3);
	midRegion.reduce(0, 3);
	highRegion.reduce(0, 3);
}








/* Generate paths for LFO when new information is available */
void Oscilloscope::generateLFOPathForDrawing(	juce::Rectangle<int> bounds,
												juce::Path &lfoStrokePath, 
												juce::Path &lfoFillPath,
												juce::Array<float> &waveTable,
												bool showBand,
												LFO lfo)
{
	using namespace juce;

	/* Vertical Midpoint of Drawing Region */
	float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

	/* Add 3px Vertical Margin */
	bounds.reduce(0, 3);

	/* Clear Previous Path Data */
	lfoFillPath.clear();
	lfoStrokePath.clear();

	if (showBand)
	{
		// Width of Area to Render Paint Data
		auto paintAreaWidth = playBackWidth;

		for (int i = 0; i <= 3 * paintAreaWidth; i++)
		{
			bool sync = lfo.isSyncedToHost();

			// Scale WaveTable by Multiplier, or by Rate, but not Both
			float scalar{ 1.f };

			if (sync)
				scalar = lfo.getWaveMultiplier();
			else
				scalar = lfo.getWaveRate() * 60.f / lfo.getHostBPM();

			// Get Relative Phase Shift
			auto mRelativePhase = lfo.getRelativePhase();

			// Get Display Phase Shift
			float mDisplayPhase = sliderScroll.getCenter() * bounds.getWidth();

			// Match Width of 1x Multiplier to Width of One Quarter-Note
			auto increment = scalar * waveTable.size() / beatSpacing;

			// Index sweeps through WaveTable Array
			float index = fmod(i * increment + mRelativePhase, waveTable.size());

			// Get value in terms of pixels.  Offset by Region's Midpoint
			float y = midY + waveTable[index] * (float)bounds.getHeight();

			// Display Offset Establishes Wave Starting Point (In-Line with Grid)
			float displayOffsetStart = bounds.getX() + bounds.getCentreX() - paintAreaWidth;

			// Incorporate Pan Shift
			int point = displayOffsetStart + mDisplayPhase;

			/* Of the calculated points, only render those which will be onscreen */
			if (point + i == bounds.getX())
			{
				lfoStrokePath.startNewSubPath(bounds.getX(), y);
				lfoFillPath.startNewSubPath(bounds.getX(), midY);
				lfoFillPath.lineTo(bounds.getX(), y);
			}

			if (point + i > bounds.getX() && point + i < bounds.getRight())
			{
				lfoStrokePath.lineTo(point + i, y);
				lfoFillPath.lineTo(point + i, y);
			}

			if (point + i == bounds.getRight())
			{
				lfoFillPath.lineTo(bounds.getRight(), y);
				lfoFillPath.lineTo(bounds.getRight(), midY);
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
	mShowCursor = showCursor->get();
	mShowPlayhead = showPlayhead->get();

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

	showCursorAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Cursor_Scope),
							toggleShowCursor);

	showPlayheadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Playhead_Scope),
							toggleShowPlayhead);
}

// Initialize and place toggle buttons.  Refactor later.
void Oscilloscope::drawToggles(bool showMenu)
{
	using namespace juce;

	/* Toggle Menu Visibility Based On Display Params */
	if (showMenu)
		buttonBounds.setBounds(buttonOptions.getX(), buttonOptions.getBottom(), buttonOptions.getWidth(), 150);
	else
		buttonBounds.setBounds(0, 0, 0, 0);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto margin = FlexItem().withHeight(5);
	auto spacer = FlexItem().withHeight(2.5);
	auto height = (150.f - 2.f * 5.f - 5.f * 2.5f) / 6.f;

	flexBox.items.add(margin);
	flexBox.items.add(FlexItem(toggleShowLow).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowMid).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowHigh).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleStackBands).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowCursor).withHeight(height));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowPlayhead).withHeight(height));
	flexBox.items.add(margin);

	flexBox.performLayout(buttonBounds);
	
	toggleShowLow.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowCursor.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowPlayhead.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowCursor.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowPlayhead.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowCursor.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowPlayhead.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);

	toggleShowLow.setButtonText("Low");
	toggleShowMid.setButtonText("Mid");
	toggleShowHigh.setButtonText("High");
	toggleStackBands.setButtonText("Stack");
	toggleShowCursor.setButtonText("Cursor");
	toggleShowPlayhead.setButtonText("Playhead");

	addAndMakeVisible(toggleShowLow);
	addAndMakeVisible(toggleShowMid);
	addAndMakeVisible(toggleShowHigh);
	addAndMakeVisible(toggleStackBands);
	addAndMakeVisible(toggleShowCursor);
	addAndMakeVisible(toggleShowPlayhead);
}

// Function to fade in view-menu.  Should optimize view-menu to its own class.
void Oscilloscope::fadeInOutComponents(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::OscilloscopeColors;

	// FADE FUNCTIONS FOR MOUSE-ENTER -> TOGGLES AND SLIDERS
	if (fadeIn || showMenu) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlpha < fadeMax)
			fadeAlpha += fadeStepUp;
		
		if (fadeAlpha > fadeMax)
		{
			fadeAlpha = fadeMax;
			//setToggleEnable(true);
		}
			
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlpha > fadeMin)
			fadeAlpha -= fadeStepDown;

		if (fadeAlpha < fadeMin)
		{
			fadeAlpha = fadeMin;
			//setToggleEnable(false);
		}
			
	}

	g.setColour(BUTTON_BACKGROUND_FILL);
	g.setOpacity(0.9f);
	g.fillRoundedRectangle(	buttonBounds.getX(),
							buttonBounds.getY(),
							buttonBounds.getWidth(),
							buttonBounds.getHeight(),
							2.f);

	buttonOptions.setAlpha(fadeAlpha);

}

void Oscilloscope::setToggleEnable(bool enabled)
{
	toggleShowLow.setEnabled(enabled);
	toggleShowMid.setEnabled(enabled);
	toggleShowHigh.setEnabled(enabled);
	toggleStackBands.setEnabled(enabled);
	toggleShowCursor.setEnabled(enabled);
	toggleShowPlayhead.setEnabled(enabled);
}

//// On mouse-enter, fade in view-menu unless dragging cursor
//void Oscilloscope::mouseEnter(const juce::MouseEvent& event)
//{
//}

// On mouse exit, fade out view-menu
void Oscilloscope::mouseExit(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	if (x < 0 || y < 0)
	{
		fadeIn = false;
	}
}

// If mouse-down within range of cursor, begin moving cursor
void Oscilloscope::mouseDown(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	bool mouseIsWithinMenu;

	if (x < buttonBounds.getRight() && y < buttonBounds.getBottom())
		mouseIsWithinMenu = true;
	else
		mouseIsWithinMenu = false;

	if (!mouseIsWithinMenu && 
		abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10 && 
		!sliderScroll.isMouseOverOrDragging() &&
		mShowCursor)
		cursorDrag = true;
}

// A mouse-up gesture indicates the cursor is done being dragged
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	auto bounds = getLocalBounds();

	cursorDrag = false;

	scopeCursorParam->setValueNotifyingHost(dragX);

	float maxWidth = (float)sliderScroll.getMaxWidth();

	int cursorX = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;

	float p1 = (float)(sliderScroll.getPoint1() - cursorX) / (float)maxWidth;
	float p2 = (float)(sliderScroll.getPoint2() - cursorX) / (float)maxWidth;

	scopePoint1Param->setValueNotifyingHost(p1);
	scopePoint2Param->setValueNotifyingHost(p2);

	panOrZoomChanging = false;

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

	if (sliderScroll.isMouseOverOrDragging())
	{
		panOrZoomChanging = true;
		updateLfoDisplay = true;
	}
	else
		panOrZoomChanging = false;
}

// Fade-In the cursor when mouse is within 10 pixels of it
void Oscilloscope::mouseMove(const juce::MouseEvent& event)
{
	auto bounds = getLocalBounds();

	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	// Check Mouse Proximity to Cursor ===================
	bool mouseIsNearCursor{ false };

	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
		mouseIsNearCursor = true;

	// Check if Cursor is Within Menu Region ==============
	bool cursorIsWithinMenu{ true };

	if (cursor.getStartX() < buttonOptions.getRight())
		cursorIsWithinMenu = true;
	else
		cursorIsWithinMenu = false;
		
	// Check If Mouse Enters Menu Region ==========
	bool mouseIsWithinMenu;

	if (x < buttonOptions.getRight() && y < buttonOptions.getBottom())
		mouseIsWithinMenu = true;
	else
		mouseIsWithinMenu = false;


	// If Mouse is Within Menu, Fade in Menu
	if (mouseIsWithinMenu)
	{
		fadeIn = true;
		fadeInCursor = false;
	}
	else
	{
		if (!showMenu)
			fadeIn = false;
	}

	// If Mouse is Within Cursor, Fade In Cursor
	if (mouseIsNearCursor)
	{
		if (mouseIsWithinMenu)
			fadeIn = true;
		else
			fadeInCursor = true;
	}
	else
		fadeInCursor = false;
}

// Reset scroll-view position on double-click
void Oscilloscope::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (!fadeIn)
	{
		if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
		{
			dragX = 0.5f;
			cursorDrag = false;
			scopeCursorParam->setValueNotifyingHost(dragX);
		}
	}

	if (sliderScroll.isMouseOver())
		updateLfoDisplay = true;

}

// Fade out the view-menu if mouse has left focus
void Oscilloscope::checkMousePosition()
{
	if (!isMouseOverOrDragging(true))
	{
		fadeInCursor = false;
		//fadeIn = false;
	}
}
