/*
  ==============================================================================

	ScrollPad.h
	Created: 5 Oct 2022 7:30:04pm
	Author:  Joe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

struct FreqLabel :	juce::Component,
					juce::Label::Listener,
					juce::ActionBroadcaster,
					juce::ActionListener,
					juce::Timer
{
public:

	FreqLabel();
	~FreqLabel();

	void paint(juce::Graphics& g) override;
	void resized() override;

	void labelTextChanged(juce::Label* labelThatHasChanged) override;

	void setLabelValue(float newValue);

	void timerCallback() override;

	void mouseEnter(const juce::MouseEvent& event) override;
	void mouseExit(const juce::MouseEvent& event) override;

	void actionListenerCallback(const juce::String& message);
	void sendBroadcast(juce::String parameterName, juce::String parameterValue);


private:

	juce::Label mLabel;

	juce::String mNameSpace{ "FreqLabel" };
	bool setDebug{ true };

	bool isBeingEdited{ false };

	int timerCounter{ 0 };
	int timerCounterMin{ 0 };
	int timerCounterMax{ 14 };

	float fadeValue{ 1.f };
	float fadeValueMin{ 0.f };
	float fadeValueMax{ 1.f };

	bool mHasFocus{ false };
	bool mForceFocus{ false };
};