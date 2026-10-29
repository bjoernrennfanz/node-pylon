# node-pylon — Native Node.js bindings for Basler pylon

Native Node.js binding for the Basler **pylon C++ API** (GenICam; **without DataProcessing** in the MVP),
built with **node-addon-api / N-API** — no SWIG, no NAN.
Default is the real build against **pylon SDK 26.09**; the mock is only
an explicit opt-in for offline work.
Target platforms: Windows x64, Linux x64/arm64, macOS (same as pypylon);
React Native out-of-tree (Windows/macOS/Linux), see `docs/rn-architecture.md`.

Branch strategy: `master` = legacy generator (C#/VS2015),
`feature/node-port-native` = N-API foundation (this branch).

## Target layout (mirrors pypylon)

```text
src/            N-API sources (addon.cc, mock_camera/nodemap, pylon_lifecycle)
scripts/        download-pylon-sdk.sh (SDK fetch via PYLON_SDK_BASE_URL secret)
lib/            JS facade (require('node-pylon') -> lib/index.js; mock.js opt-in only)
types/          TypeScript types (*.d.ts)
samples/        list-devices.js, grab.js (mirrors pypylon grab.py), grab-options.js,
                nodemap-params.js, grab-attributes.js, grab-mock.js (legacy)
test/           Smoke + API-surface + nodemap/grab tests (real SDK, emulator)
docs/           Docs mirroring the pypylon programmers_guide (installation, quickstart, API, CI)
binding.gyp     node-gyp build (Node 20/22, N-API 8, C++17, real-SDK default)
CMakeLists.txt  CMake alternative (also real-SDK default)
```

## Prerequisites

- Node.js 20 or 22 (LTS), `npm`
- C++17 toolchain only for local builds: MSVC 2022 / GCC 11+ / Clang 14+
- Real build (default): `PYLON_SDK_BASE_URL` environment variable
  (agent secret locally, GitHub Actions secret in CI) — the download script
  fetches pylon SDK 26.09 per OS/arch into `.pylon-sdk/`; no camera needed
  (tests use the SDK emulator via `PYLON_CAMEMU`)

## Build

```bash
npm install   # SDK download (secret) + native real build
npm run rebuild  # rebuild (SDK reused from cache when present)

# Mock only as an explicit opt-in (no SDK, offline work):
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test

# CMake alternative (real default; mock: -DNODE_PYLON_USE_MOCK=ON):
# sh scripts/download-pylon-sdk.sh
# cmake -S . -B build-cmake && cmake --build build-cmake
```

Result: `build/Release/node_pylon.node`.

## CI (GitHub Actions — real SDK, no mock gate)

The pipeline (`.github/workflows/ci.yml`) builds against the real SDK 26.09:
matrix **Linux x64/arm64 + Windows x64 + macOS x Node 20/22**,
steps secret check → SDK download → `npm run rebuild:real` →
leak check → `npm test` (with `PYLON_CAMEMU=2`) → samples
(+ `.node` artifact per combination; Windows including runtime DLLs).
Releases with real prebuilds via `.github/workflows/release.yml` on tag `v*`.
Details: [docs/ci-releases.md](docs/ci-releases.md).

## Docs (mirrors the pypylon programmers_guide)

[docs/index.md](docs/index.md) — entry point; [installation](docs/installation.md),
[quickstart](docs/quickstart.md), [programmer's guide](docs/programmers-guide.md),
[API reference](docs/api-reference.md), [samples](docs/samples.md),
[testing](docs/testing.md), [CI & releases](docs/ci-releases.md).

## Quickstart (real SDK; emulator instead of a camera)

```bash
export PYLON_CAMEMU=2   # emulator devices, no camera needed
npm test                          # all tests against the real build
npm run sample:list               # list devices (emulator: 2 devices)
npm run sample:grab               # grab sample mirroring pypylon grab.py
node samples/grab-options.js 100 80  # frame with width/height
```

```js
const pylon = require('node-pylon');
pylon.initialize();
try {
  console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
  const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
  const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
  camera.open();
  camera.startGrabbing();
  const res = camera.retrieveResult(5000); // null on timeout
  if (res && res.succeeded) console.log(res.width, res.height, res.buffer.length);
  camera.stopGrabbing();
  camera.close();
} finally {
  pylon.terminate();
}
```

Without a camera/emulator, `CreateFirstDevice` throws a JS exception;
`retrieveResult` returns `null` on timeout.

## React Native desktop (mock state, real wiring follows)

Out-of-tree per https://reactnative.dev/docs/out-of-tree-platforms:
`react-native-windows` (Microsoft), `react-native-macos` (Microsoft),
Linux via community port (fallback: Electron + Node addon from this repo).
Shared C++ core `react-native/shared/pylon_core.{h,cpp}` — write once,
adapt from N-API (`src/addon.cc`) and JSI/TurboModule only (no double wrapping).
Spec: `react-native/specs/NativePylon.js`, facade: `react-native/src/index.ts`
(with mock fallback, the sample also runs without a native module), sample:
`react-native/sample/App.tsx`. Details: `docs/rn-architecture.md`.

```bash
npm run test:rn   # RN static check (docs/core/spec/adapters/sample)
```

## N-API rules (binding)

- Only `node-addon-api`/N-API; no V8 internals, no NAN, no SWIG.
- `NAPI_VERSION=8`, `NAPI_DISABLE_CPP_EXCEPTIONS` — errors as JS exceptions.
- Never block the JS loop with pylon threads/grab loops (AsyncWorker/ThreadSafeFunction).
- Image data as Buffer/TypedArray without unnecessary copies.
- Never commit binaries/secrets (see `.gitignore`).

## License

See `LICENSE`. pylon SDK binaries are **not** included in this repo
(fetch via the `PYLON_SDK_BASE_URL` secret, see `docs/installation.md`).
