# Testing (Real-SDK, ohne Kamera)

```bash
npm test   # node --test test/*.test.js — gegen den echten Build
```

- Die Tests laufen gegen das nativ gebaute Modul
  (`build/Release/node_pylon.node`, pylon SDK 26.09).
- Ohne physische Kamera deckt der **SDK-Kamera-Emulator** die
  Gerätepfade ab (Enumeration, Open, Grab-Loop, DeviceInfo):

  ```bash
  export PYLON_CAMEMU=2   # zwei Emulator-Geräte (Linux/macOS/Windows)
  npm test
  ```

  Die CI setzt `PYLON_CAMEMU=2` automatisch (siehe `docs/ci-releases.md`).
  Laufzeit-Umgebung pro OS exportiert die CI ebenfalls
  (`LD_LIBRARY_PATH`/`GENICAM_GENTL64_PATH` auf Linux,
  `DYLD_LIBRARY_PATH` auf macOS, `Runtime\x64` im `PATH` auf Windows).
- Ohne erreichbares Gerät (weder Kamera noch Emulator) werden die
  geräteabhängigen Tests übersprungen (`t.skip`), geräteunabhängige
  Assertions (Versionen, API-Surface, `grabMock`) laufen immer.
- `test/smoke.test.js` — Versionen, `isMock()`, Lifecycle-Refcount,
  TlFactory-Enumeration, Open/Grab-Loop/Close, Fehlerpfade.
- `test/api-surface.test.js` — prüft, dass alle in `types/index.d.ts`
  exportierten Funktionen in `lib/index.js` existieren und die
  dokumentierten Signaturen einhalten (Rückgabetypen, Buffer-Länge).
- `test/nodemap.test.js`, `test/grab.test.js` — Nodemap/Parameter-API
  und Bild-Handling (deterministischer Parameterspeicher, Zero-copy-Buffer).
- `test/rn-static.test.js` — React-Native-Statikcheck (Dateien/Core/Spec).

## Mock-Modus (Opt-in)

```bash
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test
```

Im Mock-Modus gelten die deterministischen Mock-Erwartungen
(`MOCK123456`, 640×480 Mono8); im Real-Modus prüfen die Tests
Emulator-/Kamera-Werte (nicht-leere Seriennummern, `succeeded`,
`buffer.length === payloadSize).
