# ADR 004: Use Oversampling for Saturation Anti-Aliasing

## Context

The `tanh` saturation stage creates harmonics. Some of those harmonics can exceed the host sample rate's Nyquist limit and fold back as aliasing, which was heard as unwanted high-frequency noise.

## Decision

Use JUCE oversampling around the nonlinear saturation stage to reduce aliasing.

## Implementation Notes

- `SaturationDSP` upsamples the audio before applying the `tanh` waveshaper.
- The oversampled signal is processed, filtered, and downsampled back to the host sample rate.
- The plugin reports the oversampling latency with `setLatencySamples`.
- Saturation changes are smoothed to reduce zipper noise.
- Maximum drive was reduced and output trim was added to make the top end less harsh.

## Outcomes

- High-frequency aliasing noise is reduced.
- CPU usage is higher than direct full-band `tanh` processing.
- The plugin introduces latency from the oversampling filters, so host latency reporting is required.
