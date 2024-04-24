/*
  ==============================================================================

    TopBanner.cpp
    Created: 29 Jan 2023 1:32:49pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "TopBanner.h"

TopBanner::TopBanner()
{
    /* Graphics Asset, Top-Center Plugin-Name "Tertiary" */
    imageTitleHeader = juce::ImageCache::getFromMemory(BinaryData::TitleHeader_png, BinaryData::TitleHeader_pngSize);
    imageCompanyTitle = juce::ImageCache::getFromMemory(BinaryData::CompanyLogo_png, BinaryData::CompanyLogo_pngSize);

    setBufferedToImage(true);
}

TopBanner::~TopBanner()
{
    
}

void TopBanner::paint(juce::Graphics& g)
{
    using namespace juce;

    buildTopBanner(g);
    
    if (mShowTrialTitle)
        buildTrialTitle(g);
}

void TopBanner::resized()
{
    
}


/* Builds the Company Title and Plugin Title images and shows them */
//==============================================================================
void TopBanner::buildTopBanner(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    /* Resize the PluginTitle image once and show */
    //==============================================================================
    targetTitleWidth = getWidth();
    
    currentTitleWidth = imageTitleHeader.getWidth();
    currentTitleHeight = imageTitleHeader.getHeight();

    while (currentTitleHeight > targetTitleHeight)
    {
        currentTitleWidth = imageTitleHeader.getWidth() * 0.75f;
        currentTitleHeight = imageTitleHeader.getHeight() * 0.75f;

        imageTitleHeader = imageTitleHeader.rescaled(currentTitleWidth, currentTitleHeight, juce::Graphics::highResamplingQuality);
    }

    juce::Rectangle<float> titleBounds{ 0, 0, (float)targetTitleWidth, (float)targetTitleHeight };
    g.drawImage(imageTitleHeader, titleBounds);

    
    /* Resize the Company Title image once and show */
    //==============================================================================
    currentCompanyWidth = imageCompanyTitle.getWidth();
    currentCompanyHeight = imageCompanyTitle.getHeight();

    while (currentCompanyHeight > targetCompanyHeight)
    {
        currentCompanyWidth = imageCompanyTitle.getWidth() * 0.75f;
        currentCompanyHeight = imageCompanyTitle.getHeight() * 0.75f;

        imageCompanyTitle = imageCompanyTitle.rescaled(currentCompanyWidth, currentCompanyHeight, juce::Graphics::highResamplingQuality);
    }

    juce::Rectangle<float> companyBounds{bounds.getRight() - imageCompanyTitle.getWidth(), 12, (float)imageCompanyTitle.getWidth(), (float)imageCompanyTitle.getHeight()};
    g.drawImage(imageCompanyTitle, companyBounds);
}

/* Shows that this product is in trial status and shows days left on the top banner */
//==============================================================================
void TopBanner::buildTrialTitle(juce::Graphics& g)
{
    // SHOW TRIAL TITLE =======================
    auto trialLabelFontTypeface = FontEditor::ControlLabels::getTypeface();
    auto trialLabelFontSize = 24.f;
    auto trialLabelFontStyle = juce::Font::FontStyleFlags::bold;
    auto trialLabelFontColor = juce::Colour(0xff817e85);
    
    auto titleFont = juce::Font(    trialLabelFontTypeface,
                                    trialLabelFontSize,
                                    trialLabelFontStyle);
    
    g.setFont(titleFont);
    g.setColour(trialLabelFontColor);
    
    juce::String stringTrial = "TRIAL: " + (juce::String)mDaysLeft + " DAYS LEFT";
    juce::Rectangle<int> trialTitleBounds = {10,5, 200, 30};
    g.drawFittedText(stringTrial, trialTitleBounds, juce::Justification::centredLeft, 2);
    
    auto trialSubtextLabelFontTypeface = FontEditor::ControlLabels::getTypeface();
    auto trialSubtextLabelFontSize = 13.f;
    auto trialSubtextLabelFontStyle = juce::Font::FontStyleFlags::bold;
    
    auto titleSubtextFont = juce::Font(     trialSubtextLabelFontTypeface,
                                            trialSubtextLabelFontSize,
                                            trialSubtextLabelFontStyle);
    
    g.setFont(titleSubtextFont);
    juce::String stringTrialSubtext = "DOUBLE-CLICK TO ACTIVATE";
    juce::Rectangle<int> trialSubTextBounds = {10,27,200,25};
    g.drawFittedText(stringTrialSubtext, trialSubTextBounds, juce::Justification::centredLeft,1);
}
