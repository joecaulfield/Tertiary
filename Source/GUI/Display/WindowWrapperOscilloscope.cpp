/*
  ==============================================================================

    WindowWrapperFrequency.cpp
    Created: 13 Oct 2022 11:12:35pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "WindowWrapperOscilloscope.h"

/* Constructor */
// ========================================================
WindowWrapperOscilloscope::WindowWrapperOscilloscope( TertiaryAudioProcessor& p, GlobalControls& gc)
                                : audioProcessor(p),globalControls(gc)
{

    addAndMakeVisible(oscilloscope);
    addAndMakeVisible(optionsMenu);
    
    buildOptionsMenuParameters();
    
    startTimerHz(60);
}

/* Destructor */
// ========================================================
WindowWrapperOscilloscope::~WindowWrapperOscilloscope()
{
    
}

/* Links menu options to parameters */
// ========================================================
void WindowWrapperOscilloscope::buildOptionsMenuParameters()
{
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

    auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)    // Bool Helper --> Part 8 Param Namespace
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
        jassert(param != nullptr);                                                                      // Error Checking
    };
    
    boolHelper(showLowBandParam,    Names::Show_Low_Scope);
    boolHelper(showMidBandParam,    Names::Show_Mid_Scope);
    boolHelper(showHighBandParam,   Names::Show_High_Scope);
    boolHelper(stackBandsParam,     Names::Stack_Bands_Scope);
    boolHelper(showCursorParam,     Names::Show_Cursor_Scope);
    boolHelper(showPlayheadParam,   Names::Show_Playhead_Scope);
    
    optionsMenu.addOptionToList("Low",
                                "Show Low Band",
                                audioProcessor.apvts,
                                params.at(Names::Show_Low_Scope));

    optionsMenu.addOptionToList("Mid",
                                "Show Mid Band",
                                audioProcessor.apvts,
                                params.at(Names::Show_Mid_Scope));

    optionsMenu.addOptionToList("High",
                                "Show High Band",
                                audioProcessor.apvts,
                                params.at(Names::Show_High_Scope));

    optionsMenu.addOptionToList("Stack Bands",
                                "Stack All Bands",
                                audioProcessor.apvts,
                                params.at(Names::Stack_Bands_Scope));

//    optionsMenu.addOptionToList("Cursor",
//                                "Show Cursor",
//                                audioProcessor.apvts,
//                                params.at(Names::Show_Cursor_Scope));
    
    audioProcessor.apvts.addParameterListener(params.at(Names::Show_Low_Scope), this);
    audioProcessor.apvts.addParameterListener(params.at(Names::Show_Mid_Scope), this);
    audioProcessor.apvts.addParameterListener(params.at(Names::Show_High_Scope), this);
    audioProcessor.apvts.addParameterListener(params.at(Names::Stack_Bands_Scope), this);
    audioProcessor.apvts.addParameterListener(params.at(Names::Show_Cursor_Scope), this);
    
    updateOptionsParameters();
}

/* Pass latest parameters to the oscilloscope */
// ========================================================
void WindowWrapperOscilloscope::updateOptionsParameters()
{
    bool showLowBand = showLowBandParam->get();
    bool showMidBand = showMidBandParam->get();
    bool showHighBand = showHighBandParam->get();
    bool stackBands = stackBandsParam->get();

    oscilloscope.updateScopeParameters(showLowBand, showMidBand, showHighBand, stackBands);
}

/* Called on component resize */
// ========================================================
void WindowWrapperOscilloscope::resized()
{
    optionsMenu.setTopLeftPosition(4,4);
    
    oscilloscope.setSize(getWidth(), getHeight() );
}

/* When a parameter is changed, update accordingly */
// ========================================================
void WindowWrapperOscilloscope::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateOptionsParameters();
}
  
/* Timer callback.  Used primarily to animate playhead.  (Not in use) */
// ========================================================
void WindowWrapperOscilloscope::timerCallback()
{
    //mShowPlayhead = oscilloscope.getShowPlayhead();
    //mPanOrZoomChanging = oscilloscope.getPanOrZoomChanging();
    //mPlayheadPositionPixel = oscilloscope.getPlayheadPositionPixel();
    
    //mLowRegion = oscilloscope.getLowRegion();
    //mMidRegion = oscilloscope.getMidRegion();
    //mHighRegion = oscilloscope.getHighRegion();
    
    //repaint();
}

// ========================================================
void WindowWrapperOscilloscope::paint(juce::Graphics& g)
{
    
}

/* Not current in use.  Animate the moving playhead. */
// ========================================================
void WindowWrapperOscilloscope::paintOverChildren(juce::Graphics& g)
{
    paintPlayhead(g);
}

/* Not currently in use.  Animate the moving playheead. */
// ========================================================
void WindowWrapperOscilloscope::paintPlayhead(juce::Graphics& g)
{
    if (mShowPlayhead && ! mPanOrZoomChanging)
    {
        g.setColour(juce::Colours::darkgrey);
    
        g.drawVerticalLine(mPlayheadPositionPixel, mLowRegion.getY(), mLowRegion.getBottom());
        g.drawVerticalLine(mPlayheadPositionPixel, mMidRegion.getY(), mMidRegion.getBottom());
        g.drawVerticalLine(mPlayheadPositionPixel, mHighRegion.getY(), mHighRegion.getBottom());
    }
}
