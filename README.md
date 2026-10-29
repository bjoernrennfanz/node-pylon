# node-pylon — Native Node.js-Bindings für Basler pylon

Natives Node.js-Binding für die Basler **pylon C++ API** (GenICam; **ohne DataProcessing** im MVP),
gebaut mit **node-addon-api / N-API** — kein SWIG, kein NAN.
Standard ist der echte Build gegen **pylon SDK 26.09**; der Mock ist nur noch
ein explizites Opt-in für Offline-Arbeit.
Zielplattformen: Windows x64, Linux x64/arm64, macOS (wie pypylon);
React-Native out-of-tree (Windows/macOS/Linux), siehe `docs/rn-architecture.md`.

Branch-Strategie: `master` = Legacy-Generator (C#/VS2015),
`feature/node-port-native` = N-API-Grundgerüst (dieser Branch).

## Zielstruktur (analog pypylon)

```text
src/            N-API-Quellen (addon.cc, mock_camera/nodemap, pylon_lifecycle)
scripts/        download-pylon-sdk.sh (SDK-Bezug via PYLON_SDK_BASE_URL-Secret)
lib/            JS-Fassade (require('node-pylon') -> lib/index.js; mock.js nur Opt-in)
types/          TypeScript-Typen (*.d.ts)
samples/        list-devices.js, grab.js (analog pypylon grab.py), grab-options.js,
                nodemap-params.js, grab-attributes.js, grab-mock.js (legacy)
test/           Smoke + API-Surface + Nodemap/Grab-Tests (Real-SDK, Emulator)
docs/           Docs analog pypylon programmers_guide (Installation, Quickstart, API, CI)
binding.gyp     node-gyp-Build (Node 20/22, N-API 8, C++17, Real-SDK-Default)
CMakeLists.txt  CMake-Alternative (ebenfalls Real-SDK-Default)
```

## Voraussetzungen

- Node.js 20 oder 22 (LTS), `npm`
- C++17-Toolchain nur für lokale Builds: MSVC 2022 / GCC 11+ / Clang 14+
- Echter Build (Default): Umgebungsvariable `PYLON_SDK_BASE_URL`
  (Agent-Secret lokal, GitHub-Actions-Secret in CI) — das Download-Skript
  lädt pylon SDK 26.09 pro OS/Arch nach `.pylon-sdk/`; keine Kamera nötig
  (Tests nutzen den SDK-Emulator via `PYLON_CAMEMU`)

## Bauen

```bash
npm install   # SDK-Download (Secret) + nativer Real-Build
npm run rebuild  # erneut bauen (SDK wird ggf. aus Cache genutzt)

# Mock nur als explizites Opt-in (ohne SDK, Offline-Arbeit):
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test

# CMake-Alternative (Real-Default; Mock: -DNODE_PYLON_USE_MOCK=ON):
# sh scripts/download-pylon-sdk.sh
# cmake -S . -B build-cmake && cmake --build build-cmake
```

Ergebnis: `build/Release/node_pylon.node`.

## CI (GitHub Actions — Real-SDK, kein Mock-Gate)

Die Pipeline (`.github/workflows/ci.yml`) baut gegen das echte SDK 26.09:
Matrix **Linux x64/arm64 + Windows x64 + macOS × Node 20/22**,
Schritte Secret-Check → SDK-Download → `npm run rebuild:real` →
Leak-Check → `npm test` (mit `PYLON_CAMEMU=2`) → Samples
(+ `.node`-Artifact pro Kombination; Windows inkl. Runtime-DLLs).
Releases mit Real-Prebuilds via `.github/workflows/release.yml` bei Tag `v*`.
Details: [docs/ci-releases.md](docs/ci-releases.md).

## Docs (analog pypylon programmers_guide)

[docs/index.md](docs/index.md) — Einstieg; [Installation](docs/installation.md),
[Quickstart](docs/quickstart.md), [Programmer's Guide](docs/programmers-guide.md),
[API-Referenz](docs/api-reference.md), [Samples](docs/samples.md),
[Testing](docs/testing.md), [CI & Releases](docs/ci-releases.md).

## Schnellstart (Real-SDK; Emulator statt Kamera)

```bash
export PYLON_CAMEMU=2   # Emulator-Geräte, keine Kamera nötig
npm test                          # alle Tests gegen den echten Build
npm run sample:list               # Geräte auflisten (Emulator: 2 Geräte)
npm run sample:grab               # Grab-Sample analog pypylon grab.py
node samples/grab-options.js 100 80  # Frame mit Breite/Hoehe
```

```js
const pylon = require('node-pylon');
pylon.initialize();
try {
  console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
  const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
  const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
  camera.open();
  camera.startGrabbing();
  const res = camera.retrieveResult(5000); // null bei Timeout
  if (res && res.succeeded) console.log(res.width, res.height, res.buffer.length);
  camera.stopGrabbing();
  camera.close();
} finally {
  pylon.terminate();
}
```

Ohne Kamera/Emulator wirft `CreateFirstDevice` eine JS-Exception;
`retrieveResult` gibt bei Timeout `null` zurück.

## React-Native Desktop (Mock-Stand, Real-Anbindung folgt)

Out-of-tree nach https://reactnative.dev/docs/out-of-tree-platforms:
`react-native-windows` (Microsoft), `react-native-macos` (Microsoft),
Linux via Community-Port (Fallback: Electron + Node-Addon aus diesem Repo).
Geteilter C++-Core `react-native/shared/pylon_core.{h,cpp}` — einmal schreiben,
von N-API (`src/addon.cc`) und JSI/TurboModule nur adaptieren (kein Doppel-Wrapping).
Spec: `react-native/specs/NativePylon.js`, Fassade: `react-native/src/index.ts`
(mit Mock-Fallback, Sample laeuft auch ohne natives Modul), Sample:
`react-native/sample/App.tsx`. Details: `docs/rn-architecture.md`.

```bash
npm run test:rn   # RN-Statikcheck (Doku/Core/Spec/Adapter/Sample)
```

## N-API-Regeln (verbindlich)

- Nur `node-addon-api`/N-API; keine V8-Internals, kein NAN, kein SWIG.
- `NAPI_VERSION=8`, `NAPI_DISABLE_CPP_EXCEPTIONS` — Fehler als JS-Exceptions.
- pylon-Threads/Grab-Loop nie im JS-Loop blockieren (AsyncWorker/ThreadSafeFunction).
- Bilddaten als Buffer/TypedArray ohne unnötige Kopien.
- Keine Binaries/Secrets committen (siehe `.gitignore`).

## Lizenz

Siehe `LICENSE`. pylon-SDK-Binaries sind **nicht** im Repo enthalten
(Bezug via `PYLON_SDK_BASE_URL`-Secret, siehe `docs/installation.md`).
