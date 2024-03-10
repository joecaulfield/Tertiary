/*
  ==============================================================================

    LFO.h
    Created: 1 Oct 2022 3:37:28pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/* Instance of LFO which drives the amplitude modulation of its corresponding audio path */
struct LFO
{
public:
    /* Pointer to the APVTS */
    juce::AudioParameterFloat* symmetryParam{ nullptr };
    juce::AudioParameterFloat* depthParam{ nullptr };
    juce::AudioParameterFloat* relativePhaseParam{ nullptr };
    juce::AudioParameterFloat* rateParam{ nullptr };
    juce::AudioParameterChoice* waveshapeParam{ nullptr };
    juce::AudioParameterChoice* multiplierParam{ nullptr };
    juce::AudioParameterBool* invertParam{ nullptr };
    juce::AudioParameterBool* syncToHostParam{ nullptr };



    //juce::Array<float> waveTableDisplayScaled;  // For Display: Scale Time by 2, Shift by Quarter-Wave
    //juce::Array<float> waveTableDisplay;        // For Display: Map Amplitudes between [-0.5 and +0.5] rather than [0 and 1]



    void setLfoID(int id) { lfoID = id; }
    int getLfoID() { return lfoID; }

    void initializeLFO(double sampleRate);
    void updateLFO(double sampleRate, double hostBPM);


    float getHostBPM() { return mHostBPM; }

    float getSampleRate() { return mSampleRate; }

    float getWaveTableSize() { return waveTableMapped.size(); }

    float getIncrement() { return mIncrement; }

    float getPosition() { return mPosition; }
    void setPosition(float newPosition) { mPosition = newPosition; }

    void setDsp(bool isProcessor) { isLinkedToDsp = isProcessor; }







    // LFO PARAMETERS
    // =============================================================
    float   getWaveRate()       { return mRate; }
    float   getRelativePhase()  { return mRelativePhase; }
    float   getWaveMultiplier() { return mMultiplierValue; }
    bool    isSyncedToHost()    { return mIsSyncedToHost; }
    float   getWaveSkew()       { return mSymmetry; }
    float   getWaveDepth()      { return mDepth; }
    int     getWaveInvert()     { return mInvert; }
    int     getWaveform()       { return mWaveShapeChoice; }


    void setWaveRate(float rate);
    void setRelativePhase(float phase);
    void setWaveMultiplier(int multChoice);
    void setSyncedToHost(bool isSyncedToHost);
    void setWaveSkew(float skew);
    void setWaveDepth(float depth);
    void setWaveInvert(bool isInverted);
    void setWaveform(int waveChoice);

    float   getRelativePhaseInSamples() { return mRelativePhaseInSamples; }






    /* Returns the waveTable to AudioProcessor as [0, 1] */
    juce::Array<float>& getWaveTableScaled() { return waveTableMapped; }

    /* Returns the waveTable to Display as [-0.5 to +0.5] */
    juce::Array<float>& getWaveTable() { return waveTable; }

    /* Downsamples the WaveTable and returns to Oscilloscope */
    juce::Array<float>& getWaveTableForDisplay(int amountToDownSample);

private:

    int lfoID = 0;

    bool isLinkedToDsp{ true };

    juce::Array<float> waveTable;           // Array to store raw LFO waveshape
    juce::Array<float> waveTableMapped;     // Mapped copy of waveTable so that max is always 1.0
    juce::Array<float> waveTableForDisplay; // Downsampled copy of waveTable

    int wtScalar{ 2 };          // Used to set the size of the LFO WaveTable



    double mSampleRate = 0.f;
    float mHostBPM{ 1.f };      // Stores the current bpm of the host application, when applicable.


    // LFO PARAMETERS
    // ==========================================================================
    float mDepth{ 1.0f };           // Stores the current LFO amplitude
    float mSymmetry{ 0.5f };        // Expressed as percentage, used to scale or compress half of the LFO's period
    float mRelativePhase = 0.f;       // Used to shift phase of LFO relative to others
    float mRelativePhaseInSamples = 0.f;       // Used to shift phase of LFO relative to others
    int mInvert{ 1 };               // 0 or 1 boolean.
    float mInvertValue{ 1.f };      // -1 or +1.  Multiplied by the WaveTable to invert or not.
    int mWaveShapeChoice{ 0 };      // Used to select which of 7 waveforms are used
    int mMultiplierChoice = 1;      // Rhythm of LFO timing (Rename to Rhythm)
    bool mIsSyncedToHost{ false };
    float mRate{ 1.0f };            // Stores the current LFO rate.


    float min{ 2.0f };          // Used to Map LFO Amplitudes
    float max{ -1.0f };         // Used to Map LFO Amplitudes
    float mIncrement = 1.f;     // Amount By Which To Increment In LFO Cycle
    float mPosition = 0.f;      // Used by processBlock to note the current position in reading the LFO cycle.
    float mMultiplierValue;       // Converts mMultiplierChoice into mathematical value


    void getProcessorParams();

    void setTempo(float hostBPM, double sampleRate);
    void calculateWaveShape(double sampleRate);
    void scaleWaveShape();
    void setWaveShapeRampDown(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeRampUp(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeSquare(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeTriangle(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeSine(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeHumpUp(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeHumpDown(int periodLeft, int periodRight, int waveTableSize);
};
