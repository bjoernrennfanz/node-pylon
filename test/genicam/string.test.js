'use strict';
// genicam: String nodes (analogous to pypylon's stringparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { requireDevice, hasNode } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam String', () => {
  it('reads read-only strings', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const model = cam.getString('DeviceModelName');
      assert.equal(typeof model, 'string');
      assert.ok(model.length > 0);
      assert.throws(() => cam.setString('DeviceModelName', 'x'), /not writable/);
    } finally {
      cam.close();
    }
  });

  it('reads and writes user strings', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      if (!hasNode(cam, 'DeviceUserID')) {
        t.skip('DeviceUserID not offered by this device');
        return;
      }
      const orig = cam.getString('DeviceUserID');
      cam.setString('DeviceUserID', 'node-pylon-test');
      assert.equal(cam.getString('DeviceUserID'), 'node-pylon-test');
      cam.setString('DeviceUserID', orig);
      assert.equal(cam.getString('DeviceUserID'), orig);
      assert.throws(() => cam.setString('DeviceUserID', 42), TypeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getString('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
