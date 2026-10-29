# CI & releases

## CI (`ci.yml`)

- Job `build-real` (required gate): matrix
  `ubuntu-22.04 / ubuntu-24.04-arm / windows-2022 / macos-14 / macos-15`
  x Node 20/22 → secret check → `scripts/download-pylon-sdk.sh` →
  `npm ci --ignore-scripts` → `npm run rebuild:real` → leak check →
  `npm test` (with `PYLON_CAMEMU=2`) → samples. Uploads
  `build/Release/node_pylon.node` as an artifact per combination
  (Windows additionally the runtime DLLs).
- Mock builds are no longer a gate; the mock remains only as a local
  opt-in (`npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`).

Windows uses `windows-2022` (VS2022 v17) + `ilammy/msvc-dev-cmd` because the
bundled node-gyp cannot detect VS v18 on `windows-latest`.

## Secrets

- `PYLON_SDK_BASE_URL` (required): base URL of the pylon SDK archive
  (Settings → Secrets → Actions). The script appends the exact filename
  per OS/arch (`pylon-sdk-26.09-{linux_x86_64,linux_aarch64,
  macos_universal,windows_x86_64}.tar.gz`). The value appears nowhere in the
  repo and never in logs: the script never prints the URL, GitHub additionally
  masks secrets, and the leak-check step fails if the secret value shows up
  in workspace files (matches are not printed). Without the secret, CI aborts
  with a clear error message.

## Releases (`release.yml`)

On tag push `v*`, `prebuild-real` builds the same OS/Node matrix against the
real SDK, tests with the emulator, and uploads the `.node` files as artifacts
(`node_pylon-<tag>-<os>-node<version>`); they are additionally attached to
the GitHub release via `gh release upload`
(Windows DLLs ship as a separate artifact).
