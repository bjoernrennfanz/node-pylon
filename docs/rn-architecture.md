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
  pixel-type helpers (`PixelTypeName`/`IsMonoPixelType`/`IsColorPixelType`),
  camera lifecycle (`OpenCamera` with access-mode validation/`CloseCamera`/
  `IsCameraOpen`, opaque int handle — the `CInstantCamera` never crosses
  the bridge), device handling (`GetCameraDeviceInfo`,
  `IsPylonDeviceAttached`/`HasOwnership`, `Attach`/`Detach`/`DestroyDevice`,
  `IsCameraDeviceRemoved`, `Get/SetCameraContext`, `GetQueuedBufferCount`),
  grab loop (`StartGrabbing`/`StopGrabbing`/`IsGrabbing`/`GrabOne`/
  `RetrieveResult` with `GrabStrategy`/`TimeoutHandling`, explicit
  open-checks), the nodemap basis (`GetNodeType`/`IsReadable`/`IsWritable`/
  `IsAvailable`, Integer/Float/Boolean/String/Enum/Command accessors) plus
  the full property set (Min/Max/Inc/Unit/Representation, ToMin/ToMax,
  TryMax, HasInc, DisplayName/Description, ValueAsString/ValueFromString,
  StringMaxLength, EnumIntValue, CommandDone, CategoryFeatures, Selector/
  Selected/Selecting, Register Length/Address/Read/Write) and discovery
  constants (`GetDiscoveryConstants`: device classes, TL types, property
  keys, access modes, accessibility — values from the SDK headers).
  Setters validate ranges/writability up front so bridges see
  deterministic "out of range"/"not writable" texts. Errors surface as
  `PylonError`, which bridges translate into JS exceptions. Wired against
  the real pylon SDK (`PYLON_ROOT`). The Node addon uses its own N-API
  wrappers in `src/`; all RN bridges only call this core —
  no second wrapping.
* **N-API lifetime:** `ObjectWrap`/`AsyncWorker` only in `src/`; image data
  as `Napi::Buffer` with a finalizer (zero-copy principle for real
  `GrabResult` pointers, mind GenDC).
* **JSI/TurboModule:** spec `react-native/specs/NativePylon.js`
  (`codegenNativeModule` + `TurboModule`, codegen-compatible types only:
  string/number/boolean/Array/Object). Covers version/discovery, pixel
  helpers, camera lifecycle + device handling, grab loop, the nodemap
  basis + full property set, register access (base64 on the bridge) and
  discovery constants (75 methods, one per core function). Codegen creates
  per-platform `AppSpecsJSI.h` scaffolds; the platform adapters
  (`windows/`, `macos/`, `linux/`) only implement JSI marshalling and
  delegate to `pylon_core_*`. Fabric/TurboModules is the default since
  RN 0.80; the legacy bridge is no longer served.
* **Images:** `core::GrabImage::buffer` is moved zero-copy into a
  `jsi::ArrayBuffer` by the adapters; the spec carries it as `buffer` in
  the grab Object. Bridges without ArrayBuffer support return a base64
  string instead — the JS facade (`react-native/src/index.ts`) decodes
  both to the same `Uint8Array` contract (covered by
  `test/rn-functional.test.js`).
