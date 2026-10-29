# Testing (real SDK, no camera)

```bash
npm test      # node --test test/pylon/ test/genicam/ — against the real build
npm run test:rn  # React Native desktop static checks — no build needed
```

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
- `test/pylon/` — TlFactory enumeration, InstantCamera open/grab-loop/close,
  device info, grab results (mirrors pypylon's emulated pylon tests).
- `test/genicam/` — nodemap/parameter API per GenICam type
  (Integer/Float/Boolean/String/Enumeration/Command, limits, tryMax,
  executeCommand; mirrors pypylon's genicam tests).
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
| `pylon/emulated/tlfactory_test.py` (EnumerateTls/CreateTl sections) | `test/pylon/tlfactory.test.js`, `test/pylon/discovery.test.js` | `EnumerateTls`/`CreateTl`/`TlInfo` objects are not wrapped; device-class coverage is behavioral via `deviceClass` fields |
| `pylon/emulated/instantcamera_test.py` (open/grab-loop sections) | `test/pylon/instantcamera.test.js`, `test/pylon/configuration.test.js` | Attach/Detach, handler registration, buffer handling modes and grab strategies are not wrapped |
| `pylon/emulated/deviceinfo_test.py`, `info_test.py` | `test/pylon/deviceinfo.test.js`, `test/pylon/discovery.test.js` | `DeviceInfo`/`Info` are plain objects, not classes; no copy/compare operators |
| `pylon/emulated/grabresult_test.py` | `test/pylon/grab.test.js` | `GrabResult` is a plain object from `grab()`/`retrieveResult()`; no smart-pointer validity/unique semantics |
| `pylon/emulated/image_test.py`, `pylonimage_test.py`, `pixeltype_test.py`, `pixeltypemapper_test.py`, `payloadtype_test.py`, `imageformatconverter` sections | `test/pylon/image.test.js` | No `PylonImage` class, converter or decompressor; format/payload coverage is behavioral via grab fields |
| `pylon/emulated/acquirecontinuousconfiguration_test.py`, `acquiresingleframeconfiguration_test.py`, `softwaretriggerconfiguration_test.py`, `configurationhelper_test.py` | `test/pylon/configuration.test.js` | Configuration objects are not wrapped; start/stop/timeout behavior is covered through the grab loop |
| `pylon/emulated/integerparameter_test.py` | `test/genicam/integer.test.js` | — |
| `pylon/emulated/floatparameter_test.py` | `test/genicam/float.test.js` | — |
| `pylon/emulated/booleanparameter_test.py` | `test/genicam/boolean.test.js` | — |
| `pylon/emulated/stringparameter_test.py` | `test/genicam/string.test.js` | — |
| `pylon/emulated/enumparameter_test.py`, `enumentryparameter_test.py` | `test/genicam/enumeration.test.js` | — |
| `pylon/emulated/commandparameter_test.py` | `test/genicam/command.test.js` | — |
| `pylon/emulated/parameter_test.py`, `nodemapwrapper_test.py`, node/error sections | `test/genicam/nodemap.test.js`, `test/genicam/nodetype.test.js` | `INode`/`NodemapWrapper`/`CParameter` classes are not wrapped; type/access/error coverage is behavioral via the typed accessors |
| `pylon/emulated/devicefactory/deviceclass/tlinfo/interface/interfaceinfo/transportlayer/device_test.py`, `container_test.py` | `test/pylon/discovery.test.js` | Factory/transport objects and `InstantCameraArray` are not wrapped; lists are plain arrays |
| `version_test.py` surface | `test/pylon/version.test.js` | — |
| genicam synthetic-XML suites (`alias`, `callback`, `category`, `converter`, `cycledetector`, `poll`, `port`, `register`, `selector`, `streaming`, `swissknife`, `floattestsuite`, `properties`) | `test/genicam/nodemap.test.js` (behavioral) | These run without a camera against hand-built XML; there is no XML-loader counterpart, so type/access/error semantics are covered against the emulator nodemap instead |
| `pylon/emulated/pylonimage` persistence, `imagepersistence_test.py`, `featurepersistence_test.py` | behavioral via `test/pylon/image.test.js` + samples | No file save/load API; Width round-trip is covered by integer tests |
| `cameraeventhandler`, `imageeventhandler`, `configurationeventhandler`, `bufferfactory_test.py`, `waitobject(s)_test.py`, `chunk` payload parsing, `sfnc_test.py`, `typemappings_test.py`, `staticdefectpixelcorrection_test.py`, `pylondatacontainer_test.py` | — (no counterpart) | No handler/buffer/wait-object API in the MVP wrapper; GenDC components are not parsed |
| `pylondataprocessing/*`, GigE/USB hardware suites, `pylongui_test.py` | — (out of scope) | DataProcessing is excluded from the MVP; GUI and real-hardware tests need physical devices |

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
