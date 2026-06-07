import React, { useMemo, useRef } from "react";

import { clampSaturation, saturationToPercentage } from "./utils/saturation";

type SaturationKnobProps = {
  value: number;
  onChange: (value: number) => void;
};

const valueToAngle = (value: number) => -135 + value * 270;

export const SaturationKnob = ({ value, onChange }: SaturationKnobProps) => {
  const dragState = useRef<{ startY: number; startValue: number } | null>(null);
  const percentage = saturationToPercentage(value);
  const rotation = useMemo(() => valueToAngle(value), [value]);

  const updateValue = (nextValue: number) => {
    onChange(clampSaturation(nextValue));
  };

  const handlePointerDown = (event: React.PointerEvent<HTMLButtonElement>) => {
    dragState.current = {
      startY: event.clientY,
      startValue: value
    };

    event.currentTarget.setPointerCapture(event.pointerId);
  };

  const handlePointerMove = (event: React.PointerEvent<HTMLButtonElement>) => {
    if (!dragState.current) {
      return;
    }

    const deltaY = dragState.current.startY - event.clientY;
    updateValue(dragState.current.startValue + deltaY / 240);
  };

  const handlePointerEnd = (event: React.PointerEvent<HTMLButtonElement>) => {
    dragState.current = null;

    if (event.currentTarget.hasPointerCapture(event.pointerId)) {
      event.currentTarget.releasePointerCapture(event.pointerId);
    }
  };

  const handleWheel = (event: React.WheelEvent<HTMLButtonElement>) => {
    event.preventDefault();
    updateValue(value + (event.deltaY < 0 ? 0.02 : -0.02));
  };

  return (
    <>
      <label className="knob-label" htmlFor="saturation">
        Saturation
      </label>
      <div className="knob-wrap">
        <button
          className="knob"
          type="button"
          style={{ "--rotation": `${rotation}deg` } as React.CSSProperties}
          onPointerDown={handlePointerDown}
          onPointerMove={handlePointerMove}
          onPointerUp={handlePointerEnd}
          onPointerCancel={handlePointerEnd}
          onWheel={handleWheel}
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
        value={value}
        onChange={(event) => updateValue(Number(event.target.value))}
        aria-valuemin={0}
        aria-valuemax={1}
        aria-valuenow={value}
      />
      <output className="value-readout" htmlFor="saturation">
        {percentage}%
      </output>
    </>
  );
};
