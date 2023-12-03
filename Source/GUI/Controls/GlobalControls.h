/*
  ==============================================================================

    GlobalControls.h
    Created: 30 Dec 2021 11:38:20am
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "InputOutputGain.h"
#include "BandControls.h"
#include "../../../Source/PluginProcessor.h"
#include "../../Utility/AllColors.h"
#include "../../Utility/UtilityFunctions.h"


struct GlobalControls : juce::Component
{
	GlobalControls(TertiaryAudioProcessor& p);

	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;
	void resized() override;

	void makeAttachments();

	juce::AudioProcessorValueTreeState& apvts;
	TertiaryAudioProcessor& audioProcessor;

	InputOutputGain inputGain{ audioProcessor};
	InputOutputGain outputGain{ audioProcessor };
    
    BandControl lowBandControls{apvts};
    BandControl midBandControls{apvts};
    BandControl highBandControls{apvts};
    
private:

    bool shouldPaintOnceOnInit{true};
    void paintOnceOnInit(juce::Graphics& g);
    
    void paintWindowBorders(juce::Graphics& g);
    void paintBandLabels(juce::Graphics& g);
    
	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using comboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

	std::unique_ptr<sliderAttachment>	inputGainAttachment, outputGainAttachment;

    juce::Rectangle<float> controlBorder;
	juce::Rectangle<float> border1, border2, border3, border4, border5, border6, labelBorder;
    
    juce::Label mLabelWaveShape,
                mLabelSkew,
                mLabelDepth,
                mLabelRhythm,
                mLabelPhase,
                mLabelBandGain;
    
    void makeLabel(juce::Label& label, juce::String textToPrint);
    
    void makeGainControlAttachments();
    void makeTimingControlAttachments();
    void makeWaveControlAttachments();
    
    /* From GainControls */
    // ============================================================
    

    std::unique_ptr<sliderAttachment>   gainLowAttachment,
                                        gainMidAttachment,
                                        gainHighAttachment;


    std::unique_ptr<buttonAttachment>   soloLowAttachment,
                                        bypassLowAttachment,
                                        muteLowAttachment,
                                        soloMidAttachment,
                                        bypassMidAttachment,
                                        muteMidAttachment,
                                        soloHighAttachment,
                                        bypassHighAttachment,
                                        muteHighAttachment;
    
    /* From TimingControls */
    // ============================================================
    
    std::unique_ptr<sliderAttachment>   rateLowAttachment,
                                        rateMidAttachment,
                                        rateHighAttachment,
                                        //multLowAttachment,
                                        //multMidAttachment,
                                        //multHighAttachment,
                                        phaseLowAttachment,
                                        phaseMidAttachment,
                                        phaseHighAttachment;
    
    std::unique_ptr<buttonAttachment>   syncLowAttachment,
                                        syncMidAttachment,
                                        syncHighAttachment;
    
    std::unique_ptr<comboBoxAttachment> multLowAttachment,
                                        multMidAttachment,
                                        multHighAttachment;
    
    /* From WaveControls */
    // ============================================================
    std::unique_ptr<sliderAttachment>   //waveLowAttachment,
                                        //waveMidAttachment,
                                        //waveHighAttachment,
                                        depthLowAttachment,
                                        depthMidAttachment,
                                        depthHighAttachment,
                                        symmetryLowAttachment,
                                        symmetryMidAttachment,
                                        symmetryHighAttachment;
    
    std::unique_ptr<comboBoxAttachment> waveLowAttachment,
                                        waveMidAttachment,
                                        waveHighAttachment;

    std::unique_ptr<buttonAttachment>   invertLowAttachment,
                                        invertMidAttachment,
                                        invertHighAttachment;
};
