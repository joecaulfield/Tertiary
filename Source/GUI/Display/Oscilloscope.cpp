/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 30 Dec 2021 11:38:53am
    Author:  Joe Caulfield

	// Class containing the oscilloscope which shows 
	// the individual LFO waveforms for viewing
  ==============================================================================
*/

#include "Oscilloscope.h"

// Constructor
Oscilloscope::Oscilloscope(TertiaryAudioProcessor& p, GlobalControls& gc)
	: audioProcessor(p), 
	lowLFO(p.lowLFO), midLFO(p.midLFO), highLFO(p.highLFO),
	globalControls(gc)
{

	using namespace Params;             // Create a Local Reference to Parameter Mapping
	const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

	auto boolHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)               // Bool Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));      // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	boolHelper(showLowBand, Names::Show_Low_Scope);
	boolHelper(showMidBand, Names::Show_Mid_Scope);
	boolHelper(showHighBand, Names::Show_High_Scope);
	boolHelper(stackBands, Names::Stack_Bands_Scope);

	auto floatHelper = [&apvts = this->audioProcessor.apvts, &params](auto& param, const auto& paramName)              // Float Helper --> Part 8 Param Namespace
	{
		param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));     // Attach Value to Parameter
		jassert(param != nullptr);                                                                      // Error Checking
	};

	floatHelper(scopeScrollParam, Names::Cursor_Position);

	dragX = scopeScrollParam->get();

	sliderScroll.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
	sliderScroll.setLookAndFeel(&scrollLookAndFeel);
	addAndMakeVisible(sliderScroll);

	addMouseListener(this, true);

	sampleRate = audioProcessor.getSampleRate();
	makeAttachments();

	updatePreferences();

	startTimerHz(30);
}

// Destructor
Oscilloscope::~Oscilloscope()
{
	sliderScroll.setLookAndFeel(nullptr);
}

// Called on component resize
void Oscilloscope::resized()
{
	drawToggles();

	auto bounds = getLocalBounds();
	bounds.reduce(4, 0);
	bounds.removeFromBottom(5);

	sliderScroll.setSize(bounds.getWidth(), 20);
	sliderScroll.setTopLeftPosition(bounds.getX(), bounds.getBottom() - sliderScroll.getHeight());
}

