'use strict';
// RN parity: grab + configuration suites, mirrored in logic from
// test/pylon/emulated/grab.test.js, grabresult.test.js (field-level parts),
// configuration.test.js and instantcamera.test.js.
//
// grabOne() is the RN one-shot grab (like Node grab()); retrieveResult()
// mirrors the loop-based path. Zero-copy: the stub hands the same
// Uint8Array shape the JSI ArrayBuffer path produces.
const { describe, it, before } = require('node:test');
const assert = require('node:assert/strict');
const { loadFacade } = require('./rn-stub-core');

let f = null;

before(async () => {
  ({ facade: f } = await loadFacade());
});

function freshCamera() {
  const cam = new f.InstantCamera();
  cam.open();
  return cam;
}

describe('rn-parity pylon grab results', () => {
  it('grab reflects nodemap geometry and pixel format', () => {
    const cam = freshCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = Math.min(cam.getIntegerMax('Width'), Math.max(cam.getIntegerMin('Width'), 64));
      const h = Math.min(cam.getIntegerMax('Height'), Math.max(cam.getIntegerMin('Height'), 48));
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      const g = cam.grabOne(5000);
      assert.equal(g.width, w);
      assert.equal(g.height, h);
      assert.equal(typeof g.pixelType, 'string');
      assert.equal(g.payloadSize, g.buffer.length);
      assert.ok(g.buffer instanceof Uint8Array);
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore */ }
      try { cam.setInteger('Height', origH); } catch { /* restore */ }
      cam.close();
    }
  });

  it('payload scales with bytes per pixel', () => {
    const cam = freshCamera();
    const entries = cam.getEnumEntries('PixelFormat');
    const origFormat = cam.getEnum('PixelFormat');
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = Math.min(cam.getIntegerMax('Width'), Math.max(cam.getIntegerMin('Width'), 128));
      const h = Math.min(cam.getIntegerMax('Height'), Math.max(cam.getIntegerMin('Height'), 64));
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      if (entries.includes('RGB8')) {
        cam.setEnum('PixelFormat', 'RGB8');
        const g = cam.grabOne(5000);
        assert.equal(g.payloadSize, w * h * 3);
        assert.equal(g.buffer.length, g.payloadSize);
        assert.equal(g.pixelType, 'RGB8');
      }
    } finally {
      try { cam.setEnum('PixelFormat', origFormat); } catch { /* restore */ }
      try { cam.setInteger('Width', origW); } catch { /* restore */ }
      try { cam.setInteger('Height', origH); } catch { /* restore */ }
      cam.close();
    }
  });

  it('delivers chunk data and deterministic frames', () => {
    const cam = freshCamera();
    try {
      cam.setBoolean('ChunkModeActive', true);
      const a = cam.grabOne(5000);
      const b = cam.grabOne(5000);
      assert.equal(b.frameNumber, a.frameNumber + 1);
      assert.equal(a.chunk.modeActive, true);
      assert.equal(a.chunk.frameID, a.frameNumber);
      assert.ok(a.timestamp > 0);
      assert.notDeepEqual(a.buffer, b.buffer);
      cam.setBoolean('ChunkModeActive', false);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity pylon grab result fields', () => {
  it('exposes geometry, payload and buffer consistently', () => {
    const cam = freshCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      cam.setInteger('Width', 64);
      cam.setInteger('Height', 48);
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      const res = cam.retrieveResult(5000);
      assert.ok(res !== null);
      assert.equal(res.succeeded, true);
      assert.equal(res.width, 64);
      assert.equal(res.height, 48);
      assert.equal(res.pixelType, cam.getEnum('PixelFormat'));
      assert.equal(res.payloadSize, res.buffer.length);
      assert.ok(res.buffer instanceof Uint8Array);
      cam.stopGrabbing();
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore */ }
      try { cam.setInteger('Height', origH); } catch { /* restore */ }
      cam.close();
    }
  });

  it('numbers timestamps, blocks and frames monotonically', () => {
    const cam = freshCamera();
    try {
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      const a = cam.retrieveResult(5000);
      const b = cam.retrieveResult(5000);
      for (const r of [a, b]) {
        assert.equal(typeof r.timestamp, 'number');
        assert.ok(r.timestamp >= 0);
        assert.equal(typeof r.blockId, 'number');
        assert.ok(r.blockId >= 0);
        assert.equal(typeof r.frameNumber, 'number');
        assert.ok(r.frameNumber >= 0);
      }
      assert.ok(b.timestamp >= a.timestamp);
      assert.ok(b.blockId >= a.blockId);
      assert.ok(b.frameNumber > a.frameNumber);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('yields deterministic per-frame buffers', () => {
    const cam = freshCamera();
    try {
      const a = cam.grabOne(5000);
      const b = cam.grabOne(5000);
      assert.equal(a.buffer.length, b.buffer.length);
      assert.notDeepEqual(a.buffer, b.buffer);
      assert.equal(a.buffer[0], 0);
    } finally {
      cam.close();
    }
  });

  it('returns null on Return-timeout', () => {
    const cam = freshCamera();
    try {
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      const res = cam.retrieveResult(0, f.TimeoutHandling.Return);
      assert.equal(res, null);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('throws when retrieving without a grab loop', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.retrieveResult(100), /Not grabbing/);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity pylon configuration', () => {
  it('open and close are idempotent', () => {
    const cam = new f.InstantCamera();
    assert.equal(cam.isOpen(), true);
    cam.open();
    assert.equal(cam.isOpen(), true);
    cam.close();
    assert.equal(cam.isOpen(), false);
    cam.close();
    assert.equal(cam.isOpen(), false);
  });

  it('start is idempotent and stop is safe when idle', () => {
    const cam = freshCamera();
    try {
      cam.stopGrabbing();
      assert.equal(cam.isGrabbing(), false);
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      assert.equal(cam.isGrabbing(), true);
      cam.stopGrabbing();
      assert.equal(cam.isGrabbing(), false);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('grabs without a running loop', () => {
    const cam = freshCamera();
    try {
      const g = cam.grabOne(5000);
      assert.ok(g.width > 0);
      assert.ok(g.height > 0);
      assert.equal(g.buffer.length, g.payloadSize);
    } finally {
      cam.close();
    }
  });

  it('rejects grab and start on a closed camera', () => {
    const cam = freshCamera();
    cam.close();
    assert.throws(() => cam.grabOne(100), /not open/);
    assert.throws(() => cam.startGrabbing(), /not open/);
  });

  it('close-while-grabbing stops the loop', () => {
    const cam = freshCamera();
    cam.startGrabbing(f.GrabStrategy.OneByOne);
    assert.equal(cam.isGrabbing(), true);
    cam.close();
    assert.equal(cam.isGrabbing(), false);
  });

  it('startGrabbingMax bounds the loop', () => {
    const cam = freshCamera();
    try {
      cam.startGrabbingMax(2, f.GrabStrategy.OneByOne);
      const a = cam.retrieveResult(5000);
      const b = cam.retrieveResult(5000);
      assert.equal(a.succeeded, true);
      assert.equal(b.succeeded, true);
      assert.equal(cam.isGrabbing(), false);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity pylon instantcamera basics', () => {
  it('opens the first camera and reports state', () => {
    const cam = f.createFirstCamera();
    try {
      assert.equal(cam.isOpen(), true);
      assert.equal(cam.isGrabbing(), false);
      assert.equal(cam.isPylonDeviceAttached(), true);
    } finally {
      cam.close();
    }
  });

  it('grabs a valid frame', () => {
    const cam = f.createFirstCamera();
    try {
      const res = cam.grabOne(5000);
      assert.equal(res.succeeded, true);
      assert.ok(res.width > 0 && res.height > 0);
      assert.equal(typeof res.pixelType, 'string');
      assert.equal(res.buffer.length, res.payloadSize);
    } finally {
      cam.close();
    }
  });

  it('runs the start/retrieve/stop loop', () => {
    const cam = f.createFirstCamera();
    try {
      assert.throws(() => cam.retrieveResult(50), /Not grabbing/);
      cam.startGrabbing(f.GrabStrategy.OneByOne);
      const res = cam.retrieveResult(5000);
      assert.equal(res.succeeded, true);
      cam.stopGrabbing();
      assert.equal(cam.isGrabbing(), false);
    } finally {
      cam.close();
    }
  });
});
