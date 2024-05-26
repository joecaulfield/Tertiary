/*
  ==============================================================================

    OptionsMenu.cpp
    Created: 24 Nov 2023 10:13:06am
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "OptionsMenu.h"


// ========================================================
OptionsMenu::OptionsMenu()
{

    setSize(100,25);
    
    addMouseListener(this, true);
    
    mButtonOptions.setLookAndFeel(&optionsLookAndFeel);
    mButtonOptions.addListener(this);

    addAndMakeVisible(mButtonOptions);

    shouldShowDropdown = false;
    
    startTimerHz(30);

}


// ========================================================
OptionsMenu::~OptionsMenu()
{
    mButtonOptions.setLookAndFeel(nullptr);
}


// ========================================================
void OptionsMenu::resized()
{

    mButtonOptions.setTopLeftPosition(0,0);
    mButtonOptions.setSize(100,25);
    
    buildDropdown();
}


// ========================================================
void OptionsMenu::paint(juce::Graphics& g)
{

    auto buttonBounds = getLocalBounds();
    
    g.setColour(juce::Colours::white);
    g.setOpacity(fadeValue);
    g.fillRoundedRectangle( buttonBounds.getX(),
                            buttonBounds.getY(),
                            buttonBounds.getWidth(),
                            buttonBounds.getHeight(),
                            2.f);
    
    // Draw beveled edges
    for (int i = 0; i < 5; i++)
    {
        g.setColour(juce::Colours::darkgrey.withMultipliedAlpha(1.f/((i*i)+0.75f)));
        g.drawRect(buttonBounds.toFloat().reduced(i,i), 1.f);
    }

    setAlpha(fadeValue);
  
}

// ========================================================
void OptionsMenu::mouseEnter(const juce::MouseEvent& event)
{

    if (!isMouseButtonDown())
    {
        if (shouldShowDropdown == false)
            mHasFocus = true;
    }

}

// ========================================================
void OptionsMenu::mouseExit(const juce::MouseEvent& event)
{

    if (!isMouseOver(true)) {
        closeDropdown();
        mHasFocus = false;
    }

}

// ========================================================
void OptionsMenu::timerCallback()
{
    // If this cursor has focus, timerCounter increases until at its max.  
    // If no focus, timerCounter decreases until at its minimum.

    if ((timerCounter > timerCounterMin) || (timerCounter < timerCounterMax))
    {
        if (mHasFocus || mForceFocus)
        {
            if (timerCounter < timerCounterMax)
                timerCounter++;
        }
        else
        {
            if (timerCounter > timerCounterMin)
                timerCounter--;
        }

        fadeValue = juce::jmap((float)timerCounter, (float)timerCounterMin, (float)timerCounterMax, fadeValueMin, fadeValueMax);
        setAlpha(fadeValue);
    }
}

// ========================================================
void OptionsMenu::buttonClicked(juce::Button* button)
{

    if (button == &mButtonOptions)
    {
        shouldShowDropdown = !shouldShowDropdown;
        
        if (shouldShowDropdown)
            openDropdown();
        else
            closeDropdown();
    }


}

// ========================================================
void OptionsMenu::openDropdown()
{

    // Allocate 25 Pixels per populated option
    auto dropSize = optionsArray.size() * 25;

    setSize (   mButtonOptions.getWidth(),
                mButtonOptions.getHeight() + dropSize
            );

    buildDropdown();
    
}

// ========================================================
void OptionsMenu::closeDropdown()
{

    setSize (   mButtonOptions.getWidth(),
                mButtonOptions.getHeight()
            );

    shouldShowDropdown = false;
}

// ========================================================
void OptionsMenu::addOptionToList(juce::String optionTitle,
                                  juce::String optionDescription,
                                  juce::AudioProcessorValueTreeState& apvts,
                                  juce::String parameterID
                                  )
{

    using namespace juce;
    
    auto newOption = std::make_unique<OptionItem>();
    
    newOption->optionTitle = optionTitle;
    newOption->optionDescription = optionDescription;
    
    newOption->toggleButton.setButtonText(optionTitle);
    newOption->toggleButton.setHelpText(optionDescription);
    
    newOption->toggleButton.setColour(    ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
    newOption->toggleButton.setColour(    ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    newOption->toggleButton.setColour(    ToggleButton::ColourIds::textColourId, juce::Colours::black);
        
    newOption->toggleButton.addListener(this);
    
    newOption->optionAttachment =  std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,
                                                                                                          parameterID,
                                                                                                          newOption->toggleButton);
    
    optionsArray.add(std::move(newOption)); // Transfers ownership to the array
}

// ========================================================
void OptionsMenu::buildDropdown()
{

    using namespace juce;
    
    auto bounds = getLocalBounds();
    bounds.setTop(bounds.getY() + mButtonOptions.getHeight());
    
    auto margin = FlexItem().withHeight(5);
    auto spacer = FlexItem().withHeight(2.5);
    auto height = (150.f - 2.f * 5.f - 5.f * 2.5f) / 6.f;

    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::column;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    flexBox.items.add(margin);
    
    for (const auto& option : optionsArray)
    {
        flexBox.items.add( FlexItem(option->toggleButton).withHeight(height));
        flexBox.items.add(spacer);
        addAndMakeVisible(option->toggleButton);
    }
    
    flexBox.items.add(margin);

    flexBox.performLayout(bounds);

}