// Graphics class
void Oscilloscope::paint(juce::Graphics& g)
{
	using namespace juce;

	using namespace AllColors::OscilloscopeColors;

	auto bounds = getLocalBounds().toFloat();

	// Set Gradient Fill
	g.setGradientFill(BACKGROUND_GRADIENT(bounds));
	g.fillAll();

	// DRAW AXIS IF NO BANDS SHOWN
	if (!mShowLowBand && !mShowMidBand && !mShowHighBand)
		drawAxis(g, scopeRegion, GRID_LINES_NONE);	// Grid when nothing shown

	// DRAW GRIDS IF LOW-BAND SHOWN
	if (mShowLowBand)
		drawAxis(g, lowRegion, GRID_LINES_LOW);		// Low Grid

	// DRAW GRIDS IF MID-BAND SHOWN
	if (mShowMidBand)
		drawAxis(g, midRegion, GRID_LINES_MID);		// Mid Grid

	// DRAW GRIDS IF HIGH-BAND SHOWN
	if (mShowHighBand)
		drawAxis(g, highRegion, GRID_LINES_HIGH);	// High Grid

	// DRAW & FILL LOW REGION
	g.setGradientFill(mLowBypass ? WAVEFORM_BYPASS_GRADIENT(lowRegion.toFloat()) : WAVEFORM_LOW_GRADIENT(lowRegion.toFloat()));
	g.setOpacity(mLowFocus ? 0.95 : 0.85);
	g.fillPath(lowPathFill);

	g.setColour(mLowBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_LOW());
	g.setOpacity(mLowFocus ? 1.f : 0.85f);
	g.strokePath(lowPath, mLowFocus?	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW & FILL MID REGION
	g.setGradientFill(mMidBypass ? WAVEFORM_BYPASS_GRADIENT(midRegion.toFloat()) : WAVEFORM_MID_GRADIENT(midRegion.toFloat()));
	g.setOpacity(mMidFocus ? 0.95 : 0.85);
	g.fillPath(midPathFill);

	fadeInOutComponents(g);

	paintBorder(g, juce::Colours::purple, bounds);
}

// Graphics class which covers components
void Oscilloscope::paintOverChildren(juce::Graphics& g)
{
	using namespace juce;

	using namespace AllColors::OscilloscopeColors;

	g.setColour(mMidBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_MID());
	g.setOpacity(mMidFocus ? 1.f : 0.85f);
	g.strokePath(midPath, mMidFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW & FILL HIGH REGION
	g.setGradientFill(mHighBypass ? WAVEFORM_BYPASS_GRADIENT(highRegion.toFloat()) : WAVEFORM_HIGH_GRADIENT(highRegion.toFloat()));
	g.setOpacity(mHighFocus ? 0.95 : 0.85);
	g.fillPath(highPathFill);

	g.setColour(mHighBypass ? REGION_BORDER_COLOR_BYPASS() : REGION_BORDER_COLOR_HIGH());
	g.setOpacity(mHighFocus ? 1.f : 0.85f);
	g.strokePath(highPath, mHighFocus? 	juce::PathStrokeType(3.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded) : 
										juce::PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));

	// DRAW REGION OUTLINES
	g.setColour(BORDER_COLOR());

	if (!mShowLowBand && !mShowMidBand && !mShowHighBand)
		g.drawRoundedRectangle(scopeRegion.toFloat(), 5.f, 1.5f);

	if (mShowLowBand)
		g.drawRoundedRectangle(lowRegion.toFloat(),  5.f, 1.5f);

	if (mShowMidBand)
		g.drawRoundedRectangle(midRegion.toFloat(),  5.f, 1.5f);

	if (mShowHighBand)
		g.drawRoundedRectangle(highRegion.toFloat(), 5.f, 1.5f);

	drawAndFadeCursor(g, getLocalBounds());
}

// Draw the gridlines within the corresponding region
void Oscilloscope::drawAxis(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::Colour color)
{
	using namespace AllColors::OscilloscopeColors;

	// DRAW VERTICAL LINES =============================
	juce::Line<float> verticalAxis;
	auto numLines = 2;
	g.setColour(color);
	g.setOpacity(0.5f);

	for (int i = 1; i <= numLines; i++)
	{
		verticalAxis.setStart(bounds.getX() + bounds.getWidth() * (float)i/(numLines+1) , bounds.getY());
		verticalAxis.setEnd(bounds.getX() + bounds.getWidth() * (float) i/(numLines + 1), bounds.getY() + bounds.getHeight());
		g.drawLine(verticalAxis, 1.f);
	}

	// DRAW HORIZONTAL LINES =============================
	g.setColour(juce::Colours::lightgrey);

	for (int i = 1; i < numDepthLines; i++)
	{
		auto y = bounds.getY() + i * (bounds.getHeight() / numDepthLines);
		g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
	}
}

// Draw the cursor.  Fade based on mouse-focus.
void Oscilloscope::drawAndFadeCursor(juce::Graphics& g, juce::Rectangle<int> bounds)
{
	using namespace AllColors::OscilloscopeColors;

	// FADE FUNCTIONS FOR MOUSE-ENTER -> TOGGLES AND SLIDERS
	if (fadeInCursor) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlphaCursor < fadeMaxCursor)
			fadeAlphaCursor += fadeStepUpCursor;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlphaCursor > fadeMinCursor)
			fadeAlphaCursor -= fadeStepDownCursor;
	}

	if (fadeAlphaCursor > fadeMaxCursor)
		fadeAlphaCursor = fadeMaxCursor;

	if (fadeAlphaCursor < fadeMinCursor)
		fadeAlphaCursor = fadeMinCursor;

	cursor.setStart(bounds.getX() + dragX*getWidth(), scopeRegion.getY()+1);
	cursor.setEnd(bounds.getX() + dragX * getWidth(), scopeRegion.getBottom()-1);

	g.setColour(CURSOR_LINE);
	g.setOpacity(fadeAlphaCursor);
	g.drawLine(cursor, 3.f);
}

