/*
  ==============================================================================
    FrequencyResponse.cpp
    Created: 30 Dec 2021 11:38:20am
    Author:  Joe Caulfield
    Class to display and control the frequency response of the crossover
  ==============================================================================
*/


#include "FrequencyResponse.h"

/* Constructor */
// ===========================================================================================
FrequencyResponse::FrequencyResponse(   TertiaryAudioProcessor& p, juce::AudioProcessorValueTreeState& apv)
    : audioProcessor(p), apvts(apv)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    buildLowMidFreqSlider();
    buildMidHighFreqSlider();
    buildLowGainSlider();
    buildMidGainSlider();
    buildHighGainSlider();
    buildLabels();
    
    makeAttachments();

    // Initialize Parameters

    using namespace Params;
    const auto& params = GetParams();

    // Update Band Mute States
    lowBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_Low_Band));
    midBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_Mid_Band));
    highBandMute = *apvts.getRawParameterValue(params.at(Names::Mute_High_Band));

    // Update Band Bypass States
    lowBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
    midBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
    highBandBypass = *apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));

    //Update Band Solo States
    lowBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Low_Band));
    midBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_Mid_Band));
    highBandSolo = *apvts.getRawParameterValue(params.at(Names::Solo_High_Band));

    newFreqLabelLow.addActionListener(&newCursorLM);
    newFreqLabelHigh.addActionListener(&newCursorMH);

    newCursorLM.addActionListener(&newFreqLabelLow);
    newCursorMH.addActionListener(&newFreqLabelHigh);

    newCursorLM.setName("LOW_CURSOR");
    newCursorMH.setName("HIGH_CURSOR");

    newCursorLowGain.setName("LOW_GAIN_CURSOR");
    newCursorMidGain.setName("MID_GAIN_CURSOR");
    newCursorHighGain.setName("HIGH_GAIN_CURSOR");

    updateResponse();
}

/* Destructor */
// ===========================================================================================
FrequencyResponse::~FrequencyResponse()
{
}

/* Build Low-Mid Slider */
// ===========================================================================================
void FrequencyResponse::buildLowMidFreqSlider()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    // Linear Slider from 0 to 1
    sliderLowMidInterface.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderLowMidInterface.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderLowMidInterface.addListener(this);
    sliderLowMidInterface.setRange(0, 1.f, 0.001f);
    sliderLowMidInterface.setAlpha(0.f);
    addAndMakeVisible(sliderLowMidInterface);

    // Linear Slider from 20 to 20k
    sliderLowMidCutoff.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderLowMidCutoff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderLowMidCutoff.addListener(this);

    addAndMakeVisible(newCursorLM);
    newCursorLM.addMouseListener(this, true);
    newCursorLM.toBehind(&sliderLowMidInterface);
}

/* Build Mid-High Slider */
// ===========================================================================================
void FrequencyResponse::buildMidHighFreqSlider()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    // Linear Slider from 0 to 1
    sliderMidHighInterface.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderMidHighInterface.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderMidHighInterface.addListener(this);
    sliderMidHighInterface.setRange(0, 1.f, 0.001f);
    sliderMidHighInterface.setAlpha(0.f);
    addAndMakeVisible(sliderMidHighInterface);

    // Linear Slider from 20 to 20k
    sliderMidHighCutoff.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderMidHighCutoff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderMidHighCutoff.addListener(this);

    addAndMakeVisible(newCursorMH);
    newCursorMH.addMouseListener(this, true);
    newCursorMH.toBehind(&sliderMidHighInterface);
}

/* Build Low Gain Slider */
// ===========================================================================================
void FrequencyResponse::buildLowGainSlider()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    sliderLowGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sliderLowGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderLowGain.addListener(this);
    sliderLowGain.setRange(0, 1.f, 0.001f);
    sliderLowGain.setAlpha(0.f);
    addAndMakeVisible(sliderLowGain);

    addAndMakeVisible(newCursorLowGain);
    newCursorLowGain.setHorizontalOrientation();
    newCursorLowGain.addMouseListener(this, true);
    newCursorLowGain.toBehind(&sliderLowGain);
}

