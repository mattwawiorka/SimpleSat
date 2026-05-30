## Saturation Requirements

```text
ID: saturation
Name: Saturation
Range: 0.0 to 1.0
Default: 0.0 or 0.2
Automatable: yes
Saved/restored by DAW: yes
```

Use JUCE parameter/state handling, preferably `AudioProcessorValueTreeState`, so the parameter can be automated and restored by the DAW.

Implement simple saturation in C++ using a `tanh` waveshaper.

Example logic:

```cpp
float drive = 1.0f + saturation * 20.0f;
float output = std::tanh(input * drive) / std::tanh(drive);
```

Keep DSP simple and readable for version 1.
