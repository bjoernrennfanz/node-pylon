'use strict';
// pylon/gigE: grab on physical GigE hardware (mirrors pypylon's
// tests/pylon/gigE/grab_test.py). Hardware-only: skips without a GigE
// camera — GitHub CI only provides the CamEmu emulator, so CI runs the
// emulated suite (`npm run test:emulated`) instead.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

function firstIndexOf(fragment) {
  pylon.initialize();
  try {
    const devices = pylon.enumerateDevices();
    return devices.findIndex((d) => String(d.deviceClass || d.DeviceClass || '')
      .toLowerCase().includes(fragment));
  } finally {
    pylon.terminate();
  }
}

describe('pylon gigE grab (hardware only)', () => {
  it('grab one frame on the first GigE camera', (t) => {
    if (!requireGigE(t)) return;
    const cam = pylon.createCameraAt(firstIndexOf('gige'));
    cam.open();
    try {
      const g = cam.grab();
      assert.ok(Buffer.isBuffer(g.buffer));
      assert.equal(g.payloadSize, g.buffer.length);
      assert.equal(typeof g.pixelFormat, 'string');
    } finally {
      cam.close();
    }
  });
});
