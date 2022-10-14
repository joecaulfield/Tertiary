/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	Class containing the oscilloscope which shows 
	the individual LFO waveforms for viewing


	Tasks:
	[3] Some code can be cleaned up by creating its own classes: Cursor, Menu Bar
    [6] For draw gridlines, can I separate calculations from paint functions to alleviate graphics overhead?
    [7] Work on paint-flags for playhead
    [8] Overall code cleanup.  Optimization appears to be in a good spot, just need to organize and document the code.
    [9] Combine both fade-in functions into one function
    [10] Cleanup math functions
 

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
	addAndMakeVisible(buttonOptions);

	addAndMakeVisible(sliderScroll);

	toggleShowLow.addListener(this);
	toggleShowMid.addListener(this);
	toggleShowHigh.addListener(this);
	toggleStackBands.addListener(this);
    toggleShowCursor.addListener(this);

	addMouseListener(this, true);

	sliderScroll.addMouseListener(this, true);

	sampleRate = audioProcessor.getSampleRate();
	makeAttachments();

	//updatePreferences();

	startTimerHz(30);

	// Initialize WaveTable Arrays
	waveTableLow.resize(sampleRate);	// Initialize Wavetable
	waveTableLow.clearQuick();			// Initialize Wavetable

	waveTableMid.resize(sampleRate);	// Initialize Wavetable
	waveTableMid.clearQuick();			// Initialize Wavetable

	waveTableHigh.resize(sampleRate);	// Initialize Wavetable
	waveTableHigh.clearQuick();			// Initialize Wavetable
    
    setBufferedToImage(true);
    setOpaque(true);
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
	updateAndCheckForChangesToDisplayPreferences();
}

/* Timer Callback */
// ========================================================
void Oscilloscope::timerCallback()
{
    updateAndCheckForChangesToDisplayPreferences();
    checkMousePosition();
    fadeInOutMenu();
    fadeInOutCursor();

    /* Check for changes to Low LFO Parameters */
    if (checkForChangesToLfo(lowLFO) || updateLfoDisplay)
    {
        waveTableLow = scaleWaveAmplitude(lowLFO.getWaveTable(), lowLFO);
        generateLFOPathForDrawing(lowRegion, lowPath, lowPathFill, waveTableLow, mShowLowBand, lowLFO);
        repaint(lowRegion);
    }

    /* Check for changes to Mid LFO Parameters */
    if (checkForChangesToLfo(midLFO)|| updateLfoDisplay)
    {
        waveTableMid = scaleWaveAmplitude(midLFO.getWaveTable(), midLFO);
        generateLFOPathForDrawing(midRegion, midPath, midPathFill, waveTableMid, mShowMidBand, midLFO);
        repaint(midRegion);
    }

    /* Check for changes to High LFO Parameters */
    if (checkForChangesToLfo(highLFO) || updateLfoDisplay)
    {
        waveTableHigh = scaleWaveAmplitude(highLFO.getWaveTable(), highLFO);
        generateLFOPathForDrawing(highRegion, highPath, highPathFill, waveTableHigh, mShowHighBand, highLFO);
        repaint(highRegion);
    }
    
    if (updateLfoDisplay || cursorDrag)
    {
        repaint();
        updateLfoDisplay = false;
    }
    
    if (!fadeCompleteMenu)
        repaint(buttonOptions.getBounds());
    
    if (!fadeCompleteCursor && mShowCursor)
        repaint(cursor.getStartX()-5, cursor.getStartY(), 10, cursor.getLength() );
    
    getPlayheadPosition();

}

/* Component parameter attachments */
// ========================================================
void Oscilloscope::makeAttachments()
{
    using namespace Params;
    const auto& params = GetParams();

    showLowAttachment =        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Show_Low_Scope),
                            toggleShowLow);

    showMidAttachment =        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Show_Mid_Scope),
                            toggleShowMid);

    showHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Show_High_Scope),
                            toggleShowHigh);

    stackBandsAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Stack_Bands_Scope),
                            toggleStackBands);

    showCursorAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Show_Cursor_Scope),
                            toggleShowCursor);

    showPlayheadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            audioProcessor.apvts,
                            params.at(Names::Show_Playhead_Scope),
                            toggleShowPlayhead);
}








/* PAINT FUNCTIONS ================================================================================================ */
/* ================================================================================================================ */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */

