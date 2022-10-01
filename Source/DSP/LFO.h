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
    

    


    void initializeLFO(double sampleRate);
    void updateLFO(double sampleRate, double hostBPM);
    void setPosition(float newPosition) {mPosition = newPosition; }
    
    int getWaveInvert() { return mInvert; }
    int getWaveform() { return mWaveshapeID; }
    
    bool isSyncedToHost() { return syncToHostParam->get(); }
    
    float getWaveSkew() { return mSymmetry; }
    float getWaveDepth() { return mDepth;  }
    float getWaveMultiplier() { return mMultiplier; }
    float getWaveRate() { return mRate; }
    float getRelativePhase() { return mRelativePhase; }
    float getHostBPM() { return mHostBPM; }
    float getWaveTableSize() {return waveTableMapped.size(); }
    float getIncrement() {return mIncrement;}
    float getPosition() {return mPosition; }
    
    
    
    /* Returns the waveTable to AudioProcessor as [0, 1] */
    juce::Array<float>& getWaveTableScaled() {return waveTableMapped; }
    
    /* Returns the waveTable to Display as [-0.5 to +0.5] */
    juce::Array<float>& getWaveTable() {return waveTable; }

private:
    
    juce::Array<float> waveTable;               // Array to store raw LFO waveshape
    juce::Array<float> waveTableMapped;         // Mapped copy of waveTable so that max is always 1.0

    int wtScalar{ 2 };          // Used to set the size of the LFO WaveTable
    int mInvert{ 1 };           // -1 or +1. Multiplied by the WaveTable to invert or not.
    int mWaveshapeID{ 0 };      // Used to select which of 7 waveforms are used
    float mDepth{ 1.0f };       // Stores the current LFO amplitude
    float mSymmetry{ 0.5f };    // Expressed as percentage, used to scale or compress half of the LFO's period
    float mRelativePhase = 0;   // Used to shift phase of LFO relative to others
    float mIncrement = 1.f;        // Amount By Which To Increment In LFO Cycle

    float mMultiplier = 10.f;    // Rhythm of LFO timing (Rename to Rhythm)
    float mHostBPM{ 1.f };      // Stores the current bpm of the host application, when applicable.
    float min{ 2.0f };            // Used to Map LFO Amplitudes
    float max{ -1.0f };            // Used to Map LFO Amplitudes
    float mRate{ 1.0f };        // Stores the current LFO rate.
    
    float mPosition = 0.f;            // Used by processBlock to note the current position in reading the LFO cycle.
    
    void setTempo(float hostBPM, double sampleRate);
    void setWaveShape(double sampleRate);
    void mapWaveShape();
    void setWaveShapeRampDown(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeRampUp(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeSquare(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeTriangle(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeSine(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeHumpUp(int periodLeft, int periodRight, int waveTableSize);
    void setWaveShapeHumpDown(int periodLeft, int periodRight, int waveTableSize);
};
