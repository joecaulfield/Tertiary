/*
  ==============================================================================

    UtilityFunctions.cpp
    Created: 16 Jan 2022 2:26:42pm
    Author:  Joe

  ==============================================================================
*/

#include "UtilityFunctions.h"

// Paints a shaded border around specified object.
void paintBorder(juce::Graphics& g, juce::Colour baseColor, juce::Rectangle<float> bounds)
{
	using namespace juce;

	//auto baseColor = juce::Colours::purple;

	//color = juce::Colours::purple;

	// DRAW BORDER 1 ====================
	juce::Rectangle<float> border;
	border.setBounds(bounds.getTopLeft().x, bounds.getTopLeft().y, bounds.getWidth(), bounds.getHeight());
	g.setColour(baseColor);
	g.drawRoundedRectangle(border, 1.f, 2.f);

	// DRAW BORDER 2 =====
	border.reduce(1, 1);
	g.setColour(baseColor.withMultipliedBrightness(0.8f));
	g.drawRoundedRectangle(border, 2.f, 2.f);

	// DRAW BORDER 3 =====
	border.reduce(1, 1);
	g.setColour(baseColor.withMultipliedBrightness(0.6f));
	g.drawRoundedRectangle(border, 2.f, 2.f);

	// DRAW BORDER 4 =====
	border.reduce(1, 1);
	g.setColour(baseColor.withMultipliedBrightness(0.3f));
	g.drawRoundedRectangle(border, 2.f, 2.f);
}

/* Conditions All Font & Text Parameters */
void buildLabelFont(    juce::Label& label,
                        juce::Justification justification,
                        juce::String typeface,
                        float fontSize,
                        juce::Font::FontStyleFlags fontStyle,
                        juce::Colour color,
                        float alpha)
{
    label.setJustificationType(justification);
    label.setFont(juce::Font(typeface, fontSize, fontStyle));
    label.setColour(juce::Label::textColourId, color);
    label.setAlpha(alpha);
}
