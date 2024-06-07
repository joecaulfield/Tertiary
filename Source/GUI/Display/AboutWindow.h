/*
  ==============================================================================

    ActivatorWindow.h
    Created: 17 Jan 2023 6:19:28pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../Utility/UtilityFunctions.h"

struct AboutWindow : juce::Component, juce::Button::Listener, juce::MouseListener
{
public:
    AboutWindow();
    ~AboutWindow();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:

    juce::String formatBuildDateTime();
  

    juce::TextButton mButtonClose;

    juce::Image imageTitleHeader;
    juce::Image imageCompanyTitle;

    float targetTitleWidth{ 800.f };
    float targetTitleHeight{ 50.f };

    float currentTitleWidth{ 100.f };
    float currentTitleHeight{ 100.f };


    float targetCompanyHeight{ 40.f };
    float targetCompanyWidth{ 330.f };

    float currentCompanyWidth{ 100.f };
    float currentCompanyHeight{ 100.f };

    juce::HyperlinkButton mButtonWebsiteLink;

    //juce::TextEditor mTextCode;
    //juce::HyperlinkButton mButtonPurchase;

    //juce::Label mLabelStatus;
    //juce::Label mLabelThankYou;
    //juce::Label mLabelLicenseStatus;

    ////juce::String mLicenseKey;


    //int mDaysLeft{ 0 };

    //bool mOkToClose{ false };

    //juce::Component* placeholder;

    //juce::Image imageTitleHeader;
    //juce::Image imageCompanyTitle;

    //float targetTitleWidth{ 800.f };
    //float targetTitleHeight{ 50.f };

    //float currentTitleWidth{ 100.f };
    //float currentTitleHeight{ 100.f };

    //float targetCompanyWidth{ 250.f };
    //float targetCompanyHeight{ 30.f };

    //float currentCompanyWidth{ 100.f };
    //float currentCompanyHeight{ 100.f };

    //void updateFlexBox();
};