* **JS facade:** `react-native/src/index.ts` — `InstantCamera`
  (handle-based, Node-parity subset incl. open/close idempotency,
  device handling, context, register bytes as Uint8Array),
  `InstantCameraArray` (pure JS, round-robin retrieve with cameraIndex),
  `GrabStrategy`/`TimeoutHandling`/`GrabLoop`/`RegistrationMode`/`Cleanup`/
  `AccessMode`, discovery constants from the native module (no drift),
  acquire configurations + full `ConfigurationHelper` subset, and error
  mapping (wrong type -> TypeError, out-of-range/invalid entry ->
  RangeError, like lib). Documented gaps (`RN_COVERAGE_GAPS`):
  TlFactory/TransportLayer/Interface object model (constants ported),
  WaitObject(s), FeaturePersistence, StaticDefectPixelCorrection,
  BufferFactory, PylonImage pipeline, ChunkAdapter triple beyond
  frameNumber/timestamp, event/callback handlers — each needs a new
  bridge object model or emitter, follow-up scope.
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
react-native/sample/App.tsx + README    RN sample (version + device list + one-frame grab, real SDK)
types/index.d.ts                        Node types (RN types in react-native/src/index.ts)
```

## Boundaries / next actions

* JSI adapter .cpp/mm bodies against the real core compile-check in CI
  (the sketches here name every `core::*` call; `test/rn-contract.test.js`
  guards the 75-method mapping).
* Follow-up candidates (je eigene Issue, je fehlendes Bridge-Stueck in
  RN_COVERAGE_GAPS): TlFactory/TransportLayer/Interface-Objektmodell,
  WaitObject(s), FeaturePersistence, StaticDefectPixelCorrection,
  BufferFactory, PylonImage-Pipeline, ChunkAdapter-Tripel, Event-Emitter-
  Bridge (image/camera/config/callback events).
* No DataProcessing (MVP out of scope), no secrets/binaries in the repo,
  no releases without management approval.

## Tests (all in `npm run test:rn`, CI needs no camera/SDK for these)

RN suites run in their own CI jobs (ZAU-174, per GF): `rn-build`
(compile-checks `pylon_core.cpp` per OS against the real SDK, then runs
`pylon_core_smoke` — the real core binary against the real SDK and the
camera emulator, no stubs) and `rn-tests` (OS x Node 20/22 matrix, no
`needs:`, parallel to the addon jobs). Facade tests need Node >= 22.6
type stripping and skip cleanly on Node 20 (`RN_NO_STRIP`; verified via
`RN_FORCE_NO_STRIP=1`: 15 pass, 96 skipped, 0 fail).

```text
test/rn-static.test.js      files/symbols/markers/doc mentions (5 tests)
test/rn-adapters.test.js    thin marshalling layers, spec/facade surface (6 tests)
test/rn-contract.test.js    facade TS validity (type stripping), spec
                            codegen-compatibility, spec->core parity
                            (75 methods), adapter mapping, sample smoke (5)
test/rn-functional.test.js  facade end-to-end vs emulator-like core stub:
                            discovery, pixel helpers, lifecycle, grabOne,
                            grab loop, nodemap roundtrips + errors,
                            timeout handling, base64 fallback,
                            configurations (11 tests)
test/rn-parity-genicam.test.js   GenICam suites mirrored from test/genicam
                            (integer/float/boolean/string/enumeration/
                            command/nodemap/nodetype/properties/register/
                            value-string/category-selector, 40 tests;
                            callback needs the event bridge — gap)
test/rn-parity-grab.test.js      grab/grabresult/configuration/
                            instantcamera-basics mirrored (17 tests)
test/rn-parity-camera.test.js    enums, open modes, device handling,
                            software trigger, helpers, InstantCameraArray
                            (15 tests)
test/rn-parity-discovery.test.js version/deviceinfo/discovery +
                            SDK constant parity (12 tests)
```

Portability accounting vs the ~300 runnable Node tests (emulated +
genicam; GigE/USB suites are hardware-only and skip in CI everywhere):
ported ~95 (all GenICam except callback, all portable grab/config/
camera/discovery parts, all device-independent constants). Not portable
without new bridge APIs: TlFactory/TransportLayer/Interface object model
(~70 tests), WaitObject(s) (39), StaticDefectPixelCorrection (24),
PylonImage pipeline (15), BufferFactory (13), FeaturePersistence (5),
event/callback handlers (~5), GenDC/chunk adapters (~8) — each listed in
`RN_COVERAGE_GAPS` with the missing bridge piece; candidates for
follow-up issues.

Functional emulator tests against the real core (`PYLON_CAMEMU`) run in
the Node suites (`npm run test:emulated`); the RN functional tests use an
in-test stub because the JSI layer needs a built app per out-of-tree
platform.
