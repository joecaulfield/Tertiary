
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TertiaryAudioProcessor::TertiaryAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    attachParametersToLayout();
    addParameterListeners();
    
	/* Initialize the LFO */
	lowLFO.initializeLFO(getSampleRate());
	midLFO.initializeLFO(getSampleRate());
	highLFO.initializeLFO(getSampleRate());

    lowLFO.setLfoID(0);
    midLFO.setLfoID(1);
    highLFO.setLfoID(2);
    
    // Set Crossover Types
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

//==============================================================================
TertiaryAudioProcessor::~TertiaryAudioProcessor()
{
}

/* Attaches all parameters to the ParameterLayout */
//==============================================================================
void TertiaryAudioProcessor::attachParametersToLayout()
{
    /* Local reference to parameter mapping */
    using namespace Params;
    const auto& params = GetParams();
    
    /* Float Helper: "Attaches" float to APVTS Parameter */
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    /* Choice Helper: "Attaches" choice to APVTS Parameter */
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    /* Bool Helper: "Attaches" bool to APVTS Parameter */
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    /* Initialize Input & Output Gain */
    floatHelper(inputGainParam, Names::Input_Gain);
    floatHelper(outputGainParam, Names::Output_Gain);
    
    /* Initialize Crossover Frequencies */
    floatHelper(lowMidCrossover,    Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover,   Names::Mid_High_Crossover_Freq);

    /* Initialize Low Band Parameters */
    boolHelper(lowBandTrem.bypassParam,     Names::Bypass_Low_Band);
    boolHelper(lowBandTrem.muteParam,       Names::Mute_Low_Band);
    boolHelper(lowBandTrem.soloParam,       Names::Solo_Low_Band);
    floatHelper(lowBandTrem.bandGainParam,  Names::Gain_Low_Band);
    floatHelper(lowLFO.symmetryParam,       Names::Symmetry_Low_LFO);
    floatHelper(lowLFO.depthParam,          Names::Depth_Low_LFO);
    choiceHelper(lowLFO.waveshapeParam,     Names::Wave_Low_LFO);
    boolHelper(lowLFO.invertParam,          Names::Invert_Low_LFO);
    floatHelper(lowLFO.relativePhaseParam,  Names::Relative_Phase_Low_LFO);
    floatHelper(lowLFO.rateParam,           Names::Rate_Low_LFO);
    boolHelper(lowLFO.syncToHostParam,      Names::Sync_Low_LFO);
    choiceHelper(lowLFO.multiplierParam,    Names::Multiplier_Low_LFO);

    // Initialize Mid Band Parameters
    boolHelper(midBandTrem.bypassParam,     Names::Bypass_Mid_Band);
    boolHelper(midBandTrem.muteParam,       Names::Mute_Mid_Band);
    boolHelper(midBandTrem.soloParam,       Names::Solo_Mid_Band);
    floatHelper(midBandTrem.bandGainParam,  Names::Gain_Mid_Band);

    floatHelper(midLFO.symmetryParam,       Names::Symmetry_Mid_LFO);
    floatHelper(midLFO.depthParam,          Names::Depth_Mid_LFO);
    choiceHelper(midLFO.waveshapeParam,     Names::Wave_Mid_LFO);
    boolHelper(midLFO.invertParam,          Names::Invert_Mid_LFO);
    floatHelper(midLFO.relativePhaseParam,  Names::Relative_Phase_Mid_LFO);
    floatHelper(midLFO.rateParam,           Names::Rate_Mid_LFO);
    boolHelper(midLFO.syncToHostParam,      Names::Sync_Mid_LFO);
    choiceHelper(midLFO.multiplierParam,    Names::Multiplier_Mid_LFO);

    // Initialize High Band Parameters
    boolHelper(highBandTrem.bypassParam,    Names::Bypass_High_Band);
    boolHelper(highBandTrem.muteParam,      Names::Mute_High_Band);
    boolHelper(highBandTrem.soloParam,      Names::Solo_High_Band);
    floatHelper(highBandTrem.bandGainParam, Names::Gain_High_Band);

    floatHelper(highLFO.symmetryParam,      Names::Symmetry_High_LFO);
    floatHelper(highLFO.depthParam,         Names::Depth_High_LFO);
    choiceHelper(highLFO.waveshapeParam,    Names::Wave_High_LFO);
    boolHelper(highLFO.invertParam,         Names::Invert_High_LFO);
    floatHelper(highLFO.relativePhaseParam, Names::Relative_Phase_High_LFO);
    floatHelper(highLFO.rateParam,          Names::Rate_High_LFO);
    boolHelper(highLFO.syncToHostParam,     Names::Sync_High_LFO);
    choiceHelper(highLFO.multiplierParam,   Names::Multiplier_High_LFO);
}

