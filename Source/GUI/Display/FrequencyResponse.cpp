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
FrequencyResponse::FrequencyResponse(    TertiaryAudioProcessor& p,
                                        juce::AudioProcessorValueTreeState& apv,
                                        GlobalControls& gc)
    : audioProcessor(p),
    globalControls(gc),
    apvts(apv),
    forwardFFT(audioProcessor.fftOrder),
    window(audioProcessor.fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris)
{

    //using namespace Params;
    //const auto& params = GetParams();

    //auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)    // Bool Helper --> Part 8 Param Namespace
    //{
    //    param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
    //    jassert(param != nullptr);                                                                      // Error Checking
    //};

    //boolHelper(showFftParameter, Names::Show_FFT);

    //mShowFFT = showFftParameter->get();

    // Choice Helper To Attach Choice to Parameter ========
    //auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    //{
    //    param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
    //    jassert(param != nullptr);
    //};

    //choiceHelper(fftPickoffParameter, Names::FFT_Pickoff);



    //mPickOffID = fftPickoffParameter->getIndex();
    //updateToggleStates();

    //switch (pickOffID)
    //{
    //    case 0: togglePickInput.setToggleState(true, false); break;        // Pre
    //    case 1: togglePickOutput.setToggleState(true, false); break;    // Post
    //}

    // Array Maintenance ==========
    //fftDrawingPoints.ensureStorageAllocated(audioProcessor.scopeSize);
    //fftDrawingPoints.resize(audioProcessor.scopeSize);

    //for (int i = 0; i < audioProcessor.scopeSize; i++)
    //    fftDrawingPoints.setUnchecked(i, 0);

    // Button Options
    //buttonOptions.setLookAndFeel(&optionsLookAndFeel);
    //buttonOptions.addListener(this);
    //buttonOptions.addMouseListener(this, true);
    //addAndMakeVisible(buttonOptions);

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

    buildLabels();
    
    makeAttachments();

    updateResponse();

}

FrequencyResponse::~FrequencyResponse()
{
    //buttonOptions.setLookAndFeel(nullptr);
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

    //showFftAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
    //                    audioProcessor.apvts,
    //                    params.at(Names::Show_FFT),
    //                    toggleShowRTA);
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
    //buttonOptions.setSize(100, 25);
    //buttonOptions.setTopLeftPosition(4, 4);

}

// Methods to call on a timed-basis
void FrequencyResponse::timerCallback()
{
    
    //if (globalControls.lowBandControls.hasBSMchanged() ||
    //    globalControls.midBandControls.hasBSMchanged() ||
    //    globalControls.highBandControls.hasBSMchanged() ||
    //    updateAndCheckForChangesToFocus() ||
    //    paintFlag)
    //{
    //    repaint();
    //    globalControls.lowBandControls.setBsmRead();
    //    globalControls.midBandControls.setBsmRead();
    //    globalControls.highBandControls.setBsmRead();
    //}

    //checkMousePosition();

    updateResponse();
    
    //fadeInOutCursorLM();
    //fadeInOutCursorMH();
    
    //fadeInOutCursorLG();
    //fadeInOutCursorMG();
    //fadeInOutCursorHG();
    
    //fadeInOutRegionLow();
    //fadeInOutRegionMid();
    //fadeInOutRegionHigh();

    //fadeButton();
    
    //drawLabels();
    
    //checkExternalFocus();
    

    //if (!fadecompletecursorlm)
    //{
    //    repaint(cursorlm.getstartx()-5, cursorlm.getstarty(), 10, cursorlm.getlength() );
    //    repaint(freqlabellow.getbounds().expanded(5, 5));
    //}

    //
    //if (!fadecompletecursormh)
    //{
    //    repaint(cursormh.getstartx()-5, cursormh.getstarty(), 10, cursormh.getlength() );
    //    repaint(freqlabelhigh.getbounds().expanded(5, 5));
    //}

    //
    //if (!fadecompletecursorlg)
    //    repaint(cursorlg.getstartx(), cursorlg.getstarty()-5, cursorlg.getlength(), 10 );
    //
    //if (!fadecompletecursormg)
    //    repaint(cursormg.getstartx(), cursormg.getstarty()-5, cursormg.getlength(), 10 );
    //
    //if (!fadecompletecursorhg)
    //    repaint(cursorhg.getstartx(), cursorhg.getstarty()-5, cursorhg.getlength(), 10 );
    //
    //if (!fadecompleteregionlow || updatedisplay)
    //    repaint();
    //
    //if (!fadecompleteregionmid|| updatedisplay)
    //    repaint();
    //
    //if (!fadecompleteregionhigh || updatedisplay)
    //    repaint();
    //
    //if (!fadecompletebutton || updatedisplay)
    //    repaint(buttonoptions.getbounds());
    //
    //if (updatedisplay)
    //{
    //    repaint();
    //    updatedisplay = false;
    //}



}

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

