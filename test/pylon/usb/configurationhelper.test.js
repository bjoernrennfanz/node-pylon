'use strict';
// pylon/usb: configuration helper on physical hardware (mirrors pypylon's
// tests/pylon/usb/configurationhelper_test.py). Hardware-only: skips without
// a USB camera — GitHub CI only provides the CamEmu emulator, so CI runs
// the emulated suite (`npm run test:emulated`) instead.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb configuration helper (hardware only)', () => {
  it('probePacketSize is a safe no-op probe on the USB camera', (t) => {
    if (!requireUsb(t)) return;
    const cam = pylon.createFirstCamera();
    cam.open();
    try {
      const result = pylon.ConfigurationHelper.probePacketSize(cam);
      assert.equal(typeof result, 'boolean');
    } finally {
      cam.close();
    }
  });
});
