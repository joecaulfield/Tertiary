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
FrequencyResponse::FrequencyResponse(    TertiaryAudioProcessor& p,
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
    //    case 0: togglePickInput.setToggleState(true, false); break;        // Pre
    //    case 1: togglePickOutput.setToggleState(true, false); break;    // Post
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

    setBufferedToImage(true);
    setOpaque(true);
    
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

    showFftAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                        audioProcessor.apvts,
                        params.at(Names::Show_FFT),
                        toggleShowRTA);
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
    
    /* [1] Update display preferences */
    /* [2] Check Mouse Position */
    checkMousePosition();
    /* [3] Any fade in/out's*/

    /* [4] Check for changes to Low, if so update */
    /* [5] Check for changes to Mid, if so update */
    /* [6] Check for changes to High, if so update */
    



    updateResponse();
    
    fadeInOutCursorLM();
    fadeInOutCursorMH();
    
    fadeInOutCursorLG();
    fadeInOutCursorMG();
    fadeInOutCursorHG();
    
    fadeInOutRegionLow();
    fadeInOutRegionMid();
    fadeInOutRegionHigh();

    fadeButton();
    
    
    checkExternalFocus();
    

    if (!fadeCompleteCursorLM)
        repaint(cursorLM.getStartX()-5, cursorLM.getStartY(), 10, cursorLM.getLength() );
    
    if (!fadeCompleteCursorMH)
        repaint(cursorMH.getStartX()-5, cursorMH.getStartY(), 10, cursorMH.getLength() );
    
    if (!fadeCompleteCursorLG)
        repaint(cursorLG.getStartX(), cursorLG.getStartY()-5, cursorLG.getLength(), 10 );
    
    if (!fadeCompleteCursorMG)
        repaint(cursorMG.getStartX(), cursorMG.getStartY()-5, cursorMG.getLength(), 10 );
    
    if (!fadeCompleteCursorHG)
        repaint(cursorHG.getStartX(), cursorHG.getStartY()-5, cursorHG.getLength(), 10 );
    
    if (!fadeCompleteRegionLow)
        repaint();
    
    if (!fadeCompleteRegionMid)
        repaint();
    
    if (!fadeCompleteRegionHigh)
        repaint();
    
    if (!fadeCompleteButton)
        repaint();
    
    //if (mShowFFT)
        //repaint();
    
//    // Check for new FFT information
//    if (audioProcessor.nextFFTBlockReady)
//    {
//        drawNextFrameOfSpectrum();
//        audioProcessor.nextFFTBlockReady = false;
//    }
}












/* Graphics Functions =========================================================================== */
/* ============================================================================================== */


// Graphics class
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
    paintCursorsGain(g);

    /* Paint Menu */
    // =========================
    paintMenu(g);

    /* Paint Border */
    // =========================
    paintBorder(g, juce::Colours::purple, bounds);
    
    DBG("Background Paint - Frequency");
}

// Draw vertical gridlines and vertical axis labels (gain)
void FrequencyResponse::paintGridFrequency(juce::Graphics& g)
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
void FrequencyResponse::paintGridGain(juce::Graphics& g)
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

void FrequencyResponse::paintResponseRegions(juce::Graphics& g)
{
    /* If low-band is hovered, draw low-band on top with highlight */
    if (mLowFocus)
    {
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? fadeMaxRegion : fadeAlphaRegionHG);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? fadeMaxRegion : fadeAlphaRegionMG);
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? fadeMaxRegion : fadeAlphaRegionLG);
    }

    // If mid-band is hovered, draw mid-band on top with highlight.  Doubles as default view.
    if (mMidFocus || (!mLowFocus && !mMidFocus && !mHighFocus))
    {
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? fadeMaxRegion : fadeAlphaRegionHG);
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? fadeMaxRegion : fadeAlphaRegionLG);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? fadeMaxRegion : fadeAlphaRegionMG);
    }

    // If high-band is hovered, draw high-band on top with highlight.
    if (mHighFocus)
    {
        paintLowRegion(  g,    responseArea, gainLowPixel,     freq1Pixel,                mLowFocus    ? fadeMaxRegion : fadeAlphaRegionLG);
        paintMidRegion(  g,    responseArea, gainMidPixel,     freq1Pixel, freq2Pixel,    mMidFocus    ? fadeMaxRegion : fadeAlphaRegionMG);
        paintHighRegion( g,    responseArea, gainHighPixel,    freq2Pixel,                mHighFocus    ? fadeMaxRegion : fadeAlphaRegionHG);
    }
}

