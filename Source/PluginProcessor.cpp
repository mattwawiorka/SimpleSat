#include "PluginProcessor.h"

#include "PluginEditor.h"

SimpleSatAudioProcessor::SimpleSatAudioProcessor()
    : AudioProcessor (BusesProperties()
          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "Parameters", createParameterLayout())
{
    saturationParameter = parameters.getRawParameterValue (saturationParameterId);
}

void SimpleSatAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    saturationDSP.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void SimpleSatAudioProcessor::releaseResources()
{
    saturationDSP.reset();
}

bool SimpleSatAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono()
        && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    return mainOutput == layouts.getMainInputChannelSet();
}

void SimpleSatAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    for (auto channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    saturationDSP.process (buffer, getSaturation());
}

juce::AudioProcessorEditor* SimpleSatAudioProcessor::createEditor()
{
    return new SimpleSatAudioProcessorEditor (*this);
}

bool SimpleSatAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String SimpleSatAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleSatAudioProcessor::acceptsMidi() const
{
    return false;
}

bool SimpleSatAudioProcessor::producesMidi() const
{
    return false;
}

bool SimpleSatAudioProcessor::isMidiEffect() const
{
    return false;
}

double SimpleSatAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleSatAudioProcessor::getNumPrograms()
{
    return 1;
}

int SimpleSatAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleSatAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SimpleSatAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SimpleSatAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void SimpleSatAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState().createXml())
        copyXmlToBinary (*state, destData);
}

void SimpleSatAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto state = getXmlFromBinary (data, sizeInBytes))
        if (state->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*state));
}

juce::AudioProcessorValueTreeState& SimpleSatAudioProcessor::getValueTreeState()
{
    return parameters;
}

float SimpleSatAudioProcessor::getSaturation() const
{
    return saturationParameter != nullptr ? saturationParameter->load() : 0.2f;
}

void SimpleSatAudioProcessor::setSaturationFromUi (float value)
{
    if (auto* parameter = parameters.getParameter (saturationParameterId))
    {
        const auto normalized = juce::jlimit (0.0f, 1.0f, value);
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost (normalized);
        parameter->endChangeGesture();
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleSatAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> layout;

    layout.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { saturationParameterId, 1 },
        "Saturation",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f },
        0.2f));

    return { layout.begin(), layout.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleSatAudioProcessor();
}
