# Quickstart (real SDK; emulator instead of a camera)

```bash
npm run rebuild                            # real build (SDK via PYLON_SDK_BASE_URL)
export PYLON_CAMEMU=2                      # two emulator devices, no camera needed
npm run sample:list                        # list devices (emulator: 2 devices)
npm run sample:grab                        # 5 frames via RetrieveResult
node samples/grab-options.js 100 80        # 100x80 frame, payload check
```

```js
const pylon = require('node-pylon');
pylon.initialize();
try {
  console.log(pylon.version(), pylon.pylonVersion());
  console.log(pylon.enumerateDevices()); // [] without camera/emulator
  const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
  camera.open();
  try {
    camera.setInteger('Width', 320);
    const res = camera.retrieveResult(5000); // null on timeout
  } finally {
    camera.close();
  }
} finally {
  pylon.terminate();
}
```

Next: [Programmer's Guide](programmers-guide.md), [Samples](samples.md).
