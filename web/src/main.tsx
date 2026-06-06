import React, { useEffect, useMemo, useState } from "react";
import { createRoot } from "react-dom/client";

import { getSaturation, setSaturation, subscribeToSaturation } from "./juceBridge";
import "./styles.css";

const valueToAngle = (value: number) => -135 + value * 270;

const App = () => {
  const [saturation, setLocalSaturation] = useState(0.2);

  useEffect(() => {
    let isMounted = true;

    getSaturation().then((value) => {
      if (isMounted) {
        setLocalSaturation(value);
      }
    });

    subscribeToSaturation((value) => {
      setLocalSaturation(value);
    });

    return () => {
      isMounted = false;
    };
  }, []);

  const percentage = Math.round(saturation * 100);
  const rotation = useMemo(() => valueToAngle(saturation), [saturation]);

  const updateSaturation = (value: number) => {
    const nextValue = Math.min(1, Math.max(0, value));
    setLocalSaturation(nextValue);
    setSaturation(nextValue);
  };

  return (
    <main className="plugin-shell">
      <section className="control-surface" aria-label="SimpleSat saturation control">
        <div className="brand">SimpleSat</div>
        <label className="knob-label" htmlFor="saturation">
          Saturation
        </label>
        <div className="knob-wrap">
          <div className="knob" style={{ "--rotation": `${rotation}deg` } as React.CSSProperties}>
            <div className="knob-indicator" />
          </div>
        </div>
        <input
          id="saturation"
          className="knob-range"
          type="range"
          min="0"
          max="1"
          step="0.001"
          value={saturation}
          onChange={(event) => updateSaturation(Number(event.target.value))}
          aria-valuemin={0}
          aria-valuemax={1}
          aria-valuenow={saturation}
        />
        <output className="value-readout" htmlFor="saturation">
          {percentage}%
        </output>
      </section>
    </main>
  );
};

createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);
