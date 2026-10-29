'use strict';
// genicam: Enumeration nodes (analogous to pypylon's enumparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam Enumeration', () => {
  it('reads value and entries, writes roundtrip', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getEnum('PixelFormat');
    try {
      const entries = cam.getEnumEntries('PixelFormat');
      assert.ok(Array.isArray(entries));
      assert.ok(entries.length > 0);
      assert.ok(entries.includes(orig));
      for (const entry of entries.slice(0, 2)) {
        cam.setEnum('PixelFormat', entry);
        assert.equal(cam.getEnum('PixelFormat'), entry);
      }
      cam.setEnum('PixelFormat', orig);
      assert.equal(cam.getEnum('PixelFormat'), orig);
    } finally {
      try { cam.setEnum('PixelFormat', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('rejects invalid entries', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setEnum('PixelFormat', 'NotARealEntry12345'), RangeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getEnum('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
