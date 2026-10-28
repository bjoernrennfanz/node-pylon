'use strict';
// ZAU-151 Abnahme-Sample: Parameter lesen/schreiben ueber alle Nodemap-Typen
// (Integer/Float/Enum/Boolean/String/Command, Get/Set/TryMax) + DeviceInfo
// + Bild-Attribute aus grab(). Laeuft ohne Kamera (Mock bzw. natives Modul).
const pylon = require('../lib');

const cam = pylon.createAndOpenCamera();
console.log('node-pylon version:', pylon.version(), '| native:', pylon.isNative(), '| mock:', pylon.isMock());
console.log('deviceInfo:', JSON.stringify(cam.getDeviceInfo()));

// Integer: Width lesen, aendern, Min/Max, TryMax.
console.log('Width:', cam.getInteger('Width'), `(min ${cam.getIntegerMin('Width')}, max ${cam.getIntegerMax('Width')})`);
cam.setInteger('Width', 320);
console.log('Width nach setInteger(320):', cam.getInteger('Width'));
console.log('Height tryMax() ->', cam.tryMax('Height'), '| jetzt:', cam.getInteger('Height'));

// Float: ExposureTime + Gain.
console.log('ExposureTime:', cam.getFloat('ExposureTime'));
cam.setFloat('ExposureTime', 8000.5);
console.log('ExposureTime nach setFloat(8000.5):', cam.getFloat('ExposureTime'));
console.log('Gain-Limits:', cam.getFloatMin('Gain'), '..', cam.getFloatMax('Gain'));

// Enumeration: PixelFormat lesen, Entries, setzen.
console.log('PixelFormat:', cam.getEnum('PixelFormat'), '| Entries:', cam.getEnumEntries('PixelFormat').join(','));
cam.setEnum('PixelFormat', 'RGB8');
console.log('PixelFormat nach setEnum(RGB8):', cam.getEnum('PixelFormat'));
cam.setEnum('PixelFormat', 'Mono8');

// Boolean + String + Command.
cam.setBoolean('ChunkModeActive', true);
console.log('ChunkModeActive:', cam.getBoolean('ChunkModeActive'));
cam.setString('DeviceUserID', 'ZAU-151-Demo');
console.log('DeviceUserID:', cam.getString('DeviceUserID'));
console.log('DeviceModelName (readonly):', cam.getString('DeviceModelName'));
cam.executeCommand('AcquisitionStart');
console.log('AcquisitionStart ausgefuehrt.');

// Bild-Attribute aus grab(): Width/Height/PixelFormat spiegeln die Nodemap.
const grab = cam.grab();
console.log(`grab: ${grab.width}x${grab.height} ${grab.pixelFormat}, payload=${grab.payloadSize}, frame=${grab.frameNumber}`);
console.log('buffer bytes:', grab.buffer.length, '| chunk:', JSON.stringify(grab.chunk));
console.log('first bytes:', Array.from(grab.buffer.subarray(0, 8)).join(','));
cam.executeCommand('AcquisitionStop');
cam.close();
console.log('OK: Nodemap lesen/schreiben + grab erfolgreich.');
