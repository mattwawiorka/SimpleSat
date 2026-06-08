# SimpleSat

SimpleSat is a saturation knob.

It can be used as VST plugin within your DAW.

It adds warmth, harmonic color, and soft clipping to an incoming audio signal. At lower settings, it can be used for subtle warmth. At higher settings, it can create more obvious grit, density, and soft clipping.

```text
Load plugin → turn knob → hear saturation
```

## Building

Build the WebView UI before configuring or building the native plugin. The native CMake build embeds the files from `web/dist` into the plugin binary using JUCE BinaryData, so the copied `.vst3` does not need the source repo's `web/dist` folder at runtime.

```powershell
cd web
npm install
npm run build
cd ..
```

Then configure and build the JUCE plugin:

```powershell
cmake -B build -DJUCE_PATH=external/JUCE
cmake --build build --config Release
```

After changing frontend code, run `npm run build` again before rebuilding the native plugin so the embedded UI assets are refreshed.

For frontend hot reload during local development, run the Vite dev server and set `SIMPLE_SAT_DEV_SERVER` to the dev server URL before launching the standalone plugin. Production builds load the embedded assets by default.
