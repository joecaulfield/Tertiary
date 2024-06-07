#include "ScopeChannel.h"

/* Constructor */
// ========================================================
ScopeChannel::ScopeChannel(juce::AudioProcessorValueTreeState& apvts, LFO& lfo, ScrollPad& scrollPad)
    :   apvts(apvts),
        lfo(lfo),
        sliderScroll(scrollPad)
{
    // Initializes the display's color-scheme based on band type
    setBandColors();
    
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping
    
    //mSampleRate = lfo.getSampleRate();
    mSampleRate = apvts.processor.getSampleRate();
    mHostBpm = lfo.getHostBPM();

    localLFO.initializeLFO(mSampleRate);

    if (mSampleRate == 0)
        mSampleRate = 48000;

    if (mHostBpm == 0 || mHostBpm == 1)
        mHostBpm = 120;

    localLFO.setDsp(false);
    
    //sendActionMessage("REQUEST_PARAMS_" + getName());
    //DBG("SC SENDING REQUEST FOR PARAMS");

    //localLFO.setWaveRate            (lfo.getWaveRate());
    //localLFO.setRelativePhase       (lfo.getRelativePhase());
    //localLFO.setWaveMultiplier      (lfo.getWaveMultiplier());
    //localLFO.setSyncedToHost        (lfo.isSyncedToHost());
    //localLFO.setWaveSkew            (lfo.getWaveSkew());
    //localLFO.setWaveDepth           (lfo.getWaveDepth());
    //localLFO.setWaveInvert          (lfo.getWaveInvert());
    //localLFO.setWaveform            (lfo.getWaveform());

    

    // Listen for changes to scrollbar
    apvts.addParameterListener(params.at(Names::Scope_Point1), this);
    apvts.addParameterListener(params.at(Names::Scope_Point2), this);
    
    // Listen for changes to Oscilloscope Options Menu
    apvts.addParameterListener(params.at(Names::Show_Low_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_Mid_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_High_Scope), this);
    apvts.addParameterListener(params.at(Names::Stack_Bands_Scope), this);

    // Initially check whether any bands are bypassed
    updateBandBypass();

    localLFO.updateLFO(mSampleRate, mHostBpm);
}

/* Destructor */
// ========================================================
ScopeChannel::~ScopeChannel()
{
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

    apvts.removeParameterListener(params.at(Names::Scope_Point1), this);
    apvts.removeParameterListener(params.at(Names::Scope_Point2), this);

    apvts.removeParameterListener(params.at(Names::Show_Low_Scope), this);
    apvts.removeParameterListener(params.at(Names::Show_Mid_Scope), this);
    apvts.removeParameterListener(params.at(Names::Show_High_Scope), this);
    apvts.removeParameterListener(params.at(Names::Stack_Bands_Scope), this);
}

