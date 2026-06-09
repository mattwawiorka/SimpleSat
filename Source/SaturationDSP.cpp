#include "SaturationDSP.h"

#include <cmath>

void SaturationDSP::prepare (double sampleRate, int maximumBlockSize, int numberOfChannels, float initialSaturation)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    initialSaturation = juce::jlimit (0.0f, 1.0f, initialSaturation);

    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        static_cast<size_t> (juce::jmax (1, numberOfChannels)),
        2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true,
        true);

    oversampling->initProcessing (static_cast<size_t> (juce::jmax (1, maximumBlockSize)));
    oversampling->reset();

    smoothedSaturation.reset (currentSampleRate * static_cast<double> (oversampling->getOversamplingFactor()), 0.02);
    smoothedSaturation.setCurrentAndTargetValue (initialSaturation);
}

void SaturationDSP::reset()
{
    if (oversampling != nullptr)
        oversampling->reset();

    smoothedSaturation.reset (currentSampleRate * static_cast<double> (oversampling != nullptr ? oversampling->getOversamplingFactor() : 1), 0.02);
}

void SaturationDSP::process (juce::AudioBuffer<float>& buffer, float saturation)
{
    saturation = juce::jlimit (0.0f, 1.0f, saturation);

    if (oversampling == nullptr)
        return;

    auto block = juce::dsp::AudioBlock<float> { buffer };
    auto oversampledBlock = oversampling->processSamplesUp (block);

    processOversampledBlock (oversampledBlock, saturation);

    oversampling->processSamplesDown (block);
}

int SaturationDSP::getLatencySamples() const
{
    return oversampling != nullptr ? static_cast<int> (std::ceil (oversampling->getLatencyInSamples())) : 0;
}

void SaturationDSP::processOversampledBlock (juce::dsp::AudioBlock<float>& block, float targetSaturation)
{
    smoothedSaturation.setTargetValue (targetSaturation);

    const auto numberOfChannels = static_cast<int> (block.getNumChannels());
    const auto numberOfSamples = static_cast<int> (block.getNumSamples());

    for (int sample = 0; sample < numberOfSamples; ++sample)
    {
        const auto saturation = smoothedSaturation.getNextValue();

        for (int channel = 0; channel < numberOfChannels; ++channel)
        {
            auto* samples = block.getChannelPointer (static_cast<size_t> (channel));
            samples[sample] = shapeSample (samples[sample], saturation);
        }
    }
}

float SaturationDSP::shapeSample (float input, float saturation) const
{
    if (saturation <= 0.0001f)
        return input;

    const auto drive = 1.0f + saturation * 14.0f;
    const auto shaped = std::tanh (input * drive) / std::tanh (drive);
    const auto outputTrim = 1.0f / (1.0f + saturation * 0.35f);

    return shaped * outputTrim;
}