void FrequencyResponse::paintCursorsFrequency(juce::Graphics& g)
{

    // Draw Low-Mid Cursor
    g.setColour(juce::Colours::white);
    g.setOpacity(mLowMidFocus ? 1.f : fadeAlphaCursorLM);
    g.drawLine(cursorLM, 3.f);
    freqLabelLow.setAlpha(  freqLabelLow.isBeingEdited() ? 1.f :
                            juce::jmap(fadeAlphaCursorLM, fadeMinCursor, fadeMaxCursor, 0.f, 1.f));

    // Draw Mid-High Cursor
    g.setColour(juce::Colours::white);
    g.setOpacity(mMidHighFocus ? 1.f : fadeAlphaCursorMH);
    g.drawLine(cursorMH, 3.f);
    freqLabelHigh.setAlpha(  freqLabelHigh.isBeingEdited() ? 1.f :
                             juce::jmap(fadeAlphaCursorMH, fadeMinCursor, fadeMaxCursor, 0.f, 1.f));
    
    drawLabels();
    
}

void FrequencyResponse::drawLabels()
{
    auto bounds = getLocalBounds();
    int labelWidth = 65;
    int labelHeight = 25;
    
    int margin = 5;
    
//    int labelX1 = bounds.getRight() - labelWidth - margin;
//    int labelX2 = bounds.getRight() - labelWidth - margin;
//    int labelY1 = bounds.getY() + margin;
//    int labelY2 = bounds.getY() + margin + labelHeight + margin;
    
    int labelX1 = cursorLM.getStartX() - labelWidth/2;
    int labelX2 = cursorMH.getStartX() - labelWidth/2;
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
    
    if (freqLabelLow.getRight() > freqLabelHigh.getX() - 2 * margin)
    {
        //labelY1 += 30;
    }
        
    freqLabelLow.setBounds(labelX1, labelY1 , labelWidth, labelHeight);
    freqLabelHigh.setBounds(labelX2, labelY2, labelWidth, labelHeight);

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
//void FrequencyResponse::paintMenu(juce::Graphics& g)
//{
//    g.setColour(juce::Colours::white);
//    g.setOpacity(0.9f);
//    g.fillRoundedRectangle( buttonBounds.getX(),
//                            buttonBounds.getY(),
//                            buttonBounds.getWidth(),
//                            buttonBounds.getHeight(),
//                            2.f);
//
//    for (int i = 0; i < 5; i++)
//    {
//        g.setColour(juce::Colours::darkgrey.withMultipliedAlpha(1.f/((i*i)+0.75f)));
//        g.drawRect(buttonBounds.toFloat().reduced(i,i), 1.f);
//    }
//}

// Paint Labels
void FrequencyResponse::paintLabels(juce::Graphics& g)
{
    /* Draw Low Frequency Label */
    // ====================================================
    g.setColour(juce::Colours::black);
    
    auto opacityL = juce::jmap(fadeAlphaCursorLM, fadeMinCursor, fadeMaxCursor, 0.f, 1.f);
    auto opacityH = juce::jmap(fadeAlphaCursorMH, fadeMinCursor, fadeMaxCursor, 0.f, 1.f);
    
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

    repaint();
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
    case 1:   {lowBandMute = true; midBandMute = true;      break; }
    case 10:  {lowBandMute = true; highBandMute = true;     break; }
    case 11:  {lowBandMute = true;                          break; }
    case 100: {midBandMute = true; highBandMute = true;     break; }
    case 101: {midBandMute = true;                          break; }
    case 110: {highBandMute = true;                         break; }
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

//void FrequencyResponse::drawToggles(bool showMenu)
//{
    //using namespace juce;

    /* Toggle Menu Visibility Based On Display Params */
    //if (showMenu)
    //    buttonBounds.setBounds(buttonOptions.getX(), buttonOptions.getBottom(), buttonOptions.getWidth(), 75);
    //else
    //{
    //    buttonBounds.setBounds(0, 0, 0, 0);
    //    updateDisplay = true;
    //}


    //FlexBox flexBox;
    //flexBox.flexDirection = FlexBox::Direction::column;
    //flexBox.flexWrap = FlexBox::Wrap::noWrap;

    //auto margin = FlexItem().withHeight(5);
    //auto spacer = FlexItem().withHeight(2.5);
    //auto height = (150.f - 2.f * 5.f - 5.f * 2.5f) / 6.f;

    //flexBox.items.add(margin);
    //flexBox.items.add(FlexItem(toggleShowRTA).withHeight(height));
    //flexBox.items.add(spacer);
    //flexBox.items.add(FlexItem(togglePickInput).withHeight(height));
    //flexBox.items.add(spacer);
    //flexBox.items.add(FlexItem(togglePickOutput).withHeight(height));
    //flexBox.items.add(margin);

    //flexBox.performLayout(buttonBounds);

    //toggleShowRTA.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
    //togglePickInput.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
    //togglePickOutput.setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

    //toggleShowRTA.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    //togglePickInput.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    //togglePickOutput.setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    //
    //toggleShowRTA.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
    //togglePickInput.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
    //togglePickOutput.setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);

    //toggleShowRTA.setButtonText("Show FFT");
    //togglePickInput.setButtonText("FFT In");
    //togglePickOutput.setButtonText("FFT Out");

    //togglePickInput.setRadioGroupId(1);
    //togglePickOutput.setRadioGroupId(1);

    //addAndMakeVisible(toggleShowRTA);
    //addAndMakeVisible(togglePickInput);
    //addAndMakeVisible(togglePickOutput);
//}

void FrequencyResponse::buttonClicked(juce::Button* button)
{
    //if (button == &buttonOptions)
    //{
    //    showMenu = !showMenu;
    //    drawToggles(showMenu);
    //    DBG("freq clicked");
    //}

    //if (button == &toggleShowRTA)
    //{
    //    mShowFFT = toggleShowRTA.getToggleState();
    //}
    //    
    //if (button == &togglePickInput || button == &togglePickOutput)
    //{
    //    mPickOffID = togglePickOutput.getToggleState();

    //    updateToggleStates();
    //}
    //
    //updateDisplay = true;

}

//void FrequencyResponse::updateToggleStates()
//{
//    // When Pickoff ID Changes,
//    // Update Toggle States
//    // Set Parameter
//
//    switch (mPickOffID)
//    {
//        case 0: // Pre
//        {
//            togglePickInput.setToggleState(true, false);
//            togglePickOutput.setToggleState(false, false); break;
//        }
//        case 1: // Post
//        {
//            togglePickOutput.setToggleState(true, false);
//            togglePickInput.setToggleState(false, false); break;
//        }
//    }
//
//    fftPickoffParameter->setValueNotifyingHost(mPickOffID);
//
//    audioProcessor.setFftPickoffPoint(mPickOffID);
//}




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
    }
}

