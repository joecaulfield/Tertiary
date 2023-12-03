/*
  ==============================================================================

    BandControls.cpp
    Created: 29 Oct 2022 8:41:50pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "BandControls.h"

/* Constructor */
//============================================================
BandControl::BandControl(juce::AudioProcessorValueTreeState& apv)
    : apvts(apv)
{

    buildToggleSync();
    buildToggleInvert();
    buildRateOrRhythm();
    buildMenuWaveshape();
    buildSliderPhase();
    buildSliderSkew();
    buildSliderDepth();
    buildSliderBandGain();
    buildToggleBypass();
    buildToggleSolo();
    buildToggleMute();
    
    runOnceOnConstructor = false;
    
    
    setBufferedToImage(true);
}

BandControl::~BandControl()
{
    mToggleSync.setLookAndFeel(nullptr);
    mToggleInvert.setLookAndFeel(nullptr);
    
    mDropRhythm.setLookAndFeel(nullptr);
    mDropWaveshape.setLookAndFeel(nullptr);
}

/* Called on repaint */
//============================================================
void BandControl::paint(juce::Graphics& g)
{
    //DBG("BandControl::paint");
    g.fillAll(juce::Colours::transparentBlack.withAlpha(0.35f));
}

/* Called on resize */
//============================================================
void BandControl::resized()
{
    updateFlexBox();
}

void BandControl::updateFlexBox()
{
    using namespace juce;

    auto bounds = getLocalBounds();
    
    auto componentBounds = bounds.reduced(5, 5);
    
    /* Create the Waveshape Row */
    //============================================================
    FlexBox waveShapeRow;
    waveShapeRow.flexDirection = FlexBox::Direction::row;
    waveShapeRow.flexWrap = FlexBox::Wrap::noWrap;
    
    waveShapeRow.items.add(FlexItem(mDropWaveshape).withFlex(1.f));
    waveShapeRow.items.add(FlexItem().withWidth(5));
    waveShapeRow.items.add(FlexItem(mToggleInvert).withWidth(40));


    /* Create the Button Row */
    //============================================================
    FlexBox buttonRow;
    buttonRow.flexDirection = FlexBox::Direction::row;
    buttonRow.flexWrap = FlexBox::Wrap::noWrap;
    buttonRow.items.add(FlexItem(mToggleSync).withFlex(1.f));
    buttonRow.items.add(FlexItem(mToggleBypass).withFlex(1.f));
    buttonRow.items.add(FlexItem(mToggleSolo).withFlex(1.f));
    buttonRow.items.add(FlexItem(mToggleMute).withFlex(1.f));

    /* Create the Full Control Layout */
    //============================================================
    FlexBox componentColumn;
    componentColumn.flexDirection = FlexBox::Direction::column;
    componentColumn.flexWrap = FlexBox::Wrap::noWrap;

    auto spacer = FlexItem().withHeight(10);
    
    componentColumn.items.add(FlexItem(waveShapeRow).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(mSliderSkew).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(mSliderDepth).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(*placeholder).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(mSliderPhase).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(mSliderBandGain).withHeight(20));
    componentColumn.items.add(spacer);
    componentColumn.items.add(FlexItem(buttonRow).withHeight(30));
    
    componentColumn.performLayout(componentBounds);
    
}

void BandControl::buildToggleSync()
{
    mToggleSync.mToggleButton.addListener(this);
    addAndMakeVisible(mToggleSync);
}

void BandControl::buildToggleInvert()
{
    mToggleInvert.mToggleButton.addListener(this);
    addAndMakeVisible(mToggleInvert);
}