// Methods to call on a timed-basis
void Oscilloscope::timerCallback()
{
	checkMousePosition();
	updateRegions();
	updatePreferences();
	checkFocus();
	repaint();
}

// Update the scope-regions based on the configured viewing scheme
void Oscilloscope::updateRegions()
{
	scopeRegion = getLocalBounds();
	scopeRegion.reduce(7, 6);
	scopeRegion.removeFromBottom(15);

	lowRegion = scopeRegion;
	midRegion = scopeRegion;
	highRegion = scopeRegion;

	if (mStackAllBands)
		drawStackedScope();

	drawLowLFO(lowRegion);
	drawMidLFO(midRegion);
	drawHighLFO(highRegion);
}

// Draw the scope boundaries based on the configured viewing scheme
void Oscilloscope::drawStackedScope()
{
	int count = 1;

	// Convert Show-Choices to Binary Counter
	int x{ 0 }, y{ 0 }, z{ 0 };

	if (mShowLowBand) x = 1;
	if (mShowMidBand) y = 1;
	if (mShowHighBand) z = 1;
	int sum = x*100 + y*10 + z;

	switch (sum)	// Determines the total number of bands to be displayed
	{
		case 0: count = 1; numDepthLines = 10;  break;
		case 1: count = 1; numDepthLines = 10;  break;
		case 10: count = 1; numDepthLines = 10; break;
		case 11: count = 2; numDepthLines = 6; break;
		case 100: count = 1; numDepthLines = 10; break;
		case 101: count = 2; numDepthLines = 6; break;
		case 110: count = 2; numDepthLines = 6; break;
		case 111: count = 3; numDepthLines = 4; break;
	}

	auto width = scopeRegion.getWidth();
	auto height = scopeRegion.getHeight() / count;

	lowRegion.setBounds(scopeRegion.getX(), scopeRegion.getY(), width, height * x);
	midRegion.setBounds(scopeRegion.getX(), scopeRegion.getY() + lowRegion.getHeight(), width, height * y);
	highRegion.setBounds(scopeRegion.getX(), scopeRegion.getY() + lowRegion.getHeight() + midRegion.getHeight(), width, height * z);

	scopeRegion.reduce(0, 2);
	lowRegion.reduce(0, 2);
	midRegion.reduce(0, 2);
	highRegion.reduce(0, 2);
}

// Draw the Low LFO within the Low Region
void Oscilloscope::drawLowLFO(juce::Rectangle<int> bounds)
{
	using namespace juce;

	float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

	bounds.reduce(0, 3);

	lowPath.clear();
	lowPathFill.clear();

	if (mShowLowBand)
	{
		for (int i = 0; i <= bounds.getWidth(); i++)
		{
			auto increment = lowLFO.waveTableDisplay.size() / bounds.getWidth();
			float y = midY + (float)lowLFO.waveTableDisplay[i * increment] * (float)bounds.getHeight();

			if (i == 0)
			{
				lowPath.startNewSubPath(bounds.getX() + i, y);
				lowPathFill.startNewSubPath(bounds.getX() + i, midY);
			}
			else
			{
				lowPath.lineTo(bounds.getX() + i, y);
				lowPathFill.lineTo(bounds.getX() + i, y);
			}	
		}
		lowPathFill.lineTo(bounds.getRight(), midY);
	}
}

// Draw the Mid LFO within the Mid Region
void Oscilloscope::drawMidLFO(juce::Rectangle<int> bounds)
{
	using namespace juce;

	float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

	bounds.reduce(0, 3);

	midPath.clear();
	midPathFill.clear();

	if (mShowMidBand)
	{
		for (int i = 0; i <= bounds.getWidth(); i++)
		{
			auto increment = midLFO.waveTableDisplay.size() / bounds.getWidth();
			float y = midY + (float)midLFO.waveTableDisplay[i * increment] * (float)bounds.getHeight();

			if (i == 0)
			{
				midPath.startNewSubPath(bounds.getX(), y);
				midPathFill.startNewSubPath(bounds.getX(), midY);
			}
			else
			{
				midPath.lineTo(bounds.getX() + i, y);
				midPathFill.lineTo(bounds.getX() + i, y);
			}
		}
		midPathFill.lineTo(bounds.getRight(), midY);
	}
}

