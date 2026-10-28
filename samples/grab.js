'use strict';
// Grab-Sample analog pypylon `grab.py` (vereinfachte Variante):
// TlFactory -> FirstDevice -> Open -> StartGrabbing -> RetrieveResult -> Close.
// Laeuft gegen echte Kamera (realer Build) oder Mock-Kamera (Mock-Build).
const pylon = require('../lib');

const NUM_FRAMES = Number(process.argv[2] || 5);

pylon.initialize();
try {
  const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
  console.log(`devices found: ${devices.length}`);
  for (const d of devices) console.log(' -', JSON.stringify(d));
  if (devices.length === 0) {
    console.error('No camera found (weder echt noch Mock).');
    process.exit(1);
  }

  const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
  try {
    console.log('device:', JSON.stringify(camera.getDeviceInfo()));
    camera.open();
    console.log('open:', camera.isOpen());
    camera.startGrabbing();
    console.log('grabbing:', camera.isGrabbing());
    for (let i = 0; i < NUM_FRAMES; i += 1) {
      const res = camera.retrieveResult(5000);
      if (res === null) {
        console.log(`frame ${i}: timeout`);
        continue;
      }
      const ok = res.succeeded ? 'grabbed' : 'FAILED';
      console.log(
        `frame ${i}: ${ok} ${res.width}x${res.height} ${res.pixelFormat}, ${res.buffer.length} bytes`
      );
      console.log('  first bytes:', Array.from(res.buffer.subarray(0, 8)).join(','));
    }
    camera.stopGrabbing();
    camera.close();
    console.log('done.');
  } finally {
    // Native Destruktor sichert StopGrabbing/Close zusätzlich ab.
  }
} finally {
  pylon.terminate();
}