/* Build Mid Gain Slider */
// ===========================================================================================
void FrequencyResponse::buildMidGainSlider()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    sliderMidGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sliderMidGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderMidGain.addListener(this);
    sliderMidGain.setRange(0, 1.f, 0.001f);
    sliderMidGain.setAlpha(0.f);
    addAndMakeVisible(sliderMidGain);

    addAndMakeVisible(newCursorMidGain);
    newCursorMidGain.setHorizontalOrientation();
    newCursorMidGain.addMouseListener(this, true);
    newCursorMidGain.toBehind(&sliderMidGain);
}

/* Build High Gain Slider */
// ===========================================================================================
void FrequencyResponse::buildHighGainSlider()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    sliderHighGain.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sliderHighGain.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderHighGain.addListener(this);
    sliderHighGain.setRange(0, 1.f, 0.001f);
    sliderHighGain.setAlpha(0.f);
    addAndMakeVisible(sliderHighGain);

    addAndMakeVisible(newCursorHighGain);
    newCursorHighGain.setHorizontalOrientation();
    newCursorHighGain.addMouseListener(this, true);
    newCursorHighGain.toBehind(&sliderHighGain);
}

/* Build Labels */
// ===========================================================================================
void FrequencyResponse::buildLabels()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    newFreqLabelLow.addActionListener(this);
    newFreqLabelHigh.addActionListener(this);
    newFreqLabelLow.setName("LOWFREQ");
    newFreqLabelHigh.setName("HIGHFREQ");
    addAndMakeVisible(newFreqLabelLow);
    addAndMakeVisible(newFreqLabelHigh);
}

/* Attach parameters to components */
// ===========================================================================================
void FrequencyResponse::makeAttachments()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    using namespace Params;
    const auto& params = GetParams();

    lowMidAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Low_Mid_Crossover_Freq),
                        sliderLowMidCutoff);

    midHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Mid_High_Crossover_Freq),
                        sliderMidHighCutoff);

    lowMidAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Low_Mid_Crossover_Freq),
                        sliderLowMidCutoff);

    lowGainAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Gain_Low_Band),
                        sliderLowGain);

    midGainAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Gain_Mid_Band),
                        sliderMidGain);

    highGainAttachment =std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        apvts,
                        params.at(Names::Gain_High_Band),
                        sliderHighGain);
}

/* Called on component resize */
// ===========================================================================================
void FrequencyResponse::resized()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

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

/* Timer Callback */
// ===========================================================================================
//void FrequencyResponse::timerCallback()
//{
//    setLabelOpacity();
//}

/* Paint Graphics Class */
// ===========================================================================================
void FrequencyResponse::paint(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::FrequencyResponseColors;

    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto bounds = getLocalBounds().toFloat();

    g.setGradientFill(BACKGROUND_GRADIENT(bounds));
    g.fillAll();

    /* Paint grids */
    // =========================
    paintGridGain(g);       // These should be buffered to images?
    paintGridFrequency(g);  // These should be buffered to images?

    /* Paint Response Regions */
    // =========================
    paintResponseRegions(g);

    /* Draw the frequency labels */
    // =========================
    drawLabels();

    /* Paint Border */
    // =========================
    paintBorder(g, ColorScheme::WindowBorders::getWindowBorderColor(), bounds);
}

/* Draw vertical grid lines (frequency) */
// ===========================================================================================
void FrequencyResponse::paintGridFrequency(juce::Graphics& g)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    using namespace juce;
    const auto textWidth = 30;
    const auto fontHeight = 12;

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

/* Draw horizontal grid lines (gain) */
// ===========================================================================================
void FrequencyResponse::paintGridGain(juce::Graphics& g)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    using namespace juce;
    const auto textWidth = 26;
    const auto fontHeight = 13;

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
        g.drawFittedText(strGain, 3, yC, textWidth, fontHeight, juce::Justification::centredRight, 1, 1);

    }
}

/* Parent function to paint the frequency response trapezoids */
// ===========================================================================================
void FrequencyResponse::paintResponseRegions(juce::Graphics& g)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    /* If low-band is hovered, draw low-band on top with highlight */
    if (mLowFocus)
    {
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? focusAlpha : noFocusAlpha);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? focusAlpha : noFocusAlpha);
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? focusAlpha : noFocusAlpha);
    }

    // If mid-band is hovered, draw mid-band on top with highlight.  Doubles as default view.
    if (mMidFocus || (!mLowFocus && !mMidFocus && !mHighFocus))
    {
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? focusAlpha : noFocusAlpha);
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? focusAlpha : noFocusAlpha);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? focusAlpha : noFocusAlpha);
    }

    // If high-band is hovered, draw high-band on top with highlight.
    if (mHighFocus)
    {
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? focusAlpha : noFocusAlpha);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? focusAlpha : noFocusAlpha);
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? focusAlpha : noFocusAlpha);
    }
}

