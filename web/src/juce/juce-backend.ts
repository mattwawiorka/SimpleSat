type JuceBackend = {
  emitEvent: (eventName: string, payload: unknown) => void;
  addEventListener: (
    eventName: string,
    callback: (value: unknown) => void,
  ) => unknown;
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

export function hasJuceBackend() {
  return getBackend() !== undefined;
}

export function callNativeFunction(
  name: string,
  ...params: unknown[]
): Promise<unknown> {
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
      resultId,
    });
  });
}

export function subscribeToBackendEvent(
  eventName: string,
  callback: (value: unknown) => void,
) {
  getBackend()?.addEventListener(eventName, callback);
}