/* Registers AudioProcessor as a listener to all parameters */
//==============================================================================
void TertiaryAudioProcessor::addParameterListeners()
{
    /* Local reference to parameter mapping */
    using namespace Params;
    const auto& params = GetParams();
    
    apvts.addParameterListener(params.at(Names::Input_Gain), this);
    apvts.addParameterListener(params.at(Names::Output_Gain), this);
    apvts.addParameterListener(params.at(Names::Low_Mid_Crossover_Freq), this);
    apvts.addParameterListener(params.at(Names::Mid_High_Crossover_Freq), this);
    apvts.addParameterListener(params.at(Names::Bypass_Low_Band), this);
    apvts.addParameterListener(params.at(Names::Bypass_Mid_Band), this);
    apvts.addParameterListener(params.at(Names::Bypass_High_Band), this);
    apvts.addParameterListener(params.at(Names::Mute_Low_Band), this);
    apvts.addParameterListener(params.at(Names::Mute_Mid_Band), this);
    apvts.addParameterListener(params.at(Names::Mute_High_Band), this);
    apvts.addParameterListener(params.at(Names::Solo_Low_Band), this);
    apvts.addParameterListener(params.at(Names::Solo_Mid_Band), this);
    apvts.addParameterListener(params.at(Names::Solo_High_Band), this);
    apvts.addParameterListener(params.at(Names::Gain_Low_Band), this);
    apvts.addParameterListener(params.at(Names::Gain_Mid_Band), this);
    apvts.addParameterListener(params.at(Names::Gain_High_Band), this);
    apvts.addParameterListener(params.at(Names::Wave_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Depth_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Symmetry_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Invert_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Sync_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Rate_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Multiplier_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Relative_Phase_Low_LFO), this);
    apvts.addParameterListener(params.at(Names::Wave_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Depth_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Symmetry_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Invert_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Sync_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Rate_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Multiplier_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Relative_Phase_Mid_LFO), this);
    apvts.addParameterListener(params.at(Names::Wave_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Depth_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Symmetry_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Invert_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Sync_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Rate_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Multiplier_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Relative_Phase_High_LFO), this);
    apvts.addParameterListener(params.at(Names::Show_Low_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_Mid_Scope), this);
    apvts.addParameterListener(params.at(Names::Show_High_Scope), this);
    apvts.addParameterListener(params.at(Names::Stack_Bands_Scope), this);
    apvts.addParameterListener(params.at(Names::Cursor_Position), this);
    apvts.addParameterListener(params.at(Names::Scope_Point1), this);
    apvts.addParameterListener(params.at(Names::Scope_Point2), this);
}

/* Recalls previously saved settings */
//==============================================================================
void TertiaryAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // USED TO SAVE PARAMETERS TO MEMORY
    //
    // Declare a Memory Output Stream variable called 'mos'
    juce::MemoryOutputStream mos(   destData,   // Destination
                                    true);      // It is appendable

    apvts.state.writeToStream(mos); // Write mos to the APVTS
}

/* Saves current settings to be recalled at a later date */
//==============================================================================
void TertiaryAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // USED TO LOAD PARAMETERS FROM MEMORY

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);   // Pull the Value Tree from memory

    if (tree.isValid()) // Check if Value Tree is valid
    {
        apvts.replaceState(tree);   // If valid, load from memory and replace current state
    }
}

