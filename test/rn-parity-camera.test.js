'use strict';
// RN parity: camera device handling + full-suite extras, mirrored in logic
// from test/pylon/emulated/instantcamera-full.test.js (portable subset:
// enums, open modes, attach/detach, context, grab variants, software
// trigger, configuration helpers, InstantCameraArray).
//
// Not ported (documented in RN_COVERAGE_GAPS): event/configuration
// handler registration (needs the event-emitter bridge), wait objects,
// buffer factory, frame-trigger-ready polling internals.
const { describe, it, before } = require('node:test');
const assert = require('node:assert/strict');
const { loadFacade } = require('./rn-stub-core');

let f = null;
let SKIP = null;

before(async () => {
  try {
    ({ facade: f } = await loadFacade());
  } catch (e) {
    if (e && e.code === 'RN_NO_STRIP') SKIP = e.message;
    else throw e;
  }
});

describe('rn-parity camera enums', () => {
  it('exposes GrabStrategy values', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    assert.equal(f.GrabStrategy.OneByOne, 0);
    assert.equal(f.GrabStrategy.LatestImageOnly, 1);
    assert.equal(f.GrabStrategy.LatestImages, 2);
    assert.equal(f.GrabStrategy.UpcomingImage, 3);
  });

  it('exposes TimeoutHandling values', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    assert.equal(f.TimeoutHandling.Return, 0);
    assert.equal(f.TimeoutHandling.ThrowException, 1);
  });

  it('exposes GrabLoop/RegistrationMode/Cleanup/AccessMode values', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    assert.equal(f.GrabLoop.ProvidedByInstantCamera, 0);
    assert.equal(f.GrabLoop.ProvidedByUser, 1);
    assert.equal(f.RegistrationMode.Append, 0);
    assert.equal(f.RegistrationMode.ReplaceAll, 1);
    assert.equal(f.Cleanup.None, 0);
    assert.equal(f.Cleanup.Delete, 1);
    assert.equal(f.AccessMode.Control, 1);
    assert.equal(f.AccessMode.Stream, 3);
    assert.equal(f.AccessMode.Event, 4);
    assert.equal(f.AccessMode.Exclusive, 5);
  });
});

describe('rn-parity camera open modes', () => {
  it('opens with every valid access mode', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    for (const mode of [1, 3, 4, 5]) {
      const cam = new f.InstantCamera(0, mode);
      try {
        assert.equal(cam.isOpen(), true);
      } finally {
        cam.close();
      }
    }
  });

  it('rejects unknown access modes', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    assert.throws(() => new f.InstantCamera(0, 99), RangeError);
  });
});

describe('rn-parity camera device handling', () => {
  it('attaches, detaches and destroys the device', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      assert.equal(cam.isPylonDeviceAttached(), true);
      assert.equal(cam.hasOwnership(), true);
      cam.detachDevice();
      assert.equal(cam.isPylonDeviceAttached(), false);
      cam.attachDevice(0);
      assert.equal(cam.isPylonDeviceAttached(), true);
      cam.destroyDevice();
      assert.equal(cam.isPylonDeviceAttached(), false);
    } finally {
      cam.close();
    }
  });

  it('indexes cameras per device', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const a = new f.InstantCamera(0);
    const b = new f.InstantCamera(1);
    try {
      assert.notEqual(
        a.getDeviceInfo().serialNumber,
        b.getDeviceInfo().serialNumber,
      );
      assert.throws(() => new f.InstantCamera(99), /device index out of range/);
    } finally {
      a.close();
      b.close();
    }
  });

  it('stores the camera context', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      cam.setCameraContext(12345);
      assert.equal(cam.getCameraContext(), 12345);
      assert.throws(() => cam.setCameraContext('x'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('reports removal and queued buffers', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      assert.equal(cam.isCameraDeviceRemoved(), false);
      assert.equal(typeof cam.getQueuedBufferCount(), 'number');
      assert.ok(cam.getQueuedBufferCount() >= 0);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity software trigger', () => {
  it('applies the trigger configuration and fires', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      f.SoftwareTriggerConfiguration.applyConfiguration(cam);
      assert.equal(cam.getEnum('AcquisitionMode'), 'Continuous');
      assert.equal(cam.getEnum('TriggerSelector'), 'FrameStart');
      assert.equal(cam.getEnum('TriggerMode'), 'On');
      assert.equal(cam.getEnum('TriggerSource'), 'Software');
      cam.executeSoftwareTrigger();
      const res = cam.grabOne(5000);
      assert.equal(res.succeeded, true);
    } finally {
      cam.close();
    }
  });

  it('runs the helper probes without throwing', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      f.ConfigurationHelper.disableAllTriggers(cam);
      f.ConfigurationHelper.disableCompression(cam);
      f.ConfigurationHelper.disableGenDC(cam);
      f.ConfigurationHelper.selectRangeComponent(cam);
      f.ConfigurationHelper.probePacketSize(cam);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity InstantCameraArray', () => {
  it('initializes one camera per index', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(2);
    try {
      assert.equal(arr.getSize(), 2);
      assert.ok(arr.getItem(0) instanceof f.InstantCamera);
      assert.ok(arr.getItem(1) instanceof f.InstantCamera);
      assert.throws(() => arr.getItem(2), RangeError);
      assert.throws(() => arr.getItem(-1), RangeError);
    } finally {
      arr.destroyDevice();
    }
  });

  it('opens and closes all cameras', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(2);
    try {
      assert.equal(arr.isOpen(), true);
      arr.close();
      assert.equal(arr.isOpen(), false);
      arr.open();
      assert.equal(arr.isOpen(), true);
    } finally {
      arr.destroyDevice();
    }
  });

  it('grabs round-robin with camera index', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(2);
    try {
      arr.startGrabbing(f.GrabStrategy.OneByOne);
      assert.equal(arr.isGrabbing(), true);
      const res = arr.retrieveResult(5000);
      assert.ok(res !== null);
      assert.equal(res.succeeded, true);
      assert.ok(res.cameraIndex === 0 || res.cameraIndex === 1);
      arr.stopGrabbing();
      assert.equal(arr.isGrabbing(), false);
    } finally {
      arr.destroyDevice();
    }
  });

  it('destroys all devices', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(2);
    arr.destroyDevice();
    assert.equal(arr.getSize(), 0);
    assert.equal(arr.isOpen(), false);
  });
});

