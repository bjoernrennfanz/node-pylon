'use strict';
// Shared test support: camera availability via the SDK emulator
// (PYLON_CAMEMU) or a real camera. Device-dependent tests skip when no
// device is reachable; everything runs against the real GenApi nodemap.
const pylon = require('../lib');

function deviceCount() {
  // Balanced initialize/terminate; 0 on any error (no device).
  try {
    pylon.initialize();
    try {
      return pylon.enumerateDevices().length;
    } finally {
      pylon.terminate();
    }
  } catch {
    return 0;
  }
}

// Skips the test when no camera/emulator is reachable.
// Returns true when a device exists.
function requireDevice(t) {
  if (deviceCount() === 0) {
    t.skip('no camera/emulator available (set PYLON_CAMEMU=2)');
    return false;
  }
  return true;
}

// True when the named node exists on the open camera.
function hasNode(cam, name) {
  try {
    cam.getNodeType(name);
    return true;
  } catch {
    return false;
  }
}

// Picks a valid integer within [min, max], preferring the given value.
function pickInt(min, max, preferred) {
  if (preferred >= min && preferred <= max) return preferred;
  return min;
}

// Counts reachable devices whose pylon device class contains `fragment`
// (device classes are 'BaslerGigE', 'BaslerUsb', 'BaslerCamEmu', ...).
// Mirrors pypylon's pylongigetestcase.py / pylonusbtestcase.py filtering:
// 0 on any error (no native build, no hardware) so hardware suites skip.
function deviceCountByClass(deviceClass) {
  try {
    pylon.initialize();
    try {
      return pylon.enumerateDevices().filter((d) => {
        const cls = d.deviceClass || d.DeviceClass || '';
        return String(cls).toLowerCase().includes(String(deviceClass).toLowerCase());
      }).length;
    } finally {
      pylon.terminate();
    }
  } catch {
    return 0;
  }
}

// Skips the test unless a physical GigE camera is reachable.
// (GitHub CI only provides the CamEmu emulator, so gigE suites always
// skip there; run `npm run test:gige` on a host with GigE hardware.)
// Returns true when a GigE device exists.
function requireGigE(t) {
  if (deviceCountByClass('GigE') === 0) {
    t.skip('no GigE camera available (hardware-only suite, CI runs emulated only)');
    return false;
  }
  return true;
}

// Skips the test unless a physical USB camera is reachable.
// (Same CI note as requireGigE; run `npm run test:usb` on hardware.)
// Returns true when a USB device exists.
function requireUsb(t) {
  if (deviceCountByClass('Usb') === 0) {
    t.skip('no USB camera available (hardware-only suite, CI runs emulated only)');
    return false;
  }
  return true;
}

module.exports = { deviceCount, requireDevice, hasNode, pickInt, deviceCountByClass, requireGigE, requireUsb };
