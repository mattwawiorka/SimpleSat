#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "PluginProcessor.h"

class SimpleSatAudioProcessorEditor final
    : public juce::AudioProcessorEditor,
      private juce::AudioProcessorValueTreeState::Listener,
      private juce::Timer
{
public:
    explicit SimpleSatAudioProcessorEditor (SimpleSatAudioProcessor& processor);
    ~SimpleSatAudioProcessorEditor() override;

    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void timerCallback() override;
    void sendSaturationToWeb (float value);
    juce::String getWebEntryUrl() const;

    SimpleSatAudioProcessor& audioProcessor;
    juce::WebBrowserComponent webView;
    std::atomic<float> latestSaturation { 0.0f };
    std::atomic<bool> shouldSendSaturation { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSatAudioProcessorEditor)
};
