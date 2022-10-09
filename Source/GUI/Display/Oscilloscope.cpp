/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	Class containing the oscilloscope which shows 
	the individual LFO waveforms for viewing


	Tasks:
	[1] Clean up fade in / fade out approaches
	[2] This class appears to be hogging graphical resources on some windows machines, especially older ones.  Implementing OpenGL helps on some machines, makes things worse on others!
	[3] Some code can be cleaned up by creating its own classes: Cursor, Menu Bar
	[4] A lot of stuff happening on the timer callback... anyway to alleviate this load?
	[5] Simplify the 'Draw & Fill xxx-Region' sections by creating its own function
  ==============================================================================
*/

#include "Oscilloscope.h"

/* Constructor */
// ========================================================
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

	boolHelper(showLowBandParam, Names::Show_Low_Scope);
	boolHelper(showMidBandParam, Names::Show_Mid_Scope);
	boolHelper(showHighBandParam, Names::Show_High_Scope);
	boolHelper(stackBandsParam, Names::Stack_Bands_Scope);
	boolHelper(showCursorParam, Names::Show_Cursor_Scope);
	boolHelper(showPlayheadParam, Names::Show_Playhead_Scope);

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	floatHelper(scopeCursorParam, Names::Cursor_Position);
	floatHelper(scopePoint1Param, Names::Scope_Point1);
	floatHelper(scopePoint2Param, Names::Scope_Point2);

	sliderScroll.initializePoints(scopePoint1Param->get(), scopePoint2Param->get());
	

	mCursorPosition = scopeCursorParam->get();

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

/* Destructor */
// ========================================================
Oscilloscope::~Oscilloscope()
{
	//sliderScroll.setLookAndFeel(nullptr);
	buttonOptions.setLookAndFeel(nullptr);
}

/* Component Resized Callback */
// ========================================================
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

/* Opens or closes the options menu */
// ========================================================
void Oscilloscope::buttonClicked(juce::Button* button)
{
	if (button == &buttonOptions)
		showMenu = !showMenu;

	drawToggles(showMenu);

	updateLfoDisplay = true;
}

/* Graphics Class */
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

/* Graphics Class to Paint over Components */
// ========================================================
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds().toFloat();

	/* Draw window border */
	paintBorder(g, juce::Colours::purple, bounds);
}

/* Draws the axes and grid-lines for corresponding LFO, based on tempo and zoom/pan scaling */
// ========================================================
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

/* Draw the cursor.  Fade when mouse moves */
// ========================================================
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

		cursor.setStart(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getY() + 1);
		cursor.setEnd(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getBottom() - 1);

		g.setColour(CURSOR_LINE);
		g.setOpacity(fadeAlphaCursor);
		g.drawLine(cursor, 3.f);
	}
}

/* Timer Callback */
// ========================================================
void Oscilloscope::timerCallback()
{
	updatePreferences();
	updateRegions();

	repaint();

	/* If the parameters of the LFO have changed, regenerate the values and drawing information for the LFO */
	if (checkIfLfoHasChanged(lowLFO) || 
		checkIfLfoHasChanged(midLFO) ||
		checkIfLfoHasChanged(highLFO) ||
		updateLfoDisplay)
	{
		waveTableLow = scaleWaveAmplitude(lowLFO.getWaveTable(), lowLFO);
		generateLFOPathForDrawing(lowRegion, lowPath, lowPathFill, waveTableLow, mShowLowBand, lowLFO);

		waveTableMid = scaleWaveAmplitude(midLFO.getWaveTable(), midLFO);
		generateLFOPathForDrawing(midRegion, midPath, midPathFill, waveTableMid, mShowMidBand, midLFO);

		waveTableHigh = scaleWaveAmplitude(highLFO.getWaveTable(), highLFO);
		generateLFOPathForDrawing(highRegion, highPath, highPathFill, waveTableHigh, mShowHighBand, highLFO);

		updateLfoDisplay = false;
	}

	checkMousePosition();
	checkFocus();
	getPlayheadPosition();
}

