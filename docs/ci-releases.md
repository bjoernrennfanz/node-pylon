# CI & Releases

## CI (`ci.yml`)

- Job `build-mock` (Pflicht-Gate): Matrix
  `ubuntu-22.04 / ubuntu-24.04-arm / windows-2022 / macos-14 / macos-15`
  × Node 20/22 → `npm ci --ignore-scripts` → `npm run rebuild:mock` →
  `npm test` → Samples. Kein SDK, keine Kamera nötig.
  Lädt pro Kombination `build/Release/node_pylon.node` als Artifact hoch.
- Job `build-real` (opt-in): Matrix `ubuntu-22.04 / windows-2022 / macos-14`
  × Node 20/22. Läuft nur mit Secret `PYLON_SDK_URL` (SDK-Tarball/Zip pro OS
  oder Artifactory-Spiegel); ohne Secret skippt der Job grün, damit Forks
  ohne SDK-Zugang grün bleiben. Echte Download-/Link-Verdrahtung folgt mit ZAU-150.

Windows nutzt `windows-2022` (VS2022 v17) + `ilammy/msvc-dev-cmd`, weil das
mitgelieferte node-gyp VS v18 auf `windows-latest` nicht erkennt.

## Secrets

- `PYLON_SDK_URL` (optional): URL zum pylon-SDK-Archiv. Herkunft/Lizenz vor
  produktiver Nutzung mit der Geschäftsführung klären; keine Binaries committen.

## Releases (`release.yml`)

Bei Tag-Push `v*` baut `prebuild-mock` dieselbe OS/Node-Matrix, testet und
lädt die `.node`-Dateien als Release-Prebuilds hoch
(`node_pylon-<tag>-<os>-node<version>`). Echte SDK-Prebuilds ersetzen die
Mock-Artefakte, sobald ZAU-150 den Real-Build verdrahtet.