/* Paints the low-frequency trapezoid */
// ===========================================================================================
void FrequencyResponse::paintLowRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto slope = 10;

    bounds.reduce(2, 0);

    // LOW REGION ========================================================
    juce::Path lowRegion;

    lowRegion.startNewSubPath(    bounds.getX(),    bounds.getBottom());
    lowRegion.lineTo(            bounds.getX(),    gainPixel);
    lowRegion.lineTo(            freq1Pixel,        gainPixel);

    if (bounds.getRight() - freq1Pixel > slope)
        lowRegion.lineTo(freq1Pixel + slope, bounds.getBottom());
    else
        lowRegion.lineTo(bounds.getRight(), bounds.getBottom());

    if (!lowBandMute && !lowBandSoloMute)
    {
        g.setGradientFill    (  makeLowRegionGradient(bounds.toFloat(), lowBandBypass) );

        g.setOpacity(alpha);
        g.fillPath(lowRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getLowBandBaseColor(), lowBandBypass ));

        g.strokePath(lowRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }
}

/* Paints the mid-frequency trapezoid */
// ===========================================================================================
void FrequencyResponse::paintMidRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float freq2Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

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

    if (!midBandMute && !midBandSoloMute)
    {
        g.setGradientFill    ( makeMidRegionGradient(bounds.toFloat(), midBandBypass ));

        g.setOpacity(alpha);
        g.fillPath(midRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getMidBandBaseColor(), midBandBypass ));

        g.strokePath(midRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }

}

/* Paints the high-frequency trapezoid */
// ===========================================================================================
void FrequencyResponse::paintHighRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq2Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

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

    if (!highBandMute && !highBandSoloMute)
    {
        g.setGradientFill    ( makeHighRegionGradient(bounds.toFloat(), highBandBypass ));

        g.setOpacity(alpha);
        g.fillPath(highRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getHighBandBaseColor(), highBandBypass ));

        g.strokePath(highRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }
}

/* Calculates and repositions labels with respect to cursors */
// ===========================================================================================
void FrequencyResponse::drawLabels()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto bounds = getLocalBounds();
    int labelWidth = 65;
    int labelHeight = 25;
    
    int marginX = 5;
    int marginY = 3;

    int labelX1 = newCursorLM.getX() + (newCursorLM.getWidth() / 2) - labelWidth / 2;
    int labelX2 = newCursorMH.getX() + (newCursorMH.getWidth() / 2) - labelWidth / 2;

    int labelY1 = bounds.getY() + marginY;
    int labelY2 = bounds.getY() + marginY;

    // Bump at left bounds
    if (labelX1 <= responseArea.getX() + marginX)
        labelX1 = responseArea.getX() + marginX;

    if (labelX2 <= responseArea.getX() + marginX)
        labelX2 = responseArea.getX() + marginX;

    // Bump at right bounds
    if (labelX1 + labelWidth  >= responseArea.getRight() - marginX)
        labelX1 = responseArea.getRight() - labelWidth - marginX;

    if (labelX2 + labelWidth  >= responseArea.getRight() - marginX)
        labelX2 = responseArea.getRight() - labelWidth - marginX;
    
    // Bump near each other
    auto distTooClose = labelWidth + marginX;
    auto center = (labelX1 + labelX2) / 2;

    if (    (   (lowBandMute || lowBandSoloMute) && (midBandMute || midBandSoloMute)    )  || 
            (   (midBandMute || midBandSoloMute) && (highBandMute || highBandSoloMute)  ) )
    {
        /* Then ignore the label-2-label boundary checks */
    }
    else
    {
        if (labelX2 - labelX1 <= distTooClose)
        {
            labelX1 = center - (labelWidth / 2) - marginX;
            labelX2 = center + (labelWidth / 2) + marginX;
        }
    }

    newFreqLabelLow.setBounds(labelX1, labelY1, labelWidth, labelHeight);
    newFreqLabelHigh.setBounds(labelX2, labelY2, labelWidth, labelHeight);
}

