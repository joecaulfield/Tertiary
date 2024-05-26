/*
  ==============================================================================

	Oscilloscope.cpp
	Created: 30 Dec 2021 11:38:53am
	Author:  Joe Caulfield

	// Class containing the oscilloscope which shows
	// the individual LFO waveforms for viewing
  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../PluginProcessor.h"
#include "ScopeChannel.h"
#include "../../Utility/AllColors.h"
#include "../../Utility/UtilityFunctions.h"
#include "ScrollPad.h"

#include "OptionsMenu.h"

struct Oscilloscope :	juce::Component
{
    Oscilloscope(TertiaryAudioProcessor& p);
	~Oscilloscope() override;

    juce::String mNameSpace{ "Oscilloscope" };
    bool setDebug{ true };

	void resized() override;

	void mouseUp(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;

    bool getPanOrZoomChanging() {return panOrZoomChanging; }

    /* Called by wrapper & options menu, to update parameters */
    void updateScopeParameters(bool showLow, bool showMid, bool showHigh, bool stackBands);

    /* Paint Functions */
    // ================================================================================
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;

    bool panOrZoomChanging{ false };

	TertiaryAudioProcessor& audioProcessor;
	ScrollLookAndFeel scrollLookAndFeel;

	ScrollPad sliderScroll;

	bool updateLfoDisplay{ true };

    ScopeChannel& getLowScope(){return lowScope;};
    ScopeChannel& getMidScope(){return midScope;};
    ScopeChannel& getHighScope(){return highScope;};
    
private:

    LFO& lowLFO;
	LFO& midLFO;
	LFO& highLFO;
    
    ScopeChannel lowScope, midScope, highScope;
    
    void buildScopeLayout();
    void buildStackedScopeLayout();
    void buildOverlappedScopeLayout();
    
    /* Area in which the scope channels should live */
    juce::Rectangle<int> scopeRegion;
    
	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	std::unique_ptr<sliderAttachment>	scrollAttachment;

    bool mShowLowBand{ true }, mShowMidBand{ true }, mShowHighBand{ true }, mStackAllBands{ false };
    bool mLowFocus{ false }, mMidFocus{ false }, mHighFocus{ false };

};
