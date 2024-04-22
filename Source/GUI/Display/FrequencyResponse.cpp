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
// ===========================================================================================
FrequencyResponse::FrequencyResponse(    TertiaryAudioProcessor& p,
                                        juce::AudioProcessorValueTreeState& apv,
                                        GlobalControls& gc)
    : audioProcessor(p),
    globalControls(gc),
    apvts(apv)
{

    buildLowMidFreqSlider();
    buildMidHighFreqSlider();
    buildLowGainSlider();
    buildMidGainSlider();
    buildHighGainSlider();
    buildLabels();
    
    makeAttachments();

    updateResponse();
}

//
// ===========================================================================================
FrequencyResponse::~FrequencyResponse()
{
}

//
// ===========================================================================================
void FrequencyResponse::buildLowMidFreqSlider()
{
    // Linear Slider from 0 to 1
    sliderLowMidInterface.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderLowMidInterface.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderLowMidInterface.addListener(this);
    sliderLowMidInterface.setRange(0, 1.f, 0.001f);
    sliderLowMidInterface.setAlpha(0.f);
    ////sliderLowMidInterface.addMouseListener(this, true);
    addAndMakeVisible(sliderLowMidInterface);

    // Linear Slider from 20 to 20k
    sliderLowMidCutoff.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderLowMidCutoff.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sliderLowMidCutoff.addListener(this);

    addAndMakeVisible(newCursorLM);
    newCursorLM.addMouseListener(this, true);
    newCursorLM.toBehind(&sliderLowMidInterface);
}

//
// ===========================================================================================
void FrequencyResponse::buildMidHighFreqSlider()
{
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

//
// ===========================================================================================
void FrequencyResponse::buildLowGainSlider()
{
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

//
// ===========================================================================================
void FrequencyResponse::buildMidGainSlider()
{
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

//
// ===========================================================================================
void FrequencyResponse::buildHighGainSlider()
{
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

// Make component->parameter attachments
// ===========================================================================================
void FrequencyResponse::makeAttachments()
{
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

    //showFftAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
    //                    audioProcessor.apvts,
    //                    params.at(Names::Show_FFT),
    //                    toggleShowRTA);
}

// Called on Component Resize
// ===========================================================================================
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
// ===========================================================================================
void FrequencyResponse::timerCallback()
{
}

//
// ===========================================================================================
void FrequencyResponse::buildLabels()
{
    // Initialize Low-Mid Cutoff Label =======
    freqLabelLow.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.f));
    
    freqLabelLow.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    freqLabelLow.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::antiquewhite);
    
    freqLabelLow.setJustificationType(juce::Justification::centred);
    
    
    freqLabelLow.setEditable(false, true);
    freqLabelLow.addListener(this);
    freqLabelLow.addMouseListener(this, true);
    addAndMakeVisible(freqLabelLow);

    // Initialize Mid-High Cutoff Label =======
    freqLabelHigh.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.f));
    
    freqLabelHigh.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    freqLabelHigh.setColour(juce::Label::ColourIds::textWhenEditingColourId, juce::Colours::antiquewhite);
    
    freqLabelHigh.setJustificationType(juce::Justification::centred);
    
    freqLabelHigh.setEditable(false, true);
    freqLabelHigh.addListener(this);
    freqLabelHigh.addMouseListener(this, true);
    addAndMakeVisible(freqLabelHigh);
    
    
    
    //======== temp
//    freqLabelLow.setSize(100, 25);
//    freqLabelHigh.setSize(100, 25);
//
//    freqLabelLow.setTopLeftPosition(0, 0);
//    freqLabelHigh.setTopLeftPosition(0,50);
}

// Graphics class
// ===========================================================================================
void FrequencyResponse::paint(juce::Graphics& g)
{
    using namespace juce;
    using namespace AllColors::FrequencyResponseColors;

    auto bounds = getLocalBounds().toFloat();

    g.setGradientFill(BACKGROUND_GRADIENT(bounds));
    g.fillAll();

    /* Paint grids */
    // =========================
    paintGridGain(g);
    paintGridFrequency(g);

    /* Paint fft */
    // =========================
    //if (mShowFFT)
        //paintFFT(g, responseArea);

    /* Paint Response Regions */
    // =========================
    paintResponseRegions(g);

    /* Draw the cursors */
    // =========================
    paintCursorsFrequency(g);

    /* Draw the gain cursors */
    // =========================
    //paintCursorsGain(g);

    /* Paint Menu */
    // =========================
    //paintMenu(g);

    /* Paint Border */
    // =========================
    paintBorder(g, ColorScheme::WindowBorders::getWindowBorderColor(), bounds);

    /* Paint Labels */
    // =========================
    paintLabels(g);
    
    paintFlag = false;
}

