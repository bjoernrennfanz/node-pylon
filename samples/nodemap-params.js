'use strict';
// Acceptance sample: read/write parameters across all nodemap types
// (Integer/Float/Enum/Boolean/String/Command, Get/Set/TryMax) + device info
// + image attributes from grab(). Runs without a camera (mock or native module).
const pylon = require('../lib');

const cam = pylon.createAndOpenCamera();
console.log('node-pylon version:', pylon.version(), '| native:', pylon.isNative(), '| mock:', pylon.isMock());
console.log('deviceInfo:', JSON.stringify(cam.getDeviceInfo()));

// Integer: read Width, change it, min/max, TryMax.
console.log('Width:', cam.getInteger('Width'), `(min ${cam.getIntegerMin('Width')}, max ${cam.getIntegerMax('Width')})`);
cam.setInteger('Width', 320);
console.log('Width after setInteger(320):', cam.getInteger('Width'));
console.log('Height tryMax() ->', cam.tryMax('Height'), '| now:', cam.getInteger('Height'));

// Float: ExposureTime + Gain.
console.log('ExposureTime:', cam.getFloat('ExposureTime'));
cam.setFloat('ExposureTime', 8000.5);
console.log('ExposureTime after setFloat(8000.5):', cam.getFloat('ExposureTime'));
console.log('Gain limits:', cam.getFloatMin('Gain'), '..', cam.getFloatMax('Gain'));

// Enumeration: read PixelFormat, entries, set it.
console.log('PixelFormat:', cam.getEnum('PixelFormat'), '| Entries:', cam.getEnumEntries('PixelFormat').join(','));
cam.setEnum('PixelFormat', 'RGB8');
console.log('PixelFormat after setEnum(RGB8):', cam.getEnum('PixelFormat'));
cam.setEnum('PixelFormat', 'Mono8');

// Boolean + String + Command.
cam.setBoolean('ChunkModeActive', true);
console.log('ChunkModeActive:', cam.getBoolean('ChunkModeActive'));
cam.setString('DeviceUserID', 'demo-user');
console.log('DeviceUserID:', cam.getString('DeviceUserID'));
console.log('DeviceModelName (read-only):', cam.getString('DeviceModelName'));
cam.executeCommand('AcquisitionStart');
console.log('AcquisitionStart executed.');

// Image attributes from grab(): Width/Height/PixelFormat mirror the nodemap.
const grab = cam.grab();
console.log(`grab: ${grab.width}x${grab.height} ${grab.pixelFormat}, payload=${grab.payloadSize}, frame=${grab.frameNumber}`);
console.log('buffer bytes:', grab.buffer.length, '| chunk:', JSON.stringify(grab.chunk));
console.log('first bytes:', Array.from(grab.buffer.subarray(0, 8)).join(','));
cam.executeCommand('AcquisitionStop');
cam.close();
console.log('OK: nodemap read/write + grab succeeded.');
