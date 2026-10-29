'use strict';
// Image attributes + chunk data from grab(). Runs without a camera.
const pylon = require('../lib');

const cam = pylon.createAndOpenCamera();
cam.setInteger('Width', 64);
cam.setInteger('Height', 48);
cam.setEnum('PixelFormat', 'Mono8');
cam.setBoolean('ChunkModeActive', true);

for (let i = 0; i < 3; i++) {
  const g = cam.grab();
  const expected = g.width * g.height * (g.pixelFormat === 'RGB8' ? 3 : g.pixelFormat === 'Mono12' ? 2 : 1);
  const ok = g.buffer.length === expected && g.buffer.length === g.payloadSize;
  console.log(`frame ${g.frameNumber}: ${g.width}x${g.height} ${g.pixelFormat} payload=${g.payloadSize} ts=${g.timestamp} chunk=${JSON.stringify(g.chunk)} ok=${ok}`);
  if (!ok) {
    console.error('Payload size does not match Width/Height/PixelFormat');
    process.exit(1);
  }
}
cam.close();
console.log('OK: image handling (attributes + chunk + buffer) consistent.');
