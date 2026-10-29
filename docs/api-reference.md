# API reference

Source: `lib/index.js`, types: `types/index.d.ts`.

| Function | Signature | Description |
|---|---|---|
| `version()` | `() => string` | Module version (`mock` suffix only in the mock opt-in) |
| `pylonVersion()` | `() => string` | pylon version from the SDK (e.g. `12.5.2.1565`) or mock identifier |
| `isMock()` | `() => boolean` | `true` only in mock builds (`NODE_PYLON_MOCK=1`) |
| `isNative()` | `() => boolean` | `true` when the native module is loaded |
| `mockActive()` | `() => boolean` | `true` only in the mock opt-in (`NODE_PYLON_USE_MOCK=1` without a native module) |
| `initialize()` / `terminate()` | `() => void` | Pylon lifecycle (refcounted; each `initialize()` reference needs `terminate()`) |
| `enumerateDevices()` | `() => PylonDeviceInfo[]` | Device infos via TlFactory (empty without camera/emulator) |
| `createFirstCamera()` / `createFirst()` | `() => InstantCamera` | TlFactory.CreateFirstDevice — throws without a detected camera |
| `TlFactory` | object | `GetInstance()`/`EnumerateDevices()`/`CreateFirstDevice()` (+ lowercase aliases), mirrors pypylon |
| `InstantCamera` | class | `open/close`, nodemap (`get/set*`, `tryMax`, `executeCommand`), grab loop (`startGrabbing/stopGrabbing/isGrabbing/retrieveResult`), `grab()` |
| `createAndOpenCamera()` | `() => InstantCamera` | Shorthand: creates + opens a camera |
| `grabMock(w?, h?)` | `(w=64, h=48) => Buffer` | Synthetic Mono8 frame, pattern `(x+y)&0xFF` (legacy helper) |

`PylonDeviceInfo`: `{ friendlyName, modelName, serialNumber, vendorName, deviceClass, fullName?, [key: string]: unknown }`.
`retrieveResult(timeoutMs)` returns `null` on timeout, otherwise
`{ succeeded, width, height, pixelType, pixelFormat, payloadSize, buffer, errorCode?, errorDescription? }`.
