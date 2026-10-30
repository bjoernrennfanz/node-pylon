'use strict';
// pylon: device info of the opened camera (analogous to pypylon's
// deviceinfo_test.py).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice } = require('../../helpers');

describe('pylon device info', () => {
  it('exposes non-empty device fields', (t) => {
    if (!requireDevice(t)) return;
    const cam = pylon.createFirstCamera();
    cam.open();
    try {
      const info = cam.getDeviceInfo();
      for (const key of ['friendlyName', 'modelName', 'serialNumber', 'vendorName', 'deviceClass']) {
        assert.equal(typeof info[key], 'string', key);
        assert.ok(info[key].length > 0, key);
      }
    } finally {
      cam.close();
    }
  });
});
