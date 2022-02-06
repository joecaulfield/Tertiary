/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP\Params.h"

struct TremoloBand
{
    juce::AudioParameterBool* bypass{ nullptr };            // Pointer to the APVTS
    juce::AudioParameterBool* mute{ nullptr };              // Pointer to the APVTS
    juce::AudioParameterBool* solo{ nullptr };              // Pointer to the APVTS
    juce::AudioParameterFloat* bandGainParam{ nullptr };    // Pointer to the APVTS

    juce::dsp::Gain<float> bandGain;                        

    void updateTremoloSettings()
    {
        bandGain.setRampDurationSeconds(0.05);              // Reduce Artifacts
        bandGain.setGainDecibels(bandGainParam->get());     // Update Gain
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        
    }

};

struct LFO
{

	// Pointers to the APVTS Parameters
    juce::AudioParameterFloat* symmetry{ nullptr };     
    juce::AudioParameterFloat* depth{ nullptr };        
    juce::AudioParameterChoice* waveshape{ nullptr };   
    juce::AudioParameterBool* invert{ nullptr };        
    juce::AudioParameterFloat* relativePhase{ nullptr };
	juce::AudioParameterFloat* displayPhase{ nullptr };	

    juce::AudioParameterFloat* rate{ nullptr };         
    juce::AudioParameterBool* syncToHost{ nullptr };    
    juce::AudioParameterChoice* multiplier{ nullptr };  


    juce::Array<float> waveTable;                           // Array to store raw LFO waveshape
    juce::Array<float> waveTableMapped;                     // Mapped copy of waveTable so that max is always 1.0
	juce::Array<float> waveTableDisplayScaled;				// For Display: Scale Time by 2, Shift by Quarter-Wave
	juce::Array<float> waveTableDisplay;					// For Display: Map Amplitudes to -/+ 0.5

    float increment = 1;		// Used to increment LFO samples within Processor
    float phase = 0;			// Used to increment LFO samples within Processor

    float mRelativePhase = 0;	// Used to shift phase of LFO relative to others
	float mMultiplier = 1.f;	// Rhythm of LFO timing (Rename to Rhythm)
	float mDisplayPhase = 0.f;	// Used in Oscilloscope View-Scroll.  Shifts phase of all displayed Waves.

	float min{ 2.0f };		// Used to Map LFO Amplitudes
	float max{ -1.0f };		// Used to Map LFO Amplitudes
	float mDepth{ 1.0f };	// LFO Amplitude Depth

	void initializeLFO(double sampleRate)
	{
		waveTable.resize(sampleRate);   // Initialize Wavetable
		waveTable.clearQuick();         // Initialize Wavetbale

		waveTableMapped.resize(sampleRate);	// Initialize Wavetable
		waveTableMapped.clearQuick();		// Initialize Wavetbale

		waveTableDisplayScaled.resize(sampleRate);	// Initialize Wavetable
		waveTableDisplayScaled.clearQuick();        // Initialize Wavetbale

		waveTableDisplay.resize(sampleRate);	// Initialize Wavetable
		waveTableDisplay.clearQuick();			// Initialize Wavetbale
	}

    void updateLFO(double sampleRate, double hostBPM)
    {
        getWaveShape(sampleRate);
        getTempo(hostBPM, sampleRate);
		getWaveShapeDisplay(hostBPM, sampleRate);
    }

    void getTempo(float hostBPM, double sampleRate)
    {
        auto mHostBPM = hostBPM;

        auto mRate = rate->get();	// Get Manual Rate Status

        mRelativePhase = relativePhase->get() / 360.0f;    // Get Relative Phase Shift

        if (mRelativePhase <= 0)
        {
            mRelativePhase = abs(mRelativePhase) * sampleRate;
        }
        else
        {
            mRelativePhase = sampleRate * (1 - mRelativePhase);
        }

        mMultiplier = 1.0f;					// Multiplier Float
        switch (multiplier->getIndex())     // Convert Multiplier Choice into Float
        {
            case 0: mMultiplier = 0.5f; break;
            case 1: mMultiplier = 1.0f; break;
            case 2: mMultiplier = 1.5f; break;
            case 3: mMultiplier = 2.0f; break;
            case 4: mMultiplier = 3.0f; break;
            case 5: mMultiplier = 4.0f; break;
        }

        if (syncToHost->get())  // BPM is Synced to Host
        {
            increment = (mHostBPM / 60.0f) * mMultiplier;
        }
        else
        {                   
            increment = mRate * mMultiplier;
        }

    }