// Draw vertical gridlines and vertical axis labels (gain)
// ===========================================================================================
void FrequencyResponse::paintGridFrequency(juce::Graphics& g)
{
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

// Draw horizontal gridlines and horizontal axis labels (frequency)
// ===========================================================================================
void FrequencyResponse::paintGridGain(juce::Graphics& g)
{
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

//
// ===========================================================================================
void FrequencyResponse::paintResponseRegions(juce::Graphics& g)
{
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

// Draw Low-Frequency Response Trapezoid
// ===========================================================================================
void FrequencyResponse::paintLowRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
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

    if (!lowBandMute)
    {
        g.setGradientFill    (  makeLowRegionGradient(bounds.toFloat(), lowBandBypass) );

        g.setOpacity(alpha);
        g.fillPath(lowRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getLowBandBaseColor(), lowBandBypass ));

        g.strokePath(lowRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }

}

// Draw Mid-Frequency Response Trapezoid
// ===========================================================================================
void FrequencyResponse::paintMidRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float freq2Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
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
        g.setGradientFill    ( makeMidRegionGradient(bounds.toFloat(), midBandBypass ));

        g.setOpacity(alpha);
        g.fillPath(midRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getMidBandBaseColor(), midBandBypass ));

        g.strokePath(midRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }

}

// Draw High-Frequency Response Trapezoid
// ===========================================================================================
void FrequencyResponse::paintHighRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq2Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
    using namespace ColorScheme::BandColors;
    using namespace Gradients::FrequencyResponse;
    
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
        g.setGradientFill    ( makeHighRegionGradient(bounds.toFloat(), highBandBypass ));

        g.setOpacity(alpha);
        g.fillPath(highRegion.createPathWithRoundedCorners(10));

        g.setColour        (    makeOutlineColor(getHighBandBaseColor(), highBandBypass ));

        g.strokePath(highRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }
}

//
// ===========================================================================================
void FrequencyResponse::paintCursorsFrequency(juce::Graphics& g)
{

    /* This should get cleaned up and placed elsewhere */

    //freqLabelLow.setAlpha(  freqLabelLow.isBeingEdited() ? 1.f :
    //                        juce::jmap(fadeAlphaCursorLM, fadeMinCursor, fadeMaxCursor, 0.f, 1.f));

    //freqLabelHigh.setAlpha(  freqLabelHigh.isBeingEdited() ? 1.f :
    //                         juce::jmap(fadeAlphaCursorMH, fadeMinCursor, fadeMaxCursor, 0.f, 1.f));
    
    freqLabelLow.setAlpha(1.f);
    freqLabelHigh.setAlpha(1.f);

    drawLabels();
    
}

//
// ===========================================================================================
void FrequencyResponse::drawLabels()
{
    auto bounds = getLocalBounds();
    int labelWidth = 65;
    int labelHeight = 25;
    
    int margin = 5;

    int labelX1 = newCursorLM.getX() + (newCursorLM.getWidth() / 2) - labelWidth / 2;
    int labelX2 = newCursorMH.getX() + (newCursorMH.getWidth() / 2) - labelWidth / 2;

    int labelY1 = bounds.getY() + margin;
    int labelY2 = bounds.getY() + margin;
    

    // Bump at left bounds
    if (labelX1 <= responseArea.getX() + margin)
        labelX1 = responseArea.getX() + margin;

    if (labelX2 <= responseArea.getX() + margin)
        labelX2 = responseArea.getX() + margin ;

    // Bump at right bounds
    if (labelX1 + labelWidth  >= responseArea.getRight() - margin)
        labelX1 = responseArea.getRight() - labelWidth - margin;

    if (labelX2 + labelWidth  >= responseArea.getRight() - margin)
        labelX2 = responseArea.getRight() - labelWidth - margin;
    
    // Bump near each other
    if (freqLabelLow.getRight() > freqLabelHigh.getX() - 2 * margin)
    {
        labelY1 += 30;
    }
       
    // Send it
    freqLabelLow.setBounds(labelX1, labelY1 , labelWidth, labelHeight);
    freqLabelHigh.setBounds(labelX2, labelY2, labelWidth, labelHeight);

}

