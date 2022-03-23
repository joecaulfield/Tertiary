/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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

    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

	// Float Helper To Attach Float to Parameter ========
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName) 
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);              
    };

	// Choice Helper To Attach Choice to Parameter ========
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

	// Bool Helper To Attach Bool to Parameter ========
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    // Initialize Input & Output Gain
    floatHelper(inputGainParam,             Names::Input_Gain);                 // Attach Value to Parameter
    floatHelper(outputGainParam,            Names::Output_Gain);                // Attach Value to Parameter

	// Initialize the LFO
	lowLFO.initializeLFO(getSampleRate());
	midLFO.initializeLFO(getSampleRate());
	highLFO.initializeLFO(getSampleRate());

    // Initialize crossover frequencies
    floatHelper(lowMidCrossover,            Names::Low_Mid_Crossover_Freq);     // Attach Value to Parameter
    floatHelper(midHighCrossover,           Names::Mid_High_Crossover_Freq);    // Attach Value to Parameter

    // Initialize Low Band Parameters
    boolHelper(lowBandTrem.bypass,          Names::Bypass_Low_Band);            // Attach Value to Parameter
    boolHelper(lowBandTrem.mute,            Names::Mute_Low_Band);              // Attach Value to Parameter
    boolHelper(lowBandTrem.solo,            Names::Solo_Low_Band);              // Attach Value to Parameter
    floatHelper(lowBandTrem.bandGainParam,  Names::Gain_Low_Band);              // Attach Value to Parameter

    floatHelper(lowLFO.symmetry,            Names::Symmetry_Low_LFO);
    floatHelper(lowLFO.depth,               Names::Depth_Low_LFO);
    choiceHelper(lowLFO.waveshape,          Names::Wave_Low_LFO);
    boolHelper(lowLFO.invert,               Names::Invert_Low_LFO);
    floatHelper(lowLFO.relativePhase,       Names::Relative_Phase_Low_LFO);
    floatHelper(lowLFO.rate,                Names::Rate_Low_LFO);
    boolHelper(lowLFO.syncToHost,           Names::Sync_Low_LFO);
    choiceHelper(lowLFO.multiplier,         Names::Multiplier_Low_LFO);


    // Initialize Mid Band Parameters
    boolHelper(midBandTrem.bypass,          Names::Bypass_Mid_Band);            // Attach Value to Parameter
    boolHelper(midBandTrem.mute,            Names::Mute_Mid_Band);              // Attach Value to Parameter
    boolHelper(midBandTrem.solo,            Names::Solo_Mid_Band);              // Attach Value to Parameter
    floatHelper(midBandTrem.bandGainParam,  Names::Gain_Mid_Band);              // Attach Value to Parameter

    floatHelper(midLFO.symmetry,            Names::Symmetry_Mid_LFO);
    floatHelper(midLFO.depth,               Names::Depth_Mid_LFO);
    choiceHelper(midLFO.waveshape,          Names::Wave_Mid_LFO);
    boolHelper(midLFO.invert,               Names::Invert_Mid_LFO);
    floatHelper(midLFO.relativePhase,       Names::Relative_Phase_Mid_LFO);
    floatHelper(midLFO.rate,                Names::Rate_Mid_LFO);
    boolHelper(midLFO.syncToHost,           Names::Sync_Mid_LFO);
    choiceHelper(midLFO.multiplier,         Names::Multiplier_Mid_LFO);

    // Initialize High Band Parameters
    boolHelper(highBandTrem.bypass,         Names::Bypass_High_Band);           // Attach Value to Parameter
    boolHelper(highBandTrem.mute,           Names::Mute_High_Band);             // Attach Value to Parameter
    boolHelper(highBandTrem.solo,           Names::Solo_High_Band);             // Attach Value to Parameter
    floatHelper(highBandTrem.bandGainParam, Names::Gain_High_Band);             // Attach Value to Parameter

    floatHelper(highLFO.symmetry,           Names::Symmetry_High_LFO);
    floatHelper(highLFO.depth,              Names::Depth_High_LFO);
    choiceHelper(highLFO.waveshape,         Names::Wave_High_LFO);
    boolHelper(highLFO.invert,              Names::Invert_High_LFO);
    floatHelper(highLFO.relativePhase,      Names::Relative_Phase_High_LFO);
    floatHelper(highLFO.rate,               Names::Rate_High_LFO);
    boolHelper(highLFO.syncToHost,          Names::Sync_High_LFO);
    choiceHelper(highLFO.multiplier,        Names::Multiplier_High_LFO);

	// Refactor - Optimize into a For Loop
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

    // Set Crossover Types
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

}