    void getWaveShape(double sampleRate)
    {

        using namespace juce;

		auto waveTableSize = sampleRate;

        auto mWaveshapeID = waveshape->getIndex();  // Get the Waveshape Index
        auto mSymmetry = symmetry->get();           // Get Symmetry Value
        mDepth = depth->get() / 100.0f;				// Get Depth Value

        auto periodLeft = waveTableSize * mSymmetry / 100.f;            // Convert Symmetry Float into Sample Ranges
        auto periodRight = waveTableSize * (1 - mSymmetry / 100.0f);    // Convert Symmetry Float into Sample Ranges

        int mInvert = 1;	// Convert Invert Choice into +/- Multiplier
        switch (invert->get())
        {
            case 0: mInvert = 1; break;
            case 1: mInvert = -1; break;
        }

        // RAMP UP =====================================================================================

        float deltaUp = 0.01f;          // Constant for Edge Rounding

        if (mWaveshapeID == 0)
        {
            for (float i = 0; i < waveTableSize; i++)
            {
				// Rounding Function
                float y = ((1 / atan(1 / deltaUp)) * atan(sin(double_Pi * (i) / waveTableSize) / deltaUp));
                
				// Waveshape Function
                float x;

				// Populate the Left-Hand Period
                if (i < periodLeft)     
                    x = mInvert * ((0.5f) * (i / periodLeft));

				// Populate the Right-Hand Period
                if (i >= periodLeft)    
                    x = mInvert * ((0.5f) + (0.5f) * (i - periodLeft) / (periodRight));

                waveTable.set(i, x * y);
            }
        }

        // RAMP DOWN =====================================================================================

        float deltaDown = 0.01f;

        if (mWaveshapeID == 1)
        {
            for (float i = 0; i < waveTableSize; i++)
            {
				// Rounding Function
                float y = ((1 / atan(1 / deltaDown)) * atan(sin(double_Pi * (i) / waveTableSize) / deltaDown));
                
				// Waveshape Function
				float x;

				// Populate the Left-Hand Period
                if (i < periodLeft)
                    x = mInvert * (1 + (0.5f) * (-i / periodLeft));

				// Populate the Right-Hand Period
                if (i >= periodLeft)
                    x = mInvert * ((0.5f) + (0.5f) * (-(i - periodLeft) / (periodRight)));

                waveTable.set(i, y * x);
            }
        }

        // SQUARE =====================================================================================

        float deltaPulse = 0.01f;	// Rounding Function

        if (mWaveshapeID == 2)
        {
            for (float i = 0; i < periodLeft; i++)
            {
                float y = mInvert * (0.5 + ((0.5 / atan(1 / deltaPulse)) * atan(sin(double_Pi * i / periodLeft) / deltaPulse)));
                waveTable.set(i, y);
            }
            for (float i = periodLeft; i < waveTableSize; i++)
            {
                float y = mInvert * (0.5 - ((0.5 / atan(1 / deltaPulse)) * atan(sin(double_Pi * (i - periodLeft) / periodRight) / deltaPulse)));
                waveTable.set(i, y);
            }
        }

        // TRIANGLE =====================================================================================
        if (mWaveshapeID == 3)
        {
			// Populate the Left-Hand Period
            for (int i = 0; i < periodLeft; i++)
            {
                float y = mInvert * ((i / periodLeft));
				waveTable.set(i, y);
            }

			// Populate the Right-Hand Period
            for (int i = periodLeft; i < waveTableSize; i++)
            {
                float y = mInvert * ((1 - ((i - periodLeft) / periodRight)));
                waveTable.set(i, y);
            }
        }

        // SINE =====================================================================================
        if (mWaveshapeID == 4)
        {
			// Populate the Left-Hand Period
            for (int i = 0; i < periodLeft; i++)
            {
                float y = mInvert * ((0.5f) + (0.5f) * sin(double_Pi * i / (periodLeft)));
                waveTable.set(i, y);
            }

			// Populate the Right-Hand Period
            for (int i = periodLeft; i < waveTableSize; i++)
            {
                float y = mInvert * ((0.5f) + (0.5f) * -sin(double_Pi * (i - periodLeft) / (periodRight)));
                waveTable.set(i, y);
            }
        }

        // LUMPS =====================================================================================
        if (mWaveshapeID == 5)
        {
			// Populate the Left-Hand Period
            for (int i = 0; i < periodLeft; i++)
            {
                float y = mInvert * ((-sin(0.5f * double_Pi * i / (periodLeft)) + 1));
                waveTable.set(i, y);
            }

			// Populate the Right-Hand Period
            for (int i = periodLeft; i < waveTableSize; i++)
            {
                float y = mInvert * ((-cos(0.5f * double_Pi * (i - periodLeft) / (periodRight)) + 1));
                waveTable.set(i, y);
            }
        }

        // HUMPS =====================================================================================
        if (mWaveshapeID == 6)
        {
			// Populate the Left-Hand Period
            for (int i = 0; i < periodLeft; i++)
            {
                float y = mInvert * (sin(0.5f * double_Pi * i / (periodLeft)));
                waveTable.set(i, y);

            }

			// Populate the Right-Hand Period
            for (int i = periodLeft; i < waveTableSize; i++)
            {
                float y = mInvert * (cos(0.5f * double_Pi * (i - periodLeft) / (periodRight)));
                waveTable.set(i, y);
            }
        }

        // GET MIN AND MAX VALUES OF LFO TO USE IN JMAP

        min = 2.0f;		// Used to calculate min value of LFO, for scaling
        max = -1.0f;    // Used to calculate max value of LFO, for scaling

        for (int sample = 0; sample < waveTable.size(); sample++)
        {
            if (waveTable[sample] < min)
            {
                min = waveTable[sample];
            }
            if (waveTable[sample] > max)
            {
                max = waveTable[sample];
            }
        }

        // SCALE LFO SO MAX VALUE IS 1, MIN VALUE IS 1 MINUS AMPLITUDE
        for (int i = 0; i < waveTable.size(); i++)
        {
            float value = jmap<float>(waveTable[i], min, max, 1.0f - mDepth, 1.0f);
            waveTableMapped.set(i, value);
        }

    }

