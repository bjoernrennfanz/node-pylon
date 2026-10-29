# Samples

All samples run against the real-SDK build; without a physical camera,
enable the emulator (`export PYLON_CAMEMU=2`).

| Sample | Invocation | What it shows |
|---|---|---|
| `samples/list-devices.js` | `npm run sample:list` | Print versions + `enumerateDevices()` |
| `samples/grab.js` | `npm run sample:grab [n]` | TlFactory → FirstDevice → Open → StartGrabbing → RetrieveResult (mirrors pypylon `grab.py`) |
| `samples/grab-options.js` | `node samples/grab-options.js [w] [h]` | Width/height via CLI, pattern check `(x+y)&0xFF` |
| `samples/nodemap-params.js` | `npm run sample:nodemap` | Read/write parameters across all nodemap types + device info |
| `samples/grab-attributes.js` | `npm run sample:grabattrs` | Image attributes, payload consistency, chunk data |
| `samples/grab-mock.js` (legacy) | `npm run sample:grab-mock` | `grabMock(64, 48)` helper, length + first bytes |
