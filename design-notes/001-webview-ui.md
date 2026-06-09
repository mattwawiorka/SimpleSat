# 001 - WebView UI

## Decision

Build the plugin UI with a JUCE-hosted WebView from the beginning, using React, TypeScript, and Vite for the frontend.

## Why This Is a Good Design Move

The WebView approach lets the project use familiar web development tools while JUCE continues to own plugin lifecycle, audio processing, parameters, automation, and state.

## Context

Version 1 needs a small, clear interface for one `Saturation` parameter. The UI should be simple enough to ship early, but it should also exercise the native/web bridge that the project intends to keep.

## Design Rules

- Keep audio processing and DAW-facing behavior in C++.
- Let React own rendering and user interaction only.
- Use the `Saturation` parameter as the bridge between React and JUCE.
- Keep version 1 UI minimal.

## Implementation Notes

- The first UI should include one large `Saturation` knob and a clear `Saturation` label.
- A current value display is acceptable if it stays simple.
- Minimal audio-plugin styling is preferred.
- The WebView bridge should send and receive parameter updates rather than duplicating parameter state in the UI.

## Avoid

- Do not add meters, presets, complex animations, or extra controls in version 1.
- Do not process audio in React.
- Do not make the UI state independent from the JUCE parameter.
