/*
  ==============================================================================

    ScopeChannel.cpp
    Created: 29 Jan 2023 3:54:47pm
    Author:  Joe Caulfield

    ScopeChannel visually displays the LFO waveform of an assigned channel / frequency band.
 
    Requirements:
 
        1. Visually depict the LFO parameters
            a. Waveshape + Invert
            b. Skew
            c. Depth
            d. Rhythm (Sync) or Frequency
            e. Phase
            
        2. Color-Code based on Band.  Color-Code based on bypass/enable.
            a. Low = Green
            b. Mid = Blue
            c. High = Purple
            d. Bypass = Grey
 
        3. Display grid lines at each 1/4 note of BPM
 
        4. Grid lines and waveform scale and move per scrollpad's pan & zoom
 
  ==============================================================================
*/

#include "ScopeChannel.h"

// ========================================================
ScopeChannel::ScopeChannel(juce::AudioProcessorValueTreeState& apvts, LFO& lfo, ScrollPad& scrollPad)
    :   apvts(apvts),
        lfo(lfo),
        sliderScroll(scrollPad)
{
    
    // Initialize WaveTable Arrays
    auto wtSize = lfo.getWaveTableSize();
    waveTable.resize(wtSize);       // Initialize Wavetable
    waveTable.clearQuick();         // Initialize Wavetable
    
    // Initializes the display's color-scheme based on band type
    setBandColors();
    
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping
    
    // Listen for changes to scrollbar
    apvts.addParameterListener(params.at(Names::Scope_Point1), this);
    apvts.addParameterListener(params.at(Names::Scope_Point2), this);
    
    // Listen for changes to Oscilloscope Options Menu
    apvts.addParameterListener(params.at(Names::Show_Low_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_Mid_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_High_Scope), this);
    apvts.addParameterListener(params.at(Names::Stack_Bands_Scope), this);
    
    // Listen for change to Low Waveform
    if (lfo.getLfoID() == 0)
    {
        apvts.addParameterListener(params.at(Names::Wave_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Invert_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Symmetry_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Depth_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Multiplier_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Rate_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Relative_Phase_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Gain_Low_Band), this);
        apvts.addParameterListener(params.at(Names::Sync_Low_LFO), this);
        apvts.addParameterListener(params.at(Names::Bypass_Low_Band), this);
    }
    
    // Listen for change to Mid Waveform
    if (lfo.getLfoID() == 1)
    {
        apvts.addParameterListener(params.at(Names::Wave_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Invert_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Symmetry_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Depth_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Multiplier_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Rate_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Relative_Phase_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Gain_Mid_Band), this);
        apvts.addParameterListener(params.at(Names::Sync_Mid_LFO), this);
        apvts.addParameterListener(params.at(Names::Bypass_Mid_Band), this);
    }

    // Listen for change to High Waveform
    if (lfo.getLfoID() == 2)
    {
        apvts.addParameterListener(params.at(Names::Wave_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Invert_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Symmetry_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Depth_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Multiplier_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Rate_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Relative_Phase_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Gain_High_Band), this);
        apvts.addParameterListener(params.at(Names::Sync_High_LFO), this);
        apvts.addParameterListener(params.at(Names::Bypass_High_Band), this);
    }

    // Initially check whether any bands are bypassed
    updateBandBypass();
    
    ////startTimerHz(30);
}

// ========================================================
ScopeChannel::~ScopeChannel()
{
    
}

// ========================================================
void ScopeChannel::timerCallback()
{
    
    // One shot latch to update focus.
    if (focusHasChanged)
    {
        repaint();
        focusHasChanged = false;
    }
    
    // Force recalc and repaint based on any changes
    if (parameterHasChanged || timerCounterParam < 5 ||
        sliderScroll.isMouseButtonDown() ||
        timerCounterInit < 10)
    {
        redrawScope();
        repaint();
    }
    
    // Add extra time for initialization of waveform
    if (timerCounterInit < 10)
        timerCounterInit++;
    
    // Add extra time after parameter changed
    if (timerCounterParam < 5)
        timerCounterParam++;

    // Flag for repaint
    parameterHasChanged = false;
}



// ========================================================
void ScopeChannel::setBandColors()
{
    auto bounds = getLocalBounds().toFloat();
    //using namespace AllColors::OscilloscopeColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::Oscilloscope;
    
    if (lfo.getLfoID() == 0)
{
        fillGradient = makeWaveformGradient(bounds, "LOW", isBandBypassed);
        strokeColor = makeOutlineColor(getLowBandBaseColor());
    }
    
    if (lfo.getLfoID() == 1)
    {
        fillGradient = makeWaveformGradient(bounds, "MID", isBandBypassed);
        strokeColor = makeOutlineColor(getMidBandBaseColor());
    }
    
    if (lfo.getLfoID() == 2)
    {
        fillGradient = makeWaveformGradient(bounds, "HIGH", isBandBypassed);
        strokeColor = makeOutlineColor(getHighBandBaseColor());
    }
}

// ========================================================
void ScopeChannel::paint(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
            
    // Fill background
    // ==================================================
    auto bounds = getLocalBounds().toFloat();
    
    g.setGradientFill(BACKGROUND_GRADIENT(bounds));
    
    if (bandsAreStacked)
        g.fillRoundedRectangle(bounds, 5.f);

    paintGridLines(g);
    paintWaveform(g);
}

