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


/* ============================================================== */
struct FrequencyResponse :	juce::Component,
							juce::Timer,
							juce::Slider::Listener,
							juce::Button::Listener,
                            juce::Label::Listener
{

	FrequencyResponse(TertiaryAudioProcessor& p, juce::AudioProcessorValueTreeState& apv, GlobalControls& gc);
	~FrequencyResponse();

	void paint(juce::Graphics& g) override;
	//void paintFFT(juce::Graphics& g, juce::Rectangle<float> bounds);
	
	void drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::Colour color) {};

	void sliderValueChanged(juce::Slider* slider) override;

    void paintResponseRegions(juce::Graphics& g);
    
	void paintLowRegion(	juce::Graphics& g,
                            juce::Rectangle<float> bounds,
                            float gainPixel,
                            float freq1Pixel,
                            float alpha);

	void paintMidRegion(	juce::Graphics& g,
                            juce::Rectangle<float> bounds,
                            float gainPixel,
                            float freq1Pixel,
                            float freq2Pixel,
                            float alpha);

	void paintHighRegion(	juce::Graphics& g,
							juce::Rectangle<float> bounds,
							float gainPixel,
							float freq2Pixel,
							float alpha);

	void paintGridGain(juce::Graphics& g);

	void paintGridFrequency(juce::Graphics& g);
    
    void paintCursorsFrequency(juce::Graphics& g);
    
    void paintCursorsGain(juce::Graphics& g);
    
    //void paintOverChildren(juce::Graphics& g);
    //float y {0.f};
    
    bool getShowFFT(){ return mShowFFT; }
    bool isMenuOpen() {return showMenu; }
    juce::Rectangle<int> getMenuButtonBounds() {return buttonBounds.toNearestInt();}
    
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

	void paintMenu(juce::Graphics& g);
    void paintLabels(juce::Graphics& g);
	void fadeInOutCursorLM();
    void fadeInOutCursorMH();
    void fadeInOutCursorLG();
    void fadeInOutCursorMG();
    void fadeInOutCursorHG();
    void fadeInOutRegionLow();
    void fadeInOutRegionMid();
    void fadeInOutRegionHigh();
    
    void fadeButton();

	void checkMousePosition();

	//void checkExternalFocus();
	void checkCursorFocus(const juce::MouseEvent& event);

	float mapLog2(float freq);

	void checkSolos();

	TertiaryAudioProcessor& audioProcessor;
	GlobalControls& globalControls;

	// Frequency Response Parameters
	juce::AudioParameterFloat* lowMidCutoff{ nullptr };     // Pointer to the APVTS
	juce::AudioParameterFloat* midHighCutoff{ nullptr };    // Pointer to the APVTS
	juce::AudioParameterFloat* lowGain{ nullptr };			// Pointer to the APVTS
	juce::AudioParameterFloat* midGain{ nullptr };			// Pointer to the APVTS
	juce::AudioParameterFloat* highGain{ nullptr };         // Pointer to the APVTS

	// Solo Params
	bool lowBandSolo{ false },		midBandSolo{ false },	highBandSolo{ false };
	bool lowBandMute{ false },		midBandMute{ false },	highBandMute{ false };
	bool lowBandBypass{ false },	midBandBypass{ false }, highBandBypass{ false };

    void labelTextChanged(juce::Label* labelThatHasChanged) override;
    
    juce::Label freqLabelLow, freqLabelHigh;
    juce::TextEditor freqLabeLowEditor, freqLabelHighEditor;
    
