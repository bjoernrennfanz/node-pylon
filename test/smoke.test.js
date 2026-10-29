'use strict';
// Smoke-Tests: laufen gegen den Real-SDK-Build (Default, mit
// Kamera-Emulator via PYLON_CAMEMU) oder den Mock-Opt-in
// (NODE_PYLON_USE_MOCK=1 + rebuild:mock). Lauf mit: npm test
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { isMockMode, requireDevice } = require('./helpers');

describe('node-pylon core', () => {
  it('exposes version strings', () => {
    assert.equal(typeof pylon.version(), 'string');
    assert.equal(typeof pylon.pylonVersion(), 'string');
    if (isMockMode()) {
      assert.match(pylon.version(), /mock/);
    } else {
      assert.doesNotMatch(pylon.version(), /mock/);
    }
  });

  it('mock flag matches build mode', () => {
    assert.equal(typeof pylon.isMock(), 'boolean');
    assert.equal(pylon.isMock(), isMockMode());
  });

  it('initialize/terminate do not throw (refcounted)', () => {
    pylon.initialize();
    pylon.initialize();
    pylon.terminate();
    pylon.terminate();
  });

  it('TlFactory enumerates devices', (t) => {
    pylon.initialize();
    try {
      const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
      assert.ok(Array.isArray(devices));
      assert.deepEqual(pylon.enumerateDevices(), devices);
      // Kleinschreib-Aliase funktionieren ebenfalls.
      assert.deepEqual(pylon.TlFactory.getInstance().enumerateDevices(), devices);
      if (isMockMode()) {
        assert.equal(devices.length, 1);
        assert.equal(devices[0].serialNumber, 'MOCK123456');
      } else if (devices.length > 0) {
        for (const d of devices) {
          assert.equal(typeof d.serialNumber, 'string');
          assert.ok(d.serialNumber.length > 0);
        }
      } else {
        t.skip('no camera/emulator available (set PYLON_CAMEMU=1 for the SDK emulator)');
      }
    } finally {
      pylon.terminate();
    }
  });

  it('InstantCamera open/grab-loop/close flow', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
      assert.equal(camera.isOpen(), false);
      assert.equal(camera.isGrabbing(), false);
      const info = camera.getDeviceInfo();
      assert.equal(typeof info.serialNumber, 'string');
      if (isMockMode()) assert.equal(info.serialNumber, 'MOCK123456');
      camera.open();
      assert.equal(camera.isOpen(), true);
      camera.startGrabbing();
      assert.equal(camera.isGrabbing(), true);
      const res = camera.retrieveResult(10000);
      assert.ok(res !== null);
      assert.equal(res.succeeded, true);
      assert.ok(res.width > 0 && res.height > 0);
      assert.equal(res.buffer.length, res.payloadSize);
      assert.equal(typeof res.pixelFormat, 'string');
      if (isMockMode()) {
        assert.equal(res.width, 640);
        assert.equal(res.height, 480);
        assert.equal(res.pixelFormat, 'Mono8');
        assert.equal(res.buffer.length, 640 * 480);
        assert.equal(res.buffer[0], 0); // (0 + frame0) & 0xFF
        const res2 = camera.retrieveResult(1000);
        assert.ok(res2 !== null && res2.buffer.length === 640 * 480);
        assert.equal(res2.buffer[0], 1); // frame1
      }
      camera.stopGrabbing();
      assert.equal(camera.isGrabbing(), false);
      camera.close();
      assert.equal(camera.isOpen(), false);
    } finally {
      pylon.terminate();
    }
  });

  it('startGrabbing without open throws', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.createFirstCamera();
      assert.throws(() => camera.startGrabbing(), /not open/i);
    } finally {
      pylon.terminate();
    }
  });

  it('retrieveResult without grabbing throws', (t) => {
    pylon.initialize();
    try {
      if (!requireDevice(t)) return;
      const camera = pylon.createFirstCamera();
      camera.open();
      assert.throws(() => camera.retrieveResult(100), /Not grabbing/);
      camera.close();
    } finally {
      pylon.terminate();
    }
  });

  it('grabMock stays backward compatible', () => {
    const buf = pylon.grabMock(8, 4);
    assert.equal(buf.length, 32);
    assert.equal(buf[0], 0);
    assert.equal(buf[1], 1);
    assert.equal(buf[8], 1); // x=0,y=1 -> 1
  });
});

describe('node-pylon nodemap defaults', () => {
  it('reads defaults and writes within limits', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    camera.open();
    try {
      assert.equal(camera.getInteger('Width'), 640);
      assert.equal(camera.getInteger('Height'), 480);
      assert.equal(camera.getFloat('ExposureTime'), 5000.0);
      assert.equal(camera.getEnum('PixelFormat'), 'Mono8');
      camera.setInteger('Width', 320);
      assert.equal(camera.getInteger('Width'), 320);
      camera.setEnum('PixelFormat', 'RGB8');
      assert.equal(camera.getEnum('PixelFormat'), 'RGB8');
      const g = camera.grab();
      assert.equal(g.width, 320);
      assert.equal(g.buffer.length, 320 * 480 * 3);
    } finally {
      camera.close();
    }
  });

  it('rejects unknown nodes and out-of-range writes', (t) => {
    if (!requireDevice(t)) return;
    const camera = pylon.createFirstCamera();
    assert.throws(() => camera.getInteger('Nope'), /Unknown node/);
    assert.throws(() => camera.setInteger('Width', 99999), RangeError);
    assert.throws(() => camera.setEnum('PixelFormat', 'YUV'), RangeError);
  });
});