/* Creates the parameter layout */
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout TertiaryAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;
    
    /* Local reference to parameter mapping */
    using namespace Params;
    const auto& params = GetParams();

    /* Used to hold choice arrays */
    juce::StringArray sa;

    /* Input Gain */
    // ===================================================================================================================
    auto gainRange = NormalisableRange<float> ( -24.f,  // Start
                                                24.f,   // Stop
                                                0.5f,   // Step Size
                                                1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Input_Gain),1},    // Parameter ID
                                                        params.at(Names::Input_Gain),                   // Parameter Name
                                                        gainRange,                                      // Range
                                                        0));                                            // Default Value
    
    /* Crossover Frequencies */
    // ===================================================================================================================

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Low_Mid_Crossover_Freq),1},    // Parameter ID
                                                        params.at(Names::Low_Mid_Crossover_Freq),                   // Parameter Name
                                                        NormalisableRange<float> (20, 20000, 1, 0.3f),              // Range
                                                        500));                                                      // Default Value
                                                        
    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Mid_High_Crossover_Freq),1},   // Parameter ID
                                                        params.at(Names::Mid_High_Crossover_Freq),                  // Parameter Name
                                                        NormalisableRange<float> (20, 20000, 1, 0.5f),                // Range
                                                        1000));                                                     // Default Value

    /* LFO Waveshape */
    // ===================================================================================================================
    sa = {  "Ramp Down",
            "Ramp Up",
            "Square",
            "Triangle",
            "Sine",
            "Hills",
            "Valleys" };

    
    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Wave_Low_LFO),1},  // Parameter ID
                                                        params.at(Names::Wave_Low_LFO),                 // Parameter Name
                                                        sa,                                             // Range
                                                        4));                                            // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Wave_Mid_LFO),1},  // Parameter ID
                                                        params.at(Names::Wave_Mid_LFO),                 // Parameter Name
                                                        sa,                                             // Range
                                                        4));                                            // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Wave_High_LFO),1}, // Parameter ID
                                                        params.at(Names::Wave_High_LFO),                // Parameter Name
                                                        sa,                                             // Range
                                                        4));                                            // INDEX of Default Value

    /* LFO Depth */
    // ===================================================================================================================
    auto depthRange = NormalisableRange<float> (    0,        // Start
                                                    100,    // Stop
                                                    0.5f,   // Step Size
                                                    1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Depth_Low_LFO),1}, // Parameter ID
                                                        params.at(Names::Depth_Low_LFO),                // Parameter Name
                                                        depthRange,                                     // Range
                                                        75));                                           // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Depth_Mid_LFO),1}, // Parameter ID
                                                        params.at(Names::Depth_Mid_LFO),                // Parameter Name
                                                        depthRange,                                     // Range
                                                        75));                                           // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Depth_High_LFO), 1},    // Parameter ID
                                                        params.at(Names::Depth_High_LFO),                   // Parameter Name
                                                        depthRange,                                         // Range
                                                        75));                                               // Default Value

    /* LFO Symmetry */
    // ===================================================================================================================
    auto symmetryRange = NormalisableRange<float> ( 12.5,    // Start
                                                    87.5,    // Stop
                                                    0.5f,    // Step Size
                                                    1.f);    // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Symmetry_Low_LFO), 1}, // Parameter ID, Version Hint
                                                        params.at(Names::Symmetry_Low_LFO),                 // Parameter Name
                                                        symmetryRange,                                      // Range
                                                        50));                                               // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Symmetry_Mid_LFO), 1},  // Parameter ID & Hint
                                                        params.at(Names::Symmetry_Mid_LFO),                 // Parameter Name
                                                        symmetryRange,                                      // Range
                                                        50));                                               // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Symmetry_High_LFO),1}, // Parameter ID & Hint
                                                        params.at(Names::Symmetry_High_LFO),                // Parameter Name
                                                        symmetryRange,                                        // Range
                                                        50));                                               // Default Value

    /* LFO Invert */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Invert_Low_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Invert_Low_LFO),                   // Parameter Name
                                                        false));                                            // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Invert_Mid_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Invert_Mid_LFO),                   // Parameter Name
                                                        false));                                            // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Invert_High_LFO),1},   // Parameter ID & Hint
                                                        params.at(Names::Invert_High_LFO),                  // Parameter Name
                                                        false));                                            // Default Value

    /* LFO Sync-To-Host */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Sync_Low_LFO),1},  // Parameter ID & Hint
                                                        params.at(Names::Sync_Low_LFO),                 // Parameter Name
                                                        true));                                         // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Sync_Mid_LFO), 1}, // Parameter ID & Hint
                                                        params.at(Names::Sync_Mid_LFO),                 // Parameter Name
                                                        true));                                         // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Sync_High_LFO), 1},    // Parameter ID & Hint
                                                        params.at(Names::Sync_High_LFO),                    // Parameter Name
                                                        true));                                             // Default Value

    /* LFO Rate */
    // ===================================================================================================================
    auto rateRange = NormalisableRange<float> ( 0.5f,   // Start
                                                12.5f,  // Stop
                                                0.1f,  // Step Size
                                                1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Rate_Low_LFO),1},  // Parameter ID & Hint
                                                        params.at(Names::Rate_Low_LFO),                 // Parameter Name
                                                        rateRange,                                      // Range
                                                        3.f));                                          // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Rate_Mid_LFO),1},  // Parameter ID & Hint
                                                        params.at(Names::Rate_Mid_LFO),                 // Parameter Name
                                                        rateRange,                                      // Range
                                                        3.f));                                          // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Rate_High_LFO), 1},    // Parameter ID & Hint
                                                        params.at(Names::Rate_High_LFO),                    // Parameter Name
                                                        rateRange,                                          // Range
                                                        3.f));                                              // Default Value

    /* LFO Multiplier */
    // ===================================================================================================================
    auto multChoices = std::vector<double>{0.5, 1.0, 1.5, 2.0, 3.0, 4.0};

    sa.clear();

    sa = {  "1/2 Note",
            "1/4 Note",
            "1/4 Note Triplet",
            "1/8 Note",
            "1/8 Note Triplet",
            "1/16 Note"};

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Multiplier_Low_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Multiplier_Low_LFO),                   // Parameter Name
                                                        sa,                                                     // Range
                                                        1));                                                    // INDEX of Default

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Multiplier_Mid_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Multiplier_Mid_LFO),                   // Parameter Name
                                                        sa,                                                     // Range
                                                        1));                                                    // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::Multiplier_High_LFO),1},   // Parameter ID & Hint
                                                        params.at(Names::Multiplier_High_LFO),                  // Parameter Name
                                                        sa,                                                     // Range
                                                        1));                                                    // INDEX of Default Value

    /* LFO Relative Phase */
    // ===================================================================================================================
    auto relativePhaseRange = NormalisableRange<float> (    -180,   // Start
                                                            180,    // Stop
                                                            1,      // Step Size
                                                            1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Relative_Phase_Low_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Relative_Phase_Low_LFO),                   // Parameter Name
                                                        relativePhaseRange,                                         // Range
                                                        0));                                                        // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Relative_Phase_Mid_LFO),1},    // Parameter ID & Hint
                                                        params.at(Names::Relative_Phase_Mid_LFO),                   // Parameter Name
                                                        relativePhaseRange,                                         // Range
                                                        0));                                                        // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Relative_Phase_High_LFO), 1},  // Parameter ID & Hint
                                                        params.at(Names::Relative_Phase_High_LFO),                  // Parameter Name
                                                        relativePhaseRange,                                         // Range
                                                        0));                                                        // Default Value

    /* Audio Band Bypass */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Bypass_Low_Band), 1},  // Parameter ID & Hint
                                                        params.at(Names::Bypass_Low_Band),                  // Parameter Name
                                                        false));                                            // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Bypass_Mid_Band), 1},  // Parameter ID & Hint
                                                        params.at(Names::Bypass_Mid_Band),                  // Parameter Name
                                                        false));                                            // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Bypass_High_Band),1},  // Parameter ID & Hint
                                                        params.at(Names::Bypass_High_Band),                 // Parameter Name
                                                        false));                                            // Default Value

    /* Audio Band Mute */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Mute_Low_Band), 1},    // Parameter ID & Hint
                                                        params.at(Names::Mute_Low_Band),                    // Parameter Name
                                                        false));                                            // Default Value
                                                                            
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Mute_Mid_Band), 1},    // Parameter ID & Hint
                                                        params.at(Names::Mute_Mid_Band),                    // Parameter Name
                                                        false));                                            // Default Value
                                                                         
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Mute_High_Band), 1},   // Parameter ID & Hint
                                                        params.at(Names::Mute_High_Band),                   // Parameter Name
                                                        false));                                            // Default Value

    /* Audio Band Solo */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Solo_Low_Band),1},     // Parameter ID & Hint
                                                        params.at(Names::Solo_Low_Band),                    // Parameter Name
                                                        false));                                            // Default Value
                                                                         
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Solo_Mid_Band), 1},    // Parameter ID & Hint
                                                        params.at(Names::Solo_Mid_Band),                    // Parameter Name
                                                        false));                                            // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Solo_High_Band), 1},   // Parameter ID & Hint
                                                        params.at(Names::Solo_High_Band),                   // Parameter Name
                                                        false));                                            // Default Value

    /* Output Gain */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Gain_Low_Band),1},     // Parameter ID & Hint
                                                        params.at(Names::Gain_Low_Band),                    // Parameter Name
                                                        gainRange,                                          // Range
                                                        0));                                                // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Gain_Mid_Band), 1},    // Parameter ID & Hint
                                                        params.at(Names::Gain_Mid_Band),                    // Parameter Name
                                                        gainRange,                                          // Range
                                                        0));                                                // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Gain_High_Band), 1},   // Parameter ID & Hint
                                                        params.at(Names::Gain_High_Band),                   // Parameter Name
                                                        gainRange,                                          // Range
                                                        0));                                                // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Output_Gain), 1},  // Parameter ID & Hint
                                                        params.at(Names::Output_Gain),                  // Parameter Name
                                                        gainRange,                                      // Range
                                                        0));                                            // Default Value

    /* Oscilloscope Display Preferences */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_Low_Scope), 1},   // Parameter ID & Hint
                                                        params.at(Names::Show_Low_Scope),                   // Parameter Name
                                                        true));

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_Mid_Scope),1},    // Parameter ID & Hint
                                                        params.at(Names::Show_Mid_Scope),                   // Parameter Name
                                                        true));

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_High_Scope), 1},  // Parameter ID & Hint
                                                        params.at(Names::Show_High_Scope),                  // Parameter Name
                                                        true));

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Stack_Bands_Scope),1}, // Parameter ID & Hint
                                                        params.at(Names::Stack_Bands_Scope),                // Parameter Name
                                                        true));

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_Cursor_Scope),1}, // Parameter ID & Hint
                                                        params.at(Names::Show_Cursor_Scope),                // Parameter Name
                                                        true));

    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_Playhead_Scope),1},   // Parameter ID & Hint
                                                        params.at(Names::Show_Playhead_Scope),                  // Parameter Name
                                                        true));

    auto cursorRange = NormalisableRange<float> (0.f,   // Start
                                                1.f,    // Stop
                                                0.0001, // Step Size
                                                1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Cursor_Position),1},   // Parameter ID & Hint
                                                        params.at(Names::Cursor_Position),                  // Parameter Name
                                                        cursorRange,                                        // Range
                                                        0.5f));                                             // Default Value

    auto pointRange = NormalisableRange<float> ( 0,     // Start
                                                100.f,  // Stop
                                                1.f,    // Step Size
                                                1.f);   // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Scope_Point1),1},  // Parameter ID & Hint
                                                        params.at(Names::Scope_Point1),                 // Parameter Name
                                                        pointRange,                                     // Range
                                                        25.f));                                            // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   ParameterID{params.at(Names::Scope_Point2),1},  // Parameter ID & Hint
                                                        params.at(Names::Scope_Point2),                    // Parameter Name
                                                        pointRange,                                        // Range
                                                        75.f));                                            // Default Value

    /* FFT Display Preferences */
    // ===================================================================================================================
    layout.add(std::make_unique<AudioParameterBool>(    ParameterID{params.at(Names::Show_FFT), 1}, // Parameter ID & Hint
                                                        params.at(Names::Show_FFT),                 // Parameter Name
                                                        true));                                     // Default Value

    sa.clear();
    sa = { "Input", "Output" };

    layout.add(std::make_unique<AudioParameterChoice>(  ParameterID{params.at(Names::FFT_Pickoff), 1},  // Parameter ID & Hint
                                                        params.at(Names::FFT_Pickoff),                  // Parameter Name
                                                        sa,
                                                        1));

    return layout;
}

