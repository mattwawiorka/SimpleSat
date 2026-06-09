# 004 - Oversampling Anti-Aliasing

## Decision

Use JUCE oversampling around the nonlinear saturation stage to reduce aliasing from the `tanh` waveshaper.

## Why This Is a Good Design Move

Oversampling reduces folded high-frequency artifacts from saturation while preserving the simple nonlinear DSP design. Reporting latency keeps host playback alignment correct.

## Context

The `tanh` saturation stage creates harmonics. Some harmonics can exceed the host sample rate's Nyquist limit and fold back as aliasing, which was heard as unwanted high-frequency noise.

## Design Rules

- Apply oversampling around the nonlinear saturation stage.
- Report oversampling filter latency to the host.
- Smooth saturation changes to reduce zipper noise.
- Keep drive and output level choices conservative enough to avoid harsh top-end behavior.

## Implementation Notes

- `SaturationDSP` upsamples audio before applying the `tanh` waveshaper.
- The oversampled signal is processed, filtered, and downsampled back to the host sample rate.
- The plugin reports oversampling latency with `setLatencySamples`.
- Maximum drive is reduced and output trim is added to make the top end less harsh.

## Avoid

- Do not apply full-band nonlinear saturation without considering aliasing.
- Do not introduce oversampling latency without reporting it to the host.
- Do not remove smoothing from parameter changes.
