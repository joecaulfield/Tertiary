/*
  ==============================================================================

    ActivatorWindow.cpp
    Created: 17 Jan 2023 6:19:28pm
    Author:  Joe Caulfield

  ==============================================================================
*/


#include "AboutWindow.h"





AboutWindow::AboutWindow()
{
    setAlpha(0.95f);


    mButtonClose.addListener(this);
    addAndMakeVisible(mButtonClose);

    addMouseListener(this, true);

    mButtonClose.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black.withAlpha(0.f));
    mButtonClose.setButtonText("X");

    imagePluginTitle = juce::ImageCache::getFromMemory(BinaryData::About_PluginTitle1_png, BinaryData::About_PluginTitle1_pngSize);
    imageCompanyTitle = juce::ImageCache::getFromMemory(BinaryData::About_CompanyLogo_png, BinaryData::About_CompanyLogo_pngSize);

    mButtonWebsiteLink.setButtonText("WonderlandAudio.com");
    mButtonWebsiteLink.setURL(juce::URL("http://www.wonderlandaudio.com"));
    mButtonWebsiteLink.setColour(juce::HyperlinkButton::textColourId, juce::Colours::oldlace);
    addAndMakeVisible(mButtonWebsiteLink);

}

AboutWindow::~AboutWindow()
{

}

void AboutWindow::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    auto bounds = getLocalBounds();

    juce::Rectangle<float> companyBounds{ bounds.getCentreX() - targetCompanyWidth / 2, 25, (float)targetCompanyWidth, (float)targetCompanyHeight };
    g.drawImage(imageCompanyTitle, companyBounds);

    juce::Rectangle<float> titleBounds{ bounds.getCentreX() - targetTitleWidth / 2, companyBounds.getBottom() + 5, (float)targetTitleWidth, (float)targetTitleHeight };
    g.drawImage(imagePluginTitle, titleBounds);

    // Draw Plugin-Details
    // ======================================================================
    g.setColour(juce::Colours::grey);
    g.drawRect(bounds);

    juce::Rectangle<int> textBounds{ bounds.getX() + 75,
                                        (int)titleBounds.getBottom() + 5,
                                        getWidth() - 20,
                                        bounds.getBottom() - (int)titleBounds.getBottom() - 50
    };

    // Define the text
    juce::String versionNumber = JucePlugin_VersionString;
    juce::String releaseDate = formatBuildDateTime();

    juce::String leftColumnText = "Version:\nRelease:";
    juce::String rightColumnText = versionNumber + "\n" + releaseDate;

    // Set the font size
    float fontSize = 20.0f; // Adjust the font size as needed
    g.setFont(fontSize);
    g.setColour(juce::Colours::white);

    #ifdef DEBUG
        g.setColour(juce::Colours::red);
    #endif // DEBUG
        

    // Define the rectangles for the two columns
    int leftColumnWidth = getWidth() / 4; // Adjust as needed
    juce::Rectangle<int> leftColumnBounds = textBounds.removeFromLeft(leftColumnWidth);
    juce::Rectangle<int> rightColumnBounds = textBounds;

    // Draw the text
    g.drawFittedText(leftColumnText, leftColumnBounds, juce::Justification::centredLeft, 2);
    g.drawFittedText(rightColumnText, rightColumnBounds, juce::Justification::centredLeft, 2);

    // Draw Window Border
    // ======================================================================
    paintBorder(g, juce::Colours::grey, getLocalBounds().toFloat());
}

void AboutWindow::resized()
{
    auto bounds = getLocalBounds();

    mButtonClose.setBounds(bounds.getRight() - 30, 5, 25, 25);

    mButtonWebsiteLink.setBounds(bounds.getX(), bounds.getBottom() - 50, bounds.getWidth(), 30);
}

juce::String AboutWindow::formatBuildDateTime()
{
    // Map month abbreviations to numbers
    std::map<juce::String, int> months = {
        {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
        {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
        {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
    };

    // Extract components from __DATE__ and __TIME__
    juce::String date(__DATE__);  // "MMM DD YYYY"
    juce::String time(__TIME__);  // "HH:MM:SS"

    // Parse the date string
    juce::String monthAbbr = date.substring(0, 3);
    int day = date.substring(4, 6).getIntValue();
    int year = date.substring(7, 11).getIntValue();
    int month = months[monthAbbr];

    // Parse the time string
    int hour = time.substring(0, 2).getIntValue();
    int minute = time.substring(3, 5).getIntValue();

    // Format the date and time as "YYYY.MM.DD HH:MM"
    return juce::String::formatted("%04d.%02d.%02d %02d:%02d", year, month, day, hour, minute);
}

void AboutWindow::buttonClicked(juce::Button* button)
{
    if (button == &mButtonClose)
    {
        setVisible(false);
    }

}




void AboutWindow::mouseExit(const juce::MouseEvent& event)
{
    if (!isMouseOver())
    {
        if (!mButtonClose.isMouseOver() && !mButtonWebsiteLink.isMouseOver())
            setVisible(false);
    }

}

