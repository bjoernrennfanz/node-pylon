# Programmer's Guide (analog pypylon)

Kapitelstruktur wie im pypylon Programmer's Guide; Standard ist der
Real-SDK-Build 26.09 (Mock nur als Opt-in ohne SDK).

1. Transport Layer / Geräte aufzählen — `enumerateDevices()` (leer ohne Kamera/Emulator; `PYLON_CAMEMU=N` stellt Emulator-Geräte bereit)
2. Kamera öffnen — `TlFactory.GetInstance().CreateFirstDevice()` + `open()` (wirft ohne erkanntes Gerät)
3. Grab-Loop — `startGrabbing()` / `retrieveResult(timeoutMs)` (`null` bei Timeout) / `stopGrabbing()`, analog pypylon
4. Nodemap/Parameter: Integer/Float/Enum/Boolean/String/Command (`get/set*`, `get*Min/Max`, `tryMax`, `executeCommand`); Fehler als JS-Exceptions (`Unknown node`, Typfehler, `RangeError`)
5. Bild-Handling: `grab()` → `{ buffer, width, height, pixelFormat, payloadSize, timestamp, frameNumber, chunk }` (Buffer zero-copy); `retrieveResult()` zusätzlich mit `succeeded`/`errorCode`/`errorDescription`
6. Fehlerbehandlung: pylon-`GenericException`s werden in JS-Exceptions übersetzt; keine C++-Exceptions über die N-API-Grenze
7. Threading: Grab-Loop/Events blockieren nie den JS-Loop (AsyncWorker/ThreadSafeFunction-Prinzip)

N-API-Regeln (verbindlich): nur `node-addon-api`/N-API, kein NAN/SWIG/V8;
`NAPI_VERSION=8`; Fehler als JS-Exceptions; keine Handles leaken.