void Oscilloscope::paint(juce::Graphics& g)
{
    DBG("Scope Background Paint");
    
	using namespace juce;
	using namespace AllColors::OscilloscopeColors;

    /* Paint Background */
    // =========================
	g.setColour(juce::Colours::black);
	g.fillAll();

    /* Paint Borders */
    // =========================
    paintRegions(g);
    
    /* Paint Grid Lines */
    // =========================
    
    /* Nothing shown */
    if (!mShowLowBand && !mShowMidBand && !mShowHighBand) paintGridLines(g, scopeRegion);

    /* Low-band shown */
    if (mShowLowBand) paintGridLines(g, lowRegion);

    /* Mid-band shown */
    if (mShowMidBand) paintGridLines(g, midRegion);

    /* High-band shown */
    if (mShowHighBand) paintGridLines(g, highRegion);

    /* Paint Low LFO Waveform */
    // =========================
    paintWaveform(g, mShowLowBand, mLowBypass, lowRegion, mLowFocus, lowPath, lowPathFill, WAVEFORM_LOW_GRADIENT(lowRegion.toFloat() ), REGION_BORDER_COLOR_LOW());
    
    /* Paint Mid LFO Waveform */
    // =========================
    paintWaveform(g, mShowMidBand, mMidBypass, midRegion, mMidFocus, midPath, midPathFill, WAVEFORM_MID_GRADIENT(midRegion.toFloat() ), REGION_BORDER_COLOR_MID() );
    
    /* Paint High LFO Waveform */
    // =========================
    paintWaveform(g, mShowHighBand, mHighBypass, highRegion, mHighFocus, highPath, highPathFill, WAVEFORM_HIGH_GRADIENT(highRegion.toFloat() ), REGION_BORDER_COLOR_HIGH() );

    /* Paint Moving Playhead */
    // =========================
    //paintPlayhead(g);
    
    /* Paint Cursor */
    // =========================
    paintCursor(g);

    /* Paint Menu */
    // =========================
    paintMenu(g);
}

void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds().toFloat();

	/* Draw window border */
	paintBorder(g, juce::Colours::purple, bounds);
}

/* Paint the regions in which each LFO will sit */
void Oscilloscope::paintRegions(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
    
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
}

/* Position of gridlines scale with zoom/pan factors. */
void Oscilloscope::paintGridLines(juce::Graphics& g, juce::Rectangle<int> bounds)
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

void Oscilloscope::paintWaveform(   juce::Graphics& g,
                                    bool shouldShowBand,
                                    bool isBandBypassed,
                                    juce::Rectangle<int> regionBounds,
                                    bool isBandFocused,
                                    juce::Path pathToDraw,
                                    juce::Path pathToFill,
                                    juce::ColourGradient fillGradient,
                                    juce::Colour strokeColor)
{
    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
    
    if (shouldShowBand)
    {
        /* Fill */
        g.setGradientFill(isBandBypassed ? WAVEFORM_BYPASS_GRADIENT(regionBounds.toFloat()) : fillGradient);
        g.setOpacity(isBandFocused ? 0.95f : 0.85f);
        g.fillPath(pathToFill);

        /* Stroke */
        g.setColour(isBandBypassed ? REGION_BORDER_COLOR_BYPASS() : strokeColor);
        g.setOpacity(isBandFocused ? 1.f : 0.85f);
        g.strokePath(pathToDraw, isBandFocused? juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) :
                                                juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
    }
}

void Oscilloscope::paintPlayhead(juce::Graphics& g)
{
//    if (mShowPlayhead && !panOrZoomChanging)
//    {
//        g.setColour(juce::Colours::darkgrey);
//        g.drawVerticalLine(playHeadPositionPixel, lowRegion.getY(), lowRegion.getBottom());
//        g.drawVerticalLine(playHeadPositionPixel, midRegion.getY(), midRegion.getBottom());
//        g.drawVerticalLine(playHeadPositionPixel, highRegion.getY(), highRegion.getBottom());
//    }
}

void Oscilloscope::paintCursor(juce::Graphics& g)
{
    using namespace AllColors::OscilloscopeColors;
    
    auto bounds = getLocalBounds();
    
    if (mShowCursor)
    {
        cursor.setStart(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getY() + 1);
        cursor.setEnd(bounds.getX() + mCursorPosition * getWidth(), scopeRegion.getBottom() - 1);

        g.setColour(CURSOR_LINE);
        g.setOpacity(fadeAlphaCursor);
        g.drawLine(cursor, 3.f);
    }
}

