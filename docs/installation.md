# Installation

## Voraussetzungen

- Node.js 20 oder 22 (LTS), `npm`
- C++17-Toolchain nur für lokale Builds: MSVC 2022 / GCC 11+ / Clang 14+
  (in der GitHub-Pipeline nicht nötig — siehe `docs/ci-releases.md`)
- Mock-Build (Default): keine weitere Abhängigkeit, kein pylon-SDK, keine Kamera
- Echter Build (ab ZAU-150): Basler pylon SDK 8.x + `PYLON_ROOT`
  (Windows: `C:\Program Files\Basler\pylon 8`, Linux: `/opt/pylon`,
  macOS: `/Library/Frameworks/pylon.framework`)

## Installieren und bauen

```bash
npm install
npm run rebuild        # = rebuild:mock, ohne SDK/Kamera
npm test               # Smoke-Tests ohne Kamera
```

Echter Build (Platzhalter bis ZAU-150 verdrahtet):

```bash
export PYLON_ROOT=/opt/pylon
npm run rebuild:real
```

CMake-Alternative:

```bash
cmake -S . -B build-cmake -DNODE_PYLON_USE_MOCK=ON && cmake --build build-cmake
```

Ergebnis: `build/Release/node_pylon.node`. Keine Binaries/Secrets committen.
