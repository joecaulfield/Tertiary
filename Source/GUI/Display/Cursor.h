/*
  ==============================================================================

	ScrollPad.h
	Created: 5 Oct 2022 7:30:04pm
	Author:  Joe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

struct Cursor : juce::Component,
				juce::ActionBroadcaster,
				juce::ActionListener,
				juce::Timer
{
public:

	Cursor();
	~Cursor();

	void mouseEnter(const juce::MouseEvent& event) override {};
	void mouseExit(const juce::MouseEvent& event) override {};

	void paint(juce::Graphics& g) override;
	void resized() override;

	void setHorizontalOrientation();

	void setFocus(bool hasFocus);

	void timerCallback();

	float getFadeValue() { return fadeValue; };
	float getFadeValueMin() { return fadeValueMin; };
	float getFadeValueMax() { return fadeValueMax; };

	void actionListenerCallback(const juce::String& message);
	void sendBroadcast(juce::String parameterName, juce::String parameterValue);

private:

	juce::Line<float> mCursor;

	bool mIsVertical{ true };

	bool mHasFocus{ false };
	bool mForceFocus{ false };

	int timerCounter{ 0 };
	int timerCounterMin{ 0 };
	int timerCounterMax{ 14 };

	float fadeValue{ 1.f };
	float fadeValueMin{ 0.25f };
	float fadeValueMax{ 1.f };

};
