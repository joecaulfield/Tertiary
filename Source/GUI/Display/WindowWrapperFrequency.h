/*
  ==============================================================================

    WindowWrapperFrequency.h
    Created: 13 Oct 2022 11:12:35pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FrequencyResponse.h"
#include "OptionsMenu.h"
#include "../../WLDebugger.h"

struct WindowWrapperFrequency : juce::Component, 
                                juce::Timer,
                                juce::AudioProcessorValueTreeState::Listener
{
    //WindowWrapperFrequency(TertiaryAudioProcessor& p, juce::AudioProcessorValueTreeState& apv, GlobalControls& gc);
    WindowWrapperFrequency(TertiaryAudioProcessor& p, juce::AudioProcessorValueTreeState& apv);
    ~WindowWrapperFrequency();
    
    void resized();
    
    void timerCallback();
    
    void paint(juce::Graphics& g);
    void paintFFT(juce::Graphics& g, juce::Rectangle<float> bounds);

    FrequencyResponse& getFrequencyResponse() { return frequencyResponse; };

private:
    /* Reference to the Audio Processor & DSP Parameters */

    juce::String mNameSpace{ "WindowWrapperFrequency" };
    bool setDebug{ true };

    TertiaryAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    
    FrequencyResponse frequencyResponse{ audioProcessor, audioProcessor.apvts };

    void buildOptionsMenuParameters();
    void updateOptionsParameters();

    OptionsMenu optionsMenu;
    juce::AudioParameterBool* showFftParam{ nullptr };
    juce::AudioParameterChoice* fftPickoffParam{ nullptr };

    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<buttonAttachment>	showFftAttachment;

    bool mShouldShowFFT{ false };

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // FFT Components =========
    void calculateNextFrameOfSpectrum();
    
    juce::Array<float> fftDrawingPoints;
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float fftConstant{ 9.9658f };

    juce::Rectangle<float> spectrumArea;
};
