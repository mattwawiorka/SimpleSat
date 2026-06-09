# 000 - Plugin Scope

## Decision

SimpleSat version 1 targets standalone use on Windows and VST3 use in FL Studio or another VST3-compatible DAW.

## Why This Is a Good Design Move

Keeping the first release focused on standalone and VST3 keeps the build, testing, and plugin behavior small enough for a beginner JUCE project while still producing a real usable plugin.

## Context

The project goal is a minimal saturation plugin: load the plugin, turn the `Saturation` knob, and hear saturation. The first scope needs to support local development outside a DAW and normal DAW use without expanding into every plugin format or platform.

## Design Rules

- Keep version 1 focused on Windows standalone and VST3 builds.
- Prioritize behavior that works in both the standalone app and a DAW.
- Avoid adding plugin formats or platform targets until the core plugin path is stable.

## Implementation Notes

- `CMakeLists.txt` should continue producing both Standalone and VST3 targets.
- DAW-facing behavior should be verified through the VST3 target.
- Local UI and DSP iteration can use the standalone target.

## Avoid

- Do not broaden version 1 scope to unrelated plugin formats before the saturation workflow is reliable.
- Do not make standalone-only behavior that cannot work in a DAW.
