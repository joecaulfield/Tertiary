/*
  ==============================================================================

    UtilityFunctions.h
    Created: 16 Jan 2022 2:26:42pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

void paintBorder(juce::Graphics& g, juce::Colour baseColor, juce::Rectangle<float> bounds);

void buildLabelFont(    juce::Label& label,
                        juce::Justification justification,
                        juce::String typeface,
                        float fontSize,
                        juce::Font::FontStyleFlags fontStyle,
                        juce::Colour color,
                        float alpha);