	// Conditions selected waveshape for oscilloscope.
	// Adds additional periods and centers them.
	// Shifts Phase by Relative-Phase Offset
	// Shifts Phase by Oscilloscope View-Scroll Offset
	// ** Need to add functionality to display changes in 'Rate'

	void getWaveShapeDisplay(float hostBPM, double sampleRate)
	{
		auto numCycles = mMultiplier*2+1;
		auto downSample = 1;
		mDisplayPhase = displayPhase->get() / 360.0f;    // Get Relative Phase Shift

		float mCorrectivePhase = 0.f;

		if (mMultiplier == 0.5f || mMultiplier == 1.5f)
			mCorrectivePhase = 0.5f * waveTable.size();

        if (mDisplayPhase <= 0)
			mDisplayPhase = abs(mDisplayPhase) * waveTable.size();
        else
			mDisplayPhase = waveTable.size() * (1 - mDisplayPhase);

		// DOWNSAMPLE, DISPLAY TWO CYCLES, SHIFT BY QUARTER WAVE + RELATIVE PHASE =====================

		auto x = 0;

		for (int i = 0; i < waveTable.size(); i++)
		{
			auto revolvingIndex = fmod(numCycles * i + mRelativePhase + mCorrectivePhase, waveTable.size());

			int displayIndex = fmod(i + mDisplayPhase, waveTable.size());

			waveTableDisplayScaled.set(displayIndex, waveTable[revolvingIndex]);
		}

		x = 0;

		// MAP AMPLITUDE TO CENTER AROUND 0 ===================
		for (int i = 0; i < waveTableDisplayScaled.size(); i++)
		{
			float value = juce::jmap<float>(waveTableDisplayScaled[i], min, max, -0.5f * mDepth, 0.5f * mDepth);
			waveTableDisplay.set(fmod(i, waveTableDisplayScaled.size()), value);
		}
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

    void parameterChanged(const juce::String& parameterID, float newValue);

    APVTS apvts{    *this,                                  // Audio Processor
                    nullptr,                                // Undo Manager
                    "Parameters",                           // Identifier &valueTreeType
                    createParameterLayout() };              // parameterLayout

	//juce::Array<float>& getWaveTable(int channel);
	juce::AudioPlayHead::CurrentPositionInfo hostInfo;		// To get BPM

	LFO lowLFO, midLFO, highLFO;	// Instances of LFO

	juce::Atomic<bool> paramChangedForScope{ false };

private:

    void updateState();			// Process Block, Update State

    double lastSampleRate;		// To get Sample Rate

    juce::AudioPlayHead* playHead;		// To get BPM


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