// Draw Low-Frequency Response Trapezoid
void FrequencyResponse::paintLowRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float alpha)
{
    using namespace AllColors::FrequencyResponseColors;
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
        g.setGradientFill    (    lowBandBypass ?
                                BYPASS_REGION_GRADIENT(bounds.toFloat()) :
                                LOW_REGION_GRADIENT(bounds.toFloat()) );

        g.setOpacity(alpha);
        g.fillPath(lowRegion.createPathWithRoundedCorners(10));

        g.setColour        (    lowBandBypass ? REGION_BORDER_COLOR_BYPASS() :
                            REGION_BORDER_COLOR_LOW());

        g.strokePath(lowRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }

}

// Draw Mid-Frequency Response Trapezoid
void FrequencyResponse::paintMidRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq1Pixel, float freq2Pixel, float alpha)
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
        g.setGradientFill    (    midBandBypass ?
                                BYPASS_REGION_GRADIENT(bounds.toFloat()) :
                                MID_REGION_GRADIENT(bounds.toFloat()) );

        g.setOpacity(alpha);
        g.fillPath(midRegion.createPathWithRoundedCorners(10));

        g.setColour        (    midBandBypass ? REGION_BORDER_COLOR_BYPASS() :
                            REGION_BORDER_COLOR_MID());

        g.strokePath(midRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }

}

// Draw High-Frequency Response Trapezoid
void FrequencyResponse::paintHighRegion(juce::Graphics& g, juce::Rectangle<float> bounds, float gainPixel, float freq2Pixel, float alpha)
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
        g.setGradientFill    (    highBandBypass ?
                                BYPASS_REGION_GRADIENT(bounds.toFloat()) :
                                HIGH_REGION_GRADIENT(bounds.toFloat()) );

        g.setOpacity(alpha);
        g.fillPath(highRegion.createPathWithRoundedCorners(10));

        g.setColour        (    highBandBypass ? REGION_BORDER_COLOR_BYPASS() :
                            REGION_BORDER_COLOR_HIGH());

        g.strokePath(highRegion.createPathWithRoundedCorners(10), juce::PathStrokeType(2.f));
    }
}

void FrequencyResponse::paintCursorsFrequency(juce::Graphics& g)
{
    // Draw Low-Mid Cursor
    g.setColour(juce::Colours::white);
    g.setOpacity(mLowMidFocus ? 1.f : fadeAlphaCursorLM);
    g.drawLine(cursorLM, 3.f);

    // Draw Mid-High Cursor
    g.setColour(juce::Colours::white);
    g.setOpacity(mMidHighFocus ? 1.f : fadeAlphaCursorMH);
    g.drawLine(cursorMH, 3.f);
}

void FrequencyResponse::paintCursorsGain(juce::Graphics& g)
{
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
}

