import {
  callNativeFunction,
  hasJuceBackend,
  subscribeToBackendEvent,
} from "./juce-backend";
import { coerceSaturation } from "../utils/saturation";

export const saturationParameter = {
  async get(): Promise<number> {
    if (!hasJuceBackend()) {
      return 0.2;
    }

    return coerceSaturation(await callNativeFunction("getSaturation"));
  },

  set(value: number) {
    void callNativeFunction("setSaturation", coerceSaturation(value));
  },

  subscribe(callback: (value: number) => void) {
    subscribeToBackendEvent("saturationChanged", (value) => {
      callback(coerceSaturation(value));
    });
  },
};
