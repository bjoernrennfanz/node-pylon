'use strict';
// Legacy-Helper: synthetischer Mono8-Frame als Buffer (kein Kamera-Grab).
// Funktioniert gegen Real- und Mock-Builds; neue Codepfade nutzen
// camera.grab() bzw. retrieveResult() (siehe samples/grab.js).
const pylon = require('../lib');

const width = Number(process.argv[2] || 64);
const height = Number(process.argv[3] || 48);
const buf = pylon.grabMock(width, height);
console.log(`grabMock: ${width}x${height} Mono8, ${buf.length} bytes, mock=${pylon.isMock()}`);
// Kleine Plausibilitaetspruefung: deterministisches Muster (x+y)&0xFF.
console.log('first bytes:', Array.from(buf.subarray(0, 8)).join(','));
