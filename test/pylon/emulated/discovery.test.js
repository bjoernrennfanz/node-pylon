'use strict';
// pylon: device/transport-layer discovery surface.
// Covers the emulated pypylon discovery tests that map onto the MVP wrapper:
// tlfactory_test.py enumeration sections, deviceinfo_test.py field checks,
// devicefactory/deviceclass/info/tlinfo/interface/interfaceinfo/transportlayer
// tests (behavioral: deviceClass string on enumerated entries), and
// container_test.py (device lists are plain arrays).
// TlInfo/InterfaceInfo objects, TransportLayer handles, DeviceFactory and
// InstantCameraArray have no MVP wrapper counterpart and are documented as
// deviations in docs/testing.md.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice, deviceCount } = require('../../helpers');

const DEVICE_FIELDS = [
  'friendlyName',
  'modelName',
  'serialNumber',
  'vendorName',
  'deviceClass',
  'fullName',
];

describe('pylon discovery', () => {
  it('enumerateDevices entries carry all device info fields', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const devices = pylon.enumerateDevices();
      assert.ok(Array.isArray(devices));
      assert.ok(devices.length > 0);
      for (const d of devices) {
        for (const key of DEVICE_FIELDS) {
          assert.equal(typeof d[key], 'string', key);
          assert.ok(d[key].length > 0, key);
        }
      }
    } finally {
      pylon.terminate();
    }
  });

  it('emulator devices expose a device class', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const devices = pylon.enumerateDevices();
      const classes = devices.map((d) => d.deviceClass);
      assert.ok(classes.length > 0);
      for (const c of classes) {
        assert.equal(typeof c, 'string');
      }
    } finally {
      pylon.terminate();
    }
  });

  it('TlFactory instance and aliases agree', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const a = pylon.TlFactory.GetInstance();
      const b = pylon.TlFactory.getInstance();
      assert.equal(a, b);
      const serials = (list) => list.map((d) => d.serialNumber).sort();
      assert.deepEqual(serials(a.EnumerateDevices()), serials(b.enumerateDevices()));
      assert.deepEqual(serials(pylon.enumerateDevices()), serials(a.EnumerateDevices()));
    } finally {
      pylon.terminate();
    }
  });

  it('CreateFirstDevice opens a device with matching device info', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const device = pylon.TlFactory.GetInstance().CreateFirstDevice();
      // CreateFirstDevice returns a PylonDevice handle (pypylon parity), not
      // the InstantCamera facade: the contract is behavioral.
      for (const method of ['open', 'close', 'isOpen', 'getDeviceInfo', 'destroy']) {
        assert.equal(typeof device[method], 'function', method);
      }
      assert.equal(device.isOpen(), false);
      device.open();
      try {
        const info = device.getDeviceInfo();
        const devices = pylon.enumerateDevices();
        const serials = devices.map((d) => d.serialNumber);
        assert.ok(serials.includes(info.serialNumber));
      } finally {
        device.close();
      }
      pylon.TlFactory.GetInstance().DestroyDevice(device);
    } finally {
      pylon.terminate();
    }
  });

  it('device count is stable across calls', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const first = pylon.enumerateDevices().length;
      const second = pylon.enumerateDevices().length;
      assert.equal(first, second);
      assert.equal(first, deviceCount());
    } finally {
      pylon.terminate();
    }
  });
});
