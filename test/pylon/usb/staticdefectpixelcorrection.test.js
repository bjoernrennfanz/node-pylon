'use strict';
// pylon/usb: static defect pixel correction on physical hardware (mirrors
// pypylon's tests/pylon/usb/staticdefectpixelcorrection_test.py).
// Hardware-only: skips without a USB camera — GitHub CI only provides the
// CamEmu emulator, so CI runs the emulated suite (`npm run test:emulated`)
// instead. Cameras without user-list support skip via RuntimeException,
// like the emulated counterpart.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb static defect pixel correction (hardware only)', () => {
  it('getDefectPixelList returns [success, vector] on the USB camera', (t) => {
    if (!requireUsb(t)) return;
    const cam = pylon.createFirstCamera();
    cam.open();
    try {
      const result = pylon.StaticDefectPixelCorrection.getDefectPixelList(
        cam, pylon.StaticDefectPixelCorrection.ListType_User);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(typeof result[0], 'boolean');
    } catch (err) {
      if (err instanceof pylon.RuntimeException) {
        t.skip(`no defect-pixel-list support on this camera: ${err.message}`);
        return;
      }
      throw err;
    } finally {
      cam.close();
    }
  });
});
