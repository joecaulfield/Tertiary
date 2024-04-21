/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	Class containing the oscilloscope which shows 
	the individual LFO waveforms for viewing
 
  ==============================================================================
*/

#include "Oscilloscope.h"

// ========================================================
Oscilloscope::Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc)
	:   audioProcessor(p),
        globalControls(gc),
        lowLFO(p.lowLFO), midLFO(p.midLFO), highLFO(p.highLFO),
        lowScope(p.apvts, p.lowLFO, sliderScroll),
        midScope(p.apvts, p.midLFO, sliderScroll),
        highScope(p.apvts, p.highLFO, sliderScroll)
{

	using namespace Params;             // Create a Local Reference to Parameter Mapping
	const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	floatHelper(scopeCursorParam,   Names::Cursor_Position);
	floatHelper(scopePoint1Param,   Names::Scope_Point1);
	floatHelper(scopePoint2Param,   Names::Scope_Point2);

	sliderScroll.initializePoints(scopePoint1Param->get(), scopePoint2Param->get());
	
    addAndMakeVisible(lowScope);
    addAndMakeVisible(midScope);
    addAndMakeVisible(highScope);
    
	mCursorPosition = scopeCursorParam->get();

	addAndMakeVisible(sliderScroll);
    
	addMouseListener(this, true);

	sliderScroll.addMouseListener(this, true);

	startTimerHz(30);

    // Set Scope Channels as Listeners to the Scroll Slider
    sliderScroll.addActionListener(&lowScope);
    sliderScroll.addActionListener(&midScope);
    sliderScroll.addActionListener(&highScope);

    //setBufferedToImage(true);
    //setOpaque(true);
}

// ========================================================
Oscilloscope::~Oscilloscope()
{
}


// ========================================================
void Oscilloscope::resized()
{

    /* Rebuild the current scope layout */
    buildScopeLayout();
    
    /* Fixed Position for Slider */
    sliderScroll.setSize(getWidth(), 25);
    sliderScroll.setTopLeftPosition(getWidth()/2-sliderScroll.getWidth()/2, getBottom()-25);
    



    /* Initialize the pan/zoom points for the scroll bar... a little rusty on this! */
	int x = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;
	float p1 = scopePoint1Param->get() / 100.f * (float)sliderScroll.getMaxWidth();
	float p2 = scopePoint2Param->get() / 100.f * (float)sliderScroll.getMaxWidth();

	/* Update ScrollPad with Loaded Points */
	sliderScroll.initializePoints(x + p1, x + p2);

}

/* Receive updated options parameters from wrapper & options menu */
// ========================================================
void Oscilloscope::updateScopeParameters(bool showLow, bool showMid, bool showHigh, bool stackBands)
{
    mShowLowBand = showLow;
    mShowMidBand = showMid;
    mShowHighBand = showHigh;
    mStackAllBands = stackBands;
    
    buildScopeLayout();
}

/* Determine layout style and perform accordingly */
// ========================================================
void Oscilloscope::buildScopeLayout()
{

    // Perform the oscilloscope layout
    if (mStackAllBands)
        buildStackedScopeLayout();
    else
        buildOverlappedScopeLayout();
}

/* Performed stacked layout.  Scopes side-by-side */
// ========================================================
void Oscilloscope::buildStackedScopeLayout()
{
    using namespace juce;

    /* Define the scope channels as 'stacked'.  Updates background color */
    // ========================================================
    lowScope.setBandsStacked(true);
    midScope.setBandsStacked(true);
    highScope.setBandsStacked(true);
    
    /* Define area in which the scope channels should exist */
    scopeRegion = getLocalBounds();
    scopeRegion.reduce(0, 6);
    scopeRegion.removeFromBottom(15);    // Make room for scroll slider
    
    /* Creates a flex-box list of the channels which will be displayed */
    // ========================================================
    
    juce::FlexBox flexBox;
    
    flexBox.flexDirection = FlexBox::Direction::column;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    auto spacer = FlexItem().withHeight(5);    // Gap between O-Scope and Freq Resp

    /* Show low band, or hide */
    // ========================================================
    if (mShowLowBand)
    {
        flexBox.items.add(FlexItem(lowScope).withFlex(1.f));
        flexBox.items.add(spacer);
    }
    else
        lowScope.setSize(0, 0);

    /* Show mid band, or hide */
    // ========================================================
    if (mShowMidBand)
    {
        flexBox.items.add(FlexItem(midScope).withFlex(1.f));
        flexBox.items.add(spacer);
    }
    else
        midScope.setSize(0, 0);
    
    /* Show high band, or hide */
    // ========================================================
    if (mShowHighBand)
    {
        flexBox.items.add(FlexItem(highScope).withFlex(1.f));
    }
    else
        highScope.setSize(0, 0);

    flexBox.performLayout(scopeRegion);
}

