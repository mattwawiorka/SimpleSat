#include "PluginEditor.h"

#include <SimpleSatWebAssets.h>

namespace
{
juce::String getMimeType (const juce::String& path)
{
    const auto extension = path.fromLastOccurrenceOf (".", true, false).toLowerCase();

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

juce::String getResourceFileName (const juce::String& requestedPath)
{
    if (requestedPath == "/")
        return "index.html";

    return requestedPath.fromLastOccurrenceOf ("/", false, false);
}

std::optional<juce::String> findBinaryDataResourceName (const juce::String& requestedFileName)
{
    for (auto index = 0; index < SimpleSatWebAssets::namedResourceListSize; ++index)
    {
        if (requestedFileName == SimpleSatWebAssets::originalFilenames[index])
            return SimpleSatWebAssets::namedResourceList[index];
    }

    return {};
}

std::optional<juce::WebBrowserComponent::Resource> getWebResource (const juce::String& requestedPath)
{
    const auto requestedFileName = getResourceFileName (requestedPath);

    if (requestedFileName.isEmpty() || requestedFileName.contains (".."))
        return std::nullopt;

    const auto resourceName = findBinaryDataResourceName (requestedFileName);

    if (! resourceName.has_value())
        return std::nullopt;

    auto dataSize = 0;
    const auto* data = SimpleSatWebAssets::getNamedResource (resourceName->toRawUTF8(), dataSize);

    if (data == nullptr || dataSize <= 0)
        return std::nullopt;

    std::vector<std::byte> bytes (static_cast<size_t> (dataSize));
    std::memcpy (bytes.data(), data, static_cast<size_t> (dataSize));

    return juce::WebBrowserComponent::Resource { std::move (bytes), getMimeType (requestedFileName) };
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
