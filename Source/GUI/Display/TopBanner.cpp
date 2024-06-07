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
    //imageTitleHeader = juce::ImageCache::getFromMemory(BinaryData::TitleHeader_png, BinaryData::TitleHeader_pngSize);
    //imageCompanyTitle = juce::ImageCache::getFromMemory(BinaryData::CompanyLogo_png, BinaryData::CompanyLogo_pngSize);

    imageTopBanner = juce::ImageCache::getFromMemory(BinaryData::TopBanner_Black_png, BinaryData::TopBanner_Black_pngSize);

    setBufferedToImage(true);
}

TopBanner::~TopBanner()
{
    
}

void TopBanner::paint(juce::Graphics& g)
{
    using namespace juce;

    auto bounds = getLocalBounds().toFloat();

    juce::Rectangle<float> titleBounds{ 0, 0, (float)targetTitleWidth, (float)targetTitleHeight };
    g.drawImage(imageTopBanner, titleBounds);
}

void TopBanner::resized()
{
    imageTopBanner = imageTopBanner.rescaled(targetTitleWidth, targetTitleHeight, juce::Graphics::highResamplingQuality);
}