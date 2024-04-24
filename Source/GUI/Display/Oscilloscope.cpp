/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	Class containing the oscilloscope which shows 
	the individual LFO waveforms for viewing
 
  ==============================================================================
*/

#include "Oscilloscope.h"

/* Constructor */
// ========================================================
Oscilloscope::Oscilloscope(TertiaryAudioProcessor& p) : audioProcessor(p),
                                                        lowLFO(p.lowLFO), midLFO(p.midLFO), highLFO(p.highLFO),
                                                        lowScope(p.apvts, p.lowLFO, sliderScroll),
                                                        midScope(p.apvts, p.midLFO, sliderScroll),
                                                        highScope(p.apvts, p.highLFO, sliderScroll)
{

	using namespace Params;             // Create a Local Reference to Parameter Mapping
	const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	floatHelper(scopeCursorParam,   Names::Cursor_Position);
	floatHelper(scopePoint1Param,   Names::Scope_Point1);
	floatHelper(scopePoint2Param,   Names::Scope_Point2);

	sliderScroll.initializePoints(scopePoint1Param->get(), scopePoint2Param->get());
	
    addAndMakeVisible(lowScope);
    addAndMakeVisible(midScope);
    addAndMakeVisible(highScope);
    
    lowScope.setName("LOW");
    midScope.setName("MID");
    highScope.setName("HIGH");

	addAndMakeVisible(sliderScroll);
    
	addMouseListener(this, true);

	sliderScroll.addMouseListener(this, true);

    // Set Scope Channels as Listeners to the Scroll Slider
    sliderScroll.addActionListener(&lowScope);
    sliderScroll.addActionListener(&midScope);
    sliderScroll.addActionListener(&highScope);
}

/* Destructor */
// ========================================================
Oscilloscope::~Oscilloscope()
{
}

/* Called on component resize */
// ========================================================
void Oscilloscope::resized()
{

    /* Rebuild the current scope layout */
    buildScopeLayout();
    
    /* Fixed Position for Slider */
    sliderScroll.setSize(getWidth(), 25);
    sliderScroll.setTopLeftPosition(getWidth()/2-sliderScroll.getWidth()/2, getBottom()-25);
    



    /* Initialize the pan/zoom points for the scroll bar... a little rusty on this! */
	int x = sliderScroll.getLocalBounds().getCentreX() - (float)sliderScroll.getMaxWidth() / 2.f;
	float p1 = scopePoint1Param->get() / 100.f * (float)sliderScroll.getMaxWidth();
	float p2 = scopePoint2Param->get() / 100.f * (float)sliderScroll.getMaxWidth();

	/* Update ScrollPad with Loaded Points */
	sliderScroll.initializePoints(x + p1, x + p2);

}

/* Receive updated options parameters from wrapper & options menu */
// ========================================================
void Oscilloscope::updateScopeParameters(bool showLow, bool showMid, bool showHigh, bool stackBands)
{
    mShowLowBand = showLow;
    mShowMidBand = showMid;
    mShowHighBand = showHigh;
    mStackAllBands = stackBands;
    
    buildScopeLayout();
}

/* Determine layout style and perform accordingly */
// ========================================================
void Oscilloscope::buildScopeLayout()
{
    // Perform the oscilloscope layout
    if (mStackAllBands)
        buildStackedScopeLayout();
    else
        buildOverlappedScopeLayout();
}