// Fade Components on Mouse Enter
void FrequencyResponse::paintMenu(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setOpacity(0.9f);
    g.fillRoundedRectangle(    buttonBounds.getX(),
                            buttonBounds.getY(),
                            buttonBounds.getWidth(),
                            buttonBounds.getHeight(),
                            2.f);

    buttonOptions.setAlpha(fadeAlphaButton);
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
    gainLowPixel = responseArea.getY() + juce::jmap(mLowGain,    -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
    gainMidPixel = responseArea.getY() + juce::jmap(mMidGain,    -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());
    gainHighPixel = responseArea.getY() + juce::jmap(mHighGain, -30.f, 30.f, float(responseArea.getBottom()), responseArea.getY());

    // Set Cursor Widths. Cursors A Fraction of Band Width
    auto lowWidth = (freq1Pixel - responseArea.getX()) * 0.75f;
    auto midWidth = (freq2Pixel - freq1Pixel) * 0.75f;
    auto highWidth = (responseArea.getRight() - freq2Pixel) * 0.75f;

    // Set Low-Gain Cursor Bounds
    auto center = (responseArea.getX() + freq1Pixel) / 2.f;

    cursorLG.setStart    (center - lowWidth / 2.f, gainLowPixel);
    cursorLG.setEnd        (center + lowWidth / 2.f, gainLowPixel);

    // Set Mid-Gain Cursor Bounds
    center = (freq1Pixel + freq2Pixel) / 2.f;
    
    cursorMG.setStart    (center - midWidth / 2.f, gainMidPixel);
    cursorMG.setEnd        (center + midWidth / 2.f, gainMidPixel);

    // Set High-Gain Cursor Bounds
    center = (freq2Pixel + responseArea.getRight()) / 2.f;

    cursorHG.setStart    (center - highWidth / 2.f, gainHighPixel);
    cursorHG.setEnd        (center + highWidth / 2.f, gainHighPixel);

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
    case 11:  {lowBandMute = true;                        break; }
    case 100: {midBandMute = true; highBandMute = true; break; }
    case 101: {midBandMute = true;                        break; }
    case 110: {highBandMute = true;                        break; }
    }
}

// Map linear slider value to true octave-logarithmic value
float FrequencyResponse::mapLog2(float freq)
{
    auto logMin = std::log2(20.f);
    auto logMax = std::log2(20000.f);

    return (std::log2(freq) - logMin) / (logMax - logMin);
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
    
    repaint();
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
    
    repaint();
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







/* Mouse Functions ============================================================================== */
/* ============================================================================================== */


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













/* Fade Functions =============================================================================== */
/* ============================================================================================== */



// Fade Cursor when within bounds
void FrequencyResponse::fadeInOutCursorLM()
{
    if (fadeInCursorLM) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursorLM < fadeMaxCursor)
        {
            fadeCompleteCursorLM = false;
            fadeAlphaCursorLM += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursorLM > fadeMaxCursor)
        {
            fadeAlphaCursorLM = fadeMaxCursor;
            fadeCompleteCursorLM = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursorLM > fadeMinCursor)
        {
            fadeCompleteCursorLM = false;
            fadeAlphaCursorLM -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursorLM < fadeMinCursor)
        {
            fadeAlphaCursorLM = fadeMinCursor;
            fadeCompleteCursorLM = true;
        }
    }
}

void FrequencyResponse::fadeInOutCursorMH()
{
    if (fadeInCursorMH) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursorMH < fadeMaxCursor)
        {
            fadeCompleteCursorMH = false;
            fadeAlphaCursorMH += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursorMH > fadeMaxCursor)
        {
            fadeAlphaCursorMH = fadeMaxCursor;
            fadeCompleteCursorMH = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursorMH > fadeMinCursor)
        {
            fadeCompleteCursorMH = false;
            fadeAlphaCursorMH -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursorMH < fadeMinCursor)
        {
            fadeAlphaCursorMH = fadeMinCursor;
            fadeCompleteCursorMH = true;
        }
    }
}

void FrequencyResponse::fadeInOutCursorLG()
{
    if (fadeInCursorLG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursorLG < fadeMaxCursor)
        {
            fadeCompleteCursorLG = false;
            fadeAlphaCursorLG += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursorLG > fadeMaxCursor)
        {
            fadeAlphaCursorLG = fadeMaxCursor;
            fadeCompleteCursorLG = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursorLG > fadeMinCursor)
        {
            fadeCompleteCursorLG = false;
            fadeAlphaCursorLG -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursorLG < fadeMinCursor)
        {
            fadeAlphaCursorLG = fadeMinCursor;
            fadeCompleteCursorLG = true;
        }
    }
}

