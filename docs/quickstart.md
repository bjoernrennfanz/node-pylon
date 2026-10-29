# Quickstart (real SDK; emulator instead of a camera)

```bash
npm run rebuild                            # real build (SDK via PYLON_SDK_BASE_URL)
export PYLON_CAMEMU=2                      # two emulator devices, no camera needed
npm run sample:list                        # list devices (emulator: 2)
npm run sample:grab                        # 5 frames via RetrieveResult (emulator)
node samples/grab-options.js 100 80        # 100x80 frame, check first bytes
```

```js
const pylon = require('node-pylon');
pylon.initialize();
try {
  console.log(pylon.version(), pylon.pylonVersion(), 'mock:', pylon.isMock());
  console.log(pylon.enumerateDevices()); // [] without camera/emulator
  const frame = pylon.grabMock(64, 48);  // Buffer, 64*48 bytes Mono8, pattern (x+y)&0xFF
} finally {
  pylon.terminate();
}
```

Next: [Programmer's guide](programmers-guide.md), [Samples](samples.md).
Mock only as an opt-in: `npm run rebuild:mock` + `NODE_PYLON_USE_MOCK=1`
(see [installation](installation.md)).
