/*
  ==============================================================================

    Oscilloscope.h
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../PluginProcessor.h"
#include "../../GUI/Controls/GlobalControls.h"
#include "../../GUI/AllColors.h"

// Create own 'Cursor' class??

struct Oscilloscope :	juce::Component,
						juce::Timer, 
						juce::MouseListener
{
	Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc);
	~Oscilloscope() override;

	void paint(juce::Graphics& g) override;
	void paintOverChildren(juce::Graphics& g) override;

	void drawBorder(juce::Graphics& g);
	void drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::Colour color);

	void drawLowLFO (juce::Rectangle<int> bounds);
	void drawMidLFO (juce::Rectangle<int> bounds);
	void drawHighLFO(juce::Rectangle<int> bounds);

	void resized() override;

	void timerCallback() override;

	void mouseEnter(const juce::MouseEvent& event) override;
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

	void drawStackedScope();

	void drawToggles();
	void drawSliders() {};
	void fadeInOutComponents(juce::Graphics& g);
	void drawAndFadeCursor(juce::Graphics& g, juce::Rectangle<int> bounds);

	void makeAttachments();

	TertiaryAudioProcessor& audioProcessor;
	GlobalControls& globalControls;
	
	ScrollLookAndFeel scrollLookAndFeel;

	juce::ToggleButton	toggleShowLow, 
						toggleShowMid, 
						toggleShowHigh, 
						toggleStackBands;

	juce::Slider sliderScroll;

	juce::AudioParameterBool* showLowBand{ nullptr };       // Pointer to the APVTS
	juce::AudioParameterBool* showMidBand{ nullptr };       // Pointer to the APVTS
	juce::AudioParameterBool* showHighBand{ nullptr };      // Pointer to the APVTS
	juce::AudioParameterBool* stackBands{ nullptr };        // Pointer to the APVTS

private:

	LFO& lowLFO;
	LFO& midLFO;
	LFO& highLFO;

	juce::Rectangle<int> lowRegion, midRegion, highRegion, scopeRegion;
	juce::Path lowPath, midPath, highPath;
	juce::Path lowPathFill, midPathFill, highPathFill;
	juce::Image lowImage, midImage, highImage;

	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	std::unique_ptr<buttonAttachment>	showLowAttachment,
										showMidAttachment,
										showHighAttachment,
										stackBandsAttachment;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	scrollAttachment;

	bool mShowLowBand, mShowMidBand, mShowHighBand, mStackAllBands;
	bool mLowFocus, mMidFocus, mHighFocus;
	bool mLowBypass{ false }, mMidBypass{ false }, mHighBypass{ false };

	bool fadeIn{ false };			bool fadeInCursor{ false };
	float fadeAlpha{ 1.f };			float fadeAlphaCursor{ 1.f };
	float fadeMax = 1.f;			float fadeMaxCursor = 1.f;
	float fadeMin = 0.0f;			float fadeMinCursor = 0.65;
	float fadeStepUp = 0.1f;		float fadeStepUpCursor = 0.1f;
	float fadeStepDown = 0.1f;		float fadeStepDownCursor = 0.1f;

	float sampleRate;

	juce::Rectangle<float> buttonBounds;

	juce::Line<float> cursor;
	bool cursorDrag{ false };
	float dragX{ 0 };

	juce::AudioParameterFloat* scopeScrollParam{ nullptr };

	float numDepthLines{ 1 };
};