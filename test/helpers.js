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

module.exports = { deviceCount, requireDevice, hasNode, pickInt };