private:
    
	juce::AudioProcessorValueTreeState& apvts;
    
    bool updateAndCheckForChangesToFocus();
    int     oldFocus[3] = {0,0,0};

    void drawLabels();
    
    bool paintFlag{true};
    
    //juce::ColourGradient makeLowRegionGradient(juce::Rectangle<float> bounds);
    //juce::ColourGradient makeMidRegionGradient(juce::Rectangle<float> bounds);
    //juce::ColourGradient makeHighRegionGradient(juce::Rectangle<float> bounds);
    
	float mLowMidCutoff, mMidHighCutoff, mLowGain, mMidGain, mHighGain;
	bool mLowFocus{ false }, mMidFocus{ false }, mHighFocus{ false }, mLowMidFocus, mMidHighFocus;
	bool mLowBypass{ false }, mMidBypass{ false }, mHighBypass{ false };


	// COMPONENT FADES ==========================================================================================================================================================================================
	bool fadeInButton{ false };		float fadeAlphaButton{ 1.f };		float fadeMaxButton = 1.f;		float fadeMinButton = 0.0f;		float fadeStepUpButton = 0.1f;		float fadeStepDownButton = 0.01f;
	bool fadeInCursorLM{ false };	float fadeAlphaCursorLM{ 1.f };		float fadeMaxCursor = 1.f;		float fadeMinCursor = 0.25f;		float fadeStepUpCursor = 0.1f;		float fadeStepDownCursor = 0.01f;
	bool fadeInCursorMH{ false };	float fadeAlphaCursorMH{ 1.f };
	bool fadeInCursorLG{ false };	float fadeAlphaCursorLG{ 1.f };		float fadeMaxRegion = 0.85f;	float fadeMinRegion = 0.65f;	float fadeStepUpRegion = 0.001f;	float fadeStepDownRegion = 0.001f;
	bool fadeInCursorMG{ false };	float fadeAlphaCursorMG{ 1.f };		
	bool fadeInCursorHG{ false };	float fadeAlphaCursorHG{ 1.f };
	bool fadeRegionLG{ false };		float fadeAlphaRegionLG{ 0.65f };
	bool fadeRegionHG{ false };		float fadeAlphaRegionHG{ 0.65f };
	bool fadeRegionMG{ false };		float fadeAlphaRegionMG{ 0.65f };
    
    bool fadeCompleteCursorLM{false};
    bool fadeCompleteCursorMH{false};
    bool fadeCompleteCursorLG{false};
    bool fadeCompleteCursorMG{false};
    bool fadeCompleteCursorHG{false};
    bool fadeCompleteRegionLow{false};
    bool fadeCompleteRegionMid{false};
    bool fadeCompleteRegionHigh{false};
    bool fadeCompleteButton{false};
    
    bool updateAllPaint{true};
	// COMPONENT FADES ==========================================================================================================================================================================================


	juce::Line<float> cursorLM, cursorMH, cursorLG, cursorMG, cursorHG;
	bool cursorDragLM{ false };
	bool cursorDragMH{ false };
	float dragX_LM{ 0 }, dragX_MH{ 0 };

	float freq1Pixel, freq2Pixel, gainLowPixel, gainMidPixel, gainHighPixel;

	juce::Rectangle<float> responseArea;


    juce::Array<float> freqs { 20, 40, 80, 160, 320, 640, 1300, 2500, 5100, 10000, 20000 };
	juce::Array<float> gain { -30, -24, -18, -12, -6, 0, 6, 12, 18, 24, 30 };

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

	// FFT Components =========
	juce::Array<float> fftDrawingPoints;
	juce::dsp::FFT forwardFFT;
	juce::dsp::WindowingFunction<float> window;
	float fftConstant{ 9.9658f };
	juce::ToggleButton mButton_FFT;

    bool updateDisplay{true};
    int menuPaintCounter{0};

    /* Options Menu =================================== */

    ButtonOptionsLookAndFeel optionsLookAndFeel;
    juce::TextButton buttonOptions;
    juce::Rectangle<float> buttonBounds;
    bool showMenu{ false };
    void drawToggles(bool showMenu);
    void buttonClicked(juce::Button* button) override;
    juce::ToggleButton    toggleShowRTA,
                        togglePickInput,
                        togglePickOutput;
    
	bool checkMenuFocus(const juce::MouseEvent& event);
	
	
	
	


	




    
    
    void updateToggleStates();
    
    
	using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	juce::AudioParameterBool* showFftParameter{ nullptr };
	juce::AudioParameterChoice* fftPickoffParameter{ nullptr };
	std::unique_ptr<buttonAttachment>	showFftAttachment;

    // Variables Storing APVTS Parameters
	int mPickOffID{ 0 };
    bool mShowFFT{ true };
    
    

    void updateStringText();
    void buildLabels();

};