/* Performed stacked layout.  Scopes side-by-side */
// ========================================================
void Oscilloscope::buildStackedScopeLayout()
{
    using namespace juce;

    /* Define the scope channels as 'stacked'.  Updates background color */
    // ========================================================
    lowScope.setBandsStacked(true);
    midScope.setBandsStacked(true);
    highScope.setBandsStacked(true);
    
    /* Define area in which the scope channels should exist */
    scopeRegion = getLocalBounds();
    scopeRegion.reduce(0, 6);
    scopeRegion.removeFromBottom(15);    // Make room for scroll slider
    
    /* Creates a flex-box list of the channels which will be displayed */
    // ========================================================
    
    juce::FlexBox flexBox;
    
    flexBox.flexDirection = FlexBox::Direction::column;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    auto spacer = FlexItem().withHeight(5);    // Gap between O-Scope and Freq Resp

    /* Show low band, or hide */
    // ========================================================
    if (mShowLowBand)
    {
        flexBox.items.add(FlexItem(lowScope).withFlex(1.f));
        flexBox.items.add(spacer);
    }
    else
        lowScope.setSize(0, 0);

    /* Show mid band, or hide */
    // ========================================================
    if (mShowMidBand)
    {
        flexBox.items.add(FlexItem(midScope).withFlex(1.f));
        flexBox.items.add(spacer);
    }
    else
        midScope.setSize(0, 0);
    
    /* Show high band, or hide */
    // ========================================================
    if (mShowHighBand)
    {
        flexBox.items.add(FlexItem(highScope).withFlex(1.f));
    }
    else
        highScope.setSize(0, 0);

    flexBox.performLayout(scopeRegion);
}

/* Performed overlapped layout. Scopes overlapped */
// ========================================================
void Oscilloscope::buildOverlappedScopeLayout()
{
    /* Defines the scope channels as 'not stacked', or 'overlapped'.  Adjusts background colors */
    lowScope.setBandsStacked(false);
    midScope.setBandsStacked(false);
    highScope.setBandsStacked(false);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowLowBand)
        lowScope.setBounds(scopeRegion);
    else
        lowScope.setSize(0, 0);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowMidBand)
        midScope.setBounds(scopeRegion);
    else
        midScope.setSize(0, 0);
    
    /* Show mid band, or hide */
    // ========================================================
    if (mShowHighBand)
        highScope.setBounds(scopeRegion);
    else
        highScope.setSize(0, 0);
    
    /* Adjust the transparancy of each channel */
    // ========================================================
    lowScope.setAlpha(1.f);
    midScope.setAlpha(1.f);
    highScope.setAlpha(1.f);
    
    float dimValue = 0.25f;
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mLowFocus && mShowLowBand) {
        lowScope.toFront(false);
        midScope.setAlpha(dimValue);
        highScope.setAlpha(dimValue);
    }
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mMidFocus && mShowMidBand) {
        midScope.toFront(false);
        lowScope.setAlpha(dimValue);
        highScope.setAlpha(dimValue);
    }
    
    /* Update channels orientation and appearance per focus */
    // ========================================================
    if (mHighFocus && mShowHighBand) {
        highScope.toFront(false);
        lowScope.setAlpha(dimValue);
        midScope.setAlpha(dimValue);
    }

}

/* Paint Graphics */
// ========================================================
void Oscilloscope::paint(juce::Graphics& g)
{
	using namespace juce;

    /* Paint Background */
    // ========================================================
    g.setColour(juce::Colours::black);
    g.fillAll();

}

/* Paint Graphics over Components */
// ========================================================
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	auto bounds = getLocalBounds().toFloat();

	/* Draw window border */
	paintBorder(g, ColorScheme::WindowBorders::getWindowBorderColor(), bounds);

}

/* A mouse-up gesture indicates the cursor is done being dragged */
// ========================================================
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	//cursorDrag = false;

	//scopeCursorParam->setValueNotifyingHost(mCursorPosition);   // Get Rid?

	float maxWidth = (float)sliderScroll.getMaxWidth();

    // Store the current pan & zoom settings in memory
	float p1 = (float)(sliderScroll.getPoint1()) / (float)maxWidth;
	float p2 = (float)(sliderScroll.getPoint2()) / (float)maxWidth;

    // Store the current pan & zoom settings in memory
	scopePoint1Param->setValueNotifyingHost(p1);
	scopePoint2Param->setValueNotifyingHost(p2);
}

/* Move the cursor while it is being dragged */
// ========================================================
void Oscilloscope::mouseDrag(const juce::MouseEvent& event)
{
	/* Force the display to continually update as the pan and zoom changes */
	if (sliderScroll.isMouseOverOrDragging())
		updateLfoDisplay = true;
}