/* Calculates the boundary values for the three frequency bands before painting */
// ===========================================================================================
void FrequencyResponse::updateResponse()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    checkSolos();

    auto killLowBand = lowBandMute || lowBandSoloMute;
    auto killMidBand = midBandMute || midBandSoloMute;
    auto killHighBand = highBandMute || highBandSoloMute;

    auto showLowBand = !lowBandMute && !lowBandSoloMute;
    auto showMidBand = !midBandMute && !midBandSoloMute;
    auto showHighBand = !highBandMute && !highBandSoloMute;

    using namespace Params;
    const auto& params = GetParams();

    auto cursorWidth = 10;

    // Get cutoff frequency parameter values
    mLowMidCutoff = *apvts.getRawParameterValue(params.at(Names::Low_Mid_Crossover_Freq));
    mMidHighCutoff = *apvts.getRawParameterValue(params.at(Names::Mid_High_Crossover_Freq));

    // Convert cutoff values to relative pixel values
    freq1Pixel = responseArea.getX() + mapLog2(mLowMidCutoff) * responseArea.getWidth();
    freq2Pixel = responseArea.getX() + mapLog2(mMidHighCutoff) * responseArea.getWidth();

    if (killLowBand && killMidBand)
        newCursorLM.setBounds(0, 0, 0, 0);
    else
        newCursorLM.setBounds(freq1Pixel - cursorWidth / 2, responseArea.getY(), cursorWidth, responseArea.getHeight());
        


    if (killMidBand && killHighBand)
        newCursorMH.setBounds(0, 0, 0, 0);
    else
        newCursorMH.setBounds(freq2Pixel - cursorWidth / 2, responseArea.getY(), cursorWidth, responseArea.getHeight());



    // Set Mid-High Crossover Cursor Bounds
    mLowGain = *apvts.getRawParameterValue(params.at(Names::Gain_Low_Band));
    mMidGain = *apvts.getRawParameterValue(params.at(Names::Gain_Mid_Band));
    mHighGain = *apvts.getRawParameterValue(params.at(Names::Gain_High_Band));

    // Convert band gain values to relative pixel values
    gainLowPixel = responseArea.getY() + juce::jmap(mLowGain,    -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
    gainMidPixel = responseArea.getY() + juce::jmap(mMidGain,    -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
    gainHighPixel = responseArea.getY() + juce::jmap(mHighGain, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());

    // Set Cursor Widths. Cursors A Fraction of Band Width
    auto lowWidth = (freq1Pixel - responseArea.getX()) * 0.75f;
    auto midWidth = (freq2Pixel - freq1Pixel) * 0.75f;
    auto highWidth = (responseArea.getRight() - freq2Pixel) * 0.75f;

    // Set Low-Gain Cursor Bounds
    // =========================================================================
    auto center = (responseArea.getX() + freq1Pixel) / 2.f;

    if (showLowBand)
    {
        newCursorLowGain.setBounds  (   center - lowWidth / 2.f, 
                                        gainLowPixel - cursorWidth / 2.f, 
                                        lowWidth, 
                                        cursorWidth
                                    );
    }
    else
        newCursorLowGain.setBounds(0,0,0,0);



    // Set Mid-Gain Cursor Bounds
    // =========================================================================
    center = (freq1Pixel + freq2Pixel) / 2.f;

    if (showMidBand)
    {
        newCursorMidGain.setBounds  (   center - midWidth / 2.f, 
                                        gainMidPixel - cursorWidth / 2.f, 
                                        midWidth, 
                                        cursorWidth
                                    );
    }
    else
        newCursorMidGain.setBounds(0,0,0,0);


    // Set High-Gain Cursor Bounds
    // =========================================================================
    center = (freq2Pixel + responseArea.getRight()) / 2.f;

    if (showHighBand)
    {
    newCursorHighGain.setBounds (   center - highWidth / 2.f, 
                                    gainHighPixel - cursorWidth / 2.f, 
                                    highWidth, 
                                    cursorWidth
                                );
    }
    else
        newCursorHighGain.setBounds(0,0,0,0);

    repaint(1,1,getWidth()-2, getHeight()-2);
}

/* Checks if a given band is soloed, and mutes the other bands if so */
// ===========================================================================================
void FrequencyResponse::checkSolos()
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");



    auto x = lowBandSolo * 100;
    auto y = midBandSolo * 10;
    auto z = highBandSolo;
    auto s = x + y + z;

    lowBandSoloMute = false;
    midBandSoloMute = false;
    highBandSoloMute = false;

    switch (s)
    {
    case 1:   { lowBandSoloMute = true; midBandSoloMute = true;     break; }
    case 10:  { lowBandSoloMute = true; highBandSoloMute = true;    break; }
    case 11:  { lowBandSoloMute = true;                             break; }
    case 100: { midBandSoloMute = true; highBandSoloMute = true;    break; }
    case 101: { midBandSoloMute = true;                             break; }
    case 110: { highBandSoloMute = true;                            break; }
    }
}

