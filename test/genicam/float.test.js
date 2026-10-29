'use strict';
// genicam: Float nodes (analogous to pypylon's floatparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam Float', () => {
  it('reads values, limits and writes roundtrip', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getFloat('ExposureTime');
    try {
      const v = cam.getFloat('ExposureTime');
      assert.equal(typeof v, 'number');
      assert.ok(v > 0);
      const mn = cam.getFloatMin('ExposureTime');
      const mx = cam.getFloatMax('ExposureTime');
      assert.ok(mn <= v && v <= mx);
      const target = Math.min(mx, Math.max(mn, orig === mn ? mn + (mx - mn) / 2 : mn));
      cam.setFloat('ExposureTime', target);
      assert.equal(cam.getFloat('ExposureTime'), target);
      cam.setFloat('ExposureTime', orig);
      assert.equal(cam.getFloat('ExposureTime'), orig);
    } finally {
      try { cam.setFloat('ExposureTime', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('rejects out-of-range writes', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const mx = cam.getFloatMax('ExposureTime');
      assert.throws(() => cam.setFloat('ExposureTime', mx * 2 + 1), RangeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getFloat('Width'), TypeError);
      assert.throws(() => cam.getFloat('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });
});
