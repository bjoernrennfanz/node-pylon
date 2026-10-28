# node-pylon — Native Node.js-Bindings für Basler pylon

Natives Node.js-Binding für die Basler **pylon C++ API** (GenICam; **ohne DataProcessing** im MVP),
gebaut mit **node-addon-api / N-API** — kein SWIG, kein NAN.
Zielplattformen: Windows x64, Linux x64/arm64, macOS (wie pypylon);
React-Native out-of-tree (Windows/macOS/Linux) folgt in ZAU-153.

Branch-Strategie: `master` = Legacy-Generator (C#/VS2015),
`port/node-addon-api-pylon8` = neues N-API-Grundgerüst (dieser Branch).

## Zielstruktur (analog pypylon)

```text
src/            N-API-Quellen (addon.cc; TlFactory/Camera/Grab folgen in ZAU-150)
lib/            JS-Fassade (require('node-pylon') -> lib/index.js)
types/          TypeScript-Typen (*.d.ts)
samples/        list-devices.js, grab-mock.js
test/           Smoke-Tests ohne Kamera (node --test)
binding.gyp     node-gyp-Build (Node 20/22, N-API 8, C++17)
CMakeLists.txt  CMake-Alternative
```

## Voraussetzungen

- Node.js 20 oder 22 (LTS), `npm`
- C++17-Toolchain: MSVC 2022 / GCC 11+ / Clang 14+
- Für den **Mock-Build**: keine weitere Abhängigkeit (kein pylon-SDK, keine Kamera nötig).
- Für den **echten Build** (ab ZAU-150): Basler pylon SDK 8.x + `PYLON_ROOT`-Env
  (Windows: `C:\Program Files\Basler\pylon 8`, Linux: `/opt/pylon`, macOS: `/Library/Frameworks/pylon.framework`).

## Bauen

```bash
npm install
# Mock-Build ohne Kamera/SDK (Default, Abnahme ZAU-149):
npm run rebuild
# oder explizit:
npm run rebuild:mock

# Echter Build gegen pylon-SDK (ab ZAU-150 verdrahtet):
# export PYLON_ROOT=/opt/pylon
# npm run rebuild -- --define=node_pylon_use_mock=0

# CMake-Alternative:
# cmake -S . -B build-cmake -DNODE_PYLON_USE_MOCK=ON && cmake --build build-cmake
```

Ergebnis: `build/Release/node_pylon.node`.

## CI (GitHub Actions — kein lokaler Compiler nötig)

Der Mock-Build läuft in der Pipeline (`.github/workflows/ci.yml`):
Matrix **Ubuntu/Windows/macOS × Node 20/22**, Schritte `npm ci` →
`npm run rebuild:mock` → `npm test` → Samples. Kein pylon-SDK, keine Kamera,
kein `gcc`/MSVC auf dem eigenen Server nötig — einfach Branch pushen bzw.
PR öffnen und den `ci`-Check abwarten.

## Schnellstart (ohne Kamera)

```bash
npm test                 # Smoke-Tests (Mock)
npm run sample:list      # Geraete auflisten (Mock: 0)
npm run sample:grab      # synthetischer Mono8-Frame
```

```js
const pylon = require('node-pylon');
console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
console.log(pylon.enumerateDevices()); // [] im Mock
const frame = pylon.grabMock(64, 48);  // Buffer, 64*48 Bytes Mono8
```

## N-API-Regeln (verbindlich)

- Nur `node-addon-api`/N-API; keine V8-Internals, kein NAN, kein SWIG.
- `NAPI_VERSION=8`, `NAPI_DISABLE_CPP_EXCEPTIONS` — Fehler als JS-Exceptions.
- pylon-Threads/Grab-Loop nie im JS-Loop blockieren (ab ZAU-150: AsyncWorker/ThreadSafeFunction).
- Bilddaten als Buffer/TypedArray ohne unnötige Kopien (ab ZAU-151).
- Keine Binaries/Secrets committen (siehe `.gitignore`).

## Roadmap (Issues)

- ZAU-149 (dieser Branch): Scaffold + Mock-Build ✅
- ZAU-150: TlFactory, InstantCamera, Grab (ohne DataProcessing)
- ZAU-151: Nodemap/Parameter-API + Bild-Handling
- ZAU-152: CI-Pipeline, Docs, Samples, Tests analog pypylon
- ZAU-153: React-Native Desktop (out-of-tree Windows/Linux/macOS)

## Lizenz

Siehe `LICENSE`. pylon-SDK-Binaries sind **nicht** im Repo enthalten.
