export function clamp(value: number, min: number, max: number) {
  return Math.min(max, Math.max(min, value));
}

export function coerceFiniteNumber(value: unknown, fallback = 0) {
  const numericValue = typeof value === "number" ? value : Number(value);
  return Number.isFinite(numericValue) ? numericValue : fallback;
}
