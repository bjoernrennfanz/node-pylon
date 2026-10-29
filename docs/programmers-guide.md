# Programmer's guide (mirrors pypylon)

Chapter structure follows the pypylon programmer's guide; the only
supported build is the real-SDK build 26.09.

1. Transport layer / enumerate devices — `enumerateDevices()` (empty without camera/emulator; `PYLON_CAMEMU=N` provides emulator devices)
2. Open a camera — `TlFactory.GetInstance().CreateFirstDevice()` + `open()` (throws without a detected device)
3. Grab loop — `startGrabbing()` / `retrieveResult(timeoutMs)` (`null` on timeout) / `stopGrabbing()`, mirrors pypylon
4. Nodemap/parameters: Integer/Float/Enum/Boolean/String/Command (`get/set*`, `get*Min/Max`, `tryMax`, `executeCommand`); errors as JS exceptions (`Unknown node`, type errors, `RangeError`)
5. Image handling: `grab()` → `{ buffer, width, height, pixelFormat, payloadSize, timestamp, frameNumber, chunk }` (zero-copy buffer); `retrieveResult()` additionally with `succeeded`/`errorCode`/`errorDescription`
6. Error handling: pylon `GenericException`s are translated into JS exceptions; no C++ exceptions across the N-API boundary
7. Threading: grab loop/events never block the JS loop (AsyncWorker/ThreadSafeFunction principle)

N-API rules (binding): only `node-addon-api`/N-API, no NAN/SWIG/V8;
`NAPI_VERSION=8`; errors as JS exceptions; never leak handles.
