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

struct ScrollPad :	juce::Component,
					juce::Slider::Listener
{
public:

	ScrollPad();
	~ScrollPad();

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDoubleClick(const juce::MouseEvent& event) override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	juce::Slider slider;

	void sliderValueChanged(juce::Slider* slider) override {};

	void calculateZoomFactor();

	void initializePoints(float newP1 = 0.f, float newP2 = 0.f);
	void updatePoints(int x1, int y1);
	void makeDefaultPoints();

	//void setCenter(float panPercentage);
	float getCenter();

	int getMaxWidth(){ return maxWidth; }
	int getMinWidth() { return minWidth; }
	float getMinZoomFactor() { return minZoomFactor; }
	float getMaxZoomFactor() { return maxZoomFactor; }
	float getPoint1() { return point1; }
	float getPoint2() { return point2; }

	//void setZoom(float zoom);
	float getZoom();

private:
	float point1;	// Value & Position of Left Thumb
	float point2;	// Value & Position of Right Thumb
	float p01, p02;

	bool leftHit{ false }, rightHit{ false }, midHit{ false };

	float xDown, x0, x1;		// Determines Mouse Movement, X-Direction
	float yDown, y0, y1;		// Determines Mouse Movement, Y-Direction

	bool shouldUpdatePoint1, shouldUpdatePoint2, shouldCheckPanOrZoom;

	int currentCenter{ 0 };
	int minWidth{ 80 };
	int defaultWidth{ 160 };
	float currentWidth { 100 };
	int maxWidth{ 360 };

	float minZoomFactor{ 0.5f };
	float currentZoomFactor{ 1.f };
	float maxZoomFactor{ 10.f };


	int thumbTolerance = 75;
	int zoomScale{ 1 };
	float zoomIncrement = 10;

};

struct Oscilloscope :	juce::Component,
						juce::Timer,
						juce::Button::Listener
{
	Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
	~Oscilloscope() override;

	/* Paint the graphics */
	void paint(juce::Graphics& g) override;

	/* Paint on top of child components */
	void paintOverChildren(juce::Graphics& g) override;

	/* Draws the axes and grid-lines for corresponding LFO, based on tempo and zoom/pan scaling */
	void drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds);

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

	void updatePreferences();
	void checkFocus();
	void checkMousePosition();
	void updateRegions();

	void getPanZoom();
	
	void getPlayheadPosition();

	bool checkIfLfoHasChanged(LFO &lfo);

	bool lowLfoChanged{ true };
	bool midLfoChanged{ true };
	bool highLfoChanged{ true };

	bool panOrZoomChanging{ false };

	int oldInvert[3] = { 0,0,0 };
	int oldShape[3] = { 0,0,0 };
	float oldSkew[3] = { 0,0,0 };
	float oldDepth[3] = { 0,0,0 };
	int oldSync[3] = { 0,0,0 };
	float oldRhythm[3] = { 0,0,0 };
	float oldRate[3] = { 0,0,0 };
	float oldPhase[3] = { 0,0,0 };

	int oldRegions[4] = { 0,0,0,0 };




	/*	Amount of not-shown pixels in the quarter-note 
	that overhangs the edge of the screen.
	Ignores display shift and assumes centered pan.	*/
	//float playBackOffset{ 0.f };
	//float playBackOffsetLeft{ 0.f };
	//float playBackOffsetRight{ 0.f };

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

	void getWavesForDisplay();
	juce::Array<float> scaleWaveAmplitude(juce::Array<float> waveTable, LFO lfo);

	float playHeadPositionPixel;

	void drawStackedScope();


	void setToggleEnable(bool enabled);
	void drawSliders() {};
	void fadeInOutComponents(juce::Graphics& g);
	void drawAndFadeCursor(juce::Graphics& g, juce::Rectangle<int> bounds);

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

	juce::AudioParameterBool* showLowBand{ nullptr };       // Pointer to the APVTS
	juce::AudioParameterBool* showMidBand{ nullptr };       // Pointer to the APVTS
	juce::AudioParameterBool* showHighBand{ nullptr };      // Pointer to the APVTS
	juce::AudioParameterBool* stackBands{ nullptr };        // Pointer to the APVTS
	juce::AudioParameterBool* showCursor{ nullptr };        // Pointer to the APVTS
	juce::AudioParameterBool* showPlayhead{ nullptr };      // Pointer to the APVTS

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
	bool mLowBypass{ false }, mMidBypass{ false }, mHighBypass{ false };

	bool fadeIn{ false };			bool fadeInCursor{ false };
	float fadeAlpha{ 1.f };			float fadeAlphaCursor{ 1.f };
	float fadeMax = 1.f;			float fadeMaxCursor = 1.f;
	float fadeMin = 0.0f;			float fadeMinCursor = 0.65;
	float fadeStepUp = 0.1f;		float fadeStepUpCursor = 0.1f;
	float fadeStepDown = 0.01f;		float fadeStepDownCursor = 0.05f;

	float sampleRate;

	

	juce::Line<float> cursor;
	bool cursorDrag{ false };
	float dragX{ 0 };
	float scrollZoom{ 0 };
	float scrollPan{ 0 };

	juce::AudioParameterFloat* scopeCursorParam{ nullptr };
	juce::AudioParameterFloat* scopePoint1Param{ nullptr };
	juce::AudioParameterFloat* scopePoint2Param{ nullptr };

	float numDepthLines{ 1 };
};