/* Map linear slider value to true octave-logarithmic value */
// ===========================================================================================
float FrequencyResponse::mapLog2(float freq)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto logMin = std::log2(20.f);
    auto logMax = std::log2(20000.f);

    return (std::log2(freq) - logMin) / (logMax - logMin);
}

/* Called on slider value change */
// ===========================================================================================
void FrequencyResponse::sliderValueChanged(juce::Slider* slider)
{
    // 'Interface' slider is linear from 0 to 1.
    // Changed by user by dragging Cutoff Cursors
    // Slider is converted from [0 to 1] to [20 to 20k] to store frequency

    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

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
            /* Convert[0 to 1](Interface) value to [20 to 20k](Cutoff) value. Send to Cutoff slider to update params. */

            auto y = slider->getValue();
            auto f = 20 * pow(2, std::log2(1000.f) * y);
            sliderMidHighCutoff.setValue(f);
        }

    }

    /*
        'Cutoff' slider is linear from 20 to 20k.
        Changed by user via attachment to external sliders.
        Slider is converted from [20 to 20k] to [0 to 1] for pixel representation 
     */ 

    if (slider == &sliderLowMidCutoff || slider == &sliderMidHighCutoff)
    {
        sliderMidHighInterface.setValue(mapLog2(sliderMidHighCutoff.getValue()));
        sliderLowMidInterface.setValue(mapLog2(sliderLowMidCutoff.getValue()));
    }
    
    if (slider == &sliderLowMidCutoff)
    {
        if (slider->getValue() > sliderMidHighCutoff.getValue())
            sliderMidHighCutoff.setValue(slider->getValue());
    }

    if (slider == &sliderMidHighCutoff)
    {
        if (slider->getValue() < sliderLowMidCutoff.getValue())
            sliderLowMidCutoff.setValue(slider->getValue());
    }
    
    //updateStringText();
    newFreqLabelLow.setLabelValue(sliderLowMidCutoff.getValue());
    newFreqLabelHigh.setLabelValue(sliderMidHighCutoff.getValue());

    updateResponse();
}

/* Called on mouse move */
// ===========================================================================================
void FrequencyResponse::mouseMove(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    checkCursorFocus(event);
}

/* Called when mouse enters parent or any child components */
// ===========================================================================================
void FrequencyResponse::mouseEnter(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    checkCursorFocus(event);
}

/* Called when mouse exits parent or any child components */
// ===========================================================================================
void FrequencyResponse::mouseExit(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    checkCursorFocus(event);
}

