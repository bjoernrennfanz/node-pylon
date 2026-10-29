# Installation

## Prerequisites

- Node.js 20 or 22 (LTS), `npm`
- C++17 toolchain only for local builds: MSVC 2022 / GCC 11+ / Clang 14+
  (not needed in the GitHub pipeline — see `docs/ci-releases.md`)
- Real build (default): Basler pylon SDK 26.09 via the download script
  (see below), no camera needed — the SDK camera emulator is enough
  for tests (`PYLON_CAMEMU=2`)
- Mock opt-in: no further dependencies, no pylon SDK, no camera
  (`npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`, see below)

## Standard installation (real SDK 26.09)

The SDK is obtained exclusively via the `PYLON_SDK_BASE_URL` environment
variable (set as an agent secret locally, as a GitHub Actions secret of the
same name in CI). The base URL appears nowhere in the repo and is never
printed to logs; the script appends the matching archive name per OS/arch
automatically:

```bash
npm install   # downloads the SDK (scripts/download-pylon-sdk.sh) + builds natively
npm test      # tests against the real build (emulator via PYLON_CAMEMU, see docs/testing.md)
```

Manually:

```bash
sh scripts/download-pylon-sdk.sh   # -> .pylon-sdk/ (PYLON_ROOT)
npm run rebuild                    # native build against .pylon-sdk
```

Without `PYLON_SDK_BASE_URL` set, the script aborts with a clear error
message (no URL leak). `PYLON_ROOT` can alternatively point at an
already-present SDK (Linux: directory with `include/`+`lib/`;
macOS: unpacked archive with `Frameworks/pylon.framework/`; Windows:
directory with `Development/`+`Runtime/`).

CMake alternative:

```bash
sh scripts/download-pylon-sdk.sh
cmake -S . -B build-cmake && cmake --build build-cmake
```

Result: `build/Release/node_pylon.node`. Never commit binaries/secrets
(`.pylon-sdk/`, `*.node`, `*.tar.gz` are ignored, see `.gitignore`).

## Mock opt-in (offline work without SDK only)

The mock is no longer the default. Only for offline work without SDK/compiler:

```bash
npm run rebuild:mock
NODE_PYLON_USE_MOCK=1 npm test
NODE_PYLON_USE_MOCK=1 node samples/list-devices.js
```

Without a native module and without this opt-in, `require('node-pylon')`
throws a clear error instead of silently falling back to the mock.
