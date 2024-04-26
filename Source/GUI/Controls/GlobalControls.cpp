/*
  ==============================================================================

    GlobalControls.cpp
    Created: 30 Dec 2021 11:38:20am
    Author:  Joe

  ==============================================================================
*/

#include "GlobalControls.h"

// GLOBAL CONTROLS =============================================================

GlobalControls::GlobalControls(TertiaryAudioProcessor& p)
	:  apvts(p.apvts), audioProcessor(p)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

	addAndMakeVisible(inputGain);
	addAndMakeVisible(outputGain);
    
    addAndMakeVisible(lowBandControls);
    addAndMakeVisible(midBandControls);
    addAndMakeVisible(highBandControls);
    
    addMouseListener(this, true);
    
    lowBandControls.addMouseListener(this, true);
    midBandControls.addMouseListener(this, true);
    highBandControls.addMouseListener(this, true);

    lowBandControls.setMode("LOW");
    midBandControls.setMode("MID");
    highBandControls.setMode("HIGH");
    
	// Determine the Label-Meters' Pickoff Point
	inputGain.setPickOffPoint("INPUT");
	outputGain.setPickOffPoint("OUTPUT");

    inputGain.setName("INPUT");
    outputGain.setName("OUTPUT");

	makeAttachments();
    makeGainControlAttachments();
    makeTimingControlAttachments();
    makeWaveControlAttachments();

    makeLabel(mLabelRhythm, "Rhythm");
    makeLabel(mLabelWaveShape, "Shape");
    makeLabel(mLabelPhase, "Phase");
    makeLabel(mLabelDepth, "Depth");
    makeLabel(mLabelSkew, "Skew");
    makeLabel(mLabelBandGain, "Gain");
    
	setSize(250, 140);
}

void GlobalControls::paint(juce::Graphics& g)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    paintWindowBorders(g);
    paintBandLabels(g);

    //if (shouldPaintOnceOnInit)
    //    paintOnceOnInit(g);
}

//void GlobalControls::paintOnceOnInit(juce::Graphics& g)
//{
//    WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());
//
//
//    
//    //shouldPaintOnceOnInit = false;
//}

void GlobalControls::paintWindowBorders(juce::Graphics& g)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace juce;
    using namespace AllColors::GlobalControlsColors;

    auto bounds = getLocalBounds().toFloat();

    g.setGradientFill(BACKGROUND_GRADIENT(bounds));

    auto windowBorderColor = ColorScheme::WindowBorders::getWindowBorderColor();
    auto dividerBorderColor = ColorScheme::WindowBorders::getWindowBorderColor().withMultipliedAlpha(0.3f);

    auto skewBorder = labelBorder;
    skewBorder.setY(mLabelSkew.getY() - 5);
    paintBorder(g, dividerBorderColor, skewBorder);
    
    auto depthBorder = labelBorder;
    depthBorder.setY(mLabelDepth.getY() - 5);
    paintBorder(g, dividerBorderColor, depthBorder);
    
    auto rhythmBorder = labelBorder;
    rhythmBorder.setY(mLabelRhythm.getY() - 5);
    paintBorder(g, dividerBorderColor, rhythmBorder);
    
    auto phaseBorder = labelBorder;
    phaseBorder.setY(mLabelPhase.getY() - 5);
    paintBorder(g, dividerBorderColor, phaseBorder);
    
    auto gainBorder = labelBorder;
    gainBorder.setY(mLabelBandGain.getY() - 5);
    paintBorder(g, dividerBorderColor, gainBorder);
    
    // Input Gain Border
    paintBorder(g, windowBorderColor, border1);

    // Text Labels Border
    paintBorder(g, windowBorderColor, border2);
    
    // Low-Band Border
    paintBorder(g, windowBorderColor, border3);
    
    // Mid-Band Border
    paintBorder(g, windowBorderColor, border4);
    
    // High-Band Border
    paintBorder(g, windowBorderColor, border5);
    
    // Output Gain Border
    paintBorder(g, windowBorderColor, border6);
}

void GlobalControls::paintBandLabels(juce::Graphics& g)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace FontEditor;
 
    auto bounds = getLocalBounds();
    
    auto controlLabelsFontTypeface = FontEditor::ControlLabels::getTypeface();
    auto controlLabelsFontSize = 23.f;
    auto controlLabelsFontStyle = juce::Font::FontStyleFlags::bold;
    auto controlLabelsFontColor = juce::Colour(0xff29242e);
    //auto controlLabelsFontTransparency = FontEditor::ControlLabels::getFontTransparency();
    
    g.setColour(controlLabelsFontColor);

    auto titleFont = juce::Font(    controlLabelsFontTypeface,
                                    controlLabelsFontSize,
                                    controlLabelsFontStyle);
    
    int x = border3.getX();
    int y = bounds.getBottom() - 28;
    int width = border3.getWidth();
    int height = 30;
    
    g.setFont(titleFont);
    g.setColour(controlLabelsFontColor);
    
    auto lowLabelBounds = juce::Rectangle<int>{x, y, width, height};

    g.drawFittedText("LOW", lowLabelBounds, juce::Justification::centred, 1);
    
    x = border4.getX();
    
    auto midLabelBounds = juce::Rectangle<int>{x, y, width, height};
    g.drawText("MID", midLabelBounds, juce::Justification::centred, 1);
    
    x = border5.getX();
    
    auto highLabelBounds = juce::Rectangle<int>{x, y, width, height};
    g.drawFittedText("HIGH", highLabelBounds, juce::Justification::centred, 1);
}

