# Testing (real SDK, no camera)

```bash
npm test            # emulated suite: test/genicam + test/pylon/emulated
npm run test:gige   # hardware-only GigE suite (skips without a GigE camera)
npm run test:usb    # hardware-only USB suite (skips without a USB camera)
npm run test:rn     # React Native desktop static checks — no build needed
```

Layout mirrors pypylon's `tests/` split (`tests/genicam`,
`tests/pylon/emulated|gigE|usb`) and its runners (`all_emulated_tests.py`,
`all_gige_tests.py`, `all_usb_tests.py`):
`npm run test:emulated` (= `npm test`) is the `all_emulated_tests.py`
counterpart, `test:gige`/`test:usb` are the `all_gige_tests.py` /
`all_usb_tests.py` counterparts, `test:all` runs everything.

- The tests run against the natively built module
  (`build/Release/node_pylon.node`, pylon SDK 26.09).
- Without physical hardware, the **SDK camera emulator** covers the
  device paths (enumeration, open, grab loop, device info, GenApi nodemap):

  ```bash
  export PYLON_CAMEMU=2   # two emulator devices (Linux/macOS/Windows)
  npm test
  ```

  CI sets `PYLON_CAMEMU=2` automatically (see `docs/ci-releases.md`).
  It also exports the runtime environment per OS
  (`LD_LIBRARY_PATH`/`GENICAM_GENTL64_PATH` on Linux,
  `DYLD_LIBRARY_PATH` on macOS, `Runtime\x64` in `PATH` on Windows).
- Without a reachable device (neither camera nor emulator), the
  device-dependent tests are skipped (`t.skip`); device-independent
  assertions (version strings, API surface) always run.
