# API reference

Source: `lib/index.js`, types: `types/index.d.ts`.

| Function | Signature | Description |
|---|---|---|
| `version()` | `() => string` | Module version |
| `pylonVersion()` | `() => string` | pylon version from the SDK (e.g. `12.5.2.1565`) |
| `initialize()` / `terminate()` | `() => void` | Pylon lifecycle (refcounted; every `initialize()` needs a `terminate()`) |
| `enumerateDevices()` | `() => PylonDeviceInfo[]` | Device infos via TlFactory (empty without camera/emulator) |
| `createFirstCamera()` / `createFirst()` | `() => InstantCamera` | TlFactory.CreateFirstDevice — throws without a detected camera |
| `TlFactory` | object | `GetInstance()`/`EnumerateDevices()`/`CreateFirstDevice()` (+ lowercase aliases), like pypylon |
| `InstantCamera` | class | `open/close`, nodemap (`get/set*`, `tryMax`, `executeCommand`), grab loop (`startGrabbing/stopGrabbing/isGrabbing/retrieveResult`), `grab()` |
| `createAndOpenCamera()` | `() => InstantCamera` | Short form: creates + opens a camera |

`PylonDeviceInfo`: `{ friendlyName, modelName, serialNumber, vendorName, deviceClass, fullName?, [key: string]: unknown }`.
`retrieveResult(timeoutMs)` returns `null` on timeout, otherwise
`{ succeeded, width, height, pixelType, pixelFormat, payloadSize, buffer, errorCode?, errorDescription? }`.
