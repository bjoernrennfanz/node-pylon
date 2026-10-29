'use strict';
// Gemeinsame Test-Helfer: Mock-Opt-in vs. Real-SDK-Modus und
// Kamera-Verfuegbarkeit (Emulator via PYLON_CAMEMU oder echte Kamera).
const pylon = require('../lib');

function isMockMode() {
  return pylon.isMock();
}

function deviceCount() {
  // initialize/terminate balanciert; false bei jedem Fehler (kein Geraet).
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

// Ueberspringt den Test, wenn keine Kamera/Emulator erreichbar ist
// (Real-Modus ohne Geraet). Gibt true zurueck, wenn ein Geraet da ist.
function requireDevice(t) {
  if (deviceCount() === 0) {
    t.skip('no camera/emulator available (set PYLON_CAMEMU=1 for the SDK emulator)');
    return false;
  }
  return true;
}

module.exports = { isMockMode, deviceCount, requireDevice };
