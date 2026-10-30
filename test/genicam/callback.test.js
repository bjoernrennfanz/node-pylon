'use strict';
// genicam: node-change callbacks (RegisterCallback polling).
// Ports callback_test.py logic: registering fires on value change. The C++
// callback only bumps a counter (no JS crossing threads); JS polls the
// count, which keeps the observable semantics identical.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam callback', () => {
  it('fires on value change and stops after deregister', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getFloat('ExposureTime');
    let handle = null;
    try {
      handle = cam.registerCallback('ExposureTime');
      assert.equal(typeof handle, 'number');
      assert.equal(cam.getCallbackCount(handle), 0);
      const mn = cam.getFloatMin('ExposureTime');
      const mx = cam.getFloatMax('ExposureTime');
      const target = orig === mn ? Math.min(mx, mn + (mx - mn) / 2 || mn + 1) : mn;
      cam.setFloat('ExposureTime', target);
      assert.ok(cam.getCallbackCount(handle) >= 1);
      cam.deregisterCallback(handle);
      handle = null;
      assert.throws(() => cam.getCallbackCount(0x7fffffff), /Unknown node/);
      assert.throws(() => cam.registerCallback('NoSuchNode12345'), /Unknown node/);
    } finally {
      try {
        if (handle !== null) cam.deregisterCallback(handle);
      } catch { /* best effort */ }
      try { cam.setFloat('ExposureTime', orig); } catch { /* best effort */ }
      cam.close();
    }
  });
});
