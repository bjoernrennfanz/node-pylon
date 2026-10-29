'use strict';
// genicam: Boolean nodes (analogous to pypylon's booleanparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice, hasNode } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam Boolean', () => {
  it('reads and writes roundtrip', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      if (!hasNode(cam, 'ChunkModeActive')) {
        t.skip('ChunkModeActive not offered by this device');
        return;
      }
      const orig = cam.getBoolean('ChunkModeActive');
      assert.equal(typeof orig, 'boolean');
      cam.setBoolean('ChunkModeActive', !orig);
      assert.equal(cam.getBoolean('ChunkModeActive'), !orig);
      cam.setBoolean('ChunkModeActive', orig);
      assert.equal(cam.getBoolean('ChunkModeActive'), orig);
      assert.throws(() => cam.setBoolean('ChunkModeActive', 'yes'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getBoolean('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