void GlobalControls::resized()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace juce;

    auto bounds = getLocalBounds();

    auto meterWidth = 32;
    auto padding = 4;
    
    auto labelWidth = 75;
    auto bandWidth = (getWidth() - labelWidth - meterWidth*2)/3.f;
    
    //auto controlWidth2 = 210;

    border1.setBounds(    bounds.getX(),        bounds.getY(), meterWidth,              bounds.getHeight());
    border2.setBounds(    border1.getRight(),   bounds.getY(), labelWidth,              bounds.getHeight());
    border3.setBounds(    border2.getRight(),   bounds.getY(), bandWidth,               bounds.getHeight());
    border4.setBounds(    border3.getRight(),   bounds.getY(), bandWidth,               bounds.getHeight());
    border5.setBounds(    border4.getRight(),   bounds.getY(), bandWidth,               bounds.getHeight());
    border6.setBounds(    bounds.getRight() - meterWidth, bounds.getY(), meterWidth,    bounds.getHeight());
   

    inputGain.setBounds(    border1.getX() + padding,
                            border1.getY() + padding,
                            border1.getWidth() - 2 * padding,
                            border1.getHeight() - 2 * padding);

    lowBandControls.setBounds( border3.getX() + padding,
                              border3.getY() + padding,
                              border3.getWidth() - 2 * padding,
                              border3.getHeight() - 2 * padding);
    
    midBandControls.setBounds( border4.getX() + padding,
                              border4.getY() + padding,
                              border4.getWidth() - 2 * padding,
                              border4.getHeight() - 2 * padding);
    
    highBandControls.setBounds( border5.getX() + padding,
                               border5.getY() + padding,
                               border5.getWidth() - 2 * padding,
                               border5.getHeight() - 2 * padding);
    
    
    outputGain.setBounds(   border6.getX() + padding,
                            border6.getY() + padding,
                            border6.getWidth() - 2 * padding,
                            border6.getHeight() - 2 * padding);

    /* Create the Full Control Layout */
    //============================================================
    auto labelBounds = border2.reduced(5 + padding, 25 + padding);
    
    mLabelWaveShape.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getWaveBounds().getCentreY() + padding);
    mLabelSkew.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getSkewBounds().getCentreY() + padding);
    mLabelDepth.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getDepthBounds().getCentreY() + padding);
    mLabelRhythm.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getRhythmBounds().getCentreY() + padding);
    mLabelPhase.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getPhaseBounds().getCentreY() + padding);
    mLabelBandGain.setCentrePosition(labelBounds.getCentreX(), lowBandControls.getGainBounds().getCentreY() + padding);
    
    labelBorder.setBounds(border2.getX(), mLabelWaveShape.getY()-5, getWidth()-meterWidth*2, mLabelWaveShape.getHeight()+10);
    
    //shouldPaintOnceOnInit = true;
}

void GlobalControls::makeAttachments()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace Params;
    const auto& params = GetParams();

    inputGainAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Input_Gain),
                            inputGain.sliderGain);

    outputGainAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Output_Gain),
                            outputGain.sliderGain);
}

void GlobalControls::makeLabel(juce::Label& label, juce::String textToPrint)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace FontEditor::ControlLabels;
    
    auto controlLabelsFontJustification = FontEditor::ControlLabels::getJustification();
    auto controlLabelsFontTypeface = FontEditor::ControlLabels::getTypeface();
    auto controlLabelsFontSize = FontEditor::ControlLabels::getFontSize();
    auto controlLabelsFontStyle = FontEditor::ControlLabels::getFontStyle();
    auto controlLabelsFontColor = FontEditor::ControlLabels::getFontColor();
    auto controlLabelsFontTransparency = FontEditor::ControlLabels::getFontTransparency();
    
    label.setJustificationType(controlLabelsFontJustification);
    
    auto myFont = juce::Font(   controlLabelsFontTypeface,
                                controlLabelsFontSize,
                                controlLabelsFontStyle);
    
    label.setFont(myFont);
    label.setAlpha(controlLabelsFontTransparency);
    label.setColour(juce::Label::textColourId, controlLabelsFontColor);
    
    label.setText(textToPrint, juce::NotificationType::dontSendNotification);
    label.setSize(50, 20);
    
    addAndMakeVisible(label);
}

