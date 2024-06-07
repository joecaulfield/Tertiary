///*
//  ==============================================================================
//
//    ActivatorWindow.cpp
//    Created: 17 Jan 2023 6:19:28pm
//    Author:  Joe Caulfield
//
//    // TERTIARY PRODUCT ID = 1064a4b1-3214-44e9-8330-6e524079647a
//    // TERTIARY DAT CONTENT =
//        NDFGNDBBOUU3RTdERTRERkI4OUE2MUJGQTA1MDUzMzQ=.hoy9CMg8JVwC2/dK8zfSWy9z9MVklzSwW54H+M/ElTwH0UYncopRczG89ZmzZbO77cgP9p7GHmnsnUEVHmc5G22jvqCjYeqAlKxfaAgNG1CHylJA2vkE0WrC+86arRWOOraP7f6DquQNbRzSasw91WSAC7Y5KTaAE362rKGfEMn1/vEdv3FxZC45/2ZxToPzavlArAjyqslVRgKdl7aH8wsU6WR1b0oHco5zI4WSaazBPZCTfngyPQiBM6ZH3SpMIgKPfM59mmfhJG/S2KOH3DZlcAlh1XIbCpMrTBvVQdKCMJf9p2ofN8mQPuLkGwdtYfoVI6G0hjbpW0WAI1ndbizgRcnJVIe/1kqk4UAJjUgaRhg2eT5PK8FBUJtDwisxGZozju2h+Jt5WnFhGWpm/+qfyzDrvsFtc8pmMTHihfed8i+0HyUWVteLLHuLJzJ+TMsFhc3mrpIAek+tTHFCpk/7jsmXoaphPA4qYcv/RZRCIBRem6CGlffQS3gkQmm8Tb8qAFmGh5LEIHHQLZ3mp9NcEP0QGtzJpYnb36FSbLSiV0CbW3NDsXzustjdAc+SxC6IirtOKuKpEcGFrWkoIaOHS3Jig7NGBHUk+lNBWC5XcNU6r0IgfzCQKMdjLKrSFH5x0PiN7zEwBfzxbv8qoHRdi/sWHH3SeP5V9ooLwybgAE+mYsRbMpBDJ/LEa/fXpeVQo7pLfnZlE68Njt//W09yk4GM1l1Fm2PWsiga0bLTbDrm29WITc0yM/T4mxBkpDUTX+D8UAYJUiHyXEeZ7WcBTYYkTuOGas2xIiw7jjU=
// 
//  ==============================================================================
//*/
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//
//#include "ActivatorWindow.h"
//#include "LexActivator-Static-Mac/headers/LexActivator.h"
//
//
//
//
//ActivatorWindow::ActivatorWindow()
//{
//    addAndMakeVisible(mTextCode);
//    mTextCode.setJustification(juce::Justification::centred);
//    
//    addAndMakeVisible(mButtonActivateFull);
//    addAndMakeVisible(mButtonActivateTrial);
//    addAndMakeVisible(mLabelStatus);
//    addAndMakeVisible(mLabelThankYou);
//    addAndMakeVisible(mLabelLicenseStatus);
//    addAndMakeVisible(mButtonPurchase);
//    
//    mButtonActivateFull.addListener(this);
//    mButtonActivateTrial.addListener(this);
//    mButtonClose.addListener(this);
//    
//    /* Graphics Asset, Top-Center Plugin-Name "Tertiary" */
//    imageTitleHeader = juce::ImageCache::getFromMemory(BinaryData::TitleHeader_png, BinaryData::TitleHeader_pngSize);
//    imageCompanyTitle = juce::ImageCache::getFromMemory(BinaryData::CompanyLogo_png, BinaryData::CompanyLogo_pngSize);
//    
//    init();
//    //Reset();
//    checkStatusOnStartup();
//    
//
//
//    
//}
//
//ActivatorWindow::~ActivatorWindow()
//{
//    
//}
//
//
//
//
//
//
//
//
//
//// License callback is invoked when IsLicenseGenuine() completes a server sync
//void LA_CC LicenseCallback(uint32_t status)
//{
//    // NOTE: Don't invoke IsLicenseGenuine(), ActivateLicense() or ActivateTrial() API functions in this callback
//    printf("\nLicense status: %d\n", status);
//}
//
//// Software release update callback is invoked when CheckForReleaseUpdate() gets a response from the server
//void LA_CC SoftwareReleaseUpdateCallback(uint32_t status)
//{
//    printf("\nRelease status: %d\n", status);
//}
//
///* Initialization for LexActivator */
//void ActivatorWindow::init()
//{
//    
//    int status;
//    
//    // Set Product Data
//    // =========================================================
//    #if _WIN32
//        // status = SetProductFile(L"ABSOLUTE_PATH_OF_PRODUCT.DAT_FILE");
//        status = SetProductData(L"NDFGNDBBOUU3RTdERTRERkI4OUE2MUJGQTA1MDUzMzQ=.hoy9CMg8JVwC2/dK8zfSWy9z9MVklzSwW54H+M/ElTwH0UYncopRczG89ZmzZbO77cgP9p7GHmnsnUEVHmc5G22jvqCjYeqAlKxfaAgNG1CHylJA2vkE0WrC+86arRWOOraP7f6DquQNbRzSasw91WSAC7Y5KTaAE362rKGfEMn1/vEdv3FxZC45/2ZxToPzavlArAjyqslVRgKdl7aH8wsU6WR1b0oHco5zI4WSaazBPZCTfngyPQiBM6ZH3SpMIgKPfM59mmfhJG/S2KOH3DZlcAlh1XIbCpMrTBvVQdKCMJf9p2ofN8mQPuLkGwdtYfoVI6G0hjbpW0WAI1ndbizgRcnJVIe/1kqk4UAJjUgaRhg2eT5PK8FBUJtDwisxGZozju2h+Jt5WnFhGWpm/+qfyzDrvsFtc8pmMTHihfed8i+0HyUWVteLLHuLJzJ+TMsFhc3mrpIAek+tTHFCpk/7jsmXoaphPA4qYcv/RZRCIBRem6CGlffQS3gkQmm8Tb8qAFmGh5LEIHHQLZ3mp9NcEP0QGtzJpYnb36FSbLSiV0CbW3NDsXzustjdAc+SxC6IirtOKuKpEcGFrWkoIaOHS3Jig7NGBHUk+lNBWC5XcNU6r0IgfzCQKMdjLKrSFH5x0PiN7zEwBfzxbv8qoHRdi/sWHH3SeP5V9ooLwybgAE+mYsRbMpBDJ/LEa/fXpeVQo7pLfnZlE68Njt//W09yk4GM1l1Fm2PWsiga0bLTbDrm29WITc0yM/T4mxBkpDUTX+D8UAYJUiHyXEeZ7WcBTYYkTuOGas2xIiw7jjU=");
//    #else
//        //  status = SetProductFile("ABSOLUTE_PATH_OF_PRODUCT.DAT_FILE");
//        status = SetProductData("NDFGNDBBOUU3RTdERTRERkI4OUE2MUJGQTA1MDUzMzQ=.hoy9CMg8JVwC2/dK8zfSWy9z9MVklzSwW54H+M/ElTwH0UYncopRczG89ZmzZbO77cgP9p7GHmnsnUEVHmc5G22jvqCjYeqAlKxfaAgNG1CHylJA2vkE0WrC+86arRWOOraP7f6DquQNbRzSasw91WSAC7Y5KTaAE362rKGfEMn1/vEdv3FxZC45/2ZxToPzavlArAjyqslVRgKdl7aH8wsU6WR1b0oHco5zI4WSaazBPZCTfngyPQiBM6ZH3SpMIgKPfM59mmfhJG/S2KOH3DZlcAlh1XIbCpMrTBvVQdKCMJf9p2ofN8mQPuLkGwdtYfoVI6G0hjbpW0WAI1ndbizgRcnJVIe/1kqk4UAJjUgaRhg2eT5PK8FBUJtDwisxGZozju2h+Jt5WnFhGWpm/+qfyzDrvsFtc8pmMTHihfed8i+0HyUWVteLLHuLJzJ+TMsFhc3mrpIAek+tTHFCpk/7jsmXoaphPA4qYcv/RZRCIBRem6CGlffQS3gkQmm8Tb8qAFmGh5LEIHHQLZ3mp9NcEP0QGtzJpYnb36FSbLSiV0CbW3NDsXzustjdAc+SxC6IirtOKuKpEcGFrWkoIaOHS3Jig7NGBHUk+lNBWC5XcNU6r0IgfzCQKMdjLKrSFH5x0PiN7zEwBfzxbv8qoHRdi/sWHH3SeP5V9ooLwybgAE+mYsRbMpBDJ/LEa/fXpeVQo7pLfnZlE68Njt//W09yk4GM1l1Fm2PWsiga0bLTbDrm29WITc0yM/T4mxBkpDUTX+D8UAYJUiHyXEeZ7WcBTYYkTuOGas2xIiw7jjU=");
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            //getchar();
//            //exit(status);
//        }
//
//    // Set Product ID
//    // =========================================================
//    #if _WIN32
//        status = SetProductId(L"1064a4b1-3214-44e9-8330-6e524079647a", LA_USER);
//    #else
//        status = SetProductId("1064a4b1-3214-44e9-8330-6e524079647a", LA_USER);
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            //getchar();
//            //exit(status);
//        }
//
//    // Set Release Version
//    // =========================================================
//    
//    #if _WIN32
//        status = SetReleaseVersion(L"0.2.0");
//    #else
//        status = SetReleaseVersion("0.2.0");
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            getchar();
//            //exit(status);
//        }
//}
//
///* Checks current license status on startup */
//void ActivatorWindow::checkStatusOnStartup()
//{
//    int status = IsLicenseGenuine();
//    if (LA_OK == status)
//    {
//        unsigned int expiryDate = 0;
//        GetLicenseExpiryDate(&expiryDate);
//        int daysLeft = (expiryDate - time(NULL)) / 86400;
//        mDaysLeft = daysLeft;
//        printf("Days left: %d\n", daysLeft);
//        printf("License is genuinely activated!\n\n");
//        
//        mLabelStatus.setText(   "License is genuinely activated!\n\n",
//                                juce::NotificationType::dontSendNotification);
//
//        updateLicenseState(3);
//
//    }
//    else if (LA_EXPIRED == status)
//    {
//        printf("License is genuinely activated but has expired!\n\n");
//        
//        mLabelStatus.setText(   "License is genuinely activated but has expired!",
//                                juce::NotificationType::dontSendNotification);
//    }
//    else if (LA_SUSPENDED == status)
//    {
//        printf("License is genuinely activated but has been suspended!\n\n");
//        
//        mLabelStatus.setText(   "License is genuinely activated but has been suspended!",
//                                juce::NotificationType::dontSendNotification);
//        
//    }
//    else if (LA_GRACE_PERIOD_OVER == status)
//    {
//        printf("License is genuinely activated but grace period is over!\n\n");
//        
//        mLabelStatus.setText(   "License is genuinely activated but grace period is over!",
//                                juce::NotificationType::dontSendNotification);
//        
//    }
//    else
//    {
//        int trialStatus;
//        trialStatus = IsTrialGenuine();
//        if (LA_OK == trialStatus)
//        {
//            unsigned int trialExpiryDate = 0;
//            GetTrialExpiryDate(&trialExpiryDate);
//            int daysLeft = (trialExpiryDate - time(NULL)) / 86400;
//            mDaysLeft = daysLeft;
//            
//            printf("Trial days left: %d\n\n", daysLeft);
//            
//            mLabelStatus.setText(   "Trial Days Left: " + (juce::String)daysLeft,
//                                    juce::NotificationType::dontSendNotification);
//            
//            updateLicenseState(1);
//        }
//        else if (LA_TRIAL_EXPIRED == trialStatus)
//        {
//            printf("Trial has expired!\n\n");
//
//            mLabelStatus.setText(   "Trial has expired!",
//                                    juce::NotificationType::dontSendNotification);
//
//            updateLicenseState(2);
//        }
//        else
//        {
//            printf("Either trial has not started or has been tampered!\n\n");
//            
//            //mLabelStatus.setText(   "Either trial has not started or has been tampered!",
//            //                       juce::NotificationType::dontSendNotification);
//            
//            mLabelStatus.setText(   "Please activate your trial.",
//                                    juce::NotificationType::dontSendNotification);
//            
//            updateLicenseState(0);
//        }
//    }
//    //getchar();
//}
//
///* Attemps to activate a full license when prompted */
//void ActivatorWindow::activate(std::string key)
//{
//    int status;
//    #if _WIN32
//        status = SetLicenseKey(L"PASTE_LICENCE_KEY");
//    #else
//        status = SetLicenseKey(key.c_str());
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            
//            mLabelStatus.setText(   "Error code: " + (juce::String)status,
//                                    juce::NotificationType::dontSendNotification);
//            //getchar();
//            //exit(status);
//        }
//
//    #if _WIN32
//        status = SetActivationMetadata(L"key1", L"value1");
//    #else
//        status = SetActivationMetadata("key1", "value1");
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            //getchar();
//            //exit(status);
//        }
//
//    status = ActivateLicense();
//    if (LA_OK == status || LA_EXPIRED == status || LA_SUSPENDED == status)
//    {
//        printf("License activated successfully: %d", status);
//        
//        updateLicenseState(3);
//        
//        mLabelStatus.setText(   "License activated successfully: " + (juce::String)status,
//                                juce::NotificationType::dontSendNotification);
//        
//        mOkToClose = true;
//    }
//    else
//    {
//        printf("License activation failed: %d", status);
//        
//        mLabelStatus.setText(   "License activation failed: " + (juce::String)status,
//                                juce::NotificationType::dontSendNotification);
//    }
//}
//
///* Attempts to activate a trial when prompted */
//void ActivatorWindow::activateTrial()
//{
//    int status;
//
//    #if _WIN32
//        status = SetTrialActivationMetadata(L"key1", L"value1");
//    #else
//        status = SetTrialActivationMetadata("key1", "value1");
//    #endif
//        if (LA_OK != status)
//        {
//            printf("Error code: %d", status);
//            
//            mLabelStatus.setText(   "Error code: " + (juce::String)status,
//                                    juce::NotificationType::dontSendNotification);
//            
//            //getchar();
//            //exit(status);
//        }
//
//    status = ActivateTrial();
//    if (LA_OK == status)
//    {
//        printf("Product trial activated successfully!\n\n");
//        
//        mLabelStatus.setText(   "Product trial activated successfully!",
//                                juce::NotificationType::dontSendNotification);
//        
//        mOkToClose = true;
//        
//        updateLicenseState(1);
//
//    }
//    else if (LA_TRIAL_EXPIRED == status)
//    {
//        printf("Product trial has expired!\n\n");
//        
//        mLabelStatus.setText(   "Product trial has expired!",
//                                juce::NotificationType::dontSendNotification);
//        
//        updateLicenseState(2);
//    }
//    else
//    {
//        printf("Product trial activation failed: %d\n\n", status);
//        
//        mLabelStatus.setText(   "Product trial activation failed: " + (juce::String)status,
//                                juce::NotificationType::dontSendNotification);
//        
//        updateLicenseState(0);
//    }
//}
//
///* Gets license state for UI updates */
//void ActivatorWindow::updateLicenseState(int state)
//{
//    /* Trial has not been started */
//    // ======================================================================
//    if (state == 0)
//    {
//        placeholder = &mButtonActivateTrial;
//        mLabelLicenseStatus.setVisible(false);
//        mButtonActivateTrial.setVisible(true);
//    }
//    
//    /* User is within trial period */
//    // ======================================================================
//    if (state == 1)
//    {
//        placeholder = &mLabelLicenseStatus;
//        mLabelLicenseStatus.setText("Your 14-Day trial has " + (juce::String)mDaysLeft + " days remaining.", juce::NotificationType::dontSendNotification);
//        mButtonActivateTrial.setVisible(false);
//        mLabelLicenseStatus.setVisible(true);
//        addAndMakeVisible(mButtonClose);
//        //mOkToClose = true;
//    }
//    
//    /* Trial has expired but user has not purchased license */
//    // ======================================================================
//    if (state == 2)
//    {
//        placeholder = &mLabelLicenseStatus;
//        mLabelLicenseStatus.setText("Your 14-Day trial has expired.", juce::NotificationType::dontSendNotification);
//    }
//    
//    /* User has purchased a full license */
//    // ======================================================================
//    if (state == 3)
//    {
//        placeholder = &mLabelLicenseStatus;
//        mLabelLicenseStatus.setText("Your license has been activated.", juce::NotificationType::dontSendNotification);
//        mTextCode.setEnabled(false);
//        // Set the text of mTextCode to be the user's license key
//        mButtonActivateFull.setEnabled(false);
//        // Ensure this window doesn't show up, unless user has just purchased license
//        addAndMakeVisible(mButtonClose);
//        //mOkToClose = true;
//    }
//
//    activationState = state;
//    updateFlexBox();
//}
//
//
//
//
//
//
//void ActivatorWindow::paint(juce::Graphics& g)
//{
//    g.setColour(juce::Colours::black);
//    g.fillAll();
//    
//    g.setColour(juce::Colours::black);
//    
//    auto bounds = getLocalBounds();
//
//    // Draw Plugin Title
//    // ======================================================================
//    currentTitleWidth = imageTitleHeader.getWidth();
//    currentTitleHeight = imageTitleHeader.getHeight();
//
//    while (currentTitleHeight > targetTitleHeight)
//    {
//        currentTitleWidth = imageTitleHeader.getWidth() * 0.75f;
//        currentTitleHeight = imageTitleHeader.getHeight() * 0.75f;
//
//        imageTitleHeader = imageTitleHeader.rescaled(currentTitleWidth, currentTitleHeight, juce::Graphics::highResamplingQuality);
//    }
//
//    juce::Rectangle<float> titleBounds{ bounds.getCentreX() - targetTitleWidth/2, 5, (float)targetTitleWidth, (float)targetTitleHeight };
//    //juce::Rectangle<float> titleBounds{ bounds.getCentreX() - targetTitleWidth/2, companyBounds.getBottom() + 5, (float)targetTitleWidth, (float)targetTitleHeight };
//    g.drawImage(imageTitleHeader, titleBounds);
//    
//    // Draw Company Title
//    // ======================================================================
//    /* Downsample once, and draw company title */
//    currentCompanyWidth = imageCompanyTitle.getWidth();
//    currentCompanyHeight = imageCompanyTitle.getHeight();
//
//    while (currentCompanyHeight > targetCompanyHeight)
//    {
//        currentCompanyWidth = imageCompanyTitle.getWidth() * 0.75f;
//        currentCompanyHeight = imageCompanyTitle.getHeight() * 0.75f;
//
//        imageCompanyTitle = imageCompanyTitle.rescaled(currentCompanyWidth, currentCompanyHeight, juce::Graphics::highResamplingQuality);
//    }
//
//    //juce::Rectangle<float> companyBounds{ bounds.getCentreX() - targetCompanyWidth/2, 5, (float)targetCompanyWidth, (float)targetCompanyHeight };
//    juce::Rectangle<float> companyBounds{ bounds.getCentreX() - targetCompanyWidth/2, titleBounds.getBottom() + 5, (float)targetCompanyWidth, (float)targetCompanyHeight };
//    g.drawImage(imageCompanyTitle, companyBounds);
//    
//    // Draw Company Title
//    // ======================================================================
//    g.setColour(juce::Colours::grey);
//    g.drawRect(bounds);
//}
//
//void ActivatorWindow::resized()
//{
//    using namespace juce;
//    auto bounds = getLocalBounds();
//    
//    mLabelStatus.setJustificationType(juce::Justification::centred);
//    mLabelStatus.setColour(juce::Label::textColourId, juce::Colours::transparentWhite.withAlpha(0.4f));
//    mLabelThankYou.setJustificationType(juce::Justification::centred);
//    mLabelLicenseStatus.setJustificationType(juce::Justification::centred);
//    mLabelThankYou.setText("Wonderland Audio thanks you for your support.", juce::NotificationType::dontSendNotification);
//    mButtonActivateFull.setButtonText("Activate\nLicense");
//    
//    mButtonClose.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black.withAlpha(0.f));
//    mButtonActivateTrial.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black.withAlpha(0.75f));
//    mButtonActivateFull.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black.withAlpha(0.75f));
//    mTextCode.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.75f));
//    
//    mTextCode.setTextToShowWhenEmpty("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX", juce::Colours::lightgrey);
//    mButtonActivateTrial.setButtonText("Click Here to Activate 14-day Trial");
//    mButtonClose.setButtonText("X");
//
//    mButtonPurchase.setButtonText("Purchase License Here");
//    mButtonPurchase.setURL(URL("http://www.wonderlandaudio.com"));
//    
//    mButtonClose.setBounds(bounds.getRight() - 30,5,25,25);
//    
//    updateFlexBox();
//}
//
//void ActivatorWindow::updateFlexBox()
//{
//    using namespace juce;
//    auto bounds = getLocalBounds();
//    
//    /* Set up Activation Row */
//    // ==================================================================
//    FlexBox activationRow;
//    activationRow.flexDirection = FlexBox::Direction::row;
//    activationRow.flexWrap = FlexBox::Wrap::noWrap;
//    
//    auto spacerWidth = FlexItem().withWidth(10);
//    
//    activationRow.items.add(FlexItem(mTextCode).withFlex(1.f));
//    activationRow.items.add(spacerWidth);
//    activationRow.items.add(FlexItem(mButtonActivateFull).withWidth(100));
//    
//    /* Build Menu FlexBox */
//    // ==================================================================
//    FlexBox componentColumn;
//    componentColumn.flexDirection = FlexBox::Direction::column;
//    componentColumn.flexWrap = FlexBox::Wrap::noWrap;
//
//    auto spacer = FlexItem().withHeight(5);
//
//    // Thank You
//    componentColumn.items.add(FlexItem(mLabelThankYou).withHeight(20));
//    componentColumn.items.add(spacer);
//
//    // Trial Info & Product Status Placeholder
//    componentColumn.items.add(FlexItem(*placeholder).withHeight(25));
//    componentColumn.items.add(spacer);
//    componentColumn.items.add(spacer);
//    
//    // Enter License Activation Row
//    componentColumn.items.add(FlexItem(activationRow).withFlex(1.f));
//    componentColumn.items.add(spacer);
//    
//    // Purchase License Button
//    componentColumn.items.add(FlexItem(mButtonPurchase).withFlex(1.f));
//
//    // Action Status
//    componentColumn.items.add(FlexItem(mLabelStatus).withFlex(1.f));
//    
//    
//    componentColumn.performLayout(bounds.withTrimmedTop(95).reduced(10, 0));
//}
//
//void ActivatorWindow::buttonClicked(juce::Button* button)
//{
//    if (button == &mButtonActivateFull)
//    {
//        std::string key = mTextCode.getText().toStdString();
//        activate(key);
//    }
//    
//    if (button == &mButtonActivateTrial)
//    {
//        activateTrial();
//    }
//    
//    if (button == &mButtonClose)
//    {
//        mOkToClose = true;
//    }
//
//}
//
//
