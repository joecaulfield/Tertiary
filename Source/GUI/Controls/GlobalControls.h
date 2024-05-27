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
//#include "../../WLDebugger.h"


struct GlobalControls : juce::Component,
                        juce::ActionBroadcaster,
                        juce::MouseListener
{
	GlobalControls(TertiaryAudioProcessor& p);

	void paint(juce::Graphics& g) override;
	//void paintOverChildren(juce::Graphics& g) override;
	void resized() override;

	void makeAttachments();

	juce::AudioProcessorValueTreeState& apvts;
	TertiaryAudioProcessor& audioProcessor;

	InputOutputGain inputGain{ audioProcessor};
	InputOutputGain outputGain{ audioProcessor };
    
    // Pass Children up to PluginEditor
    BandControl& getLowControl() {return lowBandControls; };
    BandControl& getMidControl() {return midBandControls; };
    BandControl& getHighControl() {return highBandControls; };
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    void checkForBandFocus();

    void sendBroadcast(juce::String parameterName, juce::String parameterValue);

private:

    BandControl lowBandControls{ apvts };
    BandControl midBandControls{ apvts };
    BandControl highBandControls{ apvts };

    bool setDebug{ false };

    juce::String mNameSpace{ "GlobalControls" };

    //bool shouldPaintOnceOnInit{true};
    //void paintOnceOnInit(juce::Graphics& g);
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
    std::unique_ptr<sliderAttachment>   depthLowAttachment,
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
