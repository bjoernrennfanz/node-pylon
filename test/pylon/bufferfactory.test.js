'use strict';
// pylon: BufferFactory allocator hooks (analogous to pypylon's
// bufferfactory_test.py, 1:1 in logic).
// The native factory is malloc-backed with allocation statistics; attaching
// it hands ownership to pylon (Cleanup_Delete), so OnReleased-equivalent
// semantics (replace/destroy/deferred release) come from the SDK itself.
// Buffer contexts are numeric ids assigned by AllocateBuffer (null without
// a custom factory); grab results expose the exact factory object.
// Holdable results (retrieveGrabResult) back the release-ordering tests;
// MaxNumBuffer exactness follows StartGrabbingMax like pypylon.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

// Forces a full GC cycle when --expose-gc is available (used for the
// destroy/deferred-release tests, where pylon defers DestroyBufferFactory
// until the camera wrapper dies / the last buffer is freed).
async function collect() {
  if (typeof global.gc !== 'function') return false;
  for (let i = 0; i < 20; i += 1) {
    global.gc();
    await new Promise((resolve) => setImmediate(resolve));
  }
  return true;
}

describe('pylon BufferFactory', () => {
  it('default construction produces a valid factory with zeroed stats', () => {
    const factory = new pylon.BufferFactory();
    assert.ok(factory);
    assert.equal(factory.allocationCount(), 0);
    assert.equal(factory.freeCount(), 0);
    assert.equal(factory.lastAllocationSize(), 0);
    assert.equal(factory.isReleased(), false);
  });

  it('AllocateBuffer is called with the payload size while grabbing', (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      assert.equal(cam.getBufferFactory(), factory);
      cam.startGrabbingMax(3);
      let grabbed = 0;
      while (cam.isGrabbing()) {
        const result = cam.retrieveGrabResult(5000);
        if (result === null) continue;
        assert.equal(result.succeeded(), true);
        result.release();
        grabbed += 1;
      }
      assert.equal(grabbed, 3);
      // StartGrabbingMax sets MaxNumBuffer to the image count, so the
      // factory sees exactly that many allocations.
      assert.equal(factory.allocationCount(), 3);
      assert.ok(factory.lastAllocationSize() > 0);
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.setBufferFactory(null);
      cam.close();
    }
  });

  it('last allocation size matches the result payload size', (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      cam.startGrabbing();
      const result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      assert.equal(result.succeeded(), true);
      assert.equal(factory.lastAllocationSize(), result.getPayloadSize());
      result.release();
      cam.stopGrabbing();
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.setBufferFactory(null);
      cam.close();
    }
  });

  it('FreeBuffer is called for every allocated buffer', async (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      cam.startGrabbingMax(3);
      const results = [];
      while (cam.isGrabbing()) {
        const result = cam.retrieveGrabResult(5000);
        if (result === null) continue;
        results.push(result);
      }
      assert.equal(results.length, 3);
      cam.stopGrabbing();
      for (const result of results) result.release();
      results.length = 0;
      // The holdable results carry zero-copy buffer views that share
      // ownership of the SDK buffers, so the frees land on GC.
      if (typeof global.gc !== 'function') {
        t.skip('needs --expose-gc to observe buffer frees');
        return;
      }
      await collect();
      assert.ok(factory.freeCount() > 0);
      assert.equal(factory.freeCount(), factory.allocationCount());
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.setBufferFactory(null);
      cam.close();
    }
  });

  it('replacing the factory releases the old one', (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      assert.equal(factory.isReleased(), false);
      cam.setBufferFactory(null);
      assert.equal(factory.isReleased(), true);
      assert.equal(cam.getBufferFactory(), null);
    } finally {
      cam.close();
    }
  });

  it('destroying the camera releases the attached factory', async (t) => {
    if (!requireDevice(t)) return;
    if (typeof global.gc !== 'function') {
      t.skip('needs --expose-gc for deterministic wrapper destruction');
      return;
    }
    const factory = new pylon.BufferFactory();
    let cam = openCamera();
    cam.setBufferFactory(factory);
    cam.close();
    cam = null;
    assert.ok(await collect());
    assert.equal(factory.isReleased(), true);
  });

  it('release is deferred while a grab result is outstanding', async (t) => {
    if (!requireDevice(t)) return;
    if (typeof global.gc !== 'function') {
      t.skip('needs --expose-gc for deterministic wrapper destruction');
      return;
    }
    const factory = new pylon.BufferFactory();
    let cam = openCamera();
    cam.setBufferFactory(factory);
    cam.startGrabbing();
    let result = cam.retrieveGrabResult(5000);
    assert.ok(result !== null);
    cam.stopGrabbing();
    cam.close();
    cam = null;
    await collect();
    // The camera is gone but the result still references the buffer, so
    // the factory must not be released yet.
    assert.equal(factory.isReleased(), false);
    result.release();
    result = null;
    // Dropping the last result frees the final SDK buffer (its zero-copy
    // views only release on GC), which finally releases the factory.
    await collect();
    assert.equal(factory.isReleased(), true);
  });

  it('getBufferContext returns the allocating context id', (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      cam.startGrabbing();
      const result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      const context = result.getBufferContext();
      assert.equal(typeof context, 'number');
      assert.ok(context > 0);
      assert.equal(result.bufferContext, context);
      assert.equal(result.GetBufferContext(), context);
      result.release();
      cam.stopGrabbing();
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.setBufferFactory(null);
      cam.close();
    }
  });

  it('getBufferContext is null without a custom factory', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      assert.equal(result.getBufferContext(), null);
      result.release();
      cam.stopGrabbing();
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.close();
    }
  });

  it('getBufferFactory returns the exact attached factory', (t) => {
    if (!requireDevice(t)) return;
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      cam.startGrabbing();
      const result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      assert.equal(result.getBufferFactory(), factory);
      assert.equal(result.bufferFactory, factory);
      assert.equal(result.GetBufferFactory(), factory);
      result.release();
      cam.stopGrabbing();
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.setBufferFactory(null);
      cam.close();
    }
  });

  it('getBufferFactory is null without a custom factory', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      assert.equal(result.getBufferFactory(), null);
      result.release();
      cam.stopGrabbing();
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      cam.close();
    }
  });

  it('factory identity survives replacement until the result is released', async (t) => {
    if (!requireDevice(t)) return;
    if (typeof global.gc !== 'function') {
      t.skip('needs --expose-gc for deterministic wrapper destruction');
      return;
    }
    const factory = new pylon.BufferFactory();
    const cam = openCamera();
    try {
      cam.setBufferFactory(factory);
      cam.startGrabbing();
      let result = cam.retrieveGrabResult(5000);
      assert.ok(result !== null);
      cam.stopGrabbing();
      cam.setBufferFactory(null);
      // The outstanding result still references the buffer, so the
      // replaced factory must not be released yet.
      assert.equal(factory.isReleased(), false);
      assert.equal(result.getBufferFactory(), factory);
      result.release();
      result = null;
      await collect();
      assert.equal(factory.isReleased(), true);
    } finally {
      try { cam.stopGrabbing(); } catch { /* already stopped */ }
      try { cam.setBufferFactory(null); } catch { /* already detached */ }
      cam.close();
    }
  });

  it('setBufferFactory validates its argument', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => cam.setBufferFactory(42), TypeError);
      const first = new pylon.BufferFactory();
      cam.setBufferFactory(first);
      // A consumed (attached) factory cannot be attached again.
      assert.throws(() => cam.setBufferFactory(first), Error);
      cam.setBufferFactory(null);
    } finally {
      try { cam.setBufferFactory(null); } catch { /* already detached */ }
      cam.close();
    }
  });
});
