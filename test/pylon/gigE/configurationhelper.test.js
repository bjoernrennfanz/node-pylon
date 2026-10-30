'use strict';
// pylon/gigE: configuration helper on physical hardware (mirrors pypylon's
// tests/pylon/gigE/configurationhelper_test.py). Hardware-only: skips without
// a GigE camera — GitHub CI only provides the CamEmu emulator, so CI runs
// the emulated suite (`npm run test:emulated`) instead.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE configuration helper (hardware only)', () => {
  it('probePacketSize runs against the GigE camera', (t) => {
    if (!requireGigE(t)) return;
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