// Draw the High LFO within the High Region
void Oscilloscope::drawHighLFO(juce::Rectangle<int> bounds)
{
	using namespace juce;

	float midY = ((float)bounds.getY() + bounds.getHeight() / 2.f);

	bounds.reduce(0, 3);

	highPath.clear();
	highPathFill.clear();

	if (mShowHighBand)
	{
		for (int i = 0; i <= bounds.getWidth(); i++)
		{
			auto increment = highLFO.waveTableDisplay.size() / bounds.getWidth();
			float y = midY + (float)highLFO.waveTableDisplay[i * increment] * (float)bounds.getHeight();

			if (i == 0)
			{
				highPath.startNewSubPath(bounds.getX() + i, y);
				highPathFill.startNewSubPath(bounds.getX() + i, midY);
			}
			else
			{
				highPath.lineTo(bounds.getX() + i, y);
				highPathFill.lineTo(bounds.getX() + i, y);
			}
		}
		highPathFill.lineTo(bounds.getRight(), midY);
	}
}

// Check for external hover focus.  Refactor for encapsulation.
void Oscilloscope::checkFocus()
{
	mLowFocus = (	globalControls.mTimingControls.timingBarLow.hasFocus || 
					globalControls.mGainControls.gainBarLow.hasFocus ||
					globalControls.mWaveControls.waveBarLow.hasFocus );

	mMidFocus = (	globalControls.mTimingControls.timingBarMid.hasFocus || 
					globalControls.mGainControls.gainBarMid.hasFocus ||
					globalControls.mWaveControls.waveBarMid.hasFocus );

	mHighFocus = (	globalControls.mTimingControls.timingBarHigh.hasFocus || 
					globalControls.mGainControls.gainBarHigh.hasFocus ||
					globalControls.mWaveControls.waveBarHigh.hasFocus );
}

// Update parameter values
void Oscilloscope::updatePreferences()
{
	using namespace Params;
	const auto& params = GetParams();

	mShowLowBand = showLowBand->get();
	mShowMidBand = showMidBand->get();
	mShowHighBand = showHighBand->get();
	mStackAllBands = stackBands->get();

	mLowBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Low_Band));
	mMidBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_Mid_Band));
	mHighBypass = *audioProcessor.apvts.getRawParameterValue(params.at(Names::Bypass_High_Band));
}

// Make component->parameter attachments
void Oscilloscope::makeAttachments()
{
	using namespace Params;
	const auto& params = GetParams();

	showLowAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Low_Scope),
							toggleShowLow);

	showMidAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_Mid_Scope),
							toggleShowMid);

	showHighAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Show_High_Scope),
							toggleShowHigh);

	stackBandsAttachment =	std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
							audioProcessor.apvts,
							params.at(Names::Stack_Bands_Scope),
							toggleStackBands);

	scrollAttachment =		std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
							audioProcessor.apvts,
							params.at(Names::Scope_Scroll),
							sliderScroll);
}

