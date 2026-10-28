# API-Referenz (Scaffold-Stand)

Quelle: `lib/index.js`, Typen: `types/index.d.ts`.

| Funktion | Signatur | Beschreibung |
|---|---|---|
| `version()` | `() => string` | Modulversion (enthält `scaffold` im Mock-Stand) |
| `pylonVersion()` | `() => string` | pylon-Version bzw. Mock-Kennung |
| `isMock()` | `() => boolean` | `true` im Mock-Build ohne SDK |
| `enumerateDevices()` | `() => PylonDeviceInfo[]` | Geräteinfos (Mock: `[]`) |
| `grabMock(w?, h?)` | `(w=64, h=48) => Buffer` | synthetischer Mono8-Frame, Muster `(x+y)&0xFF` |

`PylonDeviceInfo`: `{ friendlyName?, modelName?, serialNumber?, [key: string]: unknown }`.

Erweiterungen (TlFactory, InstantCamera, Nodemap) folgen in ZAU-150/151
und werden hier dokumentiert, sobald sie im Mock oder echt verfügbar sind.