// Paint Labels
// ===========================================================================================
void FrequencyResponse::paintLabels(juce::Graphics& g)
{
    /* Draw Low Frequency Label */
    // ====================================================
    g.setColour(juce::Colours::black);
    
    //auto opacityL = juce::jmap(fadeAlphaCursorLM, fadeMinCursor, fadeMaxCursor, 0.f, 1.f);
    //auto opacityH = juce::jmap(fadeAlphaCursorMH, fadeMinCursor, fadeMaxCursor, 0.f, 1.f);
    
    auto opacityL = 1.f;
    auto opacityH = 1.f;

    g.setOpacity(   freqLabelLow.isBeingEdited() ? 1.f : opacityL);
    
    g.fillRoundedRectangle(freqLabelLow.getBounds().toFloat(), 3.f);
    
    g.setColour(juce::Colours::darkgrey);
    
    g.setOpacity(   freqLabelLow.isBeingEdited() ? 1.f : opacityL);

    g.drawRoundedRectangle(freqLabelLow.getBounds().toFloat(), 3.f, 1.f);
    
    /* Draw High Frequency Label */
    // ====================================================
    g.setColour(juce::Colours::black);
    
    g.setOpacity(   freqLabelHigh.isBeingEdited() ? 1.f : opacityH);
    
    g.fillRoundedRectangle(freqLabelHigh.getBounds().toFloat(), 3.f);
    
    g.setColour(juce::Colours::darkgrey);
    
    g.setOpacity(   freqLabelHigh.isBeingEdited() ? 1.f : opacityH);
    
    g.drawRoundedRectangle(freqLabelHigh.getBounds().toFloat(), 3.f, 1.f);
}

// Update the response prior to painting.
// ===========================================================================================
void FrequencyResponse::updateResponse()
{
    using namespace Params;
    const auto& params = GetParams();

    auto cursorWidth = 10;

    // Get cutoff frequency parameter values
    mLowMidCutoff = *apvts.getRawParameterValue(params.at(Names::Low_Mid_Crossover_Freq));
    mMidHighCutoff = *apvts.getRawParameterValue(params.at(Names::Mid_High_Crossover_Freq));

    // Convert cutoff values to relative pixel values
    freq1Pixel = responseArea.getX() + mapLog2(mLowMidCutoff) * responseArea.getWidth();
    freq2Pixel = responseArea.getX() + mapLog2(mMidHighCutoff) * responseArea.getWidth();

    newCursorLM.setBounds(freq1Pixel - cursorWidth / 2, responseArea.getY(), cursorWidth, responseArea.getHeight());

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

    newCursorLowGain.setBounds  (   center - lowWidth / 2.f, 
                                    gainLowPixel - cursorWidth / 2.f, 
                                    lowWidth, 
                                    cursorWidth
                                );


    // Set Mid-Gain Cursor Bounds
    // =========================================================================
    center = (freq1Pixel + freq2Pixel) / 2.f;

    newCursorMidGain.setBounds  (   center - midWidth / 2.f, 
                                    gainMidPixel - cursorWidth / 2.f, 
                                    midWidth, 
                                    cursorWidth
                                );

    // Set High-Gain Cursor Bounds
    // =========================================================================
    center = (freq2Pixel + responseArea.getRight()) / 2.f;

    newCursorHighGain.setBounds (   center - highWidth / 2.f, 
                                    gainHighPixel - cursorWidth / 2.f, 
                                    highWidth, 
                                    cursorWidth
                                );


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

    repaint();
}

// Check if bands are soloed.  Negate mute if so.
// ===========================================================================================
void FrequencyResponse::checkSolos()
{
    auto x = lowBandSolo * 100;
    auto y = midBandSolo * 10;
    auto z = highBandSolo;
    auto s = x + y + z;

    switch (s)
    {
    case 1:   {lowBandMute = true; midBandMute = true;      break; }
    case 10:  {lowBandMute = true; highBandMute = true;     break; }
    case 11:  {lowBandMute = true;                          break; }
    case 100: {midBandMute = true; highBandMute = true;     break; }
    case 101: {midBandMute = true;                          break; }
    case 110: {highBandMute = true;                         break; }
    }
}

// Map linear slider value to true octave-logarithmic value
// ===========================================================================================
float FrequencyResponse::mapLog2(float freq)
{
    auto logMin = std::log2(20.f);
    auto logMax = std::log2(20000.f);

    return (std::log2(freq) - logMin) / (logMax - logMin);
}

// Called on slider value change
// ===========================================================================================
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
    
    updateStringText();
    
    updateResponse();
}