/* Inspects mouse position to see if mouse is hovering over any cursor */
// ===========================================================================================
void FrequencyResponse::checkCursorFocus(const juce::MouseEvent& event)
{
    //if (setDebug)
    //    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

    auto xM = event.getPosition().getX();
    auto yM = event.getPosition().getY();

    auto mousePoint = getLocalPoint(event.eventComponent, event.getPosition());

    // Establish top-bottom bounds of Dummy Sliders
    auto sliderTop = responseArea.getY() + juce::jmap(24.f, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY())-5;
    auto sliderBot = responseArea.getY() + juce::jmap(-24.f, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY())+5;

    // Check if mouse is on cursor, place dummy slider underneath mouse

    // Low-Mid
    // =============================================================
    if (newCursorLM.getBounds().contains(mousePoint))
    {
        sliderLowMidInterface.setBounds(responseArea.getX(), yM, responseArea.getWidth(), 5);
        newCursorLM.setFocus(true);
    }
    else
    {
        sliderLowMidInterface.setBounds(0, 0, 0, 0);
        newCursorLM.setFocus(false);
    }

    // Mid-High
    // =============================================================
    if (newCursorMH.getBounds().contains(mousePoint))
    {
        sliderMidHighInterface.setBounds(responseArea.getX(), yM, responseArea.getWidth(), 5);
        newCursorMH.setFocus(true);
    }
    else
    {
        sliderMidHighInterface.setBounds(0, 0, 0, 0);
        newCursorMH.setFocus(false);
    }

    // Low Gain
    // =============================================================
    if (newCursorLowGain.getBounds().contains(mousePoint))
    {
        //sliderLowGain.setBounds(xM, sliderTop, 5, (sliderBot - sliderTop));
        sliderLowGain.setBounds(newCursorLowGain.getX() + xM, sliderTop, 5, (sliderBot - sliderTop));
        newCursorLowGain.setFocus(true);
    }
    else
    {
        sliderLowGain.setBounds(0, 0, 0, 0);
        newCursorLowGain.setFocus(false);
    }

    // Mid Gain
    // =============================================================
    if (newCursorMidGain.getBounds().contains(mousePoint))
    {
        sliderMidGain.setBounds(newCursorMidGain.getX() + xM, sliderTop, 5, (sliderBot - sliderTop));
        newCursorMidGain.setFocus(true);
    }
    else
    {
        sliderMidGain.setBounds(0, 0, 0, 0);
        newCursorMidGain.setFocus(false);
    }

    // High Gain
    // =============================================================
    if (newCursorHighGain.getBounds().contains(mousePoint))
    {
        sliderHighGain.setBounds(newCursorHighGain.getX() + xM, sliderTop, 5, (sliderBot - sliderTop));
        newCursorHighGain.setFocus(true);
    }
    else
    {
        sliderHighGain.setBounds(0, 0, 0, 0);
        newCursorHighGain.setFocus(false);
    }
}

/* Receives broadcasts of parameter changes from relevant areas of program */
// ===========================================================================================
void FrequencyResponse::actionListenerCallback(const juce::String& message)
{
    auto bandName = message.replaceSection(5, 30, "");
    bandName = bandName.removeCharacters("x");

    auto paramName = message.replaceSection(0, 10, "");
    paramName = paramName.replaceSection(10, 25, "");
    paramName = paramName.removeCharacters("x");

    juce::String paramValue = message.replaceSection(0, 25, "");
    paramValue = paramValue.removeCharacters("x");

    DBG("FREQ " << paramName);

    if (paramName == "GAIN") 
    {
    }

    if (paramName == "BYPASS")
    {
        if (bandName == "LOW")
            lowBandBypass = paramValue.getIntValue();

        if (bandName == "MID")
            midBandBypass = paramValue.getIntValue();

        if (bandName == "HIGH")
            highBandBypass = paramValue.getIntValue();
    }

    if (paramName == "SOLO")
    {
        if (bandName == "LOW")
            lowBandSolo = paramValue.getIntValue();

        if (bandName == "MID")
            midBandSolo = paramValue.getIntValue();

        if (bandName == "HIGH")
            highBandSolo = paramValue.getIntValue();
    }

    if (paramName == "MUTE")
    {
        if (bandName == "LOW")
            lowBandMute = paramValue.getIntValue();

        if (bandName == "MID")
            midBandMute = paramValue.getIntValue();

        if (bandName == "HIGH")
            highBandMute = paramValue.getIntValue();
    }

    /* Reformat this code standard */
    if (paramName == "FOCUS")
    {
        mLowFocus = false;
        mMidFocus = false;
        mHighFocus = false;

        if (paramValue == "LOW")
            mLowFocus = true;

        if (paramValue == "MID")
            mMidFocus = true;

        if (paramValue == "HIGH")
            mHighFocus = true;
    }

    if (paramName == "LOWFREQ")
    {
        auto value = paramValue.getFloatValue();
        sliderLowMidCutoff.setValue(value);
    }

    if (paramName == "HIGHFREQ")
    {
        auto value = paramValue.getFloatValue();
        sliderMidHighCutoff.setValue(value);
    }

    updateResponse();
}
