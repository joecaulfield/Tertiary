

#pragma once

#include <JuceHeader.h>
#include "DSP/Params.h"
#include "DSP/LFO.h"

/* Instance of audio-effect path, in existence post-crossover and pre-summing */
struct TremoloBand
{
    
    /* Pointers to the APVTS */
    juce::AudioParameterBool* bypassParam{ nullptr };
    juce::AudioParameterBool* muteParam{ nullptr };
    juce::AudioParameterBool* soloParam{ nullptr };
    juce::AudioParameterFloat* bandGainParam{ nullptr };

    
    juce::dsp::Gain<float> bandGain;                        

    void updateTremoloSettings()
    {
        bandGain.setRampDurationSeconds(0.05);              // Reduce Artifacts
        bandGain.setGainDecibels(bandGainParam->get());     // Update Gain
    }

    //void process(juce::AudioBuffer<float>& buffer) {}

};

//==============================================================================

class TertiaryAudioProcessor  : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    TertiaryAudioProcessor();
    ~TertiaryAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	float getRmsValue(const int pickOffPoint, const int channel) const {
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

    using APVTS = juce::AudioProcessorValueTreeState;       // Alias
    static APVTS::ParameterLayout createParameterLayout();  // Parameter Layout Function

    void parameterChanged(const juce::String& parameterID, float newValue) {};

	void setFftPickoffPoint(int point);

    APVTS apvts{    *this,                                  // Audio Processor
                    nullptr,                                // Undo Manager
                    "Parameters",                           // Identifier &valueTreeType
                    createParameterLayout() };              // parameterLayout

	//juce::Array<float>& getWaveTable(int channel);
	juce::AudioPlayHead::CurrentPositionInfo hostInfo;		// To get BPM

	bool hitPlay{ false };
	bool setLatchPlay{ false };
	bool setLatchStop{ true };
	bool rateChanged{ true };
	bool multChanged{ true };
	bool fftPickoffPointIsInput{ false };

    double playPosition;
    
    
    float multLow{ 1.f },       multMid{ 1.f },     multHigh{ 1.f };
	float oldMultLow{ 0.f },    oldMultMid{ 0.f },  oldMultHigh{ 0.f };
	

	LFO lowLFO, midLFO, highLFO;	// Instances of LFO

	juce::Atomic<bool> paramChangedForScope{ false };


	std::atomic<double> getPlayPosition() { return playPosition; }

	// FFT Components ===================

	enum
	{
		fftOrder = 12,
		fftSize = 1 << fftOrder,
		scopeSize = 512
	};

	float fifo[fftSize];
	float fftData[2 * fftSize];

	bool nextFFTBlockReady = false;
	float scopeData[scopeSize];

	int fifoIndex = 0;

	void pushNextSampleIntoFifo(float sample);

    
private:

    void updateState();			// Process Block, Update State
	void resetLfoPosition(LFO &lfo);

    /* Pulls audio levels from process block for Input & Output Meters */
    void updateMeterLevels (juce::LinearSmoothedValue<float> &left,
                            juce::LinearSmoothedValue<float> &right,
                            juce::AudioBuffer<float> &buffer);
    
    void pushSignalToFFT (juce::AudioBuffer<float> &buffer);
    
    double lastSampleRate;		// To get Sample Rate

    juce::AudioPlayHead* playHead;		//i To get BPM

	//int lfoPhase{ 0 };

    // CROSSOVER =========================================================================

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;       // Crossover
    //      fc0     fc1                                         // Crossover
    Filter  LP1,    AP2,                                        // Crossover
            HP1,    LP2,                                        // Crossover
                    HP2;                                        // Crossover

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;  // An array of 3 buffers which contain the filtered audio

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };  // Pointer to the APVTS
    juce::AudioParameterFloat* midHighCrossover{ nullptr }; // Pointer to the APVTS

    void splitBands(const juce::AudioBuffer<float>& inputBuffer);    // Crossover Function

    // INPUT-OUTPUT GAIN =====================================================================================

    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam{ nullptr };               // Pointer to the APVTS
    juce::AudioParameterFloat* outputGainParam{ nullptr };              // Pointer to the APVTS

    template<typename T, typename U>                                    // Function to Apply Input/Output Gain
    void applyGain(T& buffer, U& gain)                                  // Function to Apply Input/Output Gain
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);              // Function to Apply Input/Output Gain
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);    // Function to Apply Input/Output Gain
        gain.process(ctx);                                              // Function to Apply Input/Output Gain
    }

    // TREMOLO BANDS ==========================================================================

    std::array<TremoloBand, 3> tremolos;
    TremoloBand& lowBandTrem = tremolos[0];     // Low Band is 0th in Array
    TremoloBand& midBandTrem = tremolos[1];     // Mid Band is 1st in Array
    TremoloBand& highBandTrem = tremolos[2];    // High Band is 2nd in Array

	// INPUT OUTPUT GAINS
	juce::LinearSmoothedValue<float>	rmsLevelInputLeft, 
										rmsLevelInputRight,
										rmsLevelOutputLeft, 
										rmsLevelOutputRight;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessor)
};
