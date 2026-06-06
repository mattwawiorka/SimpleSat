type JuceBackend = {
  callNativeFunction: (
    name: string,
    argumentsList: unknown[],
    callback?: (result: unknown) => void
  ) => void;
  addEventListener: (eventName: string, callback: (value: unknown) => void) => void;
};

declare global {
  interface Window {
    __JUCE__?: {
      backend?: JuceBackend;
    };
  }
}

const getBackend = () => window.__JUCE__?.backend;

export const getSaturation = async (): Promise<number> => {
  const backend = getBackend();

  if (!backend) {
    return 0.2;
  }

  return new Promise((resolve) => {
    backend.callNativeFunction("getSaturation", [], (result) => {
      resolve(coerceSaturation(result));
    });
  });
};

export const setSaturation = (value: number) => {
  getBackend()?.callNativeFunction("setSaturation", [coerceSaturation(value)]);
};

export const subscribeToSaturation = (callback: (value: number) => void) => {
  getBackend()?.addEventListener("saturationChanged", (value) => {
    callback(coerceSaturation(value));
  });
};

export const coerceSaturation = (value: unknown): number => {
  const numericValue = typeof value === "number" ? value : Number(value);

  if (!Number.isFinite(numericValue)) {
    return 0;
  }

  return Math.min(1, Math.max(0, numericValue));
};
