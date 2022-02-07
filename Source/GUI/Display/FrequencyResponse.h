/*
  ==============================================================================

	FrequencyResponse.cpp
	Created: 30 Dec 2021 11:38:20am
	Author:  Joe Caulfield

	Class to display and control the frequency response of the crossover

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../../GUI/Controls/GlobalControls.h"

struct FrequencyResponse : juce::Component,
	juce::Timer,
	juce::MouseListener,
	juce::Slider::Listener
{

	FrequencyResponse(juce::AudioProcessorValueTreeState& apv, GlobalControls& gc);

	void paint(juce::Graphics& g) override;
	void drawBorder(juce::Graphics& g);
	void drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::Colour color) {};

	void sliderValueChanged(juce::Slider* slider) override;

	void drawLowRegion(juce::Graphics& g,
		juce::Rectangle<float> bounds,
		float gainPixel,
		float freq1Pixel,
		float alpha);

	void drawMidRegion(juce::Graphics& g,
		juce::Rectangle<float> bounds,
		float gainPixel,
		float freq1Pixel,
		float freq2Pixel,
		float alpha);

	void drawHighRegion(juce::Graphics& g,
		juce::Rectangle<float> bounds,
		float gainPixel,
		float freq2Pixel,
		float alpha);

	void drawGridVertical(juce::Graphics& g);

	void drawGridHorizontal(juce::Graphics& g);

	void resized() override;

	void makeAttachments();

	void timerCallback() override;

	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override {};
	void mouseUp(const juce::MouseEvent& event) override {};
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override {};
	void mouseDoubleClick(const juce::MouseEvent& event) override {};

	void updateResponse();

	void fadeInOutComponents();
	void fadeCursor();
	void fadeRegions();

	void checkMousePosition();

	void checkExternalFocus();
	void checkCursorFocus(const juce::MouseEvent& event);

	float mapLog2(float freq);

	void checkSolos();

	//TertiaryAudioProcessor& audioProcessor;
	GlobalControls& globalControls;

	// Frequency Response Parameters
	juce::AudioParameterFloat* lowMidCutoff{ nullptr };         // Pointer to the APVTS
	juce::AudioParameterFloat* midHighCutoff{ nullptr };         // Pointer to the APVTS
	juce::AudioParameterFloat* lowGain{ nullptr };         // Pointer to the APVTS
	juce::AudioParameterFloat* midGain{ nullptr };         // Pointer to the APVTS
	juce::AudioParameterFloat* highGain{ nullptr };         // Pointer to the APVTS

	// Solo Params
	bool lowBandSolo{ false }, midBandSolo{ false }, highBandSolo{ false };
	bool lowBandMute{ false }, midBandMute{ false }, highBandMute{ false };
	bool lowBandBypass{ false }, midBandBypass{ false }, highBandBypass{ false };

private:
	juce::AudioProcessorValueTreeState& apvts;

	//juce::Atomic<bool> paramsChanged{ false };

	float mLowMidCutoff, mMidHighCutoff, mLowGain, mMidGain, mHighGain;
	bool mLowFocus{ false }, mMidFocus{ false }, mHighFocus{ false }, mLowMidFocus, mMidHighFocus;
	bool mLowBypass{ false }, mMidBypass{ false }, mHighBypass{ false };


	// COMPONENT FADES ==========================================================================================================================================================================================
	bool fadeIn{ false };			float fadeAlpha{ 1.f };				float fadeMax = 1.f;			float fadeMin = 0.0f;			float fadeStepUp = 0.1f;			float fadeStepDown = 0.1f;
	bool fadeInCursorLM{ false };	float fadeAlphaCursorLM{ 1.f };		float fadeMaxCursor = 1.f;		float fadeMinCursor = 0.25;		float fadeStepUpCursor = 0.1f;		float fadeStepDownCursor = 0.1f;
	bool fadeInCursorMH{ false };	float fadeAlphaCursorMH{ 1.f };
	bool fadeInCursorLG{ false };	float fadeAlphaCursorLG{ 1.f };		float fadeMaxRegion = 0.85f;	float fadeMinRegion = 0.65f;	float fadeStepUpRegion = 0.001f;	float fadeStepDownRegion = 0.001f;
	bool fadeInCursorMG{ false };	float fadeAlphaCursorMG{ 1.f };		
	bool fadeInCursorHG{ false };	float fadeAlphaCursorHG{ 1.f };
	bool fadeRegionLG{ false };		float fadeAlphaRegionLG{ 0.65f };
	bool fadeRegionHG{ false };		float fadeAlphaRegionHG{ 0.65f };
	bool fadeRegionMG{ false };		float fadeAlphaRegionMG{ 0.65f };
	// COMPONENT FADES ==========================================================================================================================================================================================


	juce::Line<float> cursorLM, cursorMH, cursorLG, cursorMG, cursorHG;
	bool cursorDragLM{ false };
	bool cursorDragMH{ false };
	float dragX_LM{ 0 }, dragX_MH{ 0 };

	float freq1Pixel, freq2Pixel, gainLowPixel, gainMidPixel, gainHighPixel;

	juce::Rectangle<float> responseArea;

	juce::Array<float> freqs
	{
		20, 40, 80, 160, 320, 640, 1300, 2500, 5100, 10000, 20000
	};

	// DRAW THE HORIZONTAL AXIS ===============
	juce::Array<float> gain
	{
		-30, -24, -18, -12, -6, 0, 6, 12, 18, 24, 30
	};

	juce::Slider	sliderLowMidCutoff,
					sliderMidHighCutoff,
					sliderLowMidInterface,
					sliderMidHighInterface,
					sliderLowGain,
					sliderMidGain,
					sliderHighGain;

	using sliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	std::unique_ptr<sliderAttachment>	lowMidAttachment,
										midHighAttachment,
										lowGainAttachment,
										midGainAttachment,
										highGainAttachment;
};