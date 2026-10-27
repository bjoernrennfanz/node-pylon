'use strict';
const pylon = require('../lib');

console.log('node-pylon version:', pylon.version());
console.log('pylon version:', pylon.pylonVersion());
console.log('mock build:', pylon.isMock());
const devices = pylon.enumerateDevices();
console.log(`devices found: ${devices.length}`);
for (const d of devices) console.log(' -', JSON.stringify(d));
