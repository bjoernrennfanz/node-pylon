# Quickstart (ohne Kamera, Mock)

```bash
npm run sample:list   # Geräte auflisten (Mock: 0)
npm run sample:grab   # synthetischer Mono8-Frame 64x48
node samples/grab-options.js 100 80  # 100x80-Frame, erste Bytes prüfen
```

```js
const pylon = require('node-pylon');
console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
console.log(pylon.enumerateDevices()); // [] im Mock
const frame = pylon.grabMock(64, 48);  // Buffer, 64*48 Bytes Mono8, Muster (x+y)&0xFF
```

Weiter: [Programmer's Guide](programmers-guide.md), [Samples](samples.md).
