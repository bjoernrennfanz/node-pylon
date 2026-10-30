'use strict';
// pylon: InstantCamera open/grab-loop/close flow (analogous to pypylon's
// instantcamera_test.py grab sections). Uses the emulator when no
// hardware is attached.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice } = require('../../helpers');

describe('pylon InstantCamera', () => {
  it('open/grab-loop/close flow', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
      assert.equal(camera.isOpen(), false);
      assert.equal(camera.isGrabbing(), false);
      camera.open();
      assert.equal(camera.isOpen(), true);
      camera.startGrabbing();
      assert.equal(camera.isGrabbing(), true);
      const res = camera.retrieveResult(10000);
      assert.ok(res !== null);
      assert.equal(res.succeeded, true);
      assert.ok(res.width > 0 && res.height > 0);
      assert.equal(res.buffer.length, res.payloadSize);
      assert.equal(typeof res.pixelFormat, 'string');
      camera.stopGrabbing();
      assert.equal(camera.isGrabbing(), false);
      camera.close();
      assert.equal(camera.isOpen(), false);
    } finally {
      pylon.terminate();
    }
  });

  it('startGrabbing without open throws', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.createFirstCamera();
      assert.throws(() => camera.startGrabbing(), /not open/i);
    } finally {
      pylon.terminate();
    }
  });

  it('retrieveResult without grabbing throws', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.createFirstCamera();
      camera.open();
      try {
        assert.throws(() => camera.retrieveResult(100), /Not grabbing/);
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});