void Oscilloscope::paintMenu(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
    
    g.setColour(BUTTON_BACKGROUND_FILL);
    g.setOpacity(0.9f);
    g.fillRoundedRectangle( buttonBounds.getX(),
                            buttonBounds.getY(),
                            buttonBounds.getWidth(),
                            buttonBounds.getHeight(),
                            2.f);
    
    buttonOptions.setAlpha(fadeAlphaMenu);
}

/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* ================================================================================================================ */
/* PAINT FUNCTIONS ================================================================================================ */
















/* FLAGS TO UPDATE PAINTING ======================================================================================= */
/* ================================================================================================================ */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */

/* Update LFO Drawing on Parameter Change */
// ========================================================
bool Oscilloscope::checkForChangesToLfo(LFO& lfo)
{
    bool hasChanged = false;

    auto index = 0;
	if (&lfo == &midLFO)    index = 1;
	if (&lfo == &highLFO)   index = 2;

    hasChanged =    checkForChangesToLfoInvert     (lfo, index) ||
                    checkForChangesToLfoWaveshape  (lfo, index) ||
                    checkForChangesToLfoSkew       (lfo, index) ||
                    checkForChangesToLfoDepth      (lfo, index) ||
                    checkForChangesToLfoSync       (lfo, index) ||
                    checkForChangesToLfoRhythm     (lfo, index) ||
                    checkForChangesToLfoRate       (lfo, index) ||
                    checkForChangesToLfoPhase      (lfo, index) ||
                    updateAndCheckForChangesToLfoFocus      (lfo) ||
                    updateAndCheckForChangesToLfoBypass     (lfo);
    
    return hasChanged;
		
}

