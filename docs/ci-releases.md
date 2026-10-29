# CI & Releases

## CI (`ci.yml`)

- Job `build-real` (mandatory gate): matrix
  `ubuntu-22.04 / ubuntu-24.04-arm / windows-2022 / macos-14 / macos-15`
  × Node 20/22 → secret check → `scripts/download-pylon-sdk.sh` →
  `npm ci --ignore-scripts` → `npm run rebuild:real` → leak check →
  `npm test` (with `PYLON_CAMEMU=2`) → samples. Uploads
  `build/Release/node_pylon.node` per combination as an artifact
  (plus the runtime DLLs on Windows).

Windows uses `windows-2022` (VS2022 v17) + `ilammy/msvc-dev-cmd`, because the
bundled node-gyp cannot detect VS v18 on `windows-latest`.

## Secrets

- `PYLON_SDK_BASE_URL` (mandatory): base URL of the pylon SDK archive
  (Settings → Secrets → Actions). The script appends the exact file name
  per OS/arch (`pylon-sdk-26.09-{linux_x86_64,linux_aarch64,
  macos_universal,windows_x86_64}.tar.gz`). The value never appears in the
  repo or in logs: the script never prints the URL, GitHub masks secrets
  additionally, and the leak-check step fails if the secret value shows up
  in workspace files (matches are never printed). Without the secret, CI
  aborts with a clear error message.

## Releases (`release.yml`)

On tag push (`v*`), `prebuild-real` builds the same OS/Node matrix against
the real SDK, tests with the emulator, and uploads the `.node` files as
artifacts (`node_pylon-<tag>-<os>-node<version>`); they are also attached
to the GitHub Release via `gh release upload`
(Windows DLLs as a separate artifact).
