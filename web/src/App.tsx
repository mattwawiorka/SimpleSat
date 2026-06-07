import { SaturationKnob } from "./SaturationKnob";
import { useSaturationParameter } from "./hooks/useSaturationParameter";

export const App = () => {
  const [saturation, setSaturation] = useSaturationParameter();

  return (
    <main className="plugin-shell">
      <section
        className="control-surface"
        aria-label="SimpleSat saturation control"
      >
        <h1 className="brand">SimpleSat</h1>
        <SaturationKnob value={saturation} onChange={setSaturation} />
      </section>
    </main>
  );
};
