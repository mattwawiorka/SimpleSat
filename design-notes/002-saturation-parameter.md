# 002 - Saturation Parameter and DSP

## Decision

Expose one automatable JUCE parameter named `Saturation` with ID `saturation`, and use it to drive simple C++ `tanh` saturation DSP.

## Why This Is a Good Design Move

One parameter keeps the plugin easy to understand, makes DAW automation and state restoration straightforward, and gives the WebView UI a single clear integration point.

## Context

Version 1 is intentionally small: the user should be able to load the plugin, turn the `Saturation` knob, and hear saturation. The DSP should be readable and reliable before adding more controls.

## Design Rules

- Use JUCE parameter/state handling, preferably `AudioProcessorValueTreeState`.
- The `Saturation` parameter must be automatable by the DAW.
- The `Saturation` value must be saved and restored by the DAW project.
- Keep the DSP implementation simple and readable.

## Implementation Notes

- Parameter ID: `saturation`
- Parameter name: `Saturation`
- Range: `0.0` to `1.0`
- Default: `0.0` or `0.2`
- Use C++ `tanh` waveshaping for the saturation stage.
- A simple drive mapping can use logic like:

```cpp
float drive = 1.0f + saturation * 20.0f;
float output = std::tanh(input * drive) / std::tanh(drive);
```

## Avoid

- Do not add extra parameters before the single-knob workflow is stable.
- Do not bypass JUCE parameter/state handling for DAW-facing state.
- Do not move DSP into the web layer.
