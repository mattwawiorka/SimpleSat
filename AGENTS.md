# AGENTS.md

## Project Summary

SimpleSat is a beginner JUCE audio plugin project.

The goal is to build a real VST3 saturation plugin with a WebView-based React UI.

Version 1 should stay intentionally small:

```text
Load plugin → turn Saturation knob → hear saturation
```

## Requirements

- Pass audio correctly
- Expose one automatable parameter called `Saturation`
- Apply simple saturation DSP in C++
- Display a WebView-based React UI
- Let the React knob control the underlying JUCE/C++ parameter
- Work standalone
- Work in a DAW
- Save and restore the parameter value within the DAW project

See adr/000-scope.md for scope details.

## Architecture

Use a split native/web architecture.

### Native layer:

- JUCE 8, C++, CMake
- Builds VST3 and Standalone
- Owns plugin lifecycle, DSP, parameters, DAW automation/state, and WebView hosting

### Web layer:

- React, TypeScript, Vite, HTML/CSS
- Owns UI rendering and user interaction
- Sends/receives parameter updates through the JUCE WebView bridge

### Rules:

- All DSP and DAW-facing behavior stays in C++.
- React must not process audio.
- The `Saturation` parameter is the bridge between UI and DSP.

Relevant ADRs:

- `adr/001-webview-ui.md`
- `adr/002-saturation.md`

## Repo Layout

- `Source/` — JUCE/C++ plugin processor, editor, DSP, and WebView host code.
- `web/` — React/TypeScript/Vite frontend for the plugin UI.
- `adr/` — architecture decision records. Read relevant ADRs before changing architecture, DSP, parameters, WebView integration, or build targets.
- `CMakeLists.txt` — native JUCE/CMake build configuration.

## Development Build Instructions

### Required tools:

- JUCE 8
- CMake
- Visual Studio Build Tools / MSVC on Windows
- Node.js and npm

### Frontend build:

From the `web` folder:

```bash
npm install
npm run dev
npm run build
```

The Vite dev server should usually run at:

```text
http://localhost:5173
```

During development, the JUCE WebView may load the Vite dev server so the UI can be iterated quickly with hot reload.

### Native build:

From the repo root:

```bash
cmake -B build
cmake --build build --config Release
```

The build should produce both:

- A VST3 plugin for FL Studio and other VST3-compatible DAWs
- A Standalone app for local development and testing

## ADRs

Read relevant ADRs before changing architecture, DSP, parameters, WebView integration, or build targets.

If a change introduces a meaningful architecture decision, add or update an ADR.
