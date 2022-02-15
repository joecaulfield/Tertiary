/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 19 Jan 2022 4:55:59pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../GUI/AllColors.h"

// Draw the Background Appearance of Option Buttons
void drawOptionButtonBackground( juce::Graphics& g, 
								 juce::Rectangle<int> bounds, 
								 bool selected, 
								 juce::Slider& slider, 
								 int bandMode);

struct WaveLookAndFeel : juce::LookAndFeel_V4
{
public:
	void setBandMode(int bandMode);

private:

	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;

	void drawWave0(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave1(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave2(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave3(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave4(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave5(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawWave6(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);

	juce::Rectangle<int> wave0, wave1, wave2, wave3, wave4, wave5, wave6;

	int mode{ 0 };
};

struct MultLookAndFeel : juce::LookAndFeel_V4
{
public:

	MultLookAndFeel();
	void setBandMode(int bandMode);

private:

	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;

	void drawMult0(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawMult1(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawMult2(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawMult3(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawMult4(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);
	void drawMult5(juce::Graphics& g, juce::Rectangle<int> bounds, int selection, juce::Slider& slider);

	juce::Rectangle<int> mult0, mult1, mult2, mult3, mult4, mult5, mult6;
	juce::Image imageMultIcon0, imageMultIcon1, imageMultIcon2, imageMultIcon3, imageMultIcon4, imageMultIcon5;

	int mode{ 0 };
};

struct StandardSliderLookAndFeel : juce::LookAndFeel_V4
{
public:

	void setBandMode(int bandMode);
	void setSuffix(juce::String unitSuffix);

private:

	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;

	int mode{ 0 };
};

struct CenterSliderLookAndFeel : juce::LookAndFeel_V4
{
public: 
	void setSuffix(juce::String unitSuffix);
	void setBandMode(int bandMode);\

private:

	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;

	int mode{ 0 };
};

struct ScrollLookAndFeel : juce::LookAndFeel_V4
{
	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;

	juce::Path getOutline (juce::Graphics& g, juce::Rectangle<float> bounds, int wheelHeight);

	float evaluateHeight(int position, juce::Rectangle<float> bounds);
};

struct InOutLookAndFeel : juce::LookAndFeel_V4
{
	void drawLinearSlider(	juce::Graphics& g, int x, int y, int width, int height,
							float sliderPos, float minSliderPos, float maxSliderPos,
							const juce::Slider::SliderStyle sliderStyle, juce::Slider& slider) override;
};

struct CrossoverLookAndFeel : juce::LookAndFeel_V4
{
	CrossoverLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;
};

struct CustomSlider : juce::Component, juce::Slider::Listener, juce::Label::Listener
{
	CustomSlider();
	~CustomSlider();

	void resized() override;
	void setStyleStandard(juce::String suffix);
	void setStyleCenter(juce::String suffix);
	void setBandMode(int mode);
	void labelTextChanged(juce::Label* labelThatHasChanged) override;
	void sliderValueChanged(juce::Slider* slider) override;
	void updateStringText();

	StandardSliderLookAndFeel standardSliderLookAndFeel;
	CenterSliderLookAndFeel centerSliderLookAndFeel;

	juce::Slider slider;
	juce::Label label;
	juce::String valueSuffix;
};