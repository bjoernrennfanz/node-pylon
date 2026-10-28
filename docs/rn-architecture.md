# RN-Desktop-Architektur (ZAU-153) — out-of-tree Windows / Linux / macOS

Stand: `feature/node-port-native`, Node-Addon-Mock aus ZAU-149 (CI grün).
Referenz: https://reactnative.dev/docs/out-of-tree-platforms (Bundling via
`react-native-<plattform>`, Metro `--platform`, `rnpm.has­te.platforms`).

## Zielbild

```
                    +-----------------------+
                    |  shared C++-Core      |  <- EINMAL schreiben
                    |  react-native/shared/ |
                    |  pylon_core.{h,cpp}   |  Mock jetzt, pylon-SDK ab ZAU-150
                    +----+-------------+----+
                         |             |
            +------------+--+   +------+--------------+
            | Node-Addon    |   | JSI / TurboModule   |  <- KEIN Doppel-Wrapping
            | src/addon.cc  |   | react-native/...    |  Core wird nur adaptiert
            | (N-API)       |   | (Codegen-Spec)      |
            +---------------+   +---------------------+
```

* **Geteilter C++-Core:** `react-native/shared/pylon_core.h/.cpp` — reines C++17,
  keine V8-/JSI-Includes. Funktionen: `EnumerateDevices()`, `GrabMockFrame(w,h)`,
  `PylonVersion()`. Mock jetzt (deterministisch, ohne SDK/Kamera); ab ZAU-150
  `#ifdef NODE_PYLON_USE_REAL` gegen `PYLON_ROOT` verdrahten. `src/addon.cc`
  und alle RN-Brücken rufen nur diesen Core — kein zweites Wrapping.
* **N-API-Lifetime:** `ObjectWrap`/`AsyncWorker` nur in `src/addon.cc`;
  Bilddaten als `Napi::Buffer` mit Finalizer (Zero-copy-Prinzip ab ZAU-151
  mit echten `GrabResult`-Pointern, GenDC beachten).
* **JSI/TurboModule:** Spec `react-native/specs/NativePylon.js` (`codegenNativeModule`
  + `TurboModule`). Codegen erzeugt pro Plattform `AppSpecsJSI.h`-Scaffolds;
  die Plattform-Adapter (`windows/`, `macos/`, `linux/`) implementieren nur
  JSI-Marshalling und delegieren an `pylon_core_*`. Fabric/TurboModules ist ab
  RN 0.80 Default; Legacy-Bridge wird nicht mehr bedient.
* **Threading:** `InstantCamera::RetrieveResult` / GrabLoop läuft auf nativem
  Thread (`AsyncWorker` bzw. JSI-`CallInvoker` + Promise); Event-Polling blockiert
  nie den JS-Loop. Details folgen in ZAU-150.

## Out-of-tree-Plattformen (verwendete Forks)

| Ziel | Paket (out-of-tree) | Maintainer | RN-Sample |
|------|---------------------|------------|-----------|
| Windows x64 | `react-native-windows` | Microsoft | `react-native-sample/` (C++/WinAppSDK, Fabric) |
| macOS (arm64/x64) | `react-native-macos` | Microsoft | gleiches Sample, Ziel `macos` |
| Linux x64/arm64 | Community-Port `react-native-linux` (Evaluierung; Fallback: Electron + Node-Addon aus diesem Repo) | Community | gleiches Sample, Ziel `linux` |

Metro-Bundling: `npx react-native bundle --platform windows|macos` löst
`.windows.js` / `.macos.js`-Suffixe auf (siehe Spec-Kommentar). Linux-Port analog,
falls der verwendete Fork Metro-Plattform registriert (`rnpm.haste.platforms`).

## Dateien in diesem Stand

```text
react-native/shared/pylon_core.{h,cpp}  reiner C++-Core (Mock), von Node + RN geteilt
react-native/specs/NativePylon.js       Codegen-Spec (TurboModule)
react-native/src/index.ts               JS-Fassade (TurboModuleRegistry + Mock-Fallback)
react-native/windows/                   C++/WinRT-Adapter (AddTurboModule-Skizze)
react-native/macos/                     ObjC++-Adapter-Skizze
react-native/linux/                     C++-Adapter-Skizze
react-native/sample/App.tsx + README    RN-Sample (Liste + GrabMock, ohne Kamera)
types/index.d.ts                        Node-Typen (RN-Typen in react-native/src/index.ts)
```

## Abgrenzung / Next Actions

* ZAU-150: Core auf echtes SDK umstellen (`PYLON_ROOT`, `TlFactory`, `InstantCamera`).
* ZAU-151: Nodemap-Typisierung + echtes Bild-Handling (Zero-copy, GenDC).
* ZAU-152: CI um RN-Codegen-Check erweitern, sobald RN-Sample per CLI baubar ist.
* Kein DataProcessing (MVP-Out-of-scope), keine Secrets/Binaries im Repo,
  keine Releases ohne Freigabe der Geschäftsführung.
