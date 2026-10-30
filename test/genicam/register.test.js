'use strict';
// genicam: register and port access (length/address/read).
// Ports register_test.py / port_test.py logic at the emulator level: raw
// IRegister nodes are device-dependent, so the suite asserts error
// translation for value nodes plus a best-effort read of any register the
// emulator exposes (Root features probed for IRegister kinds).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam register', () => {
  it('rejects value nodes as registers', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getRegisterLength('Width'), TypeError);
      assert.throws(() => cam.getRegisterAddress('Width'), TypeError);
      assert.throws(() => cam.readRegister('Width'), TypeError);
      assert.throws(() => cam.getRegisterLength('NoSuchNode12345'), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('reads a device register when offered', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      let features = [];
      try {
        features = cam.getCategoryFeatures('Root');
      } catch {
        t.skip('Root category unavailable on this device');
        return;
      }
      let register = null;
      for (const name of features) {
        try {
          const len = cam.getRegisterLength(name);
          if (Number.isInteger(len) && len > 0) {
            register = name;
            break;
          }
        } catch {
          // Not a register — keep probing.
        }
      }
      if (register === null) {
        t.skip('no IRegister node exposed by this device');
        return;
      }
      const address = cam.getRegisterAddress(register);
      assert.equal(typeof address, 'number');
      const data = cam.readRegister(register);
      assert.ok(Buffer.isBuffer(data));
      assert.ok(data.length > 0);
    } finally {
      cam.close();
    }
  });
});
