# Samples

All samples run against the real-SDK build; without physical hardware,
enable the emulator (`export PYLON_CAMEMU=2`).

| Sample | Call | What it shows |
|---|---|---|
| `samples/list-devices.js` | `npm run sample:list` | Versions + `enumerateDevices()` output |
| `samples/grab.js` | `npm run sample:grab [n]` | TlFactory → FirstDevice → Open → StartGrabbing → RetrieveResult (like pypylon `grab.py`) |
| `samples/grab-options.js` | `node samples/grab-options.js [w] [h]` | Width/height via CLI, payload check |
| `samples/nodemap-params.js` | `npm run sample:nodemap` | Read/write parameters across all nodemap types + DeviceInfo |
| `samples/grab-attributes.js` | `npm run sample:grabattrs` | Image attributes, payload consistency, chunk data |
