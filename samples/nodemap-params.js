'use strict';
// Acceptance sample: read/write parameters across all nodemap types
// (Integer/Float/Enum/Boolean/String/Command, Get/Set/TryMax) + device info
// + image attributes from grab(). Needs a camera or the SDK emulator
// (PYLON_CAMEMU=2). All writes stay inside the device's reported limits.
const pylon = require('../lib');

const cam = pylon.createAndOpenCamera();
console.log('node-pylon version:', pylon.version(), '| pylon:', pylon.pylonVersion());
console.log('deviceInfo:', JSON.stringify(cam.getDeviceInfo()));

function clamp(v, mn, mx) {
  return Math.min(mx, Math.max(mn, v));
}

// Integer: read Width, change it, min/max, TryMax.
const wMin = cam.getIntegerMin('Width');
const wMax = cam.getIntegerMax('Width');
console.log('Width:', cam.getInteger('Width'), `(min ${wMin}, max ${wMax})`);
cam.setInteger('Width', clamp(320, wMin, wMax));
console.log('Width after setInteger:', cam.getInteger('Width'));
console.log('Height tryMax() ->', cam.tryMax('Height'), '| now:', cam.getInteger('Height'));

// Float: ExposureTime + Gain.
const eMin = cam.getFloatMin('ExposureTime');
const eMax = cam.getFloatMax('ExposureTime');
console.log('ExposureTime:', cam.getFloat('ExposureTime'));
const eTarget = clamp(8000.5, eMin, eMax);
cam.setFloat('ExposureTime', eTarget);
console.log('ExposureTime after setFloat:', cam.getFloat('ExposureTime'));
console.log('Gain limits:', cam.getFloatMin('Gain'), '..', cam.getFloatMax('Gain'));

// Enumeration: read PixelFormat, entries, set it.
const entries = cam.getEnumEntries('PixelFormat');
console.log('PixelFormat:', cam.getEnum('PixelFormat'), '| Entries:', entries.join(','));
const other = entries.find((e) => e !== cam.getEnum('PixelFormat')) || entries[0];
cam.setEnum('PixelFormat', other);
console.log('PixelFormat after setEnum:', cam.getEnum('PixelFormat'));

// Boolean + String + Command.
cam.setBoolean('ChunkModeActive', true);
console.log('ChunkModeActive:', cam.getBoolean('ChunkModeActive'));
cam.setString('DeviceUserID', 'demo-user');
console.log('DeviceUserID:', cam.getString('DeviceUserID'));
console.log('DeviceModelName (read-only):', cam.getString('DeviceModelName'));
// Execute a side-effect-free standard command if the device offers one
// (the emulator rejects the acquisition commands outside a running grab).
let executed = null;
for (const name of ['UserSetSave', 'ForceFailedBuffer', 'AcquisitionStop']) {
  try {
    cam.getNodeType(name);
    cam.executeCommand(name);
    executed = name;
    break;
  } catch {
    // Not offered here — try the next one.
  }
}
console.log('executed command:', executed === null ? '(none offered)' : executed);

// Image attributes from grab(): Width/Height/PixelFormat mirror the nodemap.
const grab = cam.grab();
console.log(`grab: ${grab.width}x${grab.height} ${grab.pixelFormat}, payload=${grab.payloadSize}, frame=${grab.frameNumber}`);
console.log('buffer bytes:', grab.buffer.length, '| chunk:', JSON.stringify(grab.chunk));
console.log('first bytes:', Array.from(grab.buffer.subarray(0, 8)).join(','));
cam.close();
console.log('OK: nodemap read/write + grab succeeded.');