// Initialize and place toggle buttons.  Refactor later.
void Oscilloscope::drawToggles()
{
	using namespace juce;

	buttonBounds.setBounds(scopeRegion.getX()+5, scopeRegion.getY()+5, 65, 70);
	buttonBounds.reduce(3, 3);

	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.flexWrap = FlexBox::Wrap::noWrap;

	auto spacer = FlexItem().withHeight(2.5);

	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowLow).withHeight(20));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowMid).withHeight(20));
	flexBox.items.add(spacer);
	flexBox.items.add(FlexItem(toggleShowHigh).withHeight(20));
	flexBox.items.add(spacer);

	flexBox.performLayout(buttonBounds);
	
	toggleShowLow.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::tickColourId, juce::Colours::black);

	toggleShowLow.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowMid.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleShowHigh.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);
	toggleStackBands.setColour(	ToggleButton::ColourIds::textColourId, juce::Colours::black);

	toggleShowLow.setButtonText("Low");
	toggleShowMid.setButtonText("Mid");
	toggleShowHigh.setButtonText("High");
	toggleStackBands.setButtonText("Stack");

	addAndMakeVisible(toggleShowLow);
	addAndMakeVisible(toggleShowMid);
	addAndMakeVisible(toggleShowHigh);
}

// Function to fade in view-menu.  Should optimize view-menu to its own class.
void Oscilloscope::fadeInOutComponents(juce::Graphics& g)
{
	using namespace juce;
	using namespace AllColors::OscilloscopeColors;

	// FADE FUNCTIONS FOR MOUSE-ENTER -> TOGGLES AND SLIDERS
	if (fadeIn && !fadeInCursor) // If mouse entered... fade Toggles Alpha up
	{
		if (fadeAlpha < fadeMax)
			fadeAlpha += fadeStepUp;

		if (fadeAlpha > fadeMax)
			fadeAlpha = fadeMax;
	}
	else // If mouse exit... fade Toggles Alpha down
	{
		if (fadeAlpha > fadeMin)
			fadeAlpha -= fadeStepDown;

		if (fadeAlpha < fadeMin)
			fadeAlpha = fadeMin;
	}

	g.setColour(BUTTON_BACKGROUND_FILL);
	g.setOpacity(fadeAlpha*0.55f);
	g.fillRoundedRectangle(buttonBounds, 5.f);

	toggleShowLow.setAlpha(fadeAlpha);
	toggleShowMid.setAlpha(fadeAlpha);
	toggleShowHigh.setAlpha(fadeAlpha);
	toggleStackBands.setAlpha(fadeAlpha);
}

// On mouse-enter, fade in view-menu unless dragging cursor
void Oscilloscope::mouseEnter(const juce::MouseEvent& event)
{
	if (!sliderScroll.isMouseOverOrDragging())
		fadeIn = true;
	else
		fadeIn = false;
}

// On mouse exit, fade out view-menu
void Oscilloscope::mouseExit(const juce::MouseEvent& event)
{
	fadeIn = false;
}

// If mouse-down within range of cursor, begin moving cursor
void Oscilloscope::mouseDown(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10 && !sliderScroll.isMouseOverOrDragging())
		cursorDrag = true;
}

// A mouse-up gesture indicates the cursor is done being dragged
void Oscilloscope::mouseUp(const juce::MouseEvent& event)
{
	cursorDrag = false;
	scopeScrollParam->setValueNotifyingHost(dragX);
}

// Move the cursor when it is dragged
void Oscilloscope::mouseDrag(const juce::MouseEvent& event)
{
	auto margin = 7;

	if (cursorDrag)
	{
		dragX = event.getPosition().getX() / (float)getWidth();

		if (event.getPosition().getX() < margin)
			dragX = margin / (float)getWidth();

		if (event.getPosition().getX() > (float)getWidth() - margin)
			dragX = ((float)getWidth() - margin) / getWidth();
	}
}

// Fade-In the cursor when mouse is within 10 pixels of it
void Oscilloscope::mouseMove(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10 && !sliderScroll.isMouseOverOrDragging())
	{
		fadeInCursor = true;
	}
	else fadeInCursor = false;
}

// Reset scroll-view position on double-click
void Oscilloscope::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (abs(event.getMouseDownPosition().getX() - cursor.getStartX()) < 10)
	{
		dragX = 0.5f;
		cursorDrag = false;
		scopeScrollParam->setValueNotifyingHost(dragX);
	}
}

// Fade out the view-menu if mouse has left focus
void Oscilloscope::checkMousePosition()
{
	if (!isMouseOverOrDragging(true))
	{
		fadeInCursor = false;
		fadeIn = false;
	}
}