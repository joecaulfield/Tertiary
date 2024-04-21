/*
  ==============================================================================

    WindowWrapperFrequency.cpp
    Created: 13 Oct 2022 11:12:35pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "WindowWrapperFrequency.h"


WindowWrapperFrequency::WindowWrapperFrequency( TertiaryAudioProcessor& p,
                                                juce::AudioProcessorValueTreeState& apv,
                                                GlobalControls& gc)
                                                
: audioProcessor(p),
globalControls(gc),
apvts(apv),
lowFrequencyLabelRef(frequencyResponse.freqLabelLow),
highFrequencyLabelRef(frequencyResponse.freqLabelHigh),
forwardFFT(audioProcessor.fftOrder),
window(audioProcessor.fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris)
{
    addAndMakeVisible(frequencyResponse);
  
    // Array Maintenance ==========
    fftDrawingPoints.ensureStorageAllocated(audioProcessor.scopeSize);
    fftDrawingPoints.resize(audioProcessor.scopeSize);

    for (int i = 0; i < audioProcessor.scopeSize; i++)
        fftDrawingPoints.setUnchecked(i, 0);
    
    startTimerHz(30);

    addAndMakeVisible(optionsMenu);

    buildOptionsMenuParameters();

}

WindowWrapperFrequency::~WindowWrapperFrequency()
{

}

void WindowWrapperFrequency::resized()
{
    frequencyResponse.setSize(getWidth(), getHeight());

    // =============================
    //spectrumArea = getLocalBounds().toFloat();
    spectrumArea = frequencyResponse.getBounds().toFloat();
    spectrumArea.reduce(2, 2);
    spectrumArea.removeFromBottom(20);
    spectrumArea.removeFromLeft(32);
    spectrumArea.removeFromRight(6);
}

void WindowWrapperFrequency::timerCallback()
{
    // Check for new FFT information
    if (audioProcessor.nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        audioProcessor.nextFFTBlockReady = false;
    }
    
    /* Repaint the FFT Area */
    repaint(spectrumArea.toNearestInt());
}

void WindowWrapperFrequency::paint(juce::Graphics& g)
{

}

void WindowWrapperFrequency::paintOverChildren(juce::Graphics& g)
{
    /* FFT Animation goes here */
    
    ///* Paint fft */
    // =========================
    if (mShouldShowFFT)
        paintFFT(g, spectrumArea);
}
 
void WindowWrapperFrequency::paintFFT(juce::Graphics& g, juce::Rectangle<float> bounds)
{

    for (int i = 1; i < audioProcessor.scopeSize; ++i)
    {
        float startY = bounds.getY() + juce::jmap(audioProcessor.scopeData[i - 1],
            0.0f,
            1.0f,
            bounds.getHeight(),
            0.0f);

        fftDrawingPoints.set(i - 1, startY);
    }

    juce::Path f;

    f.startNewSubPath(bounds.getX() + 2, bounds.getBottom());

    int curve = 3;

    for (int i = 0; i < fftDrawingPoints.size() - curve - 1; i++)
    {
        if (i % curve == 0 && i > 0)
        {
            float x0 = juce::jmap((float)i - curve, 0.f,
                (float)(fftDrawingPoints.size()) - 1.f,
                2.f,
                bounds.getWidth() - 2);

            float x1 = juce::jmap((float)i,
                0.f,
                (float)(fftDrawingPoints.size()) - 1.f,
                2.f,
                bounds.getWidth() - 2);

            float x2 = juce::jmap((float)i + curve,
                0.f,
                (float)(fftDrawingPoints.size()) - 1.f,
                2.f,
                bounds.getWidth() - 2);

            juce::Point<float> point0 = { bounds.getX() + x0, fftDrawingPoints[i - curve] - 2 };
            juce::Point<float> point1 = { bounds.getX() + x1, fftDrawingPoints[i] - 2 };
            juce::Point<float> point2 = { bounds.getX() + x2, fftDrawingPoints[i + curve] - 2 };

            float tempMax = juce::jmin(point0.getY(), point1.getY(), point2.getY());

            f.cubicTo(point0, point1, point2);
        }

    }

    f.lineTo(bounds.getRight(), bounds.getBottom());
    f.closeSubPath();

    // Fill FFT Background
    float p1 = 0.25f;
    float p2 = 0.5f;

    auto gradient = juce::ColourGradient(juce::Colours::grey,
        bounds.getBottomLeft(),
        juce::Colours::lightgrey,
        bounds.getTopRight(), false);

    gradient.addColour(p1, juce::Colours::white.withBrightness(1.25f));
    gradient.addColour(p2, juce::Colours::whitesmoke.withBrightness(1.25f));

    g.setGradientFill(gradient);
    g.setOpacity(0.75f);
    g.fillPath(f);

    // Fill FFT Outline
    g.setColour(juce::Colours::darkgrey);
    g.setOpacity(0.8f);
    g.strokePath(f, juce::PathStrokeType(0.5f));

}




