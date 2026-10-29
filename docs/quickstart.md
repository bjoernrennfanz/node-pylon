# Quickstart (Real-SDK; Emulator statt Kamera)

```bash
npm run rebuild                            # echter Build (SDK via PYLON_SDK_BASE_URL)
export PYLON_CAMEMU=2                      # zwei Emulator-Geräte, keine Kamera nötig
npm run sample:list                        # Geräte auflisten (Emulator: 2)
npm run sample:grab                        # 5 Frames via RetrieveResult (Emulator)
node samples/grab-options.js 100 80        # 100x80-Frame, erste Bytes prüfen
```

```js
const pylon = require('node-pylon');
pylon.initialize();
try {
  console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
  console.log(pylon.enumerateDevices()); // [] ohne Kamera/Emulator
  const frame = pylon.grabMock(64, 48);  // Buffer, 64*48 Bytes Mono8, Muster (x+y)&0xFF
} finally {
  pylon.terminate();
}
```

Weiter: [Programmer's Guide](programmers-guide.md), [Samples](samples.md).
Mock nur als Opt-in: `npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`
(siehe [Installation](installation.md)).
