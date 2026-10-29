# Testing (real SDK, no camera)

```bash
npm test   # node --test test/*.test.js — against the real build
```

- Tests run against the natively built module
  (`build/Release/node_pylon.node`, pylon SDK 26.09).
- Without a physical camera, the **SDK camera emulator** covers the
  device paths (enumeration, open, grab loop, device info):

  ```bash
  export PYLON_CAMEMU=2   # two emulator devices (Linux/macOS/Windows)
  npm test
  ```

  CI sets `PYLON_CAMEMU=2` automatically (see `docs/ci-releases.md`).
  CI also exports the runtime environment per OS
  (`LD_LIBRARY_PATH`/`GENICAM_GENTL64_PATH` on Linux,
  `DYLD_LIBRARY_PATH` on macOS, `Runtime\x64` on `PATH` on Windows).
- Without a reachable device (neither camera nor emulator), the
  device-dependent tests are skipped (`t.skip`); device-independent
  assertions (versions, API surface, `grabMock`) always run.
- `test/smoke.test.js` — versions, `isMock()`, lifecycle refcount,
  TlFactory enumeration, open/grab-loop/close, error paths.
- `test/api-surface.test.js` — checks that every function exported from
  `types/index.d.ts` exists in `lib/index.js` and honors the
  documented signatures (return types, buffer length).
- `test/nodemap.test.js`, `test/grab.test.js` — nodemap/parameter API
  and image handling (deterministic parameter store, zero-copy buffer).
- `test/rn-static.test.js` — React Native static check (files/core/spec).

## Mock mode (opt-in)

```bash
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test
```

In mock mode the deterministic mock expectations apply
(`MOCK123456`, 640x480 Mono8); in real mode the tests check
emulator/camera values (non-empty serial numbers, `succeeded`,
`buffer.length === payloadSize`).
