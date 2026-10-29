'use strict';
// Legacy helper: synthetic Mono8 frame as a buffer (no camera grab).
// Works against real and mock builds; new code paths use
// camera.grab() / retrieveResult() (see samples/grab.js).
const pylon = require('../lib');

const width = Number(process.argv[2] || 64);
const height = Number(process.argv[3] || 48);
const buf = pylon.grabMock(width, height);
console.log(`grabMock: ${width}x${height} Mono8, ${buf.length} bytes, mock=${pylon.isMock()}`);
// Quick plausibility check: deterministic pattern (x+y)&0xFF.
console.log('first bytes:', Array.from(buf.subarray(0, 8)).join(','));