/* Performed overlapped layout.  Scopes overlapped */
// ========================================================
void Oscilloscope::buildOverlappedScopeLayout()
{
    /* Defines the scope channels as 'not stacked', or 'overlapped'.  Adjusts background colors */
    lowScope.setBandsStacked(false);
    midScope.setBandsStacked(false);
    highScope.setBandsStacked(false);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowLowBand)
        lowScope.setBounds(scopeRegion);
    else
        lowScope.setSize(0, 0);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowMidBand)
        midScope.setBounds(scopeRegion);
    else
        midScope.setSize(0, 0);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowHighBand)
        highScope.setBounds(scopeRegion);
    else
        highScope.setSize(0, 0);
    
    /* Adjust the transparancy of each channel */
    // ========================================================
    lowScope.setAlpha(1.f);
    midScope.setAlpha(1.f);
    highScope.setAlpha(1.f);
    
    float dimValue = 0.25f;
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mLowFocus && mShowLowBand) {
        lowScope.toFront(false);
        midScope.setAlpha(dimValue);
        highScope.setAlpha(dimValue);
    }
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mMidFocus && mShowMidBand) {
        midScope.toFront(false);
        lowScope.setAlpha(dimValue);
        highScope.setAlpha(dimValue);
    }
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mHighFocus && mShowHighBand) {
        highScope.toFront(false);
        lowScope.setAlpha(dimValue);
        midScope.setAlpha(dimValue);
    }

}

/* Receives notifications if a band has focus, and updates display accordingly */
// ========================================================
bool Oscilloscope::checkForChangesToFocus()
{

    /* Flag system to store old focus and detect new focus.  Ensures repainting on triggered focus-change */
    
    oldFocus[0] = mLowFocus;
    mLowFocus = globalControls.lowBandControls.isMouseOverOrDragging(true);

    oldFocus[1] = mMidFocus;
    mMidFocus = globalControls.midBandControls.isMouseOverOrDragging(true);

    oldFocus[2] = mHighFocus;
    mHighFocus = globalControls.highBandControls.isMouseOverOrDragging(true);

    /* Compare parameters */
    if (oldFocus[0] != mLowFocus)
    {
        oldFocus[0] = mLowFocus;
        return true;
    }
    
    /* Compare parameters */
    if (oldFocus[1] != mMidFocus)
    {
        oldFocus[1] = mMidFocus;
        return true;
    }
    
    /* Compare parameters */
    if (oldFocus[2] != mHighFocus)
    {
        oldFocus[2] = mHighFocus;
        return true;
    }
    
    return false;
}

/* Timer Callback */
// ========================================================
void Oscilloscope::timerCallback()
{

    checkMousePosition();
    fadeInOutCursor();

    /* Updates band appearance and Z-direction based on mouse focus */
    // ========================================================
    if (checkForChangesToFocus())
    {
        lowScope.setBandFocus(mLowFocus);
        midScope.setBandFocus(mMidFocus);
        highScope.setBandFocus(mHighFocus);
        
        // Rebuild the overlapped band layout, to reorder in the Z-Direction
        if (!mStackAllBands)
            buildOverlappedScopeLayout();

    }
    
    /* Not sure */
    // ========================================================
    //if (updateLfoDisplay || cursorDrag)
    //{
        //repaint();
        //updateLfoDisplay = false;
    //}
    
    /* Not sure */
    // ========================================================
    //if (!fadeCompleteCursor && mShowCursor)
    //    repaint(cursor.getStartX()-5, cursor.getStartY(), 10, cursor.getLength() );
    
    /* Not currently in use. Move to wrapper */
    // ========================================================
    getPlayheadPosition();
}