void FrequencyResponse::fadeInOutCursorMG()
{
    if (fadeInCursorMG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursorMG < fadeMaxCursor)
        {
            fadeCompleteCursorMG = false;
            fadeAlphaCursorMG += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursorMG > fadeMaxCursor)
        {
            fadeAlphaCursorMG = fadeMaxCursor;
            fadeCompleteCursorMG = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursorMG > fadeMinCursor)
        {
            fadeCompleteCursorMG = false;
            fadeAlphaCursorMG -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursorMG < fadeMinCursor)
        {
            fadeAlphaCursorMG = fadeMinCursor;
            fadeCompleteCursorMG = true;
        }
    }
}

void FrequencyResponse::fadeInOutCursorHG()
{
    if (fadeInCursorHG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaCursorHG < fadeMaxCursor)
        {
            fadeCompleteCursorHG = false;
            fadeAlphaCursorHG += fadeStepUpCursor;
        }
            
        /* Fade in complete */
        if (fadeAlphaCursorHG > fadeMaxCursor)
        {
            fadeAlphaCursorHG = fadeMaxCursor;
            fadeCompleteCursorHG = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaCursorHG > fadeMinCursor)
        {
            fadeCompleteCursorHG = false;
            fadeAlphaCursorHG -= fadeStepDownCursor;
        }

        /* Fade out complete */
        if (fadeAlphaCursorHG < fadeMinCursor)
        {
            fadeAlphaCursorHG = fadeMinCursor;
            fadeCompleteCursorHG = true;
        }
    }
}

void FrequencyResponse::fadeInOutRegionLow()
{
    if (fadeRegionLG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaRegionLG <= fadeMaxRegion)
        {
            fadeCompleteRegionLow = false;
            fadeAlphaRegionLG += fadeStepUpRegion;
        }
            
        /* Fade in complete */
        if (fadeAlphaRegionLG >= fadeMaxRegion)
        {
            fadeAlphaRegionLG = fadeMaxRegion;
            fadeCompleteRegionLow = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaRegionLG >= fadeMinRegion)
        {
            fadeCompleteRegionLow = false;
            fadeAlphaRegionLG -= fadeStepDownRegion;
        }

        /* Fade out complete */
        if (fadeAlphaRegionLG <= fadeMinRegion)
        {
            fadeAlphaRegionLG = fadeMinRegion;
            fadeCompleteRegionLow = true;
        }
    }
}

void FrequencyResponse::fadeInOutRegionMid()
{
    if (fadeRegionMG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaRegionMG <= fadeMaxRegion)
        {
            fadeCompleteRegionMid = false;
            fadeAlphaRegionMG += fadeStepUpRegion;
        }
            
        /* Fade in complete */
        if (fadeAlphaRegionMG >= fadeMaxRegion)
        {
            fadeAlphaRegionMG = fadeMaxRegion;
            fadeCompleteRegionMid = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaRegionMG >= fadeMinRegion)
        {
            fadeCompleteRegionMid = false;
            fadeAlphaRegionMG -= fadeStepDownRegion;
        }

        /* Fade out complete */
        if (fadeAlphaRegionMG <= fadeMinRegion)
        {
            fadeAlphaRegionMG = fadeMinRegion;
            fadeCompleteRegionMid = true;
        }
    }
}

void FrequencyResponse::fadeInOutRegionHigh()
{
    if (fadeRegionHG) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaRegionHG <= fadeMaxRegion)
        {
            fadeCompleteRegionHigh = false;
            fadeAlphaRegionHG += fadeStepUpRegion;
        }
            
        /* Fade in complete */
        if (fadeAlphaRegionHG >= fadeMaxRegion)
        {
            fadeAlphaRegionHG = fadeMaxRegion;
            fadeCompleteRegionHigh = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaRegionHG >= fadeMinRegion)
        {
            fadeCompleteRegionHigh = false;
            fadeAlphaRegionHG -= fadeStepDownRegion;
        }

        /* Fade out complete */
        if (fadeAlphaRegionHG <= fadeMinRegion)
        {
            fadeAlphaRegionHG = fadeMinRegion;
            fadeCompleteRegionHigh = true;
        }
    }
}

