

#pragma once

#include <JuceHeader.h>
#include "DSP/Params.h"
#include "DSP/LFO.h"

/* Audio-Path-Wrapper holding the parameters unique to the multiple effect bands */
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



    using APVTS = juce::AudioProcessorValueTreeState;       // Alias
    static APVTS::ParameterLayout createParameterLayout();  // Parameter Layout Function

    void parameterChanged(const juce::String& parameterID, float newValue) override {};

	void setFftPickoffPoint(int point);

    APVTS apvts{    *this,                                  // Audio Processor
                    nullptr,                                // Undo Manager
                    "Parameters",                           // Identifier &valueTreeType
                    createParameterLayout() };              // parameterLayout


    
    /* Input & Output Meters */
    // =========================================================================
    float getRmsValue(const int pickOffPoint, const int channel) const;

    /* LFO */
    // =========================================================================
	LFO lowLFO, midLFO, highLFO;	// Instances of LFO

    /* Oscilloscope */
    // =========================================================================
    std::atomic<double> getPlayPosition() { return playPosition; }
    juce::Atomic<bool> paramChangedForScope{ false };

	/* Frequency Response */
    // =========================================================================
    void pushNextSampleIntoFifo(float sample);
    
    enum
    {
        fftOrder = 12,
        fftSize = 1 << fftOrder,
        scopeSize = 512
    };
    
    float fifo[fftSize];
    float fftData[2 * fftSize];
    float scopeData[scopeSize];
    bool nextFFTBlockReady = false;
    int fifoIndex = 0;
    


private:

    /* Main DSP */
    void applyAmplitudeModulation();
    void sumAudioBands(juce::AudioBuffer<float>& buffer);
    
    
    void updateState();			// Process Block, Update State
    
	void forceSynchronization(LFO &lfo);
    double playPosition;
    
    /* Parameter Change Detection Flags */
    // =========================================================================
    float multLow{ 1.f },       multMid{ 1.f },     multHigh{ 1.f };
    float oldMultLow{ 0.f },    oldMultMid{ 0.f },  oldMultHigh{ 0.f };
    bool hitPlay{ false };
    bool setLatchPlay{ false };
    bool setLatchStop{ true };
    bool rateChanged{ true };
    bool multChanged{ true };

    void attachParametersToLayout();
    void addParameterListeners();

    /* FrequencyResponse */
    // =========================================================================
    bool fftPickoffPointIsInput{ false };
    void pushSignalToFFT (juce::AudioBuffer<float> &buffer);
    
    /* BPM and Host Info */
    // =========================================================================
    juce::AudioPlayHead::CurrentPositionInfo hostInfo;        // To get BPM
    double lastSampleRate;        // To get Sample Rate
    juce::AudioPlayHead* playHead;        //i To get BPM
    
    /* Crossover */
    // =========================================================================
    void applyCrossover(const juce::AudioBuffer<float>& inputBuffer);    // Crossover Function
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter  LP1, AP2, HP1, LP2, HP2;
    juce::AudioParameterFloat* lowMidCrossover{ nullptr };  // Pointer to the APVTS
    juce::AudioParameterFloat* midHighCrossover{ nullptr }; // Pointer to the APVTS
    
    /* Input & Output Gain */
    // =========================================================================
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam{ nullptr };               // Pointer to the APVTS
    juce::AudioParameterFloat* outputGainParam{ nullptr };              // Pointer to the APVTS
    
    /* Input & Output Meters */
    // =========================================================================
    juce::LinearSmoothedValue<float> rmsLevelInputLeft, rmsLevelInputRight, rmsLevelOutputLeft, rmsLevelOutputRight;
    void updateMeterLevels ( juce::LinearSmoothedValue<float> &left, juce::LinearSmoothedValue<float> &right, juce::AudioBuffer<float> &buffer);
    
    // Function to apply input/output gain
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    
    /* Tremolo-Bands */
    // =========================================================================
    std::array<TremoloBand, 3> tremolos;        // Array of param-wrappers containing byp/mute/solo for each band
    TremoloBand& lowBandTrem = tremolos[0];     // Low Band
    TremoloBand& midBandTrem = tremolos[1];     // Mid Band
    TremoloBand& highBandTrem = tremolos[2];    // High Band
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;  // An array of 3 buffers which contain the filtered audio

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TertiaryAudioProcessor)
};