describe('rn-parity camera open modes (strings)', () => {
  it('opens with symbolic access mode names', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    for (const mode of ['Control', 'Stream', 'Event', 'Exclusive']) {
      const cam = new f.InstantCamera(0, mode);
      try {
        assert.equal(cam.isOpen(), true);
      } finally {
        cam.close();
      }
    }
  });

  it('rejects unknown symbolic modes', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    assert.throws(() => new f.InstantCamera(0, 'Bogus'), RangeError);
    const cam = new f.InstantCamera(0);
    cam.close();
    try {
      assert.throws(() => cam.open('Bogus'), RangeError);
      assert.equal(cam.isOpen(), false);
    } finally {
      cam.close();
    }
  });

  it('reopens after close', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    cam.close();
    assert.equal(cam.isOpen(), false);
    cam.open();
    assert.equal(cam.isOpen(), true);
    cam.close();
  });
});

describe('rn-parity camera device errors', () => {
  it('rejects invalid attach and double detach', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(0);
    try {
      assert.throws(() => cam.attachDevice(99), /device index out of range/);
      cam.detachDevice();
      assert.throws(() => cam.detachDevice(), /no device attached/);
      assert.throws(() => cam.getDeviceInfo(), /no device attached/);
    } finally {
      cam.close();
    }
  });

  it('exposes defaults and full name', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = new f.InstantCamera(1);
    try {
      assert.equal(cam.getCameraContext(), 0);
      assert.equal(cam.getQueuedBufferCount(), 0);
      const info = cam.getDeviceInfo();
      assert.ok(info.fullName.includes(info.serialNumber));
      assert.equal(info.serialNumber, 'EMU000002');
    } finally {
      cam.close();
    }
  });

  it('keeps two handles independent', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const a = new f.InstantCamera(0);
    const b = new f.InstantCamera(0);
    try {
      a.setInteger('Width', 64);
      assert.equal(a.getInteger('Width'), 64);
      assert.equal(b.getInteger('Width'), 640);
    } finally {
      a.close();
      b.close();
    }
  });
});

describe('rn-parity InstantCameraArray edges', () => {
  it('handles an empty array safely', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray();
    assert.equal(arr.getSize(), 0);
    assert.equal(arr.isOpen(), false);
    assert.equal(arr.isGrabbing(), false);
    assert.equal(arr.retrieveResult(10, f.TimeoutHandling.Return), null);
    arr.open();
    arr.close();
    arr.stopGrabbing();
  });

  it('grabs from a single-camera array', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(1);
    try {
      arr.startGrabbing(f.GrabStrategy.OneByOne);
      const res = arr.retrieveResult(5000);
      assert.ok(res !== null);
      assert.equal(res.cameraIndex, 0);
      assert.equal(res.succeeded, true);
    } finally {
      arr.destroyDevice();
    }
  });

  it('returns stable item instances', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const arr = new f.InstantCameraArray(2);
    try {
      assert.equal(arr.getItem(0), arr.getItem(0));
    } finally {
      arr.destroyDevice();
    }
  });
});
