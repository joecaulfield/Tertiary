/*
  ==============================================================================

	ScrollPad.h
	Created: 5 Oct 2022 7:30:04pm
	Author:  Joe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

struct Cursor :	juce::Component,
				juce::ActionBroadcaster
{
public:

	Cursor();
	~Cursor();

	void mouseEnter(const juce::MouseEvent& event) override {};
	void mouseExit(const juce::MouseEvent& event) override {};
	//void mouseDown(const juce::MouseEvent& event) override;
	//void mouseDrag(const juce::MouseEvent& event) override;
	//void mouseDoubleClick(const juce::MouseEvent& event) override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	//void sendBroadcast(juce::String parameterName, juce::String parameterValue);

	void setHorizontalOrientation();

	void setFocus(bool hasFocus);

private:

	juce::Line<float> mCursor;

	bool isVertical{ true };

	bool mHasFocus{ false };
};