TertiaryAudioProcessor::~TertiaryAudioProcessor()
{
}

//==============================================================================
const juce::String TertiaryAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TertiaryAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TertiaryAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TertiaryAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TertiaryAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TertiaryAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TertiaryAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TertiaryAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TertiaryAudioProcessor::getProgramName (int index)
{
    return {};
}

void TertiaryAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TertiaryAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;                    // Create a ProcessSpec object called 'spec'
    spec.maximumBlockSize = samplesPerBlock;        // Define the Maximum Block Size
    spec.numChannels = getTotalNumOutputChannels(); // Define the Maximum Number of Channels the Comp will use
    spec.sampleRate = sampleRate;                   // Define the Sample Rate

	lastSampleRate = getSampleRate();

    inputGain.prepare(spec);    // Pass spec information into Input Gain
    outputGain.prepare(spec);   // Pass spec information into Output Gain

    inputGain.setRampDurationSeconds(0.05);     // Reduce Artifacts
    outputGain.setRampDurationSeconds(0.05);    // Reduce Artifacts

    LP1.prepare(spec);   // Pass above information into Low Pass Filter
    HP1.prepare(spec);   // Pass above information into High Pass Filter

    AP2.prepare(spec);   // Testing AP Filter

    LP2.prepare(spec);   // Pass above information into Low Pass Filter
    HP2.prepare(spec);   // Pass above information into High Pass Filter

    // Prepare Filter Buffers so that memory allocation does not occur at time of processing
    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }

	// RMS Level Smoothing for Meters
	rmsLevelInputLeft.reset(sampleRate, 0.5);	rmsLevelInputLeft.setCurrentAndTargetValue(-100.f);
	rmsLevelInputRight.reset(sampleRate, 0.5);	rmsLevelInputRight.setCurrentAndTargetValue(-100.f);
	rmsLevelOutputLeft.reset(sampleRate, 0.5);	rmsLevelOutputLeft.setCurrentAndTargetValue(-100.f);
	rmsLevelOutputRight.reset(sampleRate, 0.5);	rmsLevelOutputRight.setCurrentAndTargetValue(-100.f);

}

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

void TertiaryAudioProcessor::updateState()
{
	auto sampleRate = getSampleRate();

	// Update Input Gain Params
	inputGain.setGainDecibels(inputGainParam->get());
	outputGain.setGainDecibels(outputGainParam->get());

	// Update Tremolo Params
	for (auto& trem : tremolos)                         // Update Tremolo Settings In One Pass
		trem.updateTremoloSettings();

	lowLFO.updateLFO(sampleRate, hostInfo.bpm);
	midLFO.updateLFO(sampleRate, hostInfo.bpm);
	highLFO.updateLFO(sampleRate, hostInfo.bpm);

	// Update Crossover Params

	auto lowMidCutoffFreq = lowMidCrossover->get();     // Cutoff Frequency
	LP1.setCutoffFrequency(lowMidCutoffFreq);           // Set Cutoff Frequency of LPF
	HP1.setCutoffFrequency(lowMidCutoffFreq);           // Set Cutoff Frequency of HPF

	auto midHighCutoffFreq = midHighCrossover->get();
	AP2.setCutoffFrequency(midHighCutoffFreq);          // Set Cutoff Frequency of APF 2
	LP2.setCutoffFrequency(midHighCutoffFreq);          // Set Cutoff Frequency of LPF 2
	HP2.setCutoffFrequency(midHighCutoffFreq);          // Set Cutoff Frequency of HPF 2

	/* Set up a SR latch on Hit-Play */
	if (hostInfo.isPlaying)
	{
		setLatchPlay = true;

		if (setLatchStop)
		{
			hitPlay = true;
			setLatchStop = false;
			
			// Force Low LFO Onto Grid
			if (hostInfo.isPlaying && lowLFO.isSyncedToHost())
			{
				float div = 1.f;

				// Special Case for Mult = 0.5x
				if (lowLFO.getWaveMultiplier() == 0.5f)
					div = 2.f;

				lowLFO.phase = fmod(playPosition, div) * lowLFO.waveTableMapped.size();
			}

			// Force Mid LFO Onto Grid
			if (hostInfo.isPlaying && midLFO.isSyncedToHost())
			{
				float div = 1.f;

				// Special Case for Mult = 0.5x
				if (midLFO.getWaveMultiplier() == 0.5f)
					div = 2.f;

				midLFO.phase = fmod(playPosition, div) * midLFO.waveTableMapped.size();
			}

			// Force High LFO Onto Grid
			if (hostInfo.isPlaying && highLFO.isSyncedToHost())
			{
				float div = 1.f;

				// Special Case for Mult = 0.5x
				if (highLFO.getWaveMultiplier() == 0.5f)
					div = 2.f;

				highLFO.phase = fmod(playPosition, div) * highLFO.waveTableMapped.size();
			}


		}
	}
	else
	{
		setLatchStop = true;

		if (setLatchPlay)
		{
			hitPlay = false;
			setLatchPlay = false;
			DBG("STOP");
		}
	}

}

