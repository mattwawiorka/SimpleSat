#pragma once

#include <juce_dsp/juce_dsp.h>

class SaturationDSP
{
public:
    void prepare (double sampleRate, int maximumBlockSize, int numberOfChannels);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, float saturation);
    int getLatencySamples() const;

private:
    void processOversampledBlock (juce::dsp::AudioBlock<float>& block, float targetSaturation);
    float shapeSample (float input, float saturation) const;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    juce::SmoothedValue<float> smoothedSaturation;
    double currentSampleRate = 44100.0;
};
