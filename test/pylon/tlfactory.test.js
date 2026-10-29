'use strict';
// pylon: TlFactory device enumeration (analogous to pypylon's
// tlfactory_test.py). Runs against the emulator when no hardware exists.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice, deviceCount } = require('../helpers');

describe('pylon TlFactory', () => {
  it('enumerates devices', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
      assert.ok(Array.isArray(devices));
      assert.ok(devices.length > 0);
      assert.deepEqual(pylon.enumerateDevices(), devices);
      // Lowercase aliases behave the same.
      assert.deepEqual(pylon.TlFactory.getInstance().enumerateDevices(), devices);
      for (const d of devices) {
        assert.equal(typeof d.serialNumber, 'string');
        assert.ok(d.serialNumber.length > 0);
        assert.equal(typeof d.modelName, 'string');
      }
    } finally {
      pylon.terminate();
    }
  });

  it('throws without any device', (t) => {
    if (deviceCount() > 0) {
      t.skip('devices present; nothing to fail on');
      return;
    }
    assert.throws(() => pylon.createFirstCamera());
  });
});
