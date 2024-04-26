#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WLDebugger.h"

//==============================================================================
TertiaryAudioProcessorEditor::TertiaryAudioProcessorEditor (TertiaryAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    if (setDebug)
        WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");


	/* Provides GPU acceleration */
	//openGLContext.attachTo(*getTopLevelComponent());

    /* Container class for top banner */
    addAndMakeVisible(topBanner);
    topBanner.addMouseListener(this, true);
    
	/* Container class for all parameter controls */
	addAndMakeVisible(globalControls);

    /* Time-Domain Display */
    addAndMakeVisible(wrapperOscilloscope);
    
	/* Frequency-Domain & Crossover Display */
    addAndMakeVisible(wrapperFrequency);


	/* Sets window size */
    float scale = 1.f;
    setSize(750*scale, 515*scale);

    
    // Add Action Listener
    globalControls.getLowControl().addActionListener    ( &wrapperOscilloscope.getOscilloscopeLow() );
    globalControls.getMidControl().addActionListener    ( &wrapperOscilloscope.getOscilloscopeMid() );
    globalControls.getHighControl().addActionListener   ( &wrapperOscilloscope.getOscilloscopeHigh() );

    globalControls.getLowControl().addActionListener    ( &wrapperFrequency.getFrequencyResponse() );
    globalControls.getMidControl().addActionListener    ( &wrapperFrequency.getFrequencyResponse() );
    globalControls.getHighControl().addActionListener   ( &wrapperFrequency.getFrequencyResponse() );
    
    globalControls.addActionListener(&wrapperOscilloscope.getOscilloscopeLow());
    globalControls.addActionListener(&wrapperOscilloscope.getOscilloscopeMid());
    globalControls.addActionListener(&wrapperOscilloscope.getOscilloscopeHigh());

    globalControls.addActionListener(&wrapperFrequency.getFrequencyResponse());
}

//==============================================================================
TertiaryAudioProcessorEditor::~TertiaryAudioProcessorEditor()
{
    WLDebugger::getInstance().closeWindow();
    juce::Logger::setCurrentLogger(nullptr);
}

//==============================================================================
void TertiaryAudioProcessorEditor::paint(juce::Graphics& g)
{
    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

	/* Set the Gradient */
	g.setGradientFill(AllColors::PluginEditorColors::BACKGROUND_GRADIENT(getLocalBounds().toFloat()));
    g.fillAll();
}

//==============================================================================
void TertiaryAudioProcessorEditor::resized()
{
    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

	using namespace juce;

    buildFlexboxLayout();
}

/* Builds the UI layout */
//==============================================================================
void TertiaryAudioProcessorEditor::buildFlexboxLayout()
{
    WLDebugger::getInstance().printMessage(mNameSpace, __func__, "");

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

/* Temporary Double-Click Callback*/
void TertiaryAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    openDebug = !openDebug;

    if (openDebug)
        WLDebugger::getInstance().openWindow();
    else
        WLDebugger::getInstance().closeWindow();
}