type JuceBackend = {
  emitEvent: (eventName: string, payload: unknown) => void;
  addEventListener: (eventName: string, callback: (value: unknown) => void) => void;
};

declare global {
  interface Window {
    __JUCE__?: {
      backend?: JuceBackend;
      initialisationData?: {
        __juce__functions?: string[];
      };
    };
  }
}

const getBackend = () => window.__JUCE__?.backend;

let lastPromiseId = 0;
const pendingNativeCalls = new Map<number, (result: unknown) => void>();

const ensureCompletionListener = (() => {
  let isListening = false;

  return () => {
    const backend = getBackend();

    if (!backend || isListening) {
      return;
    }

    backend.addEventListener("__juce__complete", (payload) => {
      const completion = payload as { promiseId?: number; result?: unknown };

      if (typeof completion.promiseId !== "number") {
        return;
      }

      pendingNativeCalls.get(completion.promiseId)?.(completion.result);
      pendingNativeCalls.delete(completion.promiseId);
    });

    isListening = true;
  };
})();

const callNativeFunction = (name: string, ...params: unknown[]): Promise<unknown> => {
  const backend = getBackend();

  if (!backend) {
    return Promise.resolve(undefined);
  }

  ensureCompletionListener();

  const resultId = lastPromiseId;
  lastPromiseId += 1;

  return new Promise((resolve) => {
    pendingNativeCalls.set(resultId, resolve);
    backend.emitEvent("__juce__invoke", {
      name,
      params,
      resultId
    });
  });
};

export const getSaturation = async (): Promise<number> => {
  const backend = getBackend();

  if (!backend) {
    return 0.2;
  }

  return coerceSaturation(await callNativeFunction("getSaturation"));
};

export const setSaturation = (value: number) => {
  void callNativeFunction("setSaturation", coerceSaturation(value));
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