void BandControl::buildRateOrRhythm()
{
    using namespace Params;
    const auto& params = GetParams();
 
    if (runOnceOnConstructor)
        mDropRhythm.addItemList(apvts.getParameter(params.at(Names::Multiplier_Low_LFO))->getAllValueStrings(), 1);
    
    mSliderRate.setStyleStandard(" Hz");
    
    mDropRhythm.setAlpha(0.85f);
    
    addAndMakeVisible(mDropRhythm);
    addAndMakeVisible(mSliderRate);
    
    if (isSyncedToHost)
    {

        mDropRhythm.setLookAndFeel(&multComboLookAndFeel);
        mDropRhythm.setTextWhenNothingSelected("Rhythm");
        
        placeholder = &mDropRhythm;
        mDropRhythm.setVisible(true);
        mSliderRate.setVisible(false);
    }
    else
    {
        placeholder = &mSliderRate;
        mDropRhythm.setVisible(false);
        mSliderRate.setVisible(true);
    }


}

void BandControl::buildMenuWaveshape()
{
    using namespace Params;
    const auto& params = GetParams();
    
    if (runOnceOnConstructor)
        mDropWaveshape.addItemList(apvts.getParameter(params.at(Names::Wave_Low_LFO))->getAllValueStrings(), 1);
       
    mDropWaveshape.setLookAndFeel(&waveComboLookAndFeel);
    mDropWaveshape.setTextWhenNothingSelected("Waveshape");
    mDropWaveshape.setAlpha(0.85f);
    addAndMakeVisible(mDropWaveshape);
}

void BandControl::buildSliderPhase()
{
    mSliderPhase.setStyleCenter(juce::CharPointer_UTF8("\xc2\xb0"));
    addAndMakeVisible(mSliderPhase);
}

void BandControl::buildSliderSkew()
{
    mSliderSkew.setStyleCenter("%");
    addAndMakeVisible(mSliderSkew);
}

void BandControl::buildSliderDepth()
{
    mSliderDepth.setStyleStandard("%");
    addAndMakeVisible(mSliderDepth);
}

void BandControl::buildSliderBandGain()
{
    mSliderBandGain.setStyleStandard(" dB");
    addAndMakeVisible(mSliderBandGain);
}

void BandControl::buildToggleBypass()
{
    mToggleBypass.mToggleButton.addListener(this);
    addAndMakeVisible(mToggleBypass);
}

void BandControl::buildToggleSolo()
{
    mToggleSolo.mToggleButton.addListener(this);
    addAndMakeVisible(mToggleSolo);
}

void BandControl::buildToggleMute()
{
    mToggleMute.mToggleButton.addListener(this);
    addAndMakeVisible(mToggleMute);
}

void BandControl::setMode(juce::String bandMode)
{
    if (bandMode == "LOW") mode = 0;

    if (bandMode == "MID") mode = 1;

    if (bandMode == "HIGH") mode = 2;

    mSliderPhase.setBandMode(mode);
    mSliderRate.setBandMode(mode);
    mSliderSkew.setBandMode(mode);
    mSliderDepth.setBandMode(mode);
    mSliderBandGain.setBandMode(mode);
    waveComboLookAndFeel.setMode(mode);
    multComboLookAndFeel.setMode(mode);
    mToggleSync.setBandMode(mode, "SYNC");
    mToggleInvert.setBandMode(mode, "INV");
    mToggleBypass.setBandMode(mode, "BYP");
    mToggleSolo.setBandMode(mode, "SOLO");
    mToggleMute.setBandMode(mode, "MUTE");
    
    
}

void BandControl::buttonClicked(juce::Button* button)
{
    if (button == &mToggleSync.mToggleButton)
    {
        isSyncedToHost = !isSyncedToHost;
        
        buildRateOrRhythm();
        updateFlexBox();
    }

    // Prevent both 'Mute' and 'Solo' from being engaged simultaneously

    if (button == &mToggleSolo.mToggleButton)
    {
        if (button->getToggleState())
            mToggleMute.mToggleButton.setToggleState(false, true);
        
        mBsmChanged = true;
    }

    if (button == &mToggleMute.mToggleButton)
    {
        if (button->getToggleState())
            mToggleSolo.mToggleButton.setToggleState(false, true);
        
        mBsmChanged = true;
    }
    
    if (button == &mToggleBypass.mToggleButton)
    {
        mBsmChanged = true;
    }
    
}
