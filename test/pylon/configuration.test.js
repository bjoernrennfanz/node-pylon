'use strict';
// pylon: grab-loop lifecycle and acquisition configuration.
// Covers the emulated pypylon configuration tests that map onto the MVP
// wrapper: instantcamera_test.py grab sections, acquiresingleframe- and
// acquirecontinuousconfiguration_test.py (start/stop grabbing),
// softwaretriggerconfiguration_test.py (timeout/return semantics via
// retrieveResult), configurationhelper_test.py (open/grab/close flow).
// Event-handler registration (cameraeventhandler, imageeventhandler,
// configurationeventhandler), buffer factories (bufferfactory_test.py) and
// wait objects (waitobject(s)_test.py) have no MVP wrapper counterpart and
// are documented as deviations in docs/testing.md.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

describe('pylon acquisition configuration', () => {
  it('open and close are idempotent', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    assert.equal(camera.isOpen(), false);
    assert.equal(camera.open(), true);
    assert.equal(camera.open(), true);
    assert.equal(camera.isOpen(), true);
    assert.equal(camera.close(), true);
    assert.equal(camera.close(), true);
    assert.equal(camera.isOpen(), false);
  });

  it('startGrabbing is idempotent, stopGrabbing is safe when idle', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    camera.open();
    try {
      camera.stopGrabbing();
      assert.equal(camera.isGrabbing(), false);
      camera.startGrabbing();
      assert.equal(camera.isGrabbing(), true);
      camera.startGrabbing();
      assert.equal(camera.isGrabbing(), true);
      camera.stopGrabbing();
      assert.equal(camera.isGrabbing(), false);
      camera.stopGrabbing();
      assert.equal(camera.isGrabbing(), false);
    } finally {
      camera.close();
    }
  });

  it('grab works without an active grab loop', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    camera.open();
    try {
      assert.equal(camera.isGrabbing(), false);
      const g = camera.grab();
      assert.ok(g.width > 0 && g.height > 0);
      assert.equal(g.buffer.length, g.payloadSize);
    } finally {
      camera.close();
    }
  });

  it('grab without open throws', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    assert.throws(() => camera.grab(), /not open/i);
  });

  it('retrieveResult returns null on timeout or a valid result', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    camera.open();
    try {
      camera.startGrabbing();
      try {
        // Zero timeout: either no frame is ready yet (null) or a frame is
        // already waiting (valid result). Both are correct Return-semantics.
        const res = camera.retrieveResult(0);
        if (res !== null) {
          assert.equal(res.succeeded, true);
          assert.equal(res.buffer.length, res.payloadSize);
        } else {
          assert.equal(res, null);
        }
      } finally {
        camera.stopGrabbing();
      }
    } finally {
      camera.close();
    }
  });

  it('close during grabbing stops the loop', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    camera.open();
    camera.startGrabbing();
    assert.equal(camera.isGrabbing(), true);
    camera.close();
    assert.equal(camera.isOpen(), false);
    assert.equal(camera.isGrabbing(), false);
  });
});
