import React, { useEffect, useMemo, useRef, useState } from "react";
import { createRoot } from "react-dom/client";

import { getSaturation, setSaturation, subscribeToSaturation } from "./juceBridge";
import "./styles.css";

const valueToAngle = (value: number) => -135 + value * 270;
const clampSaturation = (value: number) => Math.min(1, Math.max(0, value));

const App = () => {
  const [saturation, setLocalSaturation] = useState(0.2);
  const dragState = useRef<{ startY: number; startValue: number } | null>(null);

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
    const nextValue = clampSaturation(value);
    setLocalSaturation(nextValue);
    setSaturation(nextValue);
  };

  const handleKnobPointerDown = (event: React.PointerEvent<HTMLButtonElement>) => {
    dragState.current = {
      startY: event.clientY,
      startValue: saturation
    };

    event.currentTarget.setPointerCapture(event.pointerId);
  };

  const handleKnobPointerMove = (event: React.PointerEvent<HTMLButtonElement>) => {
    if (!dragState.current) {
      return;
    }

    const deltaY = dragState.current.startY - event.clientY;
    updateSaturation(dragState.current.startValue + deltaY / 240);
  };

  const handleKnobPointerEnd = (event: React.PointerEvent<HTMLButtonElement>) => {
    dragState.current = null;

    if (event.currentTarget.hasPointerCapture(event.pointerId)) {
      event.currentTarget.releasePointerCapture(event.pointerId);
    }
  };

  const handleKnobWheel = (event: React.WheelEvent<HTMLButtonElement>) => {
    event.preventDefault();
    updateSaturation(saturation + (event.deltaY < 0 ? 0.02 : -0.02));
  };

  return (
    <main className="plugin-shell">
      <section className="control-surface" aria-label="SimpleSat saturation control">
        <div className="brand">SimpleSat</div>
        <label className="knob-label" htmlFor="saturation">
          Saturation
        </label>
        <div className="knob-wrap">
          <button
            className="knob"
            type="button"
            style={{ "--rotation": `${rotation}deg` } as React.CSSProperties}
            onPointerDown={handleKnobPointerDown}
            onPointerMove={handleKnobPointerMove}
            onPointerUp={handleKnobPointerEnd}
            onPointerCancel={handleKnobPointerEnd}
            onWheel={handleKnobWheel}
            aria-label="Saturation"
            aria-valuemin={0}
            aria-valuemax={100}
            aria-valuenow={percentage}
          >
            <div className="knob-indicator" />
          </button>
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