//
// ===========================================================================================
void FrequencyResponse::labelTextChanged(juce::Label* labelThatHasChanged)
{
    // Means of handling invalid input entries,
    // while allowing some flexibility and auto-interpretation of user-entered values

    juce::String original = labelThatHasChanged->getText();

    juce::String entryString = labelThatHasChanged->getText();
    float entryFloat{ 0.f };

    // Simplify Text
    entryString = entryString.toLowerCase();            // Lower Case
    entryString = entryString.removeCharacters(" ");    // Remove Spaces
    entryString = entryString.removeCharacters("hz");    // Remove Units

    // Check for Non-Numerical Text ==================================================
    bool containsText = false;
    bool containsNum = false;

    for (int i = 0; i < entryString.length(); i++)
    {
        if ((int)entryString[i] < 48 || (int)entryString[i] > 57)
            containsText = true;

        if ((int)entryString[i] >= 48 && (int)entryString[i] <= 57)
            containsNum = true;
    }

    if (!containsText) // Handle entry as pure numerical
        entryFloat = entryString.getFloatValue();
    else // Handle entry as alphanumeric combination
    {
        if (entryString.containsChar('k'))
        {
            entryString.removeCharacters("k");
            entryFloat = entryString.getFloatValue() * 1000.f;
        }
        else // Unaccepted entry, truncate the decimal
            entryFloat = juce::roundToInt(entryString.getFloatValue());
    }

    if (labelThatHasChanged == &freqLabelLow)
    {
        if (!containsNum)
        {
            labelThatHasChanged->setText("20 Hz", juce::NotificationType::sendNotification);
            sliderLowMidCutoff.setValue(20.f);
        }
        else
            sliderLowMidCutoff.setValue(entryFloat);
    }


    if (labelThatHasChanged == &freqLabelHigh)
    {
        if (!containsNum)
        {
            labelThatHasChanged->setText("20 kHz", juce::NotificationType::dontSendNotification);
            sliderMidHighCutoff.setValue(20000);
        }
        else
            sliderMidHighCutoff.setValue(entryFloat);
    }

    paintFlag = true;
}

//
// ===========================================================================================
void FrequencyResponse::updateStringText()
{
    juce::String stringLM;
    juce::String stringMH;

    // Abbreviate Values In The Kilohertz
    if (sliderLowMidCutoff.getValue() <= 999.f)
        stringLM = (juce::String)sliderLowMidCutoff.getValue() + " Hz";
    else
    {
        auto num = sliderLowMidCutoff.getValue();
        num /= 10.f;
        num = juce::roundFloatToInt(num);
        num /= 100.f;
        stringLM = (juce::String)(num) + " kHz";
    }

    
    if (sliderMidHighCutoff.getValue() <= 999.f)
        stringMH = (juce::String)sliderMidHighCutoff.getValue() + " Hz";
    else
    {
        auto num = sliderMidHighCutoff.getValue();
        num /= 10.f;
        num = juce::roundFloatToInt(num);
        num /= 100.f;
        stringMH = (juce::String)(num) + " kHz";
    }


    freqLabelLow.setText(stringLM, juce::NotificationType::dontSendNotification);
    freqLabelHigh.setText(stringMH, juce::NotificationType::dontSendNotification);

}

// Called on Mouse Move
// ===========================================================================================
void FrequencyResponse::mouseMove(const juce::MouseEvent& event)
{
    checkCursorFocus(event);
}

// Called on Mouse Enter
// ===========================================================================================
void FrequencyResponse::mouseEnter(const juce::MouseEvent& event)
{
    checkCursorFocus(event);
}

// Called on Mouse Exit
// ===========================================================================================
void FrequencyResponse::mouseExit(const juce::MouseEvent& event)
{
    checkCursorFocus(event);
}

// Check to see if mouse is focusing on any cursor for parameter change
// ===========================================================================================
void FrequencyResponse::checkCursorFocus(const juce::MouseEvent& event)
{
    auto xM = event.getPosition().getX();
    auto yM = event.getPosition().getY();


    //auto parentComponent = newCursorLM.getParentComponent(); // Assuming both have the same parent
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

//
// ===========================================================================================
void FrequencyResponse::actionListenerCallback(const juce::String& message)
{

    auto paramName = message.replaceSection(0, 10, "");
    paramName = paramName.replaceSection(10, 25, "");
    paramName = paramName.removeCharacters("x");

    juce::String paramValue = message.replaceSection(0, 25, "");
    paramValue = paramValue.removeCharacters("x");


    DBG(paramName << "   " << paramValue);

    if (paramName == "GAIN")
        updateResponse();

    if (paramName == "BYPASS")
        updateResponse();

    if (paramName == "SOLO")
        updateResponse();

    if (paramName == "MUTE")
        updateResponse();

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

}
