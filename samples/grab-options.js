'use strict';
// Grab with CLI options (mirrors the pypylon grab sample).
// Usage: node samples/grab-options.js [width] [height]
// Uses the camera emulator when no hardware is attached (PYLON_CAMEMU=2).
const pylon = require('../lib');

const width = Number(process.argv[2] || 64);
const height = Number(process.argv[3] || 48);
if (!Number.isInteger(width) || !Number.isInteger(height) || width <= 0 || height <= 0) {
  console.error('usage: node samples/grab-options.js [width] [height]');
  process.exitCode = 2;
  return;
}

pylon.initialize();
try {
  // pypylon: InstantCamera(TlFactory.GetInstance().CreateFirstDevice())
  const cam = pylon.InstantCamera.wrapNative(
    pylon.TlFactory.GetInstance().CreateFirstDevice()
  );
  cam.open();
  try {
    const wMin = cam.getIntegerMin('Width');
    const wMax = cam.getIntegerMax('Width');
    const hMin = cam.getIntegerMin('Height');
    const hMax = cam.getIntegerMax('Height');
    const w = Math.min(wMax, Math.max(wMin, width));
    const h = Math.min(hMax, Math.max(hMin, height));
    cam.setInteger('Width', w);
    cam.setInteger('Height', h);
    const g = cam.grab();
    console.log(`grab-options: ${g.width}x${g.height} ${g.pixelFormat}, ${g.buffer.length} bytes`);
    const expected = g.width * g.height * (g.pixelFormat === 'RGB8' ? 3 : g.pixelFormat === 'Mono12' ? 2 : 1);
    if (g.buffer.length !== expected || g.buffer.length !== g.payloadSize) {
      console.error(`payload mismatch: got ${g.buffer.length}, want ${expected}`);
      process.exitCode = 1;
    } else {
      console.log(`payload ok, first bytes: ${Array.from(g.buffer.subarray(0, 8)).join(',')}`);
    }
  } finally {
    cam.close();
  }
} finally {
  pylon.terminate();
}
