'use strict';
// genicam: Integer nodes (analogous to pypylon's integerparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { requireDevice, pickInt } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam Integer', () => {
  it('reads values, limits and writes roundtrip', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getInteger('Width');
    try {
      const v = cam.getInteger('Width');
      assert.equal(typeof v, 'number');
      const mn = cam.getIntegerMin('Width');
      const mx = cam.getIntegerMax('Width');
      assert.ok(mn <= v && v <= mx);
      const target = pickInt(mn, mx, v === mn ? Math.min(mx, mn + 64) : mn);
      cam.setInteger('Width', target);
      assert.equal(cam.getInteger('Width'), target);
      cam.setInteger('Width', orig);
      assert.equal(cam.getInteger('Width'), orig);
    } finally {
      try { cam.setInteger('Width', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('rejects out-of-range writes', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const mx = cam.getIntegerMax('Width');
      assert.throws(() => cam.setInteger('Width', mx + 1), RangeError);
      assert.throws(() => cam.setInteger('PayloadSize', 10), /not writable/);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getInteger('ExposureTime'), TypeError);
      assert.throws(() => cam.getInteger('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('tryMax sets the maximum', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getInteger('Height');
    try {
      const max = cam.tryMax('Height');
      assert.equal(max, cam.getIntegerMax('Height'));
      assert.equal(cam.getInteger('Height'), max);
      assert.throws(() => cam.tryMax('PixelFormat'), TypeError);
    } finally {
      try { cam.setInteger('Height', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });
});