/* Housekeeping PRIOR TO audio processing */
//==============================================================================
void TertiaryAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    /* Prepare DSP ProcessSpec */
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    /* Pass spec information into Input & Output Gain */
    inputGain.prepare(spec);
    outputGain.prepare(spec);

    /* Pass spec information into filters */
    LP1.prepare(spec);
    HP1.prepare(spec);
    AP2.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);

    /* Set size for filter buffers prior to audio processing */
    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }

    /* Used to reduce audio artifacts by smoothing the gain changes */
    inputGain.setRampDurationSeconds(0.05);
    outputGain.setRampDurationSeconds(0.05);
    
    /* RMS Level Smoothing for Meters */
    rmsLevelInputLeft.reset(sampleRate, 0.5);   rmsLevelInputLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelInputRight.reset(sampleRate, 0.5);  rmsLevelInputRight.setCurrentAndTargetValue(-100.f);
    rmsLevelOutputLeft.reset(sampleRate, 0.5);  rmsLevelOutputLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelOutputRight.reset(sampleRate, 0.5); rmsLevelOutputRight.setCurrentAndTargetValue(-100.f);

}

/* Called upon any change in multiplier value.  Forces all LFO read-back positions to
   be synchronized with each other (when no host BPM), or forces all LFO read-back positions
   to be synchronized with the host grid */
