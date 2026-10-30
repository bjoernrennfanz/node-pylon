'use strict';
// pylon: StaticDefectPixelCorrection (analogous to pypylon's
// staticdefectpixelcorrection_test.py, 1:1 in logic).
// Camera-dependent get/set/normalize calls skip when the emulator lacks
// user-list support (translated RuntimeException, like pypylon's
// RuntimeException skip). Input validation runs before any SDK call, so
// the typemap tests need no camera support.
// Note: unlike Python, every JS object is truthy, so the empty-vector
// falsiness check is expressed via empty()/size() instead.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

// Runs fn with an open camera; skips the test when the camera has no
// static-defect-pixel-list support (SDK RuntimeException).
function withSupportedCamera(t, fn) {
  if (!requireDevice(t)) return;
  const cam = openCamera();
  try {
    try {
      fn(cam);
    } catch (err) {
      if (err instanceof pylon.RuntimeException) {
        t.skip(`static defect pixel list not supported by this camera: ${err.message}`);
        return;
      }
      throw err;
    }
  } finally {
    cam.close();
  }
}

describe('pylon StaticDefectPixelCorrection', () => {
  it('surface symbols exist', () => {
    assert.ok(pylon.StaticDefectPixelCorrection);
    assert.equal(pylon.StaticDefectPixelCorrection.ListType_Factory, 0);
    assert.equal(pylon.StaticDefectPixelCorrection.ListType_User, 1);
  });

  it('getDefectPixelList returns [success, vector]', (t) => {
    withSupportedCamera(t, (cam) => {
      const result = pylon.StaticDefectPixelCorrection.getDefectPixelList(
        cam, pylon.StaticDefectPixelCorrection.ListType_User);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(typeof result[0], 'boolean');
      assert.ok(result[1] instanceof pylon.StaticDefectPixelVector);
    });
  });

  it('setDefectPixelList accepts [x, y] and [x, y, type] entries', (t) => {
    withSupportedCamera(t, (cam) => {
      const result = pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam,
        [[10, 20], [12, 25, 0]],
        pylon.StaticDefectPixelCorrection.ListType_User);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(typeof result[0], 'boolean');
      assert.ok(result[1] instanceof pylon.StaticDefectPixelVector);
      pylon.StaticDefectPixelCorrection.SetDefectPixelList(cam, [[10, 20]]);
    });
  });

  it('normalizePixelList accepts entries and returns [success, vector]', (t) => {
    withSupportedCamera(t, (cam) => {
      const result = pylon.StaticDefectPixelCorrection.normalizePixelList(
        cam, [[11, 33, 0], [11, 33, 0], [3, 2, 0]]);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(typeof result[0], 'boolean');
      assert.ok(result[1] instanceof pylon.StaticDefectPixelVector);
    });
  });

  it('new vector has size 0 and reports empty', () => {
    const vector = new pylon.StaticDefectPixelVector();
    assert.equal(vector.length, 0);
    assert.equal(vector.size(), 0);
    assert.equal(vector.empty(), true);
  });

  it('reserve(n) ensures capacity() is at least n', () => {
    const vector = new pylon.StaticDefectPixelVector();
    vector.reserve(10);
    assert.ok(vector.capacity() >= 10);
  });

  it('resize(n) makes size() equal to n', () => {
    const vector = new pylon.StaticDefectPixelVector();
    vector.resize(5);
    assert.equal(vector.size(), 5);
    assert.equal(vector.length, 5);
  });

  it('clear() resets size to 0 and empty() to true', () => {
    const vector = new pylon.StaticDefectPixelVector();
    vector.resize(3);
    vector.clear();
    assert.equal(vector.length, 0);
    assert.equal(vector.empty(), true);
  });

  it('swap() exchanges the contents of two vectors', () => {
    const a = new pylon.StaticDefectPixelVector();
    const b = new pylon.StaticDefectPixelVector();
    a.resize(4);
    a.swap(b);
    assert.equal(a.length, 0);
    assert.equal(b.length, 4);
  });

  it('iterating an empty vector yields no items', () => {
    const vector = new pylon.StaticDefectPixelVector();
    assert.deepEqual([...vector], []);
    assert.deepEqual(vector.toArray(), []);
  });

  it('push/at round-trip pixel entries', () => {
    const vector = new pylon.StaticDefectPixelVector();
    vector.push([7, 9]);
    vector.push({ x: 1, y: 2, type: 0 });
    assert.equal(vector.size(), 2);
    const first = vector.at(0);
    assert.equal(first.X, 7);
    assert.equal(first.Y, 9);
    assert.deepEqual(vector.toArray(), [{ x: 7, y: 9, type: 0 }, { x: 1, y: 2, type: 0 }]);
    assert.throws(() => vector.at(2), RangeError);
  });

  it('default-constructed StaticDefectPixel is zeroed', () => {
    const pixel = new pylon.StaticDefectPixel();
    assert.equal(pixel.X, 0);
    assert.equal(pixel.Y, 0);
    assert.equal(pixel.Type, 0);
  });

  it('StaticDefectPixel fields are writable and readable', () => {
    const pixel = new pylon.StaticDefectPixel();
    pixel.X = 1023;
    pixel.Y = 767;
    pixel.Type = pylon.StaticDefectPixelType_Reserved;
    assert.equal(pixel.X, 1023);
    assert.equal(pixel.x, 1023);
    assert.equal(pixel.Y, 767);
    assert.equal(pixel.y, 767);
    assert.equal(pixel.Type, pylon.StaticDefectPixelType_Reserved);
    const placed = new pylon.StaticDefectPixel(4, 5, 0);
    assert.equal(placed.X, 4);
    assert.equal(placed.Y, 5);
  });

  it('StaticDefectPixelType_Reserved is 0', () => {
    assert.equal(pylon.StaticDefectPixelType_Reserved, 0);
  });

  it('setDefectPixelList rejects a non-list argument', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, 'not_a_list', pylon.StaticDefectPixelCorrection.ListType_User), TypeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects a non-sequence element', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, ['not_a_tuple'], pylon.StaticDefectPixelCorrection.ListType_User), TypeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects an entry with one value', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[10]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects an entry with four values', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[10, 20, 0, 99]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects x above uint16 max', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[65536, 0]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects y above uint16 max', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[0, 65536]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects negative coordinates', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[-1, 0]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[0, -1]], pylon.StaticDefectPixelCorrection.ListType_User), RangeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList rejects non-integer coordinates', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [['a', 20]], pylon.StaticDefectPixelCorrection.ListType_User), TypeError);
    } finally {
      cam.close();
    }
  });

  it('setDefectPixelList accepts a list [x, y] entry', (t) => {
    withSupportedCamera(t, (cam) => {
      const result = pylon.StaticDefectPixelCorrection.setDefectPixelList(
        cam, [[10, 20]], pylon.StaticDefectPixelCorrection.ListType_User);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
    });
  });

  it('unknown list type is rejected', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.StaticDefectPixelCorrection.getDefectPixelList(cam, 99), RangeError);
    } finally {
      cam.close();
    }
  });
});
