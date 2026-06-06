#pragma once

#include <juce_dsp/juce_dsp.h>

class SaturationDSP
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int numberOfChannels);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, float saturation);

private:
    float shapeSample (float input, float saturation) const;
};
