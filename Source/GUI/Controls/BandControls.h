/*
  ==============================================================================

    BandControls.h
    Created: 29 Oct 2022 8:41:50pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../../Source/PluginProcessor.h"
#include "../../Utility/AllColors.h"
#include "../../Utility/MyLookAndFeel.h"
#include "../../DSP/Params.h"


struct BandControl :    juce::Component,
                        juce::Button::Listener,
                        juce::ActionBroadcaster
                        /*juce::MouseListener,*/
                        
{
    BandControl(juce::AudioProcessorValueTreeState& apv);
    ~BandControl();
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    void buttonClicked(juce::Button* button) override;

    //void mouseEnter(const juce::MouseEvent& event) override {};
    //void mouseExit(const juce::MouseEvent& event) override {};
    //void mouseMove(const juce::MouseEvent& event) override {};
    
    void setMode(juce::String bandMode);

    //bool hasBSMchanged() {return mBsmChanged; }
    //void setBsmRead() {mBsmChanged = false;}
    
    void sendBroadcast(juce::String parameterName, juce::String parameterValue);

    /* Called by GlobalControls for Positioning */
    juce::Rectangle<int> getWaveBounds() {return mDropWaveshape.getBoundsInParent(); }
    juce::Rectangle<int> getSkewBounds() {return mSliderSkew.getBoundsInParent(); }
    juce::Rectangle<int> getDepthBounds() {return mSliderDepth.getBoundsInParent(); }
    juce::Rectangle<int> getRhythmBounds() {return placeholder->getBoundsInParent(); }
    juce::Rectangle<int> getPhaseBounds() {return mSliderPhase.getBoundsInParent(); }
    juce::Rectangle<int> getGainBounds() {return mSliderBandGain.getBoundsInParent(); }
    
    CustomSlider    mSliderRate,
                    mSliderPhase,
                    mSliderSkew,
                    mSliderDepth,
                    mSliderBandGain;
    
    CustomToggle    mToggleSync,
                    mToggleInvert,
                    mToggleBypass,
                    mToggleSolo,
                    mToggleMute;
    
    juce::ComboBox  mDropWaveshape,
                    mDropRhythm;
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    int mode{0};
    juce::String bandModeName;
    
    bool isSyncedToHost {false};
    
    bool runOnceOnConstructor{true};

    void buildToggleSync();
    void buildToggleInvert();
    void buildRateOrRhythm();
    void buildMenuWaveshape();
    void buildSliderPhase();
    void buildSliderSkew();
    void buildSliderDepth();
    void buildSliderBandGain();
    void buildToggleBypass();
    void buildToggleSolo();
    void buildToggleMute();
    void updateFlexBox();

    //bool mBsmChanged{true};

    /* Holds place of either Rate or Rhythm component */
    juce::Component* placeholder;
    
    WaveComboLookAndFeel waveComboLookAndFeel;
    MultComboLookAndFeel multComboLookAndFeel;
    
    /* From GainControls */
    // ====================================
    using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<sliderAttachment>   gainLowAttachment,
                                        gainMidAttachment,
                                        gainHighAttachment;
    
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<buttonAttachment>   soloLowAttachment,
                                        bypassLowAttachment,
                                        muteLowAttachment,
                                        soloMidAttachment,
                                        bypassMidAttachment,
                                        muteMidAttachment,
                                        soloHighAttachment,
                                        bypassHighAttachment,
                                        muteHighAttachment;
    
};