// ========================================================
void Oscilloscope::paint(juce::Graphics& g)
{
	using namespace juce;

    /* Paint Background */
    // ========================================================
    g.setColour(juce::Colours::black);
    g.fillAll();

    /* Paint Moving Playhead. Not in use */
    // ========================================================
    //paintPlayhead(g);
    
    /* Paint Cursor. Not in use */
    // ========================================================
    //paintCursor(g);

}

// ========================================================
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds().toFloat();

	/* Draw window border */
	paintBorder(g, ColorScheme::WindowBorders::getWindowBorderColor(), bounds);

}

// ========================================================
void Oscilloscope::paintCursor(juce::Graphics& g)
{
    using namespace AllColors::OscilloscopeColors;
    
    auto bounds = getLocalBounds();
    
    if (mShowCursor)
    {
        cursor.setStart(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getY() + 1);
        cursor.setEnd(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getBottom() - 1);

        g.setColour(juce::Colours::white);
        g.setOpacity(fadeAlphaCursor);
        g.drawLine(cursor, 3.f);
    }
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

/* A mouse-up gesture indicates the cursor is done being dragged */
// ========================================================
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	cursorDrag = false;

	scopeCursorParam->setValueNotifyingHost(mCursorPosition);   // Get Rid?

	float maxWidth = (float)sliderScroll.getMaxWidth();

    // Get rid?  Cursor is 86'ed
	int cursorX = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;

    // Store the current pan & zoom settings in memory
	float p1 = (float)(sliderScroll.getPoint1() - cursorX) / (float)maxWidth;
	float p2 = (float)(sliderScroll.getPoint2() - cursorX) / (float)maxWidth;

    // Store the current pan & zoom settings in memory
	scopePoint1Param->setValueNotifyingHost(p1);
	scopePoint2Param->setValueNotifyingHost(p2);

    // Was used when playhead was active... still need since playhead is 86'ed?
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
}

/* A double-click near the cursor or on the ScrollBar resets default values */
// ========================================================
void Oscilloscope::mouseDoubleClick(const juce::MouseEvent& event)
{
	/* If a double-click occurs near the cursor, reset the cursor to center */
    if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
    {
        //mCursorPosition = 0.5f;
        //cursorDrag = false;
        //scopeCursorParam->setValueNotifyingHost(mCursorPosition);
        //updateLfoDisplay = true;
    }

	/* If the scrollbar is double-clicked, pan and zoom have been reset to default,
	so update the display accordingly. */
	if (sliderScroll.isMouseOver())
    {
        //updateLfoDisplay = true;
        //lowScope.redrawScope();
        //midScope.redrawScope();
        //highScope.redrawScope();
    }
    


}

/* Fade out the cursor if mouse has left focus */
// ========================================================
void Oscilloscope::checkMousePosition()
{
	if (!isMouseOverOrDragging(true))
		fadeInCursor = false;
}

/* Fade Cursor Function */
// ========================================================
void Oscilloscope::fadeInOutCursor()
{
    if (fadeInCursor) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursor < fadeMaxCursor)
        {
            fadeCompleteCursor = false;
            fadeAlphaCursor += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursor > fadeMaxCursor)
        {
            fadeAlphaCursor = fadeMaxCursor;
            fadeCompleteCursor = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursor > fadeMinCursor)
        {
            fadeCompleteCursor = false;
            fadeAlphaCursor -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursor < fadeMinCursor)
        {
            fadeAlphaCursor = fadeMinCursor;
            fadeCompleteCursor = true;
        }
    }
}


