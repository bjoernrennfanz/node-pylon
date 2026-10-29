# CI & Releases

## CI (`ci.yml`)

- Job `build-real` (Pflicht-Gate): Matrix
  `ubuntu-22.04 / ubuntu-24.04-arm / windows-2022 / macos-14 / macos-15`
  × Node 20/22 → Secret-Check → `scripts/download-pylon-sdk.sh` →
  `npm ci --ignore-scripts` → `npm run rebuild:real` → Leak-Check →
  `npm test` (mit `PYLON_CAMEMU=2`) → Samples. Lädt pro Kombination
  `build/Release/node_pylon.node` als Artifact hoch
  (Windows zusätzlich die Runtime-DLLs).
- Mock-Builds sind kein Gate mehr; der Mock bleibt nur als lokales
  Opt-in (`npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`).

Windows nutzt `windows-2022` (VS2022 v17) + `ilammy/msvc-dev-cmd`, weil das
mitgelieferte node-gyp VS v18 auf `windows-latest` nicht erkennt.

## Secrets

- `PYLON_SDK_BASE_URL` (Pflicht): Basis-URL zum pylon-SDK-Archiv
  (Settings → Secrets → Actions). Das Skript hängt den exakten Dateinamen
  pro OS/Arch an (`pylon-sdk-26.09-{linux_x86_64,linux_aarch64,
  macos_universal,windows_x86_64}.tar.gz`). Der Wert erscheint nirgends im
  Repo und nie in Logs: Das Skript gibt die URL nie aus, GitHub maskiert
  Secrets zusätzlich, und der Leak-Check-Step schlägt fehl, falls der
  Secret-Wert in Workspace-Dateien auftaucht (Treffer werden nicht gedruckt).
  Ohne Secret bricht die CI mit einer klaren Fehlermeldung ab.

## Releases (`release.yml`)

Bei Tag-Push `v*` baut `prebuild-real` dieselbe OS/Node-Matrix gegen das
echte SDK, testet mit Emulator und lädt die `.node`-Dateien als Artifacts
hoch (`node_pylon-<tag>-<os>-node<version>`); zusätzlich werden sie per
`gh release upload` an das GitHub Release angehängt
(Windows-DLLs liegen als separates Artifact bei).