// Called on Mouse Move
void FrequencyResponse::mouseMove(const juce::MouseEvent& event)
{
    checkCursorFocus(event);
}

// Called on Mouse Enter
void FrequencyResponse::mouseEnter(const juce::MouseEvent& event)
{
    checkCursorFocus(event);
}

// Called on Mouse Exit
void FrequencyResponse::mouseExit(const juce::MouseEvent& event)
{
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
    //if (fadeInButton || showMenu) // If mouse entered... fade Toggles Alpha up
    //{
    //    /* Fade in until complete */
    //    if (fadeAlphaButton < fadeMaxButton)
    //    {
    //        fadeCompleteButton = false;
    //        fadeAlphaButton += fadeStepUpButton;
    //    }
    //        
    //    /* Fade in complete */
    //    if (fadeAlphaButton >= fadeMaxButton)
    //    {
    //        fadeAlphaButton = fadeMaxButton;
    //        fadeCompleteButton = true;
    //    }
    //}
    //else // If mouse exit... fade Toggles Alpha down
    //{
    //    /* Fade out until complete */
    //    if (fadeAlphaButton > fadeMinButton)
    //    {
    //        fadeCompleteButton = false;
    //        fadeAlphaButton -= fadeStepDownButton;
    //    }

    //    /* Fade out complete */
    //    if (fadeAlphaButton < fadeMinButton)
    //    {
    //        fadeAlphaButton = fadeMinButton;
    //        fadeCompleteButton = true;
    //    }
    //}
    
    //buttonOptions.setAlpha(fadeAlphaButton);
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

    // Mouse is on Low-Mid Cursor or Label
    if (abs(xM - xLM) < xMargin || freqLabelLow.isMouseOver())
        fadeInCursorLM = true;
    else
        fadeInCursorLM = false;
    
    // Mouse is on Mid-High Cursor
    if (abs((xM - xMH)) < xMargin || freqLabelHigh.isMouseOver())
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



    // Mouse is out of window, kill all fades
    if (xM < x0 || xM > x1 || yM < y0 || yM > y1)
    {
        fadeRegionLG = false;
        fadeRegionMG = false;
        fadeRegionHG = false;
        //fadeInCursorLM = false;
        //fadeInCursorMH = false;
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

//bool FrequencyResponse::checkMenuFocus(const juce::MouseEvent& event)
//{
//    auto x = event.getPosition().getX();
//    auto y = event.getPosition().getY();
//
//    bool mouseIsInMenu{ false };
//
//    if (x > buttonBounds.getX() && x < buttonBounds.getRight() &&
//        y > buttonBounds.getY() && y < buttonBounds.getBottom())
//        mouseIsInMenu = true;
//
//    if (buttonOptions.isMouseOverOrDragging() || mouseIsInMenu)
//    {
//        fadeInButton = true;
//        fadeInCursorLM = false;
//        fadeInCursorMH = false;
//        fadeInCursorLG = false;
//        fadeInCursorMG = false;
//        fadeInCursorHG = false;
//        return true;
//    }
//    else
//    {
//        fadeInButton = false;
//        return false;
//    }
//}

bool FrequencyResponse::updateAndCheckForChangesToFocus()
{

    oldFocus[0] = mLowFocus || fadeRegionLG;
    mLowFocus = globalControls.lowBandControls.isMouseOverOrDragging(true);

    oldFocus[1] = mMidFocus || fadeRegionMG;
    mMidFocus = globalControls.midBandControls.isMouseOverOrDragging(true);

    oldFocus[2] = mHighFocus || fadeRegionHG;
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



// ========================================================
void FrequencyResponse::actionListenerCallback(const juce::String& message)
{

    auto paramName = message.replaceSection(0, 10, "");
    paramName = paramName.replaceSection(10, 25, "");
    paramName = paramName.removeCharacters("x");

    juce::String paramValue = message.replaceSection(0, 25, "");
    paramValue = paramValue.removeCharacters("x");


    DBG(paramName);

    if (paramName == "GAIN")
    {
        updateResponse();
    }

    if (paramName == "BYPASS")
    {
        updateResponse();
    }

    if (paramName == "SOLO")
    {
        updateResponse();
    }

    if (paramName == "MUTE")
    {
        updateResponse();
    }

}