//==============================================================================
void TertiaryAudioProcessor::forceSynchronization(LFO& lfo)
{
    if (/*hostInfo.isPlaying && */ lfo.isSyncedToHost())
    {
        /* When host BPM present, force LFO position to sync to host */
        float div = 1.f;
        div = 1.f / lfo.getWaveMultiplier();
        float playPositionScaled = fmod(playPosition, div);
        float newPhase = playPositionScaled * lfo.getWaveMultiplier() * lfo.getWaveTableSize();
        lfo.setPosition( fmod(newPhase, lfo.getWaveTableSize()) );
    }
    else if (!hostInfo.isPlaying && lfo.isSyncedToHost())
    {
        /* When no host BPM, force LFO position to sync to each other */
        if (lowLFO.isSyncedToHost())
            lfo.setPosition( lowLFO.getPosition() );
        else if (midLFO.isSyncedToHost())
            lfo.setPosition( midLFO.getPosition() );
        else if (highLFO.isSyncedToHost())
            lfo.setPosition( highLFO.getPosition() );
    }

}

//==============================================================================
void TertiaryAudioProcessor::setFftPickoffPoint(int point)
{
    switch (point)
    {
    case 0: {fftPickoffPointIsInput = true;
        break; }

    case 1: {fftPickoffPointIsInput = false;
        break; }
    }
}

/* Returns value of RMS level to the Input Meters upon request */
//==============================================================================
float TertiaryAudioProcessor::getRmsValue(const int pickOffPoint, const int channel) const {
    jassert(channel == 0 || channel == 1);

    if (pickOffPoint == 0)
    {
        if (channel == 0)
            return rmsLevelInputLeft.getCurrentValue();
        if (channel == 1)
            return rmsLevelInputRight.getCurrentValue();
        return 0;
    }

    if (pickOffPoint == 1)
    {
        if (channel == 0)
            return rmsLevelOutputLeft.getCurrentValue();
        if (channel == 1)
            return rmsLevelOutputRight.getCurrentValue();
        return 0;
    }
}

