'use strict';
// Grab mit CLI-Optionen (analog pypylon grab-Sample, Mock ohne Kamera).
// Aufruf: node samples/grab-options.js [width] [height]
const pylon = require('../lib');

const width = Number(process.argv[2] || 64);
const height = Number(process.argv[3] || 48);
if (!Number.isInteger(width) || !Number.isInteger(height) || width <= 0 || height <= 0) {
  console.error('usage: node samples/grab-options.js [width] [height]');
  process.exitCode = 2;
  return;
}
const buf = pylon.grabMock(width, height);
console.log(`grab-options: ${width}x${height} Mono8, ${buf.length} bytes, mock=${pylon.isMock()}`);
if (buf.length !== width * height) {
  console.error(`length mismatch: got ${buf.length}, want ${width * height}`);
  process.exitCode = 1;
  return;
}
// Deterministisches Muster (x+y)&0xFF wie in src/addon.cc.
let bad = -1;
for (let y = 0; y < height && bad < 0; y += 1) {
  for (let x = 0; x < width; x += 1) {
    if (buf[y * width + x] !== ((x + y) & 0xff)) { bad = y * width + x; break; }
  }
}
if (bad >= 0) {
  console.error(`pattern mismatch at byte ${bad}`);
  process.exitCode = 1;
} else {
  console.log(`pattern ok, first bytes: ${Array.from(buf.subarray(0, 8)).join(',')}`);
}
