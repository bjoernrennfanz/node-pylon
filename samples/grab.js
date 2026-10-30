'use strict';
// Grab sample mirroring pypylon `grab.py` (simplified):
// TlFactory -> FirstDevice -> Open -> StartGrabbing -> RetrieveResult -> Close.
// Uses the camera emulator when no hardware is attached (PYLON_CAMEMU=2).
const pylon = require('../lib');

const NUM_FRAMES = Number(process.argv[2] || 5);

pylon.initialize();
try {
  const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
  console.log(`devices found: ${devices.length}`);
  for (const d of devices) console.log(' -', JSON.stringify(d));
  if (devices.length === 0) {
    console.error('No camera found (no hardware, no emulator).');
    process.exit(1);
  }

  // pypylon grab.py: InstantCamera(TlFactory.GetInstance().CreateFirstDevice())
  // (createFirstCamera attaches the first device to a real InstantCamera;
  // the raw TlFactory device handle has no grab methods).
  const camera = pylon.createFirstCamera();
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
