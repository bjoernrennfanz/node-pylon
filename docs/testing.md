# Testing (real SDK, no camera)

```bash
npm test   # node --test test/pylon/ test/genicam/ — against the real build
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
- `test/rn-static.test.js` — React Native static check (files, core, spec;
  run with `npm run test:rn`, no build needed).
