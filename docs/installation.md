# Installation

## Voraussetzungen

- Node.js 20 oder 22 (LTS), `npm`
- C++17-Toolchain nur für lokale Builds: MSVC 2022 / GCC 11+ / Clang 14+
  (in der GitHub-Pipeline nicht nötig — siehe `docs/ci-releases.md`)
- Echter Build (Default): Basler pylon SDK 26.09 per Download-Skript
  (siehe unten), keine Kamera nötig — der SDK-Kamera-Emulator genügt
  für Tests (`PYLON_CAMEMU=2`)
- Mock-Opt-in: keine weitere Abhängigkeit, kein pylon-SDK, keine Kamera
  (`npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`, siehe unten)

## Standard-Installation (Real-SDK 26.09)

Der SDK-Bezug läuft ausschließlich über die Umgebungsvariable
`PYLON_SDK_BASE_URL` (lokal als Agent-Secret gesetzt, in CI als
GitHub-Actions-Secret gleichen Namens). Die Basis-URL steht nirgends im
Repo und wird nie in Logs ausgegeben; das Skript hängt den passenden
Archivnamen pro OS/Arch automatisch an:

```bash
npm install   # lädt das SDK (scripts/download-pylon-sdk.sh) + baut nativ
npm test      # Tests gegen den echten Build (Emulator via PYLON_CAMEMU, s. docs/testing.md)
```

Manuell:

```bash
sh scripts/download-pylon-sdk.sh   # -> .pylon-sdk/ (PYLON_ROOT)
npm run rebuild                    # nativer Build gegen .pylon-sdk
```

Ohne gesetztes `PYLON_SDK_BASE_URL` bricht das Skript mit einer klaren
Fehlermeldung ab (kein URL-Leak). `PYLON_ROOT` kann alternativ auf ein
bereits vorhandenes SDK zeigen (Linux: Verzeichnis mit `include/`+`lib/`;
macOS: entpacktes Archiv mit `Frameworks/pylon.framework/`; Windows:
Verzeichnis mit `Development/`+`Runtime/`).

CMake-Alternative:

```bash
sh scripts/download-pylon-sdk.sh
cmake -S . -B build-cmake && cmake --build build-cmake
```

Ergebnis: `build/Release/node_pylon.node`. Keine Binaries/Secrets committen
(`.pylon-sdk/`, `*.node`, `*.tar.gz` sind ignoriert, siehe `.gitignore`).

## Mock-Opt-in (nur Offline-Arbeit ohne SDK)

Der Mock ist kein Default mehr. Nur für Offline-Arbeit ohne SDK/Compiler:

```bash
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test
NODE_PYLON_USE_MOCK=1 node samples/list-devices.js
```

Ohne natives Modul und ohne dieses Opt-in wirft `require('node-pylon')`
einen klaren Fehler statt still auf den Mock zurückzufallen.
