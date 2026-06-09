# 003 - WebView UI Packaging

## Decision

Embed the built React/Vite frontend assets into the plugin binary for packaged plugin use.

## Why This Is a Good Design Move

A copied `.vst3` should load its UI without depending on the source repository or local filesystem paths. Embedded assets make the plugin portable and avoid a blank UI when the repo is not present.

## Context

During local development, loading the WebView UI from `web/dist` or a Vite dev server is useful. Packaged plugin builds need to carry their HTML, CSS, and JavaScript with the plugin itself.

## Design Rules

- Treat embedded WebView assets as the packaged-plugin path.
- Keep the Vite dev server path available only as a local development override.
- Fail clearly if required built frontend assets are missing during native configure/build.

## Implementation Notes

- Build the React/Vite UI into `web/dist`.
- Use `juce_add_binary_data` in CMake to embed those files as `SimpleSatWebAssets`.
- Link `SimpleSatWebAssets` into the plugin target.
- Serve embedded assets through `WebBrowserComponent::withResourceProvider`.
- Keep `SIMPLE_SAT_DEV_SERVER` as a local development override for Vite hot reload.

## Avoid

- Do not make a copied `.vst3` depend on `repo/web/dist` existing at runtime.
- Do not rely on fragile `file://` paths for packaged UI loading.
- Do not allow native builds to silently produce a plugin with a blank UI when frontend assets are missing.