// ========================================================
void ScopeChannel::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateBandBypass();
    parameterHasChanged = true;
    
    // Store the changed parameter, will use to check if everything is up to date.
    parameterChangedID = parameterID;
    parameterChangedNewValue = newValue;
    
    timerCounterParam = 0;
}

// ========================================================
void ScopeChannel::actionListenerCallback(const juce::String& message)
{
    
    redrawScope();
    repaint();
    
}

// ========================================================
void ScopeChannel::paintGridLines(juce::Graphics& g)
{
    using namespace AllColors::OscilloscopeColors;

    auto bounds = getLocalBounds();
    
    /* Used to store current beat spacing, to detect a change based on new beat spacing */
    //float oldBeatSpacing = beatSpacing;

    // DRAW VERTICAL LINES =============================
    juce::Line<float> verticalAxis;

    // Convert Zoom Factor to Int
    // Represents Number of Full Quarter-Notes to be Shown
    float zoomFactor = sliderScroll.getZoom();

    g.setColour(juce::Colours::lightgrey);

    g.setOpacity(0.35f);
    
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

            g.setOpacity(0.35f);
            
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
    }

    g.setColour(juce::Colours::lightgrey);
    g.setOpacity(0.35f);
    
    // DRAW HORIZONTAL LINES =============================
    for (int i = 1; i < numDepthLines; i++)
    {
        auto y = bounds.getY() + i * (bounds.getHeight() / numDepthLines);
        g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
    }
}

// ========================================================
void ScopeChannel::paintWaveform(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
    using namespace ColorScheme::BandColors;
    
    /* Fill */
    g.setGradientFill(fillGradient);
    g.setOpacity(isBandFocused ? 0.95f : 0.85f);
    g.fillPath(lfoFill);

    /* Stroke */
    g.setColour(isBandBypassed ? makeOutlineColor(getBypassBaseColor()): strokeColor);
    g.setOpacity(isBandFocused ? 1.f : 0.75f);
    g.strokePath(lfoPath, isBandFocused? juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) :
                                            juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
}




// ========================================================
void ScopeChannel::redrawScope()
{
  
    waveTable = scaleWaveAmplitude();
    generateLFOPathForDrawing(lfoPath, lfoFill, waveTable, lfo);
    repaint();
}

/* Scales incoming WaveTable by Amplitude Parameter */
// ========================================================
juce::Array<float> ScopeChannel::scaleWaveAmplitude()
{

    auto waveTable = lfo.getWaveTableForDisplay(waveTableDownSampleSize);
    
    //DBG(waveTable.size());
    
    float mDepth = lfo.getWaveDepth();
    
    float min = 2.0f;        // Used to calculate min value of LFO, for scaling
    float max = -1.0f;        // Used to calculate max value of LFO, for scaling
    
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
void ScopeChannel::generateLFOPathForDrawing(   juce::Path &lfoStrokePath,
                                                juce::Path &lfoFillPath,
                                                juce::Array<float> &waveTable,
                                                LFO& lfo)
{
    using namespace juce;

    auto bounds = getLocalBounds();
    
    auto lfoWaveMultiplier = lfo.getWaveMultiplier();
    auto lfoWaveRate = lfo.getWaveRate();
    auto lfoHostBPM = lfo.getHostBPM();
    auto lfoRelativePhase = lfo.getRelativePhase();
    auto lfoSync = lfo.isSyncedToHost();
    
    /* Vertical Midpoint of Drawing Region */
    float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

    /* Add 3px Vertical Margin */
    bounds.reduce(0, 3);

    /* Clear Previous Path Data */
    lfoFillPath.clear();
    lfoStrokePath.clear();

    // Width of Area to Render Paint Data
    auto paintAreaWidth = playBackWidth;

    for (int i = 0; i <= 3 * paintAreaWidth; i++)
    {
        bool sync = lfoSync;

        // Scale WaveTable by Multiplier, or by Rate, but not Both
        float scalar{ 1.f };

        if (sync)
            scalar = lfoWaveMultiplier;
        else
            scalar = lfoWaveRate * 60.f / lfoHostBPM;

        // Get Relative Phase Shift.  Divide by 2 takes it from 360-deg to 180-deg shift.
        auto mRelativePhase = lfoRelativePhase / waveTableDownSampleSize / 2;

        // Get Display Phase Shift
        float mDisplayPhase = sliderScroll.getCenter() * bounds.getWidth();

        // Match Width of 1x Multiplier to Width of One Quarter-Note
        auto increment = scalar * waveTable.size() / (beatSpacing);

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


/* State of focus is set by parent Oscilloscope */
// ========================================================
void ScopeChannel::setBandFocus(bool hasFocus)
{
    isBandFocused = hasFocus;
    focusHasChanged = true;
}

/* Check bypass state */
// ========================================================
void ScopeChannel::updateBandBypass()
{
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping
    
    if (lfo.getLfoID() == 0)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));

    if (lfo.getLfoID() == 1)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));

    if (lfo.getLfoID() == 2)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
}

// ========================================================
void ScopeChannel::resized()
{
}


