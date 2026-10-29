'use strict';
// Shared test helpers: mock opt-in vs. real-SDK mode and
// camera availability (emulator via PYLON_CAMEMU or a real camera).
const pylon = require('../lib');

function isMockMode() {
  return pylon.isMock();
}

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

// Skips the test when no camera/emulator is reachable
// (real mode without a device). Returns true when a device exists.
function requireDevice(t) {
  if (deviceCount() === 0) {
    t.skip('no camera/emulator available (set PYLON_CAMEMU=1 for the SDK emulator)');
    return false;
  }
  return true;
}

module.exports = { isMockMode, deviceCount, requireDevice };
