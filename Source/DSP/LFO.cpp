/*
  ==============================================================================

    LFO.cpp
    Created: 1 Oct 2022 3:37:28pm
    Author:  Joe Caulfield

  ==============================================================================
*/

#include "LFO.h"

/* Called once upon runtime */
//==============================================================================
void LFO::initializeLFO(double sampleRate)
{
    waveTable.resize(sampleRate / wtScalar);
    waveTable.clearQuick();

    waveTableMapped.resize(sampleRate / wtScalar);
    waveTableMapped.clearQuick();
}

/* Called when LFO parameters are changed or updated */
//==============================================================================
void LFO::updateLFO(double sampleRate, double hostBPM)
{
    setWaveShape(sampleRate);
    setTempo(hostBPM, sampleRate);
}

/* Updates the Time-Domain settings of the LFO */
//==============================================================================
void LFO::setTempo(float hostBPM, double sampleRate)
{
    mHostBPM = hostBPM;

    // Get the manual-rate parameter
    mRate = rateParam->get();

    // Get the Relative Phase Shift
    mRelativePhase = relativePhaseParam->get() / 360.0f;

    // Phase Shift is + / - 180 Degrees
    if (mRelativePhase <= 0)    // 0 to +180
        mRelativePhase = abs(mRelativePhase) * sampleRate;
    else                        // 0 to -180
        mRelativePhase = sampleRate * (1 - mRelativePhase);

    switch (multiplierParam->getIndex())     // Convert Multiplier Choice into Float
    {
        case 0: mMultiplier = 0.5f; break;
        case 1: mMultiplier = 1.0f; break;
        case 2: mMultiplier = 1.5f; break;
        case 3: mMultiplier = 2.0f; break;
        case 4: mMultiplier = 3.0f; break;
        case 5: mMultiplier = 4.0f; break;
    }

    if (syncToHostParam->get())  // LFO rate is BPM * Rhythm
        mIncrement = (mHostBPM / (60.0f*wtScalar)) * mMultiplier;    // Converting
    else
        mIncrement = mRate / wtScalar;

}

/* Called when change in LFO shape is prompted */
//==============================================================================
void LFO::setWaveShape(double sampleRate)
{
    using namespace juce;

    auto waveTableSize = sampleRate / wtScalar;

    /* Get the Symmetry(Skew) Parameter and convert into sample-lengths of each period */
    mSymmetry = symmetryParam->get();
    
    auto periodLeft = waveTableSize * mSymmetry / 100.f;            // Convert Symmetry Float into Sample Ranges
    auto periodRight = waveTableSize * (1 - mSymmetry / 100.0f);    // Convert Symmetry Float into Sample Ranges

    /* Get the Depth selection */
    mDepth = depthParam->get() / 100.0f;

    /* Check whether the LFO should be inverted or not */
    mInvert = invertParam->get();
    
    switch (mInvert)
    {
        case 0: mInvert = 1; break;
        case 1: mInvert = -1; break;
    }
    
    /* Check for waveshape selection and update wavetable */
    mWaveshapeID = waveshapeParam->getIndex();
    
    switch (mWaveshapeID)
    {
        case 0: setWaveShapeRampDown    (periodLeft, periodRight, waveTableSize); break;
        case 1: setWaveShapeRampUp      (periodLeft, periodRight, waveTableSize); break;
        case 2: setWaveShapeSquare      (periodLeft, periodRight, waveTableSize); break;
        case 3: setWaveShapeTriangle    (periodLeft, periodRight, waveTableSize); break;
        case 4: setWaveShapeSine        (periodLeft, periodRight, waveTableSize); break;
        case 5: setWaveShapeHumpDown    (periodLeft, periodRight, waveTableSize); break;
        case 6: setWaveShapeHumpUp      (periodLeft, periodRight, waveTableSize); break;
    }

    /* Map waveshape amplitudes from [-0.5 to +0.5] to [0 to +1] */
    mapWaveShape();

}

