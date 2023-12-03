/*
  ==============================================================================

    ActivatorWindow.h
    Created: 17 Jan 2023 6:19:28pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct ActivatorWindow :    juce::Component, juce::Button::Listener
{
public:
    ActivatorWindow();
    ~ActivatorWindow();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    int getLicenseState() {return activationState; }
    int getTrialDaysLeft() {return mDaysLeft; }
    
    juce::TextButton mButtonActivateFull;
    juce::TextButton mButtonActivateTrial;
    juce::TextButton mButtonClose;
    
    bool isOkToClose() {return mOkToClose; }
    void resetOkToClose() {mOkToClose = false; }
        
private:
    juce::TextEditor mTextCode;
    juce::HyperlinkButton mButtonPurchase;
    
    juce::Label mLabelStatus;
    juce::Label mLabelThankYou;
    juce::Label mLabelLicenseStatus;
    
    //juce::String mLicenseKey;
    
    
    int mDaysLeft {0};
    
    bool mOkToClose {false};
    
    juce::Component* placeholder;
    
    juce::Image imageTitleHeader;
    juce::Image imageCompanyTitle;
    
    float targetTitleWidth{ 800.f };
    float targetTitleHeight{ 50.f };

    float currentTitleWidth{ 100.f };
    float currentTitleHeight{ 100.f };

    float targetCompanyWidth{ 250.f };
    float targetCompanyHeight{ 30.f };
    
    float currentCompanyWidth{ 100.f };
    float currentCompanyHeight{ 100.f };
    
    void updateFlexBox();
    
    void updateLicenseState(int state);
    int activationState = 0;
    // 0 = Should Activate Trial
    // 1 = Within Trial Period
    // 2 = Trial Has Expired, No Purchase
    // 3 = Full Version Purchased
    
    void init();
    void checkStatusOnStartup();
    void activate(std::string key);
    void activateTrial();
};