void FrequencyResponse::fadeButton()
{
    if (fadeInButton || showMenu) // If mouse entered... fade Toggles Alpha up
    {
        /* Fade in until complete */
        if (fadeAlphaButton <= fadeMaxButton)
        {
            fadeCompleteButton = false;
            fadeAlphaButton += fadeStepUpButton;
        }
            
        /* Fade in complete */
        if (fadeAlphaButton >= fadeMaxButton)
        {
            fadeAlphaButton = fadeMaxButton;
            fadeCompleteButton = true;
        }
    }
    else // If mouse exit... fade Toggles Alpha down
    {
        /* Fade out until complete */
        if (fadeAlphaButton >= fadeMinButton)
        {
            fadeCompleteButton = false;
            fadeAlphaButton -= fadeStepDownButton;
        }

        /* Fade out complete */
        if (fadeAlphaButton <= fadeMinButton)
        {
            fadeAlphaButton = fadeMinButton;
            fadeCompleteButton = true;
        }
    }
}










/* Focus Functions ============================================================================== */
/* ============================================================================================== */

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

    //bool    lmCursorIsInMenu{ false },
    //        mhCursorIsInMenu{ false },
    //        lgCursorIsInMenu{ false },
    //        mgCursorIsInMenu{ false },
    //        hgCursorIsInMenu{ false };

    //// Check if LM Cursor is in Menu
    //if (cursorLM.getStartX() < buttonBounds.getRight())
    //    lmCursorIsInMenu = true;

    //// Check if MH Cursor is in Menu
    //if (cursorMH.getStartX() < buttonBounds.getRight())
    //    mhCursorIsInMenu = true;

    //// Check if LG Cursor is in Menu
    //auto lgCenter = cursorLG.getStartX() + cursorLG.getEndX() / 2.f;

    //if (lgCenter < buttonBounds.getRight() && cursorLG.getStartY() < buttonBounds.getBottom())
    //    lgCursorIsInMenu = true;

    //// Check if MG Cursor is in Menu
    //auto mgCenter = cursorMG.getStartX() + cursorMG.getEndX() / 2.f;

    //if (mgCenter < buttonBounds.getRight() && cursorMG.getStartY() < buttonBounds.getBottom())
    //    mgCursorIsInMenu = true;

    //// Check if HG Cursor is in Menu
    //auto hgCenter = cursorHG.getStartX() + cursorHG.getEndX() / 2.f;

    //if (hgCenter < buttonBounds.getRight() && cursorHG.getStartY() < buttonBounds.getBottom())
    //    hgCursorIsInMenu = true;

    //if (lmCursorIsInMenu && showMenu)
    //    fadeInCursorLM = false;

    //if (mhCursorIsInMenu && showMenu)
    //    fadeInCursorMH = false;


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

// Check for external band hovering.  Refactor for encapsulation.
void FrequencyResponse::checkExternalFocus()
{
    mLowFocus = (    globalControls.mTimingControls.timingBarLow.hasFocus ||
                    globalControls.mGainControls.gainBarLow.hasFocus ||
                    globalControls.mWaveControls.waveBarLow.hasFocus) || fadeRegionLG;

    mMidFocus = (    globalControls.mTimingControls.timingBarMid.hasFocus ||
                    globalControls.mGainControls.gainBarMid.hasFocus ||
                    globalControls.mWaveControls.waveBarMid.hasFocus) || fadeRegionMG;

    mHighFocus = (    globalControls.mTimingControls.timingBarHigh.hasFocus ||
                    globalControls.mGainControls.gainBarHigh.hasFocus ||
                    globalControls.mWaveControls.waveBarHigh.hasFocus) || fadeRegionHG;

    mLowMidFocus =  globalControls.mXoverControls.lowMidFocus;
    mMidHighFocus = globalControls.mXoverControls.midHighFocus;
}
