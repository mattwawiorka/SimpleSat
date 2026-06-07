import { useEffect, useState } from "react";

import { saturationParameter } from "../juce/saturation-bridge";
import { clampSaturation } from "../utils/saturation";

export const useSaturationParameter = () => {
  const [saturation, setLocalSaturation] = useState(0.2);

  useEffect(() => {
    let isMounted = true;

    saturationParameter.get().then((value) => {
      if (isMounted) {
        setLocalSaturation(value);
      }
    });

    saturationParameter.subscribe((value) => {
      setLocalSaturation(value);
    });

    return () => {
      isMounted = false;
    };
  }, []);

  const setSaturation = (value: number) => {
    const nextValue = clampSaturation(value);
    setLocalSaturation(nextValue);
    saturationParameter.set(nextValue);
  };

  return [saturation, setSaturation] as const;
};
