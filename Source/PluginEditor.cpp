#include "PluginEditor.h"

namespace
{
juce::String getMimeType (const juce::File& file)
{
    const auto extension = file.getFileExtension().toLowerCase();

    if (extension == ".html")
        return "text/html";

    if (extension == ".js")
        return "text/javascript";

    if (extension == ".css")
        return "text/css";

    if (extension == ".json")
        return "application/json";

    if (extension == ".svg")
        return "image/svg+xml";

    return "application/octet-stream";
}

std::optional<juce::WebBrowserComponent::Resource> getWebResource (const juce::String& requestedPath)
{
    auto relativePath = requestedPath == "/" ? juce::String { "index.html" }
                                             : requestedPath.fromFirstOccurrenceOf ("/", false, false);

    if (relativePath.contains (".."))
        return std::nullopt;

    const auto file = juce::File (SIMPLE_SAT_WEB_ROOT)
        .getChildFile ("dist")
        .getChildFile (relativePath);

    if (! file.existsAsFile())
        return std::nullopt;

    juce::MemoryBlock data;

    if (! file.loadFileAsData (data))
        return std::nullopt;

    std::vector<std::byte> bytes (data.getSize());
    std::memcpy (bytes.data(), data.getData(), data.getSize());

    return juce::WebBrowserComponent::Resource { std::move (bytes), getMimeType (file) };
}

juce::WebBrowserComponent::Options createBrowserOptions (SimpleSatAudioProcessor& processor)
{
    return juce::WebBrowserComponent::Options {}
        .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options (
            juce::WebBrowserComponent::Options::WinWebView2 {}
                .withUserDataFolder (juce::File::getSpecialLocation (juce::File::tempDirectory))
                .withStatusBarDisabled())
        .withNativeIntegrationEnabled()
        .withNativeFunction (
            "setSaturation",
            [&processor] (const juce::Array<juce::var>& arguments,
                          juce::WebBrowserComponent::NativeFunctionCompletion completion)
            {
                if (! arguments.isEmpty())
                    processor.setSaturationFromUi (static_cast<float> (static_cast<double> (arguments[0])));

                completion (processor.getSaturation());
            })
        .withNativeFunction (
            "getSaturation",
            [&processor] (const juce::Array<juce::var>& arguments,
                          juce::WebBrowserComponent::NativeFunctionCompletion completion)
            {
                juce::ignoreUnused (arguments);
                completion (processor.getSaturation());
            })
        .withResourceProvider ([] (const auto& url)
            {
                return getWebResource (url);
            });
}
}

SimpleSatAudioProcessorEditor::SimpleSatAudioProcessorEditor (SimpleSatAudioProcessor& processor)
    : AudioProcessorEditor (&processor),
      audioProcessor (processor),
      webView (createBrowserOptions (processor))
{
    setSize (420, 520);

    addAndMakeVisible (webView);
    webView.goToURL (getWebEntryUrl());

    latestSaturation.store (audioProcessor.getSaturation());
    audioProcessor.getValueTreeState().addParameterListener (
        SimpleSatAudioProcessor::saturationParameterId, this);

    startTimerHz (30);
}

SimpleSatAudioProcessorEditor::~SimpleSatAudioProcessorEditor()
{
    stopTimer();
    audioProcessor.getValueTreeState().removeParameterListener (
        SimpleSatAudioProcessor::saturationParameterId, this);
}

void SimpleSatAudioProcessorEditor::resized()
{
    webView.setBounds (getLocalBounds());
}

void SimpleSatAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == SimpleSatAudioProcessor::saturationParameterId)
    {
        latestSaturation.store (newValue);
        shouldSendSaturation.store (true);
    }
}

void SimpleSatAudioProcessorEditor::timerCallback()
{
    if (shouldSendSaturation.exchange (false))
        sendSaturationToWeb (latestSaturation.load());
}

void SimpleSatAudioProcessorEditor::sendSaturationToWeb (float value)
{
    webView.emitEventIfBrowserIsVisible (
        "saturationChanged",
        juce::var (juce::jlimit (0.0f, 1.0f, value)));
}

juce::String SimpleSatAudioProcessorEditor::getWebEntryUrl() const
{
    const auto devServer = juce::SystemStats::getEnvironmentVariable (
        "SIMPLE_SAT_DEV_SERVER",
        {});

    if (devServer.isNotEmpty())
        return devServer;

    return juce::WebBrowserComponent::getResourceProviderRoot();
}
