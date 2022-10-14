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
#include "../../GUI/Controls/GlobalControls.h"
#include "../../GUI/AllColors.h"
#include "../UtilityFunctions.h"
#include "ScrollPad.h"

struct Oscilloscope :	juce::Component,
						juce::Timer,
						juce::Button::Listener
{
	Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
	~Oscilloscope() override;


	/* Generate paths for LFO when new information is available */
	void generateLFOPathForDrawing(	juce::Rectangle<int> bounds,
									juce::Path &lfoStrokePath,
									juce::Path &lfoFillPath,
									juce::Array<float>& waveTable,
									bool showBand,
									LFO lfo);

	/* Actions upon resize occur here. Plug-in is fixed-size, so this is called once upon instantiation */
	void resized() override;

	/* Timer Callback */
	void timerCallback() override;

	void mouseEnter(const juce::MouseEvent& event) override {};
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDoubleClick(const juce::MouseEvent& event) override;

    //void updatePreferences();
	void checkMousePosition();
	void getPlayheadPosition();

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

	void getWavesForDisplay() {};
	juce::Array<float> scaleWaveAmplitude(juce::Array<float> waveTable, LFO lfo);

	float playHeadPositionPixel;

	void drawStackedScope();


	void setToggleEnable(bool enabled);
	void drawSliders() {};
    
    bool getShowPlayhead() {return mShowPlayhead; }
    bool getPanOrZoomChanging() {return panOrZoomChanging; }
    float getPlayheadPositionPixel() {return playHeadPositionPixel; }
    
    juce::Rectangle<int> getLowRegion() {return lowRegion; }
    juce::Rectangle<int> getMidRegion() {return midRegion; }
    juce::Rectangle<int> getHighRegion() {return highRegion; }
    
    /* Paint Functions */
    // ================================================================================
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void paintRegions(juce::Graphics& g);
    void paintPlayhead(juce::Graphics& g);
    void paintCursor(juce::Graphics& g);
    void paintMenu(juce::Graphics& g);
    void paintGridLines(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintWaveform( juce::Graphics& g,
                        bool shouldShowBand,
                        bool isBandBypassed,
                        juce::Rectangle<int> regionBounds,
                        bool isBandFocused,
                        juce::Path pathToDraw,
                        juce::Path pathToFill ,
                        juce::ColourGradient fillGradient,
                        juce::Colour strokeColor);


    /* Paint-Flag Functions */
    // ================================================================================
    void updateAndCheckForChangesToDisplayPreferences();
    bool checkForChangesToLfo           (LFO& lfo);
    bool checkForChangesToLfoInvert     (LFO& lfo, int index);
    bool checkForChangesToLfoWaveshape  (LFO& lfo, int index);
    bool checkForChangesToLfoSkew       (LFO& lfo, int index);
    bool checkForChangesToLfoDepth      (LFO& lfo, int index);
    bool checkForChangesToLfoSync       (LFO& lfo, int index);
    bool checkForChangesToLfoRhythm     (LFO& lfo, int index);
    bool checkForChangesToLfoRate       (LFO& lfo, int index);
    bool checkForChangesToLfoPhase      (LFO& lfo, int index);
    bool updateAndCheckForChangesToLfoFocus      (LFO& lfo);
    bool updateAndCheckForChangesToLfoBypass     (LFO& lfo);


    
    
    //bool hasBypassChangedLow{false};
    //bool hasBypassChangedMid{false};
    //bool hasBypassChangedHigh{false};
    
    
    //bool lowLfoChanged{ true };
    //bool midLfoChanged{ true };
    //bool highLfoChanged{ true };

    bool panOrZoomChanging{ false };

    bool mLowBypass{ false }, mMidBypass{ false }, mHighBypass{ false };
    

    
    int     oldInvert[3] = { 0,0,0 };
    int     oldShape[3] = { 0,0,0 };
    float   oldSkew[3] = { 0,0,0 };
    float   oldDepth[3] = { 0,0,0 };
    int     oldSync[3] = { 0,0,0 };
    float   oldRhythm[3] = { 0,0,0 };
    float   oldRate[3] = { 0,0,0 };
    float   oldPhase[3] = { 0,0,0 };
    int     oldBypass[3] = { 0,0,0 };
    int     oldFocus[3] = {0,0,0};


    int     oldRegions[4] = { 0,0,0,0 };
    

    
    
    
    
    
    
    
    
    
    

	void makeAttachments();

	TertiaryAudioProcessor& audioProcessor;
	GlobalControls& globalControls;
	ScrollLookAndFeel scrollLookAndFeel;
	


	/* Options Menu =================================== */

	ButtonOptionsLookAndFeel optionsLookAndFeel;
	juce::TextButton buttonOptions;
	juce::Rectangle<float> buttonBounds;
	bool showMenu{ false };
	void drawToggles(bool showMenu);
	void buttonClicked(juce::Button* button) override;
	juce::ToggleButton	toggleShowLow, 
						toggleShowMid, 
						toggleShowHigh, 
						toggleStackBands,
						toggleShowCursor,
						toggleShowPlayhead;

    /* Pointers to the APVTS */
	juce::AudioParameterBool* showLowBandParam{ nullptr };
	juce::AudioParameterBool* showMidBandParam{ nullptr };
	juce::AudioParameterBool* showHighBandParam{ nullptr };
	juce::AudioParameterBool* stackBandsParam{ nullptr };
	juce::AudioParameterBool* showCursorParam{ nullptr };
	juce::AudioParameterBool* showPlayheadParam{ nullptr };

	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	std::unique_ptr<buttonAttachment>	showLowAttachment,
										showMidAttachment,
										showHighAttachment,
										stackBandsAttachment,
										showCursorAttachment,
										showPlayheadAttachment;


	ScrollPad sliderScroll;

	bool updateLfoDisplay{ true };

	float mDisplayPhase = 0.f;

private:

	LFO& lowLFO;
	LFO& midLFO;
	LFO& highLFO;

	int timerCounter = 0;

	juce::Array<float> waveTableLow;
	juce::Array<float> waveTableMid;
	juce::Array<float> waveTableHigh;

	juce::Rectangle<int> lowRegion, midRegion, highRegion, scopeRegion;
	juce::Path lowPath, midPath, highPath;
	juce::Path lowPathFill, midPathFill, highPathFill;
	juce::Image lowImage, midImage, highImage;



	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	scrollAttachment;

	bool mShowLowBand, mShowMidBand, mShowHighBand, mStackAllBands, mShowCursor, mShowPlayhead;
	bool mLowFocus, mMidFocus, mHighFocus;


    void fadeInOutMenu();
    void fadeInOutCursor();
    
	bool fadeInMenu{ false };			bool fadeInCursor{ false };
	float fadeAlphaMenu{ 1.f };			float fadeAlphaCursor{ 1.f };
	float fadeMaxMenu = 1.f;			float fadeMaxCursor = 1.f;
	float fadeMinMenu = 0.0f;			float fadeMinCursor = 0.65;
	float fadeStepUpMenu = 0.1f;		float fadeStepUpCursor = 0.1f;
	float fadeStepDownMenu = 0.01f;		float fadeStepDownCursor = 0.05f;
    bool fadeCompleteMenu{false};       bool fadeCompleteCursor{false};

	float sampleRate;

	

	juce::Line<float> cursor;
	bool cursorDrag{ false };
	float mCursorPosition{ 0 };
	float scrollZoom{ 0 };
	float scrollPan{ 0 };

	juce::AudioParameterFloat* scopeCursorParam{ nullptr };
	juce::AudioParameterFloat* scopePoint1Param{ nullptr };
	juce::AudioParameterFloat* scopePoint2Param{ nullptr };

	float numDepthLines{ 1 };
};