/* Update LFO Drawing on Parameter Change */
// ========================================================
bool Oscilloscope::checkIfLfoHasChanged(LFO& lfo)
{
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

/* Checks host for the current playhead position */
// ========================================================
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

/* Updates the scope region visibility based on current view configuration */
// ========================================================
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

/* Draws the scope region based on current view configuration */
// ========================================================
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

/* Scales incoming WaveTable by Amplitude Parameter */
// ========================================================
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

/* Generate paths for LFO when new information is available */
// ========================================================
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

/* Checks for external hover focus */
// ========================================================
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

/* Update parameter values */
// ========================================================
void Oscilloscope::updatePreferences()
{
	using namespace Params;
	const auto& params = GetParams();

	mShowLowBand = showLowBandParam->get();
	mShowMidBand = showMidBandParam->get();
	mShowHighBand = showHighBandParam->get();
	mStackAllBands = stackBandsParam->get();
	mShowCursor = showCursorParam->get();
	mShowPlayhead = showPlayheadParam->get();

	mLowBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
	mMidBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
	mHighBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
}

/* Component parameter attachments */
// ========================================================
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

/* Initialize and draw toggle buttons */
// ========================================================
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

/* Fade in the view menu */
// ========================================================
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
	g.setOpacity(0.9f);
	g.fillRoundedRectangle(	buttonBounds.getX(),
							buttonBounds.getY(),
							buttonBounds.getWidth(),
							buttonBounds.getHeight(),
							2.f);

	buttonOptions.setAlpha(fadeAlpha);
}

/* Sets toggle menu to be enabled */
// ========================================================
void Oscilloscope::setToggleEnable(bool enabled)
{
	toggleShowLow.setEnabled(enabled);
	toggleShowMid.setEnabled(enabled);
	toggleShowHigh.setEnabled(enabled);
	toggleStackBands.setEnabled(enabled);
	toggleShowCursor.setEnabled(enabled);
	toggleShowPlayhead.setEnabled(enabled);
}

/* On mouse exit, fade out menu */
// ========================================================
void Oscilloscope::mouseExit(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	if (x < 0 || y < 0)
		fadeIn = false;
}

/* If mouse-down within range of cursor, begin moving cursor but only if the menu is not open */
// ========================================================
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

/* A mouse-up gesture indicates the cursor is done being dragged */
// ========================================================
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	cursorDrag = false;

	scopeCursorParam->setValueNotifyingHost(mCursorPosition);

	float maxWidth = (float)sliderScroll.getMaxWidth();

	int cursorX = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;

	float p1 = (float)(sliderScroll.getPoint1() - cursorX) / (float)maxWidth;
	float p2 = (float)(sliderScroll.getPoint2() - cursorX) / (float)maxWidth;

	scopePoint1Param->setValueNotifyingHost(p1);
	scopePoint2Param->setValueNotifyingHost(p2);

	panOrZoomChanging = false;
}

/* Move the cursor while it is being dragged */
// ========================================================
void Oscilloscope::mouseDrag(const juce::MouseEvent& event)
{
	auto margin = 7;

	if (cursorDrag)
	{
		mCursorPosition = event.getPosition().getX() / (float)getWidth();

		if (event.getPosition().getX() < margin)
			mCursorPosition = margin / (float)getWidth();

		if (event.getPosition().getX() > (float)getWidth() - margin)
			mCursorPosition = ((float)getWidth() - margin) / getWidth();
	}

	/* Force the display to continually update as the pan and zoom changes */
	if (sliderScroll.isMouseOverOrDragging())
	{
		panOrZoomChanging = true;
		updateLfoDisplay = true;
	}
	else
		panOrZoomChanging = false;
}

/* Fade-in the cursor when the mouse is within 10 pixels of it */
// ========================================================
void Oscilloscope::mouseMove(const juce::MouseEvent& event)
{

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

/* A double-click near the cursor or on the ScrollBar resets default values */
// ========================================================
void Oscilloscope::mouseDoubleClick(const juce::MouseEvent& event)
{
	/* If a double-click occurs near the cursor, reset the cursor to center */
	if (!fadeIn)
	{
		if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
		{
			mCursorPosition = 0.5f;
			cursorDrag = false;
			scopeCursorParam->setValueNotifyingHost(mCursorPosition);
		}
	}

	/* If the scrollbar is double-clicked, pan and zoom have been reset to default,
	so update the display accordingly. */
	if (sliderScroll.isMouseOver())
		updateLfoDisplay = true;
}

/* Fade out the cursor if mouse has left focus */
// ========================================================
void Oscilloscope::checkMousePosition()
{
	if (!isMouseOverOrDragging(true))
		fadeInCursor = false;
}