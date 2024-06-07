/*
  ==============================================================================

    TopBanner.h
    Created: 29 Jan 2023 1:32:49pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../../Utility/AllColors.h"

struct TopBanner :  juce::Component, juce::MouseListener
{
public:
    TopBanner();
    ~TopBanner();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setShowTrialTitle(bool shouldShow) {mShowTrialTitle = shouldShow;}
    void setDaysLeft(int trialDaysLeft) {mDaysLeft = trialDaysLeft;}
    
    //void buttonClicked(juce::Button* button) override {};
    void mouseDoubleClick (const juce::MouseEvent &event) override;
    
private:
    
    void buildTopBanner(juce::Graphics& g);
    //void buildTrialTitle(juce::Graphics& g);
    
    /* Graphics Asset, Top-Center Plugin-Name "Tertiary" */
    juce::Image imageTitleHeader;
    juce::Image imageCompanyTitle;
    
    float targetCompanyWidth{ 250.f };
    float targetCompanyHeight{ 30.f };
    
    float currentCompanyWidth{ 100.f };
    float currentCompanyHeight{ 100.f };
    
    float targetTitleWidth{ 800.f };
    float targetTitleHeight{ 50.f };

    float currentTitleWidth{ 100.f };
    float currentTitleHeight{ 100.f };
    
    bool mShowTrialTitle{true};
    int mDaysLeft{999};
    
};