bool Oscilloscope::checkForChangesToLfoInvert(LFO& lfo, int index)
{
    auto newInvert = lfo.getWaveInvert();
    
    if (oldInvert[index] != newInvert)
    {
        oldInvert[index] = newInvert;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoWaveshape(LFO &lfo, int index)
{
    auto newShape = lfo.getWaveform();
    
    if (oldShape[index] != newShape)
    {
        oldShape[index] = newShape;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoSkew(LFO &lfo, int index)
{
    auto newSkew = lfo.getWaveSkew();
    
    if (oldSkew[index] != newSkew)
    {
        oldSkew[index] = newSkew;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoDepth(LFO& lfo, int index)
{
    auto newDepth = lfo.getWaveDepth();
    
    if (oldDepth[index] != newDepth)
    {
        oldDepth[index] = newDepth;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoSync(LFO& lfo, int index)
{
    auto newSync = lfo.isSyncedToHost();
    
    if (oldSync[index] != newSync)
    {
        oldSync[index] = newSync;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoRhythm(LFO& lfo, int index)
{
    auto newRhythm = lfo.getWaveMultiplier();
    
    if (oldRhythm[index] != newRhythm)
    {
        oldRhythm[index] = newRhythm;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoRate(LFO& lfo, int index)
{
    auto newRate = lfo.getWaveRate();
    
    if (oldRate[index] != newRate)
    {
        oldRate[index] = newRate;
        return true;
    }   else return false;
}

bool Oscilloscope::checkForChangesToLfoPhase(LFO& lfo, int index)
{
    auto newPhase = lfo.getRelativePhase();
    
    if (oldPhase[index] != newPhase)
    {
        oldPhase[index] = newPhase;
        return true;
    }   else return false;
}

bool Oscilloscope::updateAndCheckForChangesToLfoFocus(LFO& lfo)
{
    /* Pull current parameters */
    if (&lfo == &lowLFO)
    {
        oldFocus[0] = mLowFocus;
    
        mLowFocus = (   globalControls.mTimingControls.timingBarLow.hasFocus ||
                        globalControls.mGainControls.gainBarLow.hasFocus ||
                        globalControls.mWaveControls.waveBarLow.hasFocus );
    }

    /* Pull current parameters */
    if (&lfo == &midLFO)
    {
        oldFocus[1] = mMidFocus;
        
        mMidFocus = (   globalControls.mTimingControls.timingBarMid.hasFocus ||
                        globalControls.mGainControls.gainBarMid.hasFocus ||
                        globalControls.mWaveControls.waveBarMid.hasFocus );
    }
    
    /* Pull current parameters */
    if (&lfo == &highLFO)
    {
        oldFocus[2] = mHighFocus;
        
        mHighFocus = (  globalControls.mTimingControls.timingBarHigh.hasFocus ||
                        globalControls.mGainControls.gainBarHigh.hasFocus ||
                        globalControls.mWaveControls.waveBarHigh.hasFocus );
    }
    
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

bool Oscilloscope::updateAndCheckForChangesToLfoBypass(LFO& lfo)
{
    using namespace Params;
    const auto& params = GetParams();
    
    /* Pull current parameters */
    if (&lfo == &lowLFO)
    {
        oldBypass[0] = mLowBypass;
        mLowBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
    }
    
    /* Pull current parameters */
    if (&lfo == &midLFO)
    {
        oldBypass[1] = mMidBypass;
        mMidBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
    }
    
    /* Pull current parameters */
    if (&lfo == &highLFO)
    {
        oldBypass[2] = mHighBypass;
        mHighBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
    }

    /* Compare parameters */
    if (oldBypass[0] != mLowBypass)
    {
        oldBypass[0] = mLowBypass;
        return true;
    }

    /* Compare parameters */
    if (oldBypass[1] != mMidBypass)
    {
        oldBypass[1] = mMidBypass;
        return true;
    }

    /* Compare parameters */
    if (oldBypass[2] != mHighBypass)
    {
        oldBypass[2] = mMidBypass;
        return true;
    }
    
    return false;
}

// ========================================================
void Oscilloscope::updateAndCheckForChangesToDisplayPreferences()
{
    mShowLowBand = showLowBandParam->get();
    mShowMidBand = showMidBandParam->get();
    mShowHighBand = showHighBandParam->get();
    
    mStackAllBands = stackBandsParam->get();
    
    mShowCursor = showCursorParam->get();
    mShowPlayhead = showPlayheadParam->get();
    
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
        scopeRegion.removeFromBottom(10);    // Make room for scroll slider

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

/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* ================================================================================================================ */
/* FLAGS TO UPDATE PAINTING ======================================================================================= */










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








/* OPTIONS MENU =================================================================================================== */
/* ================================================================================================================ */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */

/* Opens or closes the options menu */
// ========================================================
void Oscilloscope::buttonClicked(juce::Button* button)
{
    if (button == &buttonOptions)
        showMenu = !showMenu;

    if (button == &toggleShowCursor)
    {
        if (!mShowCursor)
            fadeAlphaCursor = 0.f;
    }
    
    drawToggles(showMenu);

    updateLfoDisplay = true;
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

/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* ================================================================================================================ */
/* OPTIONS MENU =================================================================================================== */











/* MOUSE CALLBACKS ================================================================================================ */
/* ================================================================================================================ */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */

/* On mouse exit, fade out menu */
// ========================================================
void Oscilloscope::mouseExit(const juce::MouseEvent& event)
{
	auto x = event.getPosition().getX();
	auto y = event.getPosition().getY();

	if (x < 0 || y < 0)
		fadeInMenu = false;
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
		fadeInMenu = true;
		fadeInCursor = false;
	}
	else
	{
		if (!showMenu)
			fadeInMenu = false;
	}

	// If Mouse is Within Cursor, Fade In Cursor
	if (mouseIsNearCursor)
	{
		if (mouseIsWithinMenu)
			fadeInMenu = true;
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
	if (!fadeInMenu)
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

/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* ================================================================================================================ */
/* MOUSE CALLBACKS ================================================================================================ */




/* Fade Menu Function */
void Oscilloscope::fadeInOutMenu()
{
    if (fadeInMenu || showMenu) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaMenu < fadeMaxMenu)
        {
            fadeCompleteMenu = false;
            fadeAlphaMenu += fadeStepUpMenu;
        }
            
        /* Fade in complete */
        if (fadeAlphaMenu > fadeMaxMenu)
        {
            fadeAlphaMenu = fadeMaxMenu;
            fadeCompleteMenu = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaMenu > fadeMinMenu)
        {
            fadeCompleteMenu = false;
            fadeAlphaMenu -= fadeStepDownMenu;
        }

        /* Fade out complete */
        if (fadeAlphaMenu < fadeMinMenu)
        {
            fadeAlphaMenu = fadeMinMenu;
            fadeCompleteMenu = true;
        }
    }
}

/* Fade Cursor Function */
void Oscilloscope::fadeInOutCursor()
{
    if (fadeInCursor || mShowCursor) // If mouse entered... fade Toggles Alpha up
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
    else if (mShowCursor) // If mouse exit... fade Toggles Alpha down
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