/* Set color theme */
// ========================================================
void ScopeChannel::setBandColors()
{
    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

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

/* Paint graphics */
// ========================================================
void ScopeChannel::paint(juce::Graphics& g)
{

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace juce;
    using namespace AllColors::OscilloscopeColors;
            
    // ==================================================
    auto bounds = getLocalBounds().toFloat();
    
    g.setGradientFill(BACKGROUND_GRADIENT(bounds));
    
    if (bandsAreStacked)
        g.fillRoundedRectangle(bounds, 5.f);

    paintGridLines(g);
    paintWaveform(g);
}

/* Recieves broadcasts of parameter changes */
// ========================================================
void ScopeChannel::actionListenerCallback(const juce::String& message)
{
    auto paramName = message.replaceSection(0, 10, "");
    paramName = paramName.replaceSection(10, 25, "");
    paramName = paramName.removeCharacters("x");

    juce::String paramValue = message.replaceSection(0, 25, "");
    paramValue = paramValue.removeCharacters("x");

    bool shouldUpdateScope = false;

    DBG("SCOPE " << paramName);

    if (paramName == "WAVESHAPE")
    {
        int waveform = 0;
        waveform = paramValue.getIntValue();
        localLFO.setWaveform(waveform);
        shouldUpdateScope = true;
    }

    if (paramName == "DEPTH")
    {
        float depth = 0.0f;
        depth = paramValue.getFloatValue();
        localLFO.setWaveDepth(depth);
        shouldUpdateScope = true;
    }

    if (paramName == "SKEW")
    {
        float skew = 50.0f;
        skew = paramValue.getFloatValue();
        localLFO.setWaveSkew(skew);
        shouldUpdateScope = true;
    }

    if (paramName == "PHASE")
    {
        float phase = 0.0f;
        phase = paramValue.getFloatValue();
        localLFO.setRelativePhase(phase);
        shouldUpdateScope = true;
    }

    if (paramName == "RHYTHM")
    {
        int rhythm = 0;
        rhythm = paramValue.getIntValue();
        localLFO.setWaveMultiplier(rhythm);
        shouldUpdateScope = true;
    }

    if (paramName == "RATE")
    {
        float rate = 0.0f;
        rate = paramValue.getFloatValue();
        localLFO.setWaveRate(rate);
        shouldUpdateScope = true;
    }

    if (paramName == "INVERT")
    {
        bool invert = false;
        invert = paramValue.getIntValue();
        localLFO.setWaveInvert(invert);
        shouldUpdateScope = true;
    }

    if (paramName == "SYNC")
    {
        bool sync = false;
        sync = paramValue.getIntValue();
        localLFO.setSyncedToHost(sync);
        shouldUpdateScope = true;
    }

    if (paramName == "SCROLLBAR")
    {
        scrollZoom = sliderScroll.getScrollZoom();
        scrollCenter = sliderScroll.getScrollCenter();
        //shouldUpdateScope = true;

        generateLFOPathForDrawing(lfoPath, lfoFill, waveTable, localLFO);
        redrawScope();
        repaint();
    }

    if (paramName == "FOCUS")
    {
        isBandFocused = false;

        if (paramValue == getName())
            isBandFocused = true;
        //else
        //    isBandFocused = false;

        //shouldUpdateScope = true;

        generateLFOPathForDrawing(lfoPath, lfoFill, waveTable, localLFO);
        redrawScope();
        repaint();
    }


    if (shouldUpdateScope)
    {
        /* Parameters have changed, so recalculate them in the local LFO */
        localLFO.updateLFO(mSampleRate, mHostBpm);
        redrawScope();
        repaint();
    }



    
}

/* Paint grid lines */
// ========================================================
void ScopeChannel::paintGridLines(juce::Graphics& g)
{

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace AllColors::OscilloscopeColors;

    auto bounds = getLocalBounds();

    // DRAW VERTICAL LINES =============================
    juce::Line<float> verticalAxis;

    g.setColour(juce::Colours::lightgrey);

    g.setOpacity(0.35f);
    
    if (bounds.getWidth() > 0)
        beatSpacing = bounds.getWidth() / (scrollZoom);
    else
        beatSpacing = 1.f;

    // Allows Grid to Split Center
    float mDisplayPhase = scrollCenter * bounds.getWidth();

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

    redrawScope();
}

/* Paint the LFO waveforms */
// ========================================================
void ScopeChannel::paintWaveform(juce::Graphics& g)
{

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

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
    g.strokePath(lfoPath, isBandFocused?    juce::PathStrokeType(2.25f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) :
                                            juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
}

/* Parent method to recalculate scope waveform points*/
// ========================================================
void ScopeChannel::redrawScope()
{
    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    waveTable = scaleWaveAmplitude();
    generateLFOPathForDrawing(lfoPath, lfoFill, waveTable, localLFO);
}

/* Scales incoming WaveTable by Amplitude Parameter */
// ========================================================
juce::Array<float> ScopeChannel::scaleWaveAmplitude()
{

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    auto waveTable = localLFO.getWaveTableForDisplay(waveTableDownSampleSize);

    float mDepth = localLFO.getWaveDepth() / 100.f;

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

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace juce;

    auto bounds = getLocalBounds();
    
    auto lfoWaveMultiplier = localLFO.getWaveMultiplier();
    auto lfoWaveRate = localLFO.getWaveRate();
    auto lfoHostBPM = localLFO.getHostBPM();
    auto lfoRelativePhase = localLFO.getRelativePhaseInSamples();
    auto lfoSync = localLFO.isSyncedToHost();
    
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
        float mDisplayPhase = scrollCenter * bounds.getWidth();
        //float mDisplayPhase = sliderScroll.getScrollCenter() * bounds.getWidth();

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

/* Check bypass state */
// ========================================================
void ScopeChannel::updateBandBypass()
{

    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping
    
    if (lfo.getLfoID() == 0)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));

    if (lfo.getLfoID() == 1)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));

    if (lfo.getLfoID() == 2)
        isBandBypassed = *apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
}

/* Called on component resize */
// ========================================================
void ScopeChannel::resized()
{
    //WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    while (scrollZoom == 0 && scrollCenter == 0)
    {
        scrollZoom = sliderScroll.getScrollZoom();
        scrollCenter = sliderScroll.getScrollCenter();
    }
}