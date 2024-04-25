/*
  ==============================================================================

    ScopeChannel.h
    Created: 29 Jan 2023 3:54:47pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../PluginProcessor.h"
#include "../../Utility/AllColors.h"
#include "ScrollPad.h"
#include <melatonin_perfetto/melatonin_perfetto.h>
#include "../../WLDebugger.h"

struct ScopeChannel :   juce::Component,
                        juce::AudioProcessorValueTreeState::Listener,
                        juce::ActionListener
{
    ScopeChannel(juce::AudioProcessorValueTreeState& apvts, LFO& lfo, ScrollPad& sliderScroll);
    ~ScopeChannel();
    
    #if PERFETTO
        std::unique_ptr<perfetto::TracingSession> tracingSession;
    #endif

    void paint(juce::Graphics& g) override;
    void paintGridLines(juce::Graphics& g);
    void paintWaveform(juce::Graphics& g);
    
    void resized() override;
    void parameterChanged(const juce::String& parameterID, float newValue) override {};
    void actionListenerCallback(const juce::String& message) override;

    void updateBandBypass();

    void setBandsStacked(bool areStacked) {bandsAreStacked = areStacked;}
    
    void redrawScope();
    
private:
    
    juce::AudioProcessorValueTreeState& apvts;

    LFO& lfo;   // Chopping Block
    LFO localLFO;

    juce::String mNameSpace { "ScopeChannel" };

    double mSampleRate;
    double mHostBpm;

    ScrollPad& sliderScroll;
    float scrollZoom = 1.f;
    float scrollCenter = 1.f;

    bool isBandFocused{false};
    bool focusHasChanged {false};
    
    bool isBandBypassed{false};
    bool bypassHasChanged{false};

    int waveTableDownSampleSize = 8;
    
    juce::String parameterChangedID = "";
    float parameterChangedNewValue {0.f};
    
    // Pixel-Width of One Complete On-Screen Beat
    float beatSpacing{ 1.f };

    /*Number of Quarter Notes Shown In Display - Including One Overhanging QN*/
    float playBackNumBeats{ 1.f };
    int playBackNumOnscreenBeats{ 2 };

    // Left-Most Complete Onscreen Beat
    float leastOnscreenLeftGridLine{ 0.f };

    // Right-Most Complete Onscreen Beat
    float greatestOnscreenRightGridLine{ 0.f };

    // Total Number of COmplete Onscreen Beats
    float onScreenNumBeats{ 1.f };

    // Pixel Width of All Beats Onscreen
    float onScreenBeatWidth{ 1.f };

    // Number of Onscreen Beats to the Left of Center
    float onScreenNumBeatsLeftFromCenter{ 1.f };
    float onScreenNumBeatsRightFromCenter{ 1.f };

    // Total Number of Beats scaled into a Pixel-Valued Width
    float playBackWidth{ 1.f };
    
    float numDepthLines{ 1 };
    
    // Having issues with values instantiating at beginning of plug-in.
    // Add timer to allow multiple passes of value initialization
    int timerCounterInit {0};
    int timerCounterParam{0};
    
    void generateLFOPathForDrawing( juce::Path &lfoStrokePath,
                                    juce::Path &lfoFillPath,
                                    juce::Array<float> &waveTable,
                                    LFO& lfo);
    
    juce::Array<float> scaleWaveAmplitude();
    
    bool bandsAreStacked{true};
        
    bool parameterHasChanged{true};
    
    juce::Path lfoPath, lfoFill;
    juce::Array<float> waveTable;
    
    void setBandColors();

    juce::Colour strokeColor;
    juce::ColourGradient fillGradient;
    
};
