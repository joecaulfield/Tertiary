/*
  ==============================================================================

    OptionsMenu.h
    Created: 24 Nov 2023 10:13:06am
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../Utility/MyLookAndFeel.h"

struct OptionItem
{
public:
    juce::ToggleButton toggleButton;
    juce::String optionTitle;
    juce::String optionDescription;
    
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<buttonAttachment> optionAttachment;
};






struct OptionsMenu :    juce::Component,
                        juce::Timer,
                        juce::Button::Listener
{
    
public:
    OptionsMenu();
    ~OptionsMenu();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    void timerCallback() override;
    
    void buttonClicked(juce::Button* button) override;
    
    void addOptionToList(   juce::String optionTitle,
                            juce::String optionDescription,
                            juce::AudioProcessorValueTreeState& apvts,
                            juce::String parameterID);
    
private:
    juce::TextButton mButtonOptions;
    ButtonOptionsLookAndFeel optionsLookAndFeel;
    
    void openDropdown();
    void closeDropdown();
    void buildDropdown();

    juce::OwnedArray<OptionItem> optionsArray;    
 
    int timerCounter{ 0 };
    int timerCounterMin{ 0 };
    int timerCounterMax{ 14 };

    float fadeValue{ 1.f };
    float fadeValueMin{ 0.f };
    float fadeValueMax{ 1.f };

    bool mHasFocus{ false };
    bool mForceFocus{ false };

    bool shouldShowDropdown{ false };
};



