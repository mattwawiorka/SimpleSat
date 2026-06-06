#include "SaturationDSP.h"

#include <cmath>

void SaturationDSP::prepare (double sampleRate, int maximumBlockSize, int numberOfChannels)
{
    juce::ignoreUnused (sampleRate, maximumBlockSize, numberOfChannels);
}

void SaturationDSP::reset()
{
}

void SaturationDSP::process (juce::AudioBuffer<float>& buffer, float saturation)
{
    saturation = juce::jlimit (0.0f, 1.0f, saturation);

    if (saturation <= 0.0f)
        return;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            samples[sample] = shapeSample (samples[sample], saturation);
    }
}

float SaturationDSP::shapeSample (float input, float saturation) const
{
    const auto drive = 1.0f + saturation * 20.0f;
    return std::tanh (input * drive) / std::tanh (drive);
}
