# RN-Desktop-Sample (ZAU-153)

Lauffähiges Sample ohne Kamera (Mock-Fallback in `react-native/src/index.ts`).

## Start pro Plattform (out-of-tree)

```bash
# Windows (react-native-windows, C++/WinAppSDK, Fabric)
npx react-native run-windows --arch x64

# macOS (react-native-macos)
npx react-native run-macos

# Linux (Community-Port react-native-linux; Evaluierung läuft —
# Fallback: Electron-App, die build/Release/node_pylon.node lädt)
npx react-native run-linux   # nur wenn der Fork die Plattform registriert
```

Metro löst `NativePylon.windows.js` / `.macos.js` neben
`react-native/specs/NativePylon.js` auf, falls plattformspezifisches JS nötig
ist (`rnpm.haste.platforms`, siehe `docs/rn-architecture.md`).

## Was das Sample zeigt

`App.tsx`: zwei Buttons — „Version + Geräte auflisten" (`enumerateDevices`,
Mock: `[]`) und „GrabMock" (64×48 Mono8, deterministischer Gradient, Base64).
Mit nativem TurboModule kommen die Werte aus `pylon_core_*`, ohne natives
Modul greift der JS-Mock — das Sample läuft in beiden Fällen.

## Echter Kamera-Betrieb (ab ZAU-150/151)

Core hinter `NODE_PYLON_USE_REAL` gegen `PYLON_ROOT` bauen, dann
`enumerateDevices()` → `CTlFactory`, `grabMock()` → `RetrieveResult`-Promise
über `CallInvoker` (blockiert den JS-Loop nicht).
