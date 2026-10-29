# RN desktop architecture — out-of-tree Windows / Linux / macOS

State: `feature/node-port-native`, Node addon on the real pylon SDK 26.09.
Reference: https://reactnative.dev/docs/out-of-tree-platforms (bundling via
`react-native-<platform>`, Metro `--platform`, `rnpm.haste.platforms`).

## Target picture

```
                     +-----------------------+
                     |  shared C++ core      |  <- written once
                     |  react-native/shared/ |
                     |  pylon_core.{h,cpp}   |  real pylon SDK
                     +----+-------------+----+
                          |             |
             +------------+--+  +------+--------------+
             | Node addon    |  | JSI / TurboModule   |  <- no double wrapping
             | src/addon.cc  |  | react-native/...    |  Core is only adapted
             | (N-API)       |  | (codegen spec)      |
             +---------------+  +---------------------+
```

* **Shared C++ core:** `react-native/shared/pylon_core.h/.cpp` — pure C++17,
  no V8/JSI includes. Functions: `EnumerateDevices()`, `PylonVersion()`,
  wired against the real pylon SDK (`PYLON_ROOT`). The Node addon uses its
  own N-API wrappers in `src/`; all RN bridges only call this core —
  no second wrapping.
* **N-API lifetime:** `ObjectWrap`/`AsyncWorker` only in `src/`; image data
  as `Napi::Buffer` with a finalizer (zero-copy principle for real
  `GrabResult` pointers, mind GenDC).
* **JSI/TurboModule:** spec `react-native/specs/NativePylon.js`
  (`codegenNativeModule` + `TurboModule`). Codegen creates per-platform
  `AppSpecsJSI.h` scaffolds; the platform adapters (`windows/`, `macos/`,
  `linux/`) only implement JSI marshalling and delegate to
  `pylon_core_*`. Fabric/TurboModules is the default since RN 0.80; the
  legacy bridge is no longer served.
* **Threading:** `InstantCamera::RetrieveResult` / grab loop runs on a native
  thread (`AsyncWorker` or JSI `CallInvoker` + promise); event polling never
  blocks the JS loop.

## Out-of-tree platforms (forks used)

| Target | Package (out-of-tree) | Maintainer | RN sample |
|------|---------------------|------------|-----------|
| Windows x64 | `react-native-windows` | Microsoft | `react-native-sample/` (C++/WinAppSDK, Fabric) |
| macOS (arm64/x64) | `react-native-macos` | Microsoft | same sample, target `macos` |
| Linux x64/arm64 | Community port `react-native-linux` (evaluation; fallback: Electron + Node addon from this repo) | Community | same sample, target `linux` |

Metro bundling: `npx react-native bundle --platform windows|macos` resolves
`.windows.js` / `.macos.js` suffixes (see spec comment). Same for the Linux port
if the fork used registers a Metro platform (`rnpm.haste.platforms`).

## Files in this state

```text
react-native/shared/pylon_core.{h,cpp}  pure C++ core (real SDK), shared by Node + RN
react-native/specs/NativePylon.js       codegen spec (TurboModule)
react-native/src/index.ts               JS facade (TurboModuleRegistry, requires the native module)
react-native/windows/                   C++/WinRT adapter sketch
react-native/macos/                     ObjC++ adapter sketch
react-native/linux/                     C++ adapter sketch
react-native/sample/App.tsx + README    RN sample (version + device list, real SDK)
types/index.d.ts                        Node types (RN types in react-native/src/index.ts)
```

## Boundaries / next actions

* RN JSI adapter implementations against the real core (ZAU-153).
* Nodemap typing + real image handling (zero-copy, GenDC).
* Extend CI with an RN codegen check once the RN sample builds via CLI.
* No DataProcessing (MVP out of scope), no secrets/binaries in the repo,
  no releases without management approval.
