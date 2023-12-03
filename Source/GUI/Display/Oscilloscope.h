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
#include "../../GUI/Controls/GlobalControls.h"
#include "../../Utility/AllColors.h"
#include "../../Utility/UtilityFunctions.h"
#include "ScrollPad.h"

#include "OptionsMenu.h"

struct Oscilloscope :	juce::Component,
						juce::Timer
{
	Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
	~Oscilloscope() override;

	void resized() override;
	void timerCallback() override;
    
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDoubleClick(const juce::MouseEvent& event) override;

	void checkMousePosition();
	void getPlayheadPosition();

    bool getPanOrZoomChanging() {return panOrZoomChanging; }
    float getPlayheadPositionPixel() {return playHeadPositionPixel; }

    /* Called by wrapper & options menu, to update parameters */
    void updateScopeParameters(bool showLow, bool showMid, bool showHigh, bool stackBands);

    /* Paint Functions */
    // ================================================================================
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void paintCursor(juce::Graphics& g);

    bool panOrZoomChanging{ false };

	TertiaryAudioProcessor& audioProcessor;
	GlobalControls& globalControls;
	ScrollLookAndFeel scrollLookAndFeel;

	ScrollPad sliderScroll;

	bool updateLfoDisplay{ true };

private:

    LFO& lowLFO;
	LFO& midLFO;
	LFO& highLFO;
    
    ScopeChannel lowScope, midScope, highScope;
    
    void buildScopeLayout();
    void buildStackedScopeLayout();
    void buildOverlappedScopeLayout();
    
    bool checkForChangesToFocus();
    int  oldFocus[3] = {0,0,0};
    
    /* Area in which the scope channels should live */
    juce::Rectangle<int> scopeRegion;
    
	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	std::unique_ptr<sliderAttachment>	scrollAttachment;

    bool mShowLowBand, mShowMidBand, mShowHighBand, mStackAllBands, mShowCursor;
	bool mLowFocus, mMidFocus, mHighFocus;
    
    //bool mShowPlayhead;

    void fadeInOutCursor();
    
    bool fadeInCursor{ false };
    float fadeAlphaCursor{ 1.f };
    float fadeMaxCursor = 1.f;
    float fadeMinCursor = 0.65f;
    float fadeStepUpCursor = 0.1f;
    float fadeStepDownCursor = 0.01f;
    bool fadeCompleteCursor{false};

	juce::Line<float> cursor;
	bool cursorDrag{ false };
	float mCursorPosition{ 0 };

	juce::AudioParameterFloat* scopeCursorParam{ nullptr };
	juce::AudioParameterFloat* scopePoint1Param{ nullptr };
	juce::AudioParameterFloat* scopePoint2Param{ nullptr };
    
    
    float beatSpacing{ 1.f };                       // Pixel-Width of One Complete On-Screen Beat
    float playBackNumBeats{ 1.f };                  // Number of Quarter Notes Shown In Display - Including One Overhanging QN
    int playBackNumOnscreenBeats{ 2 };
    float leastOnscreenLeftGridLine{ 0.f };         // Left-Most Complete Onscreen Beat
    float greatestOnscreenRightGridLine{ 0.f };     // Right-Most Complete Onscreen Beat
    float onScreenNumBeats{ 1.f };                  // Total Number of Complete Onscreen Beats
    float onScreenBeatWidth{ 1.f };                 // Pixel Width of All Beats Onscreen
    float onScreenNumBeatsLeftFromCenter{ 1.f };
    float onScreenNumBeatsRightFromCenter{ 1.f };
    float playBackWidth{ 1.f };                     // Total Number of Beats scaled into a Pixel-Valued Width
    float playHeadPositionPixel;

};
