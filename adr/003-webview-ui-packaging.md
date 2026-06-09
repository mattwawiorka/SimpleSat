# ADR 003: Embed WebView UI Assets

## Context

SimpleSat uses a React/Vite UI inside a JUCE WebView.

Loading the UI from the source tree works during local development:

```text
repo/web/dist
```

But a copied `.vst3` should not depend on the source repo existing at runtime. The plugin needs to carry its WebView HTML/CSS/JS with it.

## Decision

Embed the built frontend assets into the plugin binary.

## Implementation Notes

- Build the React/Vite UI into `web/dist`.
- Use `juce_add_binary_data` in CMake to embed those files as `SimpleSatWebAssets`.
- Link `SimpleSatWebAssets` into the plugin target.
- Serve embedded assets through `WebBrowserComponent::withResourceProvider`.
- Keep `SIMPLE_SAT_DEV_SERVER` as a local development override for Vite hot reload.

The current CMake flow expects `web/dist` to exist before native configure/build. If it is missing, CMake fails with a clear error instead of producing a plugin with a blank UI.

## Outcomes

- A copied `.vst3` can load its UI without the source repo.
- WebView loading avoids fragile `file://` paths.
- Frontend changes require rebuilding `web/dist` before rebuilding the plugin.