//==============================================================================
void TertiaryAudioProcessor::pushNextSampleIntoFifo(float sample)
{
    /* If the fifo contains enough data, set a flag
    * to say that the next frame should now be rendered */

    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}






/* ============================================================================================================== */
/* MAIN DSP ===================================================================================================== */
/* \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/   \/  \/  \/  \/  \/  \/  \/  \/  \/  */


/* Main Audio Signal Processing Callback */
//==============================================================================
void TertiaryAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    /* Housekeeping */
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    /* Set Up Host Playhead Data */
    playHead = this->getPlayHead();
    playPosition = hostInfo.ppqPosition;

    if (playHead != nullptr)
        playHead->getCurrentPosition(hostInfo);

    /* Clear Input Buffers */
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    /* Update Processing State */
    updateState();

    /* Apply Input Gain */
    applyGain(buffer, inputGain);

    /* If Pickoff Point is Input, Update FFT */
    if (fftPickoffPointIsInput) { pushSignalToFFT(buffer); }
    
    /* Get Levels for Input Meters */
    updateMeterLevels(rmsLevelInputLeft, rmsLevelInputRight, buffer);

    /* Apply Crossover */
    applyCrossover(buffer);

    /* Apply Amplitude Modulation */
    applyAmplitudeModulation();
    
    /* Apply Individual Band Gains on Lows, Mids, Highs */
    for (size_t i = 0; i < filterBuffers.size(); ++i) { applyGain(filterBuffers[i], tremolos[i].bandGain); }

    /* Check for Bypass, Solo, Mute and Re-sum the Individual Bands */
    sumAudioBands(buffer);
    
    /* Apply Output Gain */
    applyGain(buffer, outputGain);

    /* If Pickoff Point is Output, Update FFT */
    if (!fftPickoffPointIsInput) {  pushSignalToFFT(buffer); }

    /* Get Levels for Output Meters */
    updateMeterLevels(rmsLevelOutputLeft, rmsLevelOutputRight, buffer);

}

/* Housekeeping DURING audio processing */
//==============================================================================
void TertiaryAudioProcessor::updateState()
{
    auto sampleRate = getSampleRate();

    /* Update Input & Output gain params*/
    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());

    /* Update tremolo effect params */
    for (auto& trem : tremolos)
        trem.updateTremoloSettings();

    // Flag to update Low LFO parameters
    if (parameterChangedLfoLow)
    {
        lowLFO.updateLFO(sampleRate, hostInfo.bpm);
        parameterChangedLfoLow = false;
    }

    // Flag to update Mid LFO parameters
    if (parameterChangedLfoMid)
    {
        midLFO.updateLFO(sampleRate, hostInfo.bpm);
        parameterChangedLfoMid = false;
    }

    // Flag to update High LFO parameters
    if (parameterChangedLfoHigh)
    {
        highLFO.updateLFO(sampleRate, hostInfo.bpm);
        parameterChangedLfoHigh = false;
    }


    // Update Crossover Params
    LP1.setCutoffFrequency(lowMidCrossover->get());
    HP1.setCutoffFrequency(lowMidCrossover->get());
    AP2.setCutoffFrequency(midHighCrossover->get());
    LP2.setCutoffFrequency(midHighCrossover->get());
    HP2.setCutoffFrequency(midHighCrossover->get());

    /* Sometimes when multiplier param is changed, LFOs become out of sync with each
       other or out-of-sync with the host grid.  The following detects Multiplier
       changes, and forces the synchronization. Cannot remember why, but didn't
       seem to work when placed in parameterChanged callback */

    multLow = lowLFO.getWaveMultiplier();
    multMid = midLFO.getWaveMultiplier();
    multHigh = highLFO.getWaveMultiplier();

    /* ... Check for change in multiplier value */
    if (oldMultLow != multLow) {    oldMultLow = multLow; multChanged = true; }
    if (oldMultMid != multMid) {    oldMultMid = multMid; multChanged = true; }
    if (oldMultHigh != multHigh) {  oldMultHigh = multHigh; multChanged = true; }

    /* ... If any multiplier param is changed, reset LFO position */
    if (multChanged == true) {
        forceSynchronization(lowLFO);
        forceSynchronization(midLFO);
        forceSynchronization(highLFO);
        multChanged = false;
    }

    /* Furthermore, we reset the LFO position on each play-after-pause to force the synchronization of the LFO's.
       An SR-Latch based on the user's hit of Play and Pause is what triggers the force synchronization */
    
    if (hostInfo.isPlaying) {
        setLatchPlay = true;

        if (setLatchStop) {
            hitPlay = true;
            setLatchStop = false;
            forceSynchronization(lowLFO);
            forceSynchronization(midLFO);
            forceSynchronization(highLFO);
        }
    }
    else
    {
        setLatchStop = true;

        if (setLatchPlay) {
            hitPlay = false;
            setLatchPlay = false;
        }
    }

}