- `test/pylon/emulated/` — TlFactory enumeration, InstantCamera open/grab-loop/close,
  device info, grab results, buffer factory, wait objects, feature
  persistence, static defect pixel correction, type mappings
  (mirrors pypylon's emulated pylon tests). `npm test` runs with
  `--expose-gc` so the factory destroy/deferred-release tests can force
  wrapper destruction deterministically.
- `test/pylon/gigE/`, `test/pylon/usb/` — hardware-only suites mirroring
  pypylon's `tests/pylon/gigE` and `tests/pylon/usb` file by file
  (grab, transport layer, configuration helper, decompression, defect-pixel
  correction plus explicit MVP-gap skips for chunk/event adapters, action
  trigger and camera events). Each suite gates on its transport
  (`requireGigE`/`requireUsb` in `test/helpers.js`, like pypylon's
  `pylongigetestcase.py`/`pylonusbtestcase.py`): without that hardware every
  test skips, so GitHub CI — which only provides the CamEmu emulator —
  stays green running just the emulated suite. Run
  `npm run test:hardware` on a host with physical cameras.
- `test/genicam/` — nodemap/parameter API per GenICam type
  (Integer/Float/Boolean/String/Enumeration/Command, limits, tryMax,
  executeCommand; mirrors pypylon's genicam tests) plus 1:1-logic ports for
  INode properties, value conversion, categories/selectors, registers and
  callbacks (`properties`, `value-string`, `category-selector`, `register`,
  `callback`).
- `test/rn-*.test.js` — React Native static checks (files, core, spec,
  adapters, sample; run with `npm run test:rn`, no build needed).

## Mapping against pypylon emulated tests

Source: `pypylon/tests` (`all_emulated_tests.py` =
`tests/genicam` + `tests/pylon/emulated`). Only emulated tests are ported;
GigE/USB hardware tests, DataProcessing suites and GUI tests are out of scope.
Each row names the pypylon module, its node-pylon counterpart and any
justified deviation.

| pypylon module(s) | node-pylon counterpart | Deviation |
| --- | --- | --- |
| `pylon/emulated/tlfactory_test.py` (27 methods) | `test/pylon/emulated/factory-transport.test.js` (TlFactory section), `test/pylon/emulated/tlfactory.test.js`, `test/pylon/emulated/discovery.test.js` | Covered: `GetInstance` singleton, `EnumerateTls`/`CreateTl(deviceClass\|TlInfo)`/`ReleaseTl`, `EnumerateDevices` with `DeviceInfo`/dict/mixed filters (`TypeError` on wrong types), `CreateDevice`/`CreateFirstDevice`/`DestroyDevice` returning `PylonDevice`, `IsDeviceAccessible`/`IsDeviceAccessibleInfo` (info default + exclusive-opened emulator behavior). `with` blocks map to try/finally; tuples map to arrays |
| `pylon/emulated/instantcamera_test.py` (46 methods) | `test/pylon/emulated/instantcamera.test.js`, `test/pylon/emulated/configuration.test.js`, `test/pylon/emulated/instantcamera-full.test.js` | Covered: attach/detach/destroy, open/close (+access-mode validation), grab strategies, TimeoutHandling, StartGrabbing variants incl. maxImages, RetrieveResult validity fields, GrabOne, software-trigger flow, event-handler registration, node-map properties, device removal/context. Not wrapped: buffer-handling modes, `WaitObject` accessors (`GetGrabResultWaitObject`), stream/event-grabber node maps, `ChunkNodeMapsEnable` plumbing |
| `pylon/emulated/deviceinfo_test.py` (55), `info_test.py` (26) | `test/pylon/emulated/factory-transport.test.js` (info sections), `test/pylon/emulated/deviceinfo.test.js`, `test/pylon/emulated/discovery.test.js`, `test/pylon/emulated/typemappings.test.js` | Covered: `DeviceInfo`/`TlInfo`/`InterfaceInfo` classes with all typed `Get/Set/IsAvailable` properties (CInfoBase + transport specifics, read-only USB/CameraLink getters), generic `GetPropertyNames` (`[count, names]`) / `GetPropertyAvailable` / `GetPropertyValue` (`[found, value]`) / `SetPropertyValue` / `IsSubset` / `IsUserProvided`, `equals`/`lessThan` comparison (C++ ordering incl. numeric-serial rule), dict helpers (`keys/values/items/toDict/update`), GigE predicates (throw `GenericException` without config). DeviceInfo gains camelCase aliases for the facade fields; `[]` subscript maps to the named methods |
| `pylon/emulated/grabresult_test.py` (64 methods) | `test/pylon/emulated/grab.test.js`, `test/pylon/emulated/grabresult.test.js` | Covered: `GrabResult` wrapper with `isValid/isUnique/release`, `succeeded/grabSucceeded`, geometry/offsets/padding, pixel/payload types + values, payload/buffer/image sizes, timestamp/block/id/image-number/skipped, error code/description, chunk/CRC, stride, zero-copy `getBuffer/getArray/getMemoryView` (shared-memory mutation check), GenDC `getDataComponentCount/getDataComponent/getDataComponentBuffer`, `toObject`; `retrieveGrabResult` returns the wrapper (fields also set for property access) and fires image handlers like `retrieveResult` |
| `pylon/emulated/image_test.py` (17), `pylonimage_test.py` (59), `pixeltype_test.py` (22), `pixeltypemapper_test.py` (14), `payloadtype_test.py` (4), `imageformatconverter_test.py` (72), `imagepersistence_test.py` (35), `imagedecompressor_test.py` (94), `pixeldata_test.py` (31), `sfnc_test.py` (14) | `test/pylon/emulated/image.test.js`, `test/pylon/emulated/image-pipeline.test.js` | Covered: `PylonImage.create/isValid/isUnique/release/copyImage/attachGrabResult`, geometry/orientation/pixel/image-size/stride, zero-copy buffer views, `save/load/canSaveWithoutConversion` (Png round-trip in temp dir), `ImageFormatConverter` output-format/bit-alignment config + `convert/getBufferSizeForConversion/imageHasDestinationFormat/isSupported{Input,Output}Format` (emulator frame Mono8->BGR8/RGB8, payload scaling), `ImageDecompressor.describe/decompress/getCompressionMode`, `PixelType`/`PayloadType` enum objects, `getPixelTypeByName/getNameByPixelType/isMono/isColor/isPacked`, `ImageFileFormat` names. Not wrapped: packed-format unpacking helpers (`GetPixelTypesForUnpacking`), plane/AOI views, user-buffer events |
| `pylon/emulated/acquirecontinuousconfiguration_test.py`, `acquiresingleframeconfiguration_test.py`, `softwaretriggerconfiguration_test.py`, `configurationhelper_test.py` | `test/pylon/emulated/configuration.test.js`, `test/pylon/emulated/instantcamera-full.test.js` | Configuration objects are JS classes (`applyConfiguration` + `onOpened`); helper statics set the same nodes, `probePacketSize` is a safe no-op probe |
| `pylon/emulated/integerparameter_test.py` | `test/genicam/integer.test.js` | — |
| `pylon/emulated/floatparameter_test.py` | `test/genicam/float.test.js` | — |
| `pylon/emulated/booleanparameter_test.py` | `test/genicam/boolean.test.js` | — |
| `pylon/emulated/stringparameter_test.py` | `test/genicam/string.test.js` | — |
| `pylon/emulated/enumparameter_test.py`, `enumentryparameter_test.py` | `test/genicam/enumeration.test.js` | — |
| `pylon/emulated/commandparameter_test.py` | `test/genicam/command.test.js` | — |
| `pylon/emulated/parameter_test.py`, `nodemapwrapper_test.py`, node/error sections | `test/genicam/nodemap.test.js`, `test/genicam/nodetype.test.js` | `INode`/`NodemapWrapper`/`CParameter` classes are not wrapped; type/access/error coverage is behavioral via the typed accessors |
| `pylon/emulated/devicefactory_test.py` (5), `deviceclass_test.py` (9), `tlinfo_test.py` (9), `transportlayer_test.py` (24), `interface_test.py` (22), `interfaceinfo_test.py` (5), `device_test.py` (4), `container_test.py` (3) | `test/pylon/emulated/factory-transport.test.js`, `test/pylon/emulated/discovery.test.js` | Covered: `IDeviceFactory` prototype chain, access-mode/accessibility integers, `TlInfo`/`InterfaceInfo` full property sets, `TransportLayer` (`getTlInfo`/nodemap node count/enumerate/create/destroy/accessibility) and `Interface` (info/open-close/nodemap-count/create/destroy/accessibility) handles, `PylonDevice` create/open/info/destroy, containers as arrays (`InstantCameraArray` stays a JS class over one native camera per enumerated device). `NodeMap`/`InterfaceNodeMap` context managers map to open/close + `getNodeMapNumNodes` (emulator interface reports 0) |
| `version_test.py` surface | `test/pylon/emulated/version.test.js` | — |
 | genicam synthetic-XML suites (`alias`, `callback`, `category`, `converter`, `cycledetector`, `poll`, `port`, `register`, `selector`, `streaming`, `swissknife`, `floattestsuite`, `properties`) | `test/genicam/nodemap.test.js` (behavioral) + `test/genicam/properties.test.js`, `value-string.test.js`, `category-selector.test.js`, `register.test.js`, `callback.test.js` | These run without a camera against hand-built XML; there is no XML-loader counterpart, so the same logic (INode availability/naming, inc/unit/representation, ToString/FromString, enum int values, command IsDone, categories/selectors, register length/address/read, callback fire counts) is covered against the emulator nodemap instead |
| `pylon/emulated/pylonimage` persistence, `imagepersistence_test.py` | `test/pylon/emulated/image-pipeline.test.js` (round-trip section) + samples | File save/load via `PylonImage.save/load` (`Bmp/Tiff/Jpeg/Png/Raw/Dng`); `canSaveWithoutConversion` gates conversions like `CImagePersistence` |
| `pylon/emulated/featurepersistence_test.py` | `test/pylon/emulated/featurepersistence.test.js` | `FeaturePersistence.save/load/saveToString/loadFromString` against the open nodemap (+ `PascalCase` aliases); camera arg is the facade camera |
| `pylon/emulated/bufferfactory_test.py` | `test/pylon/emulated/bufferfactory.test.js` | malloc-backed native `BufferFactory` with stats (`allocationCount/freeCount/lastAllocationSize/isReleased`); one attach per instance; contexts are numeric ids (`null` without a factory); provenance on holdable results (`retrieveGrabResult`); `MaxNumBuffer` exactness via `startGrabbingMax`; no JS `AllocateBuffer` overrides |
| `pylon/emulated/waitobject_test.py`, `waitobjects_test.py` | `test/pylon/emulated/waitobject.test.js`, `test/pylon/emulated/waitobjects.test.js` | Synchronous waits with ms timeouts; `withIndex=true` yields `[result, index]`; background-thread sections run sequentially (a sync wait blocks the JS loop) with identical SDK transitions |
| `pylon/emulated/typemappings_test.py` | `test/pylon/emulated/typemappings.test.js` | `String_t`/`StringList_t` map to string/`[count, names]` via the `DeviceInfo` class; exceptions are identical class objects on `pylon` and `pylon.genicam`; node-accessor guards keep plain `Error/TypeError/RangeError` |
| `pylon/emulated/staticdefectpixelcorrection_test.py` | `test/pylon/emulated/staticdefectpixelcorrection.test.js` | `[success, vector]` pairs; entries as `[x, y]/[x, y, type]` or `{x, y, type?}`; empty vectors are truthy in JS (`empty()` instead); camera calls skip on `RuntimeException` without user-list support |
| `pylon/emulated/pylondatacontainer_test.py` (28) | `test/pylon/emulated/grabresult.test.js` (GenDC section) | `getDataComponentCount/getDataComponent/getDataComponentBuffer` over `CPylonDataContainer`; plain-image payloads report 0 components and throw `RangeError` on indexed access |
| `cameraeventhandler`, `imageeventhandler`, `configurationeventhandler`, `chunk` payload parsing | `test/pylon/emulated/instantcamera-full.test.js` (handler sections) | Handlers are synchronous JS callbacks for lifecycle/image events (no grab-thread dispatch); device-node event delivery via an event-grab thread is a follow-up; chunk payload parsing is behavioral via `isChunkDataAvailable` |
| `pylon/gigE/*_test.py` (10 files) | `test/pylon/gigE/*.test.js` | Hardware-only suite (`npm run test:gige`), file-by-file mirror: grab, GigE transport layer, configuration helper, decompression and defect-pixel correction exercise the wrapped surface on hardware; action trigger, chunk adapter/image, event adapter and camera events skip explicitly (MVP gaps, see row above). Without GigE hardware every test skips, so CI stays green |
| `pylon/usb/*_test.py` (8 files) | `test/pylon/usb/*.test.js` | Hardware-only suite (`npm run test:usb`), same pattern: grab, configuration helper, decompression and defect-pixel correction run on hardware; chunk adapter/image, event adapter and camera events skip explicitly (MVP gaps) |
| `pylondataprocessing/*`, `pylongui_test.py` | — (out of scope) | DataProcessing is excluded from the MVP; GUI tests need a display |

## React Native desktop tests

```bash
npm run test:rn   # test/rn-static.test.js + test/rn-adapters.test.js
```

- `test/rn-static.test.js` — docs, shared core, TurboModule spec, facade,
  adapters and sample exist; core stays free of N-API/JSI includes.
- `test/rn-adapters.test.js` — Windows/macOS/Linux adapters stay thin
  marshalling layers over `react-native/shared/pylon_core`, the spec keeps
  its shape, and the sample only uses the documented facade.
- Both files are static checks: they run on every OS in CI without a
  camera, SDK or compiler.
