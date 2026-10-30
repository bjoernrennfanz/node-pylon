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