void TertiaryAudioProcessor::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{

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

void TertiaryAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Get Host PlayHead for BPM Sync ===========================
    playHead = this->getPlayHead();

	playPosition = hostInfo.ppqPosition;

    if (playHead != nullptr)
        playHead->getCurrentPosition(hostInfo);

	// Clear Input Buffers
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateState();  // Update Processing State

    applyGain(buffer, inputGain);   // Apply Input Gain

	// Get Input Gains for Meters =========
	rmsLevelInputLeft.skip(buffer.getNumSamples());
	rmsLevelInputRight.skip(buffer.getNumSamples());
	{
		const auto newValueLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

		// If value is ascending, we use instantatneous value.
		// If value is descending, we smooth value on its way down.
		if (newValueLeft < rmsLevelInputLeft.getCurrentValue())
			rmsLevelInputLeft.setTargetValue(newValueLeft);
		else rmsLevelInputLeft.setCurrentAndTargetValue(newValueLeft);
	}

	{
		const auto newValueRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

		// If value is ascending, we use instantatneous value.
		// If value is descending, we smooth value on its way down.

		if (newValueRight < rmsLevelInputRight.getCurrentValue())
			rmsLevelInputRight.setTargetValue(newValueRight);
		else rmsLevelInputRight.setCurrentAndTargetValue(newValueRight);
	}

	// APPLY CROSSOVER =====
    splitBands(buffer);

	// Maybe need to apply gain to all trem-bands in same sample-pass?

    // Apply Low Band Tremolo =======================================================================

    auto* lowWriteL = filterBuffers[0].getWritePointer(0);
    auto* lowWriteR = filterBuffers[0].getWritePointer(1);

    for (int sample = 0; sample < filterBuffers[0].getNumSamples(); ++sample)
    {
        float mGainLowLFO = lowLFO.waveTableMapped[fmod((lowLFO.phase + lowLFO.mRelativePhase), lowLFO.waveTableMapped.size())];

		lowLFO.phase = fmod((lowLFO.phase + lowLFO.increment), lowLFO.waveTableMapped.size());		
		
        if (!lowBandTrem.bypass->get())
        {
            lowWriteL[sample] *= mGainLowLFO;
            lowWriteR[sample] *= mGainLowLFO;

			//lowWriteL[sample] = mGainLowLFO;
        }
    }

    // Apply Mid Band Tremolo =======================================================================

    auto* midWriteL = filterBuffers[1].getWritePointer(0);
    auto* midWriteR = filterBuffers[1].getWritePointer(1);

    for (int sample = 0; sample < filterBuffers[1].getNumSamples(); ++sample)
    {
        float mGainMidLFO = midLFO.waveTableMapped[fmod((midLFO.phase + midLFO.mRelativePhase), midLFO.waveTableMapped.size())];

		midLFO.phase = fmod((midLFO.phase + midLFO.increment), midLFO.waveTableMapped.size());

		if (!midBandTrem.bypass->get())
        {
            midWriteL[sample] *= mGainMidLFO;
            midWriteR[sample] *= mGainMidLFO;
        }
    }

    // Apply High Band Tremolo =======================================================================

    auto* highWriteL = filterBuffers[2].getWritePointer(0);
    auto* highWriteR = filterBuffers[2].getWritePointer(1);

    for (int sample = 0; sample < filterBuffers[2].getNumSamples(); ++sample)
    {
        float mGainHighLFO = highLFO.waveTableMapped[fmod((highLFO.phase + highLFO.mRelativePhase), highLFO.waveTableMapped.size())];
 
		highLFO.phase = fmod((highLFO.phase + highLFO.increment), highLFO.waveTableMapped.size());

        if (!highBandTrem.bypass->get())
        {
            highWriteL[sample] *= mGainHighLFO;
            highWriteR[sample] *= mGainHighLFO;
        }
    }

    for (size_t i = 0; i < filterBuffers.size(); ++i)   // size_t is an unsigned int that is the result of the sizeof operator
    {
        applyGain(filterBuffers[i], tremolos[i].bandGain);  // Apply individual band gains to Lows, Mids, Highs
    }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear(); // Clear buffer before adding audio (filter buffers) to it

    // Each channel of filter buffer needs to be copied back to input buffer
    // Helper function to do so...
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for (auto i = 0; i < nc; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    // Check for if any bands are soloed
    auto bandsAreSoloed = false;
    for (auto& trem : tremolos)
    {
        if (trem.solo->get())
        {
            bandsAreSoloed = true;
            break;
        }
    }

    if (bandsAreSoloed)										// If bands are soloed
    {
        for (size_t i = 0; i < tremolos.size(); ++i)		// Loop through all bands
        {
            auto& trem = tremolos[i];
            if (trem.solo->get())                           // If that band is soloed
            {
                addFilterBand(buffer, filterBuffers[i]);    // Add the corresponding buffer
            }
        }
    }
    else
    {
        for (size_t i = 0; i < tremolos.size(); ++i)		// Loop through all bands
        {
            auto& trem = tremolos[i];
            if (!trem.mute->get())                          // If the trem is NOT muted
            {
                addFilterBand(buffer, filterBuffers[i]);    // Add the corresponding buffer
            }
        }
    }

    applyGain(buffer, outputGain);  // Apply Output Gain

	// Get Input Gains for Meters ===================================================================
	rmsLevelOutputLeft.skip(buffer.getNumSamples());
	rmsLevelOutputRight.skip(buffer.getNumSamples());
	{
		const auto newValueLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

		// If value is ascending, we use instantatneous value.
		// If value is descending, we smooth value on its way down.

		if (newValueLeft < rmsLevelOutputLeft.getCurrentValue())
			rmsLevelOutputLeft.setTargetValue(newValueLeft);
		else rmsLevelOutputLeft.setCurrentAndTargetValue(newValueLeft);
	}

	{
		const auto newValueRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

		// If value is ascending, we use instantatneous value.
		// If value is descending, we smooth value on its way down.

		if (newValueRight < rmsLevelOutputRight.getCurrentValue())
			rmsLevelOutputRight.setTargetValue(newValueRight);
		else rmsLevelOutputRight.setCurrentAndTargetValue(newValueRight);
	}

	

}

//==============================================================================
bool TertiaryAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TertiaryAudioProcessor::createEditor()
{
    return new TertiaryAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);    // Returns an automated, generic plugin editor
}

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

void TertiaryAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // USED TO LOAD PARAMETERS FROM MEMORY

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);   // Pull the Value Tree from memory

    if (tree.isValid()) // Check if Value Tree is valid
    {
        apvts.replaceState(tree);   // If valid, load from memory and replace current state
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout TertiaryAudioProcessor::createParameterLayout()    //
{
    APVTS::ParameterLayout layout;

    using namespace juce;
    using namespace Params;             // Create a Local Reference to Parameter Mapping
    const auto& params = GetParams();   // Create a Local Reference to Parameter Mapping

    juce::StringArray sa;               // Used to hold choice arrays

    // INPUT & OUTPUT GAIN

    auto gainRange = NormalisableRange<float> ( -24.f,                                          // Start
                                                24.f,                                           // Stop
                                                0.5f,                                           // Step Size
                                                1.f);                                           // Skew 

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Input_Gain),   // Parameter ID
                                                        params.at(Names::Input_Gain),   // Parameter Name
                                                        gainRange,                      // Range
                                                        0));                            // Default Value

    // CROSSOVER FREQUENCIES PARAMETER ====================================================================================================================

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Low_Mid_Crossover_Freq),       // Parameter ID
                                                        params.at(Names::Low_Mid_Crossover_Freq),       // Parameter Name
                                                        NormalisableRange<float> (20, 20000, 1, 0.3f),     // Range
                                                        500));                                          // Default Value
                                                        
    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Mid_High_Crossover_Freq),      // Parameter ID
                                                        params.at(Names::Mid_High_Crossover_Freq),      // Parameter Name
                                                        NormalisableRange<float> (20, 20000, 1, 0.5f),	// Range
                                                        1000));                                         // Default Value

    // LFO WAVE PARAMETER ====================================================================================================================

    sa = {  "Ramp Up", 
            "Ramp Down", 
            "Square", 
            "Triangle", 
            "Sine", 
            "Lumps", 
            "Humps" };

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Wave_Low_LFO),         // Parameter ID
                                                        params.at(Names::Wave_Low_LFO),         // Parameter Name
                                                        sa,                                     // Range
                                                        4));                                    // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Wave_Mid_LFO),         // Parameter ID
                                                        params.at(Names::Wave_Mid_LFO),         // Parameter Name
                                                        sa,                                     // Range
                                                        4));                                    // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Wave_High_LFO),        // Parameter ID
                                                        params.at(Names::Wave_High_LFO),        // Parameter Name
                                                        sa,                                     // Range
                                                        4));                                    // INDEX of Default Value

    // LFO DEPTH PARAMETER ====================================================================================================================

    auto depthRange = NormalisableRange<float> (    0,                                          // Start
                                                    100,                                        // Stop
                                                    0.5f,                                       // Step Size
                                                    1.f);                                       // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Depth_Low_LFO),        // Parameter ID
                                                        params.at(Names::Depth_Low_LFO),        // Parameter Name
                                                        depthRange,                             // Range
                                                        75));                                   // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Depth_Mid_LFO),        // Parameter ID
                                                        params.at(Names::Depth_Mid_LFO),        // Parameter Name
                                                        depthRange,                             // Range
                                                        75));                                   // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Depth_High_LFO),        // Parameter ID
                                                        params.at(Names::Depth_High_LFO),        // Parameter Name
                                                        depthRange,                             // Range
                                                        75));                                   // Default Value

    // LFO SYMMETRY PARAMETER ====================================================================================================================

    auto symmetryRange = NormalisableRange<float> ( 12.5,                                       // Start
                                                    87.5,                                       // Stop
                                                    0.5f,                                       // Step Size
                                                    1.f);                                       // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Symmetry_Low_LFO),     // Parameter ID
                                                        params.at(Names::Symmetry_Low_LFO),     // Parameter Name
                                                        symmetryRange,                          // Range
                                                        50));                                   // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Symmetry_Mid_LFO),     // Parameter ID
                                                        params.at(Names::Symmetry_Mid_LFO),     // Parameter Name
                                                        symmetryRange,                          // Range
                                                        50));                                   // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Symmetry_High_LFO),    // Parameter ID
                                                        params.at(Names::Symmetry_High_LFO),    // Parameter Name
                                                        symmetryRange,							// Range
                                                        50));                                   // Default Value

    // LFO INVERT PARAMETER ====================================================================================================================

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Invert_Low_LFO),       // Parameter ID
                                                        params.at(Names::Invert_Low_LFO),       // Parameter Name
                                                        false));								// Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Invert_Mid_LFO),       // Parameter ID
                                                        params.at(Names::Invert_Mid_LFO),       // Parameter Name
                                                        false));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Invert_High_LFO),      // Parameter ID
                                                        params.at(Names::Invert_High_LFO),      // Parameter Name
                                                        false));                                // Default Value

    // LFO SYNC PARAMETER ====================================================================================================================

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Sync_Low_LFO),         // Parameter ID
                                                        params.at(Names::Sync_Low_LFO),         // Parameter Name
                                                        true));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Sync_Mid_LFO),         // Parameter ID
                                                        params.at(Names::Sync_Mid_LFO),         // Parameter Name
                                                        true));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Sync_High_LFO),        // Parameter ID
                                                        params.at(Names::Sync_High_LFO),        // Parameter Name
                                                        true));                                // Default Value

    // LFO RATE PARAMETER ====================================================================================================================

    auto rateRange = NormalisableRange<float> ( 0.5f,											// Start
                                                12.5f,                                           // Stop
                                                0.25f,											// Step Size
                                                1.f);                                           // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Rate_Low_LFO),         // Parameter ID
                                                        params.at(Names::Rate_Low_LFO),         // Parameter Name
                                                        rateRange,                              // Range
                                                        3.f));                                 // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Rate_Mid_LFO),         // Parameter ID
                                                        params.at(Names::Rate_Mid_LFO),         // Parameter Name
                                                        rateRange,                              // Range
                                                        3.f));                                 // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Rate_High_LFO),         // Parameter ID
                                                        params.at(Names::Rate_High_LFO),         // Parameter Name
                                                        rateRange,                              // Range
                                                        3.f));                                 // Default Value

    // LFO MULTIPLIER PARAMETER ====================================================================================================================

    auto multChoices = std::vector<double>{0.5, 1.0, 1.5, 2.0, 3.0, 4.0};                                                                       

    sa.clear();

    sa = {  "1/2 Note", 
            "1/4 Note", 
            "1/4 Note Triplet", 
            "1/8 Note", 
            "1/8 Note Triplet", 
            "1/16 Note"};

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Multiplier_Low_LFO),   // Parameter ID
                                                        params.at(Names::Multiplier_Low_LFO),   // Parameter Name
                                                        sa,                                     // Range
                                                        1));                                    // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Multiplier_Mid_LFO),   // Parameter ID
                                                        params.at(Names::Multiplier_Mid_LFO),   // Parameter Name
                                                        sa,                                     // Range
                                                        1));                                    // INDEX of Default Value

    layout.add(std::make_unique<AudioParameterChoice>(  params.at(Names::Multiplier_High_LFO),  // Parameter ID
                                                        params.at(Names::Multiplier_High_LFO),  // Parameter Name
                                                        sa,                                     // Range
                                                        1));                                    // INDEX of Default Value

    // LFO RELATIVE PHASE PARAMETER ============================================================================================================

    auto relativePhaseRange = NormalisableRange<float> (    -180,                               // Start
                                                            180,                                // Stop
                                                            1,                                  // Step Size
                                                            1.f);                               // Skew

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Relative_Phase_Low_LFO),   // Parameter ID
                                                        params.at(Names::Relative_Phase_Low_LFO),   // Parameter Name
                                                        relativePhaseRange,                         // Range
                                                        0));                                        // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Relative_Phase_Mid_LFO),   // Parameter ID
                                                        params.at(Names::Relative_Phase_Mid_LFO),   // Parameter Name
                                                        relativePhaseRange,                         // Range
                                                        0));                                        // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Relative_Phase_High_LFO),  // Parameter ID
                                                        params.at(Names::Relative_Phase_High_LFO),  // Parameter Name
                                                        relativePhaseRange,                         // Range
                                                        0));                                        // Default Value

    // BYPASS PARAMETER ====================================================================================================================
     
    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Bypass_Low_Band),      // Parameter ID
                                                        params.at(Names::Bypass_Low_Band),      // Parameter Name
                                                        false));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Bypass_Mid_Band),      // Parameter ID
                                                        params.at(Names::Bypass_Mid_Band),      // Parameter Name
                                                        false));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Bypass_High_Band),     // Parameter ID
                                                        params.at(Names::Bypass_High_Band),     // Parameter Name
                                                        false));                                // Default Value


    // MUTE PARAMETER ====================================================================================================================

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Mute_Low_Band),        // Parameter ID
                                                        params.at(Names::Mute_Low_Band),        // Parameter Name
                                                        false));                                // Default Value
                                                                            
    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Mute_Mid_Band),        // Parameter ID
                                                        params.at(Names::Mute_Mid_Band),        // Parameter Name
                                                        false));                                // Default Value
                                                                         
    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Mute_High_Band),       // Parameter ID
                                                        params.at(Names::Mute_High_Band),       // Parameter Name
                                                        false));                                // Default Value

    // SOLO PARAMETER ====================================================================================================================

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Solo_Low_Band),        // Parameter ID
                                                        params.at(Names::Solo_Low_Band),        // Parameter Name
                                                        false));                                // Default Value
                                                                         
    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Solo_Mid_Band),        // Parameter ID
                                                        params.at(Names::Solo_Mid_Band),        // Parameter Name
                                                        false));                                // Default Value

    layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Solo_High_Band),       // Parameter ID
                                                        params.at(Names::Solo_High_Band),       // Parameter Name
                                                        false));                                // Default Value

    // OUTPUT GAINS ====================================================================================================================

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Gain_Low_Band),        // Parameter ID
                                                        params.at(Names::Gain_Low_Band),        // Parameter Name
                                                        gainRange,                              // Range
                                                        0));                                    // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Gain_Mid_Band),        // Parameter ID
                                                        params.at(Names::Gain_Mid_Band),        // Parameter Name
                                                        gainRange,                              // Range
                                                        0));                                    // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Gain_High_Band),       // Parameter ID
                                                        params.at(Names::Gain_High_Band),       // Parameter Name
                                                        gainRange,                              // Range
                                                        0));                                    // Default Value

    layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Output_Gain),          // Parameter ID
                                                        params.at(Names::Output_Gain),          // Parameter Name
                                                        gainRange,                              // Range
                                                        0));                                    // Default Value

	// OSCILLOSCOPE DISPLAY PREFERENCES ====================================================================================================================

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Show_Low_Scope),       // Parameter ID
                                                        params.at(Names::Show_Low_Scope),       // Parameter Name
                                                        true));  

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Show_Mid_Scope),       // Parameter ID
                                                        params.at(Names::Show_Mid_Scope),       // Parameter Name
                                                        true));  

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Show_High_Scope),      // Parameter ID
														params.at(Names::Show_High_Scope),      // Parameter Name
														true));  

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Stack_Bands_Scope),	// Parameter ID
														params.at(Names::Stack_Bands_Scope),    // Parameter Name
														true));  

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Show_Cursor_Scope),	// Parameter ID
														params.at(Names::Show_Cursor_Scope),    // Parameter Name
														true)); 

	layout.add(std::make_unique<AudioParameterBool>(    params.at(Names::Show_Playhead_Scope),	// Parameter ID
														params.at(Names::Show_Playhead_Scope),  // Parameter Name
														true)); 

	auto cursorRange = NormalisableRange<float> (0.f,	// Start
                                                1.f,	// Stop
                                                0.0001,	// Step Size
                                                1.f);	// Skew 

	layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Cursor_Position),	// Parameter ID
														params.at(Names::Cursor_Position),  // Parameter Name
														cursorRange,                        // Range
														0.5f));                             // Default Value

	auto pointRange = NormalisableRange<float> ( 0,		// Start
                                                100.f,	// Stop
                                                1.f,	// Step Size
                                                1.f);	// Skew 

	layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Scope_Point1),		// Parameter ID
														params.at(Names::Scope_Point1),     // Parameter Name
														pointRange,                        // Range
														25.f));							// Default Value

	layout.add(std::make_unique<AudioParameterFloat>(   params.at(Names::Scope_Point2),		// Parameter ID
														params.at(Names::Scope_Point2),		// Parameter Name
														pointRange,							// Range
														75.f));								// Default Value

    return layout;
}

void TertiaryAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    using namespace juce;
    using namespace Params;
    const auto& params = GetParams();

	// If we update 'Sync To Host', reset the phase of all
	// LFOs so they remain in phase with each other.
    if (parameterID == params.at(Names::Sync_Low_LFO)  || 
        parameterID == params.at(Names::Sync_Mid_LFO)  ||
        parameterID == params.at(Names::Sync_High_LFO))
    {
        //lowLFO.phase = 0;
        //midLFO.phase = 0;
        //highLFO.phase = 0;
    }

	//paramChangedForScope.set(true);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TertiaryAudioProcessor();
}