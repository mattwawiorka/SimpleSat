import { clamp, coerceFiniteNumber } from "./numbers";

export function clampSaturation(value: number) {
  return clamp(value, 0, 1);
}

export function coerceSaturation(value: unknown) {
  return clampSaturation(coerceFiniteNumber(value));
}

export function saturationToPercentage(value: number) {
  return Math.round(clampSaturation(value) * 100);
}
