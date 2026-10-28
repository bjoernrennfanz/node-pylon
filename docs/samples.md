# Samples

Alle Samples laufen ohne Kamera (Mock-Build).

| Sample | Aufruf | Was es zeigt |
|---|---|---|
| `samples/list-devices.js` | `npm run sample:list` | Versionen + `enumerateDevices()` ausgeben |
| `samples/grab-mock.js` | `npm run sample:grab` | `grabMock(64, 48)`, Länge + erste Bytes |
| `samples/grab-options.js` | `node samples/grab-options.js [w] [h]` | Breite/Höhe per CLI, Musterprüfung `(x+y)&0xFF` |

Legacy-Dateien `samples/Grab.js` / `samples/InspectModule.js` stammen aus dem
alten C#-Generator und sind kein Teil der Node-API; sie bleiben bis zur
Bereinigung unangetastet.
