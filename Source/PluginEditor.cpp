/*
	The Plugin Editor is the top-level component which contains all child classes related to the UI.

	Oscilloscope: Top-Left; Child Class representing the time-domain response of LFO parameters
	Frequency Response: Top-Right; Child Class representing the crossover parameters and displays a FFT
	Global Controls: Lower-Half; Child Class containing all user parameters.

	Plugin Editor also contains company title, plug-in name, version information.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TertiaryAudioProcessorEditor::TertiaryAudioProcessorEditor (TertiaryAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	/* Provides GPU acceleration */
	openGLContext.attachTo(*getTopLevelComponent());

    /* Container class for top banner */
    addAndMakeVisible(topBanner);
    
	/* Container class for all parameter controls */
	addAndMakeVisible(globalControls);

    /* Time-Domain Display */
    addAndMakeVisible(wrapperOscilloscope);
    
	/* Frequency-Domain & Crossover Display */
    addAndMakeVisible(wrapperFrequency);


    
	/* Sets window size */
    float scale = 1.f;
    setSize(750*scale, 515*scale);


    startTimerHz(30);

    /* Check for license */
    
    /* If trial is not yet activated, or is expired, open the activator dialog */
    ////if (activatorWindow.getLicenseState() == 0 || activatorWindow.getLicenseState() == 2)
    ////{
        // LICENSING WINDOW TEMPORARILY DISABLED...........
        //openLicenseWindow();
    ////}


    /* Add mouse listeners */
    ////activatorWindow.mButtonActivateFull.addListener(this);
    ////activatorWindow.mButtonActivateTrial.addListener(this);
    ////activatorWindow.mButtonClose.addListener(this);
    
    ////topBanner.setDaysLeft(activatorWindow.getTrialDaysLeft());
    topBanner.addMouseListener(this, true);
    
    // Add Action Listener
    globalControls.getLowControl().addActionListener    ( &wrapperOscilloscope.getOscilloscopeLow() );
    globalControls.getMidControl().addActionListener    ( &wrapperOscilloscope.getOscilloscopeMid() );
    globalControls.getHighControl().addActionListener   ( &wrapperOscilloscope.getOscilloscopeHigh() );

    globalControls.getLowControl().addActionListener    ( &wrapperFrequency.getFrequencyResponse() );
    globalControls.getMidControl().addActionListener    ( &wrapperFrequency.getFrequencyResponse() );
    globalControls.getHighControl().addActionListener   ( &wrapperFrequency.getFrequencyResponse() );
    
    setBufferedToImage(true);
    setOpaque(true);
}

//==============================================================================
TertiaryAudioProcessorEditor::~TertiaryAudioProcessorEditor()
{
	
}

//==============================================================================
void TertiaryAudioProcessorEditor::paint(juce::Graphics& g)
{
	/* Set the Gradient */
	g.setGradientFill(AllColors::PluginEditorColors::BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
	g.fillAll();
}

//==============================================================================
void TertiaryAudioProcessorEditor::resized()
{
	using namespace juce;

    buildFlexboxLayout();
}

/* Builds the UI layout */
//==============================================================================
void TertiaryAudioProcessorEditor::buildFlexboxLayout()
{
    using namespace juce;
    
    auto bounds = getLocalBounds();
    bounds.reduce(5, 0);
    
    /* Create a horizontal FlexBox container to arrange the Oscilloscope and Frequency Response */
    juce::FlexBox flexBoxRow;
    flexBoxRow.flexDirection = FlexBox::Direction::row;
    flexBoxRow.flexWrap = FlexBox::Wrap::noWrap;

    auto spacerH = FlexItem().withWidth(5);    // Gap between O-Scope and Freq Resp

    flexBoxRow.items.add(FlexItem(wrapperOscilloscope).withFlex(1.f));  // Insert Scope Wrapper
    flexBoxRow.items.add(spacerH);                                      // Insert Spacer
    flexBoxRow.items.add(FlexItem(wrapperFrequency).withFlex(1.f));     // Insert Freq Wrapper
    
    /* Create a vertical flexBox container to arrange the Row and Global Controls */
    
    juce::FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::column;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    flexBox.items.add(FlexItem(topBanner).withHeight(50));
    flexBox.items.add(FlexItem(flexBoxRow).withHeight(getHeight() * 0.4f));
    flexBox.items.add(FlexItem(globalControls).withFlex(1.f));
    
    flexBox.performLayout(bounds);
}

//==============================================================================
void TertiaryAudioProcessorEditor::timerCallback()
{
////    // Checks if user has clicked the 'Close' or 'Activate' buttons on the ActivatorWindow
////    if (buttonAction == true && activatorWindow.isOkToClose())
////        closeLicenseWindow();
////
}

/* Opens the license window */
//==============================================================================
//void TertiaryAudioProcessorEditor::openLicenseWindow()
//{
//    addAndMakeVisible(activatorWindow);
//    
//    //if (activatorWindow.getLicenseState() == 1)
//    //    checkIfShouldShowTrialTitle();
//
//    activatorWindow.setSize(getWidth()*0.6f, getHeight()*0.5f);
//    activatorWindow.setCentreRelative(0.5f, 0.5f);
//
//    activatorWindow.setAlpha(0.95f);
//}
//
///* Closes the license window - fades out if an Activation was performed, immediate if 'X' button */
////==============================================================================
//void TertiaryAudioProcessorEditor::closeLicenseWindow()
//{
//    counterWait++;
//    
//    // If user has hit the 'Close' button, close does not wait
//    if (doNotWait)
//    {
//        counterWait = 45;
//        doNotWait = false;
//    }
//
//    // Wait A Moment
//    if (counterWait > 45)
//    {
//        // Fade Out Window
//        activatorWindow.setAlpha((float)counterFade / 30.f);
//        
//        if (counterFade > 0)
//            counterFade--;
//        else
//        {
//            activatorWindow.setVisible(false);
//            counterFade = 30;
//            counterWait = 0;
//            buttonAction = false;
//            checkIfShouldShowTrialTitle();
//        }
//
//    }
//}
//
///* Updates whether or not the trial title should be shown in top banner, based on updates with Activator Window */
////==============================================================================
//void TertiaryAudioProcessorEditor::checkIfShouldShowTrialTitle()
//{
//    if (activatorWindow.getLicenseState() == 1)
//        topBanner.setShowTrialTitle(true);
//    else
//        topBanner.setShowTrialTitle(false);
//    
//    repaint();
//}
//
///* Closes License Window on Trial Activate, License Activate, or Close (When Valid) */
////==============================================================================
//void TertiaryAudioProcessorEditor::buttonClicked(juce::Button *button)
//{
//    if (button == &activatorWindow.mButtonClose)
//        doNotWait = true;
//    else
//        doNotWait = false;
//    
//    buttonAction = true;
//}
//
//
///* Opens License Window on Top Banner Double Click */
////==============================================================================
//void TertiaryAudioProcessorEditor::mouseDoubleClick (const juce::MouseEvent &event)
//{
//    if (event.getPosition().getY() < 50)
//    {
//        addAndMakeVisible(activatorWindow);
//        activatorWindow.resetOkToClose();
//        openLicenseWindow();
//    }
//
//}