/* Updates FFT information for FrequencyResponse Module */
//==============================================================================
void TertiaryAudioProcessor::pushSignalToFFT (juce::AudioBuffer<float> &buffer)
{
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float sample = 0.f;

        for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        {
            sample += buffer.getSample(channel, i) / 2.f;
        }

        pushNextSampleIntoFifo(sample);
    }
}

/* Pulls RMS audio levels from ProcessBlock and updates the Input and Output meters. */
//==============================================================================
void TertiaryAudioProcessor::updateMeterLevels ( juce::LinearSmoothedValue<float> &left,
                                                 juce::LinearSmoothedValue<float> &right,
                                                 juce::AudioBuffer<float> &buffer)
{
    auto totalNumSamples = buffer.getNumSamples();
    
    left.skip(totalNumSamples);
    
    const auto newValueLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    
    /* If value is ascending, we use instantatneous value.
       If value is descending, we smooth value on its way down. */
    
    if (newValueLeft < left.getCurrentValue())
        left.setTargetValue(newValueLeft);
    else left.setCurrentAndTargetValue(newValueLeft);


    /* If Stereo, do the same to the right */
    if (getTotalNumInputChannels() > 1)
    {
        right.skip(totalNumSamples);

        const auto newValueRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

        /* If value is ascending, we use instantatneous value.
        If value is descending, we smooth value on its way down. */

        if (newValueRight < right.getCurrentValue())
            right.setTargetValue(newValueRight);
        else right.setCurrentAndTargetValue(newValueRight);
    }
    else
        right = left;
    }

/* Applies the three-way crossover to the incoming audio signal */
//==============================================================================
void TertiaryAudioProcessor::applyCrossover(const juce::AudioBuffer<float>& inputBuffer)
{
    /* See PDF for signal-flow diagram */
    
    // Copy incoming audio into all filter buffers
    for (auto& fb : filterBuffers)
        fb = inputBuffer;

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);     // Create audio block for the buffer containing LOWS
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);     // Create audio block for the buffer containing MIDS
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);     // Create audio block for the buffer containing HIGHS

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);  // Create a context for the block
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);  // Create a context for the block
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);  // Create a context for the block

    LP1.process(fb0Ctx);                    // LP1 and AP2 create LOWS
    AP2.process(fb0Ctx);                    // LP1 and AP2 create LOWS

    HP1.process(fb1Ctx);                    // HP1 splits two ways: MIDS and HIGHS
    filterBuffers[2] = filterBuffers[1];    // Make a copy before BPF, to prepare for HIGHS
    LP2.process(fb1Ctx);                    // HP1 and LP2 create BPF

    HP2.process(fb2Ctx);                    // HP1, and HP2 create HIGHS
}

/* Applies the amplitude modulation to create the tremolo effect */
//==============================================================================
void TertiaryAudioProcessor::applyAmplitudeModulation()
{
    /* Apply Amplitude Modulation */
    for (int sample = 0; sample < filterBuffers[0].getNumSamples(); ++sample)
    {
        auto waveTableSize = lowLFO.getWaveTableSize();
        auto totalNumInputChannels = getTotalNumInputChannels();
        
        /* Index = Current Position in the Given WaveTable.
        Apply a single offset based on the Relative Phase Shift.*/
        int lowIndex = fmod((lowLFO.getPosition() + lowLFO.getRelativePhase()), (float)waveTableSize);
        int midIndex = fmod((midLFO.getPosition() + midLFO.getRelativePhase()), (float)waveTableSize);
        int highIndex = fmod((highLFO.getPosition() + highLFO.getRelativePhase()), (float)waveTableSize);

        /* LFO Instantaneous Amplitude, usd to Modulate Input Signal */
        float mGainLowLFO = lowLFO.getWaveTableScaled()[lowIndex];
        float mGainMidLFO = midLFO.getWaveTableScaled()[midIndex];
        float mGainHighLFO = highLFO.getWaveTableScaled()[highIndex];

        /* Update the LFO's Instantaneous Position */
        lowLFO.setPosition( fmod((lowLFO.getPosition() + lowLFO.getIncrement()), waveTableSize) );
        midLFO.setPosition( fmod((midLFO.getPosition() + midLFO.getIncrement()), waveTableSize) );
        highLFO.setPosition( fmod((highLFO.getPosition() + highLFO.getIncrement()), waveTableSize) );

        /* Amplitude Modulation on Lows */
        if (!lowBandTrem.bypassParam->get())
        {
            for (int i = 0; i < totalNumInputChannels; i++) {
                filterBuffers[0].getWritePointer(i)[sample] *= mGainLowLFO; }
        }

        /* Amplitude Modulation on Mids */
        if (!midBandTrem.bypassParam->get())
        {
            for (int i = 0; i < totalNumInputChannels; i++) {
                filterBuffers[1].getWritePointer(i)[sample] *= mGainMidLFO; }
        }

        /* Amplitude Modulation on Highs */
        if (!highBandTrem.bypassParam->get())
        {
            for (int i = 0; i < totalNumInputChannels; i++)    {
                filterBuffers[2].getWritePointer(i)[sample] *= mGainHighLFO; }
        }
    }
}

