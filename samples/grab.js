'use strict';
// Grab sample mirroring pypylon `grab.py` (simplified):
// TlFactory -> FirstDevice -> Open -> StartGrabbing -> RetrieveResult -> Close.
// Runs against a real camera (real build) or the mock camera (mock build).
const pylon = require('../lib');

const NUM_FRAMES = Number(process.argv[2] || 5);

pylon.initialize();
try {
  const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
  console.log(`devices found: ${devices.length}`);
  for (const d of devices) console.log(' -', JSON.stringify(d));
  if (devices.length === 0) {
    console.error('No camera found (neither real nor mock).');
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
    // The native destructor also covers StopGrabbing/Close.
  }
} finally {
  pylon.terminate();
}
