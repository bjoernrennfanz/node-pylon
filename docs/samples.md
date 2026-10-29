# Samples

Alle Samples laufen gegen den Real-SDK-Build; ohne physische Kamera den
Emulator aktivieren (`export PYLON_CAMEMU=2`).

| Sample | Aufruf | Was es zeigt |
|---|---|---|
| `samples/list-devices.js` | `npm run sample:list` | Versionen + `enumerateDevices()` ausgeben |
| `samples/grab.js` | `npm run sample:grab [n]` | TlFactory → FirstDevice → Open → StartGrabbing → RetrieveResult (analog pypylon `grab.py`) |
| `samples/grab-options.js` | `node samples/grab-options.js [w] [h]` | Breite/Höhe per CLI, Musterprüfung `(x+y)&0xFF` |
| `samples/nodemap-params.js` | `npm run sample:nodemap` | Parameter lesen/schreiben über alle Nodemap-Typen + DeviceInfo |
| `samples/grab-attributes.js` | `npm run sample:grabattrs` | Bild-Attribute, Payload-Konsistenz, Chunk-Daten |
| `samples/grab-mock.js` (legacy) | `npm run sample:grab-mock` | `grabMock(64, 48)`-Helper, Länge + erste Bytes |