void GlobalControls::makeGainControlAttachments()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace Params;
    const auto& params = GetParams();

    // GAIN =====================
    gainLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Gain_Low_Band),
                            lowBandControls.mSliderBandGain.slider);
    

    gainMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Gain_Mid_Band),
                            midBandControls.mSliderBandGain.slider);

    gainHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Gain_High_Band),
                            highBandControls.mSliderBandGain.slider);

    // SOLO =====================
    soloLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Solo_Low_Band),
                            lowBandControls.mToggleSolo.mToggleButton);

    soloMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Solo_Mid_Band),
                            midBandControls.mToggleSolo.mToggleButton);

    soloHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Solo_High_Band),
                            highBandControls.mToggleSolo.mToggleButton);

    // BYPASS =====================
    bypassLowAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Bypass_Low_Band),
                            lowBandControls.mToggleBypass.mToggleButton);

    bypassMidAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Bypass_Mid_Band),
                            midBandControls.mToggleBypass.mToggleButton);

    bypassHighAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Bypass_High_Band),
                            highBandControls.mToggleBypass.mToggleButton);

    // MUTE =====================
    muteLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Mute_Low_Band),
                            lowBandControls.mToggleMute.mToggleButton);

    muteMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Mute_Mid_Band),
                            midBandControls.mToggleMute.mToggleButton);

    muteHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Mute_High_Band),
                            highBandControls.mToggleMute.mToggleButton);
}

void GlobalControls::makeTimingControlAttachments()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace Params;
    const auto& params = GetParams();

    // SYNC TO HOST ================
    syncLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Sync_Low_LFO),
                            lowBandControls.mToggleSync.mToggleButton);

    syncMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Sync_Mid_LFO),
                            midBandControls.mToggleSync.mToggleButton);

    syncHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Sync_High_LFO),
                            highBandControls.mToggleSync.mToggleButton);

    // RATE ================================
    rateLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Rate_Low_LFO),
                            lowBandControls.mSliderRate.slider);

    rateMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Rate_Mid_LFO),
                            midBandControls.mSliderRate.slider);

    rateHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Rate_High_LFO),
                            highBandControls.mSliderRate.slider);

    // MULTIPLIER AKA RHYTHM ================================
    multLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Multiplier_Low_LFO),
                            lowBandControls.mDropRhythm);

    multMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Multiplier_Mid_LFO),
                            midBandControls.mDropRhythm);

    multHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Multiplier_High_LFO),
                            highBandControls.mDropRhythm);

    // PHASE ================================
    phaseLowAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Relative_Phase_Low_LFO),
                            lowBandControls.mSliderPhase.slider);

    phaseMidAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Relative_Phase_Mid_LFO),
                            midBandControls.mSliderPhase.slider);

    phaseHighAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Relative_Phase_High_LFO),
                            highBandControls.mSliderPhase.slider);
}

void GlobalControls::makeWaveControlAttachments()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    using namespace Params;
    const auto& params = GetParams();

    // WAVESHAPE ===================
    waveLowAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Wave_Low_LFO),
                            lowBandControls.mDropWaveshape);

    waveMidAttachment =     std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Wave_Mid_LFO),
                            midBandControls.mDropWaveshape);

    waveHighAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                            apvts,
                            params.at(Names::Wave_High_LFO),
                            highBandControls.mDropWaveshape);

    // DEPTH ===================
    depthLowAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Depth_Low_LFO),
                            lowBandControls.mSliderDepth.slider);

    depthMidAttachment =    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Depth_Mid_LFO),
                            midBandControls.mSliderDepth.slider);

    depthHighAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Depth_High_LFO),
                            highBandControls.mSliderDepth.slider);

    // SYMMETRY AKA SKEW ===================
    symmetryLowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Symmetry_Low_LFO),
                            lowBandControls.mSliderSkew.slider);

    symmetryMidAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Symmetry_Mid_LFO),
                            midBandControls.mSliderSkew.slider);

    symmetryHighAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                            apvts,
                            params.at(Names::Symmetry_High_LFO),
                            highBandControls.mSliderSkew.slider);

    // INVERT ===================
    invertLowAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Invert_Low_LFO),
                            lowBandControls.mToggleInvert.mToggleButton);

    invertMidAttachment =   std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Invert_Mid_LFO),
                            midBandControls.mToggleInvert.mToggleButton);

    invertHighAttachment =  std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                            apvts,
                            params.at(Names::Invert_High_LFO),
                            highBandControls.mToggleInvert.mToggleButton);
}


// Guarantees a 30-Character long message
void GlobalControls::sendBroadcast(juce::String parameterName, juce::String parameterValue)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    juce::String delimiter = ":::::";
    juce::String bandName = "xxxxx";
    auto message = bandName + delimiter + parameterName.paddedLeft('x', 10) + delimiter + parameterValue.paddedLeft('x', 10);
    sendActionMessage(message);
}


void GlobalControls::mouseEnter(const juce::MouseEvent& event)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    checkForBandFocus();
}

void GlobalControls::mouseExit(const juce::MouseEvent& event)
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    checkForBandFocus();
}


void GlobalControls::checkForBandFocus()
{
    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, getName());

    auto whoHasFocus = "NONE";

    if (lowBandControls.isMouseOver(true))
        whoHasFocus = "LOW";

    if (midBandControls.isMouseOver(true))
        whoHasFocus = "MID";

    if (highBandControls.isMouseOver(true))
        whoHasFocus = "HIGH";

    sendBroadcast("FOCUS", whoHasFocus);

}