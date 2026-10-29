# Installation

## Requirements

- Node.js 20 or 22 (LTS), `npm`
- C++17 toolchain only for local builds: MSVC 2022 / GCC 11+ / Clang 14+
  (not needed in the GitHub pipeline — see `docs/ci-releases.md`)
- Real build only: Basler pylon SDK 26.09 via the download script
  (see below); no camera needed — the SDK camera emulator is enough
  for tests (`PYLON_CAMEMU=2`)

## Standard installation (real SDK 26.09)

The SDK is fetched exclusively via the `PYLON_SDK_BASE_URL` environment
variable (set locally as an agent secret, in CI as a GitHub Actions
secret of the same name). The base URL never appears in the repo and is
never printed to logs; the script appends the matching per-OS archive
name automatically:

```bash
npm install   # downloads the SDK (scripts/download-pylon-sdk.sh) + native build
npm test      # tests against the real build (emulator via PYLON_CAMEMU, see docs/testing.md)
```

Manual steps:

```bash
sh scripts/download-pylon-sdk.sh   # -> .pylon-sdk/ (PYLON_ROOT)
npm run rebuild                    # native build against .pylon-sdk
```

Without `PYLON_SDK_BASE_URL` set, the script aborts with a clear error
message (no URL leak). `PYLON_ROOT` can alternatively point at an
already present SDK (Linux: directory with `include/`+`lib/`; macOS:
extracted archive with `Frameworks/pylon.framework/`; Windows:
directory with `Development/`+`Runtime/`).

CMake alternative:

```bash
sh scripts/download-pylon-sdk.sh
cmake -S . -B build-cmake && cmake --build build-cmake
```

Result: `build/Release/node_pylon.node`. Never commit binaries/secrets
(`.pylon-sdk/`, `*.node`, `*.tar.gz` are ignored, see `.gitignore`).