/* Checks for Bypass, Solo, Mute and re-sums audio bands appropriately */
//==============================================================================
void TertiaryAudioProcessor::sumAudioBands(juce::AudioBuffer<float>& buffer)
{
    using namespace juce;
    
    /* Clear the Input Buffer */
    buffer.clear();
    
    /* Helper function to add filter bands back into audio path */
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for (auto i = 0; i < nc; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    /* Check if any bands are solo'ed */
    auto bandsAreSoloed = false;
    for (auto& trem : tremolos)
    {
        if (trem.soloParam->get())
        {
            bandsAreSoloed = true;
            break;
        }
    }

    /* If any bands are solo'ed, only add those back to the audio path */
    if (bandsAreSoloed)
    {
        for (size_t i = 0; i < tremolos.size(); ++i)
        {
            auto& trem = tremolos[i];
            if (trem.soloParam->get())
            {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }
    else
    /* Otherwise add everything except for those which are muted */
    {
        for (size_t i = 0; i < tremolos.size(); ++i)
        {
            auto& trem = tremolos[i];
            if (!trem.muteParam->get())
            {
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }
}

/* /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\   /\  /\  /\  /\  /\  /\  /\  /\  /\  */
/* MAIN DSP ===================================================================================================== */
/* ============================================================================================================== */










//==============================================================================
const juce::String TertiaryAudioProcessor::getName() const { return JucePlugin_Name; }

//==============================================================================
bool TertiaryAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
bool TertiaryAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
bool TertiaryAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
double TertiaryAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
int TertiaryAudioProcessor::getNumPrograms() { return 1; }

//==============================================================================
int TertiaryAudioProcessor::getCurrentProgram() { return 0; }

//==============================================================================
void TertiaryAudioProcessor::setCurrentProgram (int index) {}

//==============================================================================
const juce::String TertiaryAudioProcessor::getProgramName (int index) { return {}; }

//==============================================================================
void TertiaryAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

//==============================================================================
void TertiaryAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TertiaryAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==============================================================================
bool TertiaryAudioProcessor::hasEditor() const { return true; }

/* Juce Boiler Plate Code */
//==============================================================================
juce::AudioProcessorEditor* TertiaryAudioProcessor::createEditor()
{
    return new TertiaryAudioProcessorEditor (*this);
	//return new juce::GenericAudioProcessorEditor(*this);    // Returns an automated, generic plugin editor
}

/* This creates new instances of the plugin */
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TertiaryAudioProcessor();
}


/* Parameter Changed Callback */
//==============================================================================
void TertiaryAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    using namespace juce;
    using namespace Params;
    const auto& params = GetParams();

    // Flag for updates to Low LFO
    if (parameterID == params.at(Wave_Low_LFO) ||
        parameterID == params.at(Symmetry_Low_LFO) ||
        parameterID == params.at(Depth_Low_LFO) ||
        parameterID == params.at(Rate_Low_LFO) ||
        parameterID == params.at(Multiplier_Low_LFO) ||
        parameterID == params.at(Relative_Phase_Low_LFO) ||
        parameterID == params.at(Invert_Low_LFO))
    {
        parameterChangedLfoLow = true;
    }
      
    // Flag for updates to Mid LFO
    if (parameterID == params.at(Wave_Mid_LFO) ||
        parameterID == params.at(Symmetry_Mid_LFO) ||
        parameterID == params.at(Depth_Mid_LFO) ||
        parameterID == params.at(Rate_Mid_LFO) ||
        parameterID == params.at(Multiplier_Mid_LFO) ||
        parameterID == params.at(Relative_Phase_Mid_LFO) ||
        parameterID == params.at(Invert_Mid_LFO))
    {
        parameterChangedLfoMid = true;
    }

    // Flag for updates to High LFO
    if (parameterID == params.at(Wave_High_LFO) ||
        parameterID == params.at(Symmetry_High_LFO) ||
        parameterID == params.at(Depth_High_LFO) ||
        parameterID == params.at(Rate_High_LFO) ||
        parameterID == params.at(Multiplier_High_LFO) ||
        parameterID == params.at(Relative_Phase_High_LFO) ||
        parameterID == params.at(Invert_High_LFO))
    {
        parameterChangedLfoHigh = true;
    }

}
