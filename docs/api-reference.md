# API-Referenz

Quelle: `lib/index.js`, Typen: `types/index.d.ts`.

| Funktion | Signatur | Beschreibung |
|---|---|---|
| `version()` | `() => string` | Modulversion (`mock`-Suffix nur im Mock-Opt-in) |
| `pylonVersion()` | `() => string` | pylon-Version aus dem SDK (z. B. `12.5.2.1565`) bzw. Mock-Kennung |
| `isMock()` | `() => boolean` | `true` nur im Mock-Build (`NODE_PYLON_MOCK=1`) |
| `isNative()` | `() => boolean` | `true`, wenn das native Modul geladen ist |
| `mockActive()` | `() => boolean` | `true` nur im Mock-Opt-in (`NODE_PYLON_USE_MOCK=1` ohne natives Modul) |
| `initialize()` / `terminate()` | `() => void` | Pylon-Lifecycle (Refcount; jede `initialize()`-Referenz braucht `terminate()`) |
| `enumerateDevices()` | `() => PylonDeviceInfo[]` | Geräteinfos via TlFactory (leer ohne Kamera/Emulator) |
| `createFirstCamera()` / `createFirst()` | `() => InstantCamera` | TlFactory.CreateFirstDevice — wirft ohne erkannte Kamera |
| `TlFactory` | Objekt | `GetInstance()`/`EnumerateDevices()`/`CreateFirstDevice()` (+ Kleinschreib-Aliase), analog pypylon |
| `InstantCamera` | Klasse | `open/close`, Nodemap (`get/set*`, `tryMax`, `executeCommand`), Grab-Loop (`startGrabbing/stopGrabbing/isGrabbing/retrieveResult`), `grab()` |
| `createAndOpenCamera()` | `() => InstantCamera` | Kurzform: erzeugt + öffnet eine Kamera |
| `grabMock(w?, h?)` | `(w=64, h=48) => Buffer` | synthetischer Mono8-Frame, Muster `(x+y)&0xFF` (Legacy-Helper) |

`PylonDeviceInfo`: `{ friendlyName, modelName, serialNumber, vendorName, deviceClass, fullName?, [key: string]: unknown }`.
`retrieveResult(timeoutMs)` gibt `null` bei Timeout zurück, sonst
`{ succeeded, width, height, pixelType, pixelFormat, payloadSize, buffer, errorCode?, errorDescription? }`.
