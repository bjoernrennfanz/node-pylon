# Programmer's Guide (analog pypylon, Stand ZAU-152)

Kapitelstruktur wie im pypylon Programmer's Guide; `[Mock]` = bereits nutzbar,
`[geplant]` = folgt mit ZAU-150/151.

1. [Mock] Transport Layer / Geräte aufzählen — `enumerateDevices()` (Mock: `[]`)
2. [Mock] Ersten Frame holen — `grabMock(w, h)` als deterministischer Mono8-Buffer
3. [geplant] InstantCamera öffnen, Grab-Loop, Software-Trigger (ZAU-150)
4. [geplant] Nodemap/Parameter: Integer/Float/Enum/Boolean/String/Command (ZAU-151)
5. [geplant] Bild-Handling: Buffer/TypedArray zero-copy, GenDC-Komponenten (ZAU-151)
6. [geplant] Fehlerbehandlung: pylon-Fehlercodes → JS-Exceptions (ZAU-150/151)
7. [geplant] Threading: Grab-Loop/Events ohne Blockade des JS-Loops (ZAU-150)

N-API-Regeln (verbindlich): nur `node-addon-api`/N-API, kein NAN/SWIG/V8;
`NAPI_VERSION=8`; Fehler als JS-Exceptions; keine Handles leaken.