void WindowWrapperFrequency::drawNextFrameOfSpectrum()
{
    // Apply Window Function to Data
    window.multiplyWithWindowingTable(audioProcessor.fftData, audioProcessor.fftSize);

    // Render FFT Data
    forwardFFT.performFrequencyOnlyForwardTransform(audioProcessor.fftData);

    auto mindB = -60.f;
    auto maxdB = 0.0f;

    for (int i = 0; i < audioProcessor.scopeSize; ++i)
    {
        auto linearScale = i / float(audioProcessor.scopeSize) * 9.699f;
        auto skewedProportionX = (20 * pow(2, linearScale)) / 20000;

        //
        auto fftDataIndex = juce::jlimit(0,                                                          // Lower
            audioProcessor.fftSize / 2,                                      // Upper Limit
            (int)(skewedProportionX * (float)audioProcessor.fftSize * 0.5f)  // Value to Constrain
        );

        // Clamps the value to within specified dB range
        auto limit = juce::jlimit(mindB,      // Lower Limit
            maxdB,      // Upper Limit
            juce::Decibels::gainToDecibels(audioProcessor.fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)audioProcessor.fftSize)  // Value to Constrain
        );


        auto level = juce::jmap(limit,  // Source Value
            mindB,  // Source Range Min
            maxdB,  // Source Range Max
            0.0f,   // Target Range Min
            1.0f    // Target Range Max
        );

        audioProcessor.scopeData[i] = level;                                   // [4]
    }


}



void WindowWrapperFrequency::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateOptionsParameters();
}



/* Links menu options to parameters */
// ========================================================
void WindowWrapperFrequency::buildOptionsMenuParameters()
{
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

    auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)    // Bool Helper --> Part 8 Param Namespace
        {
            param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
            jassert(param != nullptr);                                                                      // Error Checking
        };

    boolHelper(showFftParam, Names::Show_FFT);

    // Choice Helper To Attach Choice to Parameter ========
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
        {
            param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
            jassert(param != nullptr);
        };

    choiceHelper(fftPickoffParam, Names::FFT_Pickoff);

    optionsMenu.addOptionToList("Show FFT",
        "Show FFT",
        audioProcessor.apvts,
        params.at(Names::Show_FFT));

    optionsMenu.addOptionToList("FFT Pre",
        "FFT Pre",
        audioProcessor.apvts,
        params.at(Names::FFT_Pickoff));

    audioProcessor.apvts.addParameterListener(params.at(Names::Show_FFT), this);
    audioProcessor.apvts.addParameterListener(params.at(Names::FFT_Pickoff), this);

    updateOptionsParameters();
}



void WindowWrapperFrequency::updateOptionsParameters()
{

    mShouldShowFFT = showFftParam->get();

    auto mPickoffID = fftPickoffParam->getIndex();
    audioProcessor.setFftPickoffPoint(mPickoffID);
}