/* Set LFO waveshape to 'Ramp Down' */
//==============================================================================
void LFO::setWaveShapeRampDown(int periodLeft, int periodRight, int waveTableSize)
{
    float deltaDown = 0.01f;

    for (float i = 0; i < waveTableSize; i++)
    {
        // Corner-Rounding Function
        float y = ((1 / atan(1 / deltaDown)) * atan(sin(juce::MathConstants<double>::pi * (i) / waveTableSize) / deltaDown));

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

/* Set LFO waveshape to 'Ramp Up' */
//==============================================================================
void LFO::setWaveShapeRampUp(int periodLeft, int periodRight, int waveTableSize)
{
    float deltaUp = 0.01f;          // Constant for Edge Rounding
    
    for (float i = 0; i < waveTableSize; i++)
    {
        // Corner-Rounding Function
        float y = ((1 / atan(1 / deltaUp)) * atan(sin(juce::MathConstants<double>::pi * (i) / waveTableSize) / deltaUp));

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

/* Set LFO waveshape to 'Square' */
//==============================================================================
void LFO::setWaveShapeSquare(int periodLeft, int periodRight, int waveTableSize)
{
    float deltaPulse = 0.01f;    // Rounding Function

    if (mWaveshapeID == 2)
    {
        for (float i = 0; i < periodLeft; i++)
        {
            float y = mInvert * (0.5 + ((0.5 / atan(1 / deltaPulse)) * atan(sin(juce::MathConstants<double>::pi* i / periodLeft) / deltaPulse)));
            waveTable.set(i, y);
        }
        for (float i = periodLeft; i < waveTableSize; i++)
        {
            float y = mInvert * (0.5 - ((0.5 / atan(1 / deltaPulse)) * atan(sin(juce::MathConstants<double>::pi * (i - periodLeft) / periodRight) / deltaPulse)));
            waveTable.set(i, y);
        }
    }
}

/* Set LFO waveshape to 'Triangle' */
//==============================================================================
void LFO::setWaveShapeTriangle(int periodLeft, int periodRight, int waveTableSize)
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

/* Set LFO waveshape to 'Sine' */
//==============================================================================
void LFO::setWaveShapeSine(int periodLeft, int periodRight, int waveTableSize)
{
        // Populate the Left-Hand Period
        for (int i = 0; i < periodLeft; i++)
        {
            float y = mInvert * ((0.5f) + (0.5f) * sin(juce::MathConstants<double>::pi * i / (periodLeft)));
            waveTable.set(i, y);
        }

        // Populate the Right-Hand Period
        for (int i = periodLeft; i < waveTableSize; i++)
        {
            float y = mInvert * ((0.5f) + (0.5f) * -sin(juce::MathConstants<double>::pi * (i - periodLeft) / (periodRight)));
            waveTable.set(i, y);
        }
}

/* Set LFO waveshape to 'Hump Up' ... Upsidedown U-Shape */
//==============================================================================
void LFO::setWaveShapeHumpDown(int periodLeft, int periodRight, int waveTableSize)
{
    // Populate the Left-Hand Period
    for (int i = 0; i < periodLeft; i++)
    {
        float y = mInvert * ((-sin(0.5f * juce::MathConstants<double>::pi* i / (periodLeft)) + 1));
        waveTable.set(i, y);
    }

    // Populate the Right-Hand Period
    for (int i = periodLeft; i < waveTableSize; i++)
    {
        float y = mInvert * ((-cos(0.5f * juce::MathConstants<double>::pi * (i - periodLeft) / (periodRight)) + 1));
        waveTable.set(i, y);
    }
}

/* Set LFO waveshape to 'Hump Down' ... U-Shape */
//==============================================================================
void LFO::setWaveShapeHumpUp(int periodLeft, int periodRight, int waveTableSize)
{
    // Populate the Left-Hand Period
    for (int i = 0; i < periodLeft; i++)
    {
        float y = mInvert * (sin(0.5f * juce::MathConstants<double>::pi * i / (periodLeft)));
        waveTable.set(i, y);

    }

    // Populate the Right-Hand Period
    for (int i = periodLeft; i < waveTableSize; i++)
    {
        float y = mInvert * (cos(0.5f * juce::MathConstants<double>::pi * (i - periodLeft) / (periodRight)));
        waveTable.set(i, y);
    }
}

/* Maps waveshape amplitudes from [-0.5 to +0.5] to [0 to +1]
 Determines current minimum and maximum amplitude values
 and scales those to fit proportionately within [0, +1] */
//==============================================================================
void LFO::mapWaveShape()
{
    min = 2.0f;
    max = -1.0f;

    for (int sample = 0; sample < waveTable.size(); sample++)
    {
        if (waveTable[sample] < min)
            min = waveTable[sample];

        if (waveTable[sample] > max)
            max = waveTable[sample];
    }

    /* Scale so max value is always 1; minimum value is 1 minus amplitude */
    for (int i = 0; i < waveTable.size(); i++)
    {
        float value = juce::jmap<float>(waveTable[i], min, max, 1.0f - mDepth, 1.0f);
        waveTableMapped.set(i, value);
    }
}


