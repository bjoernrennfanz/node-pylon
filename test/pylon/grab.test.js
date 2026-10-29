'use strict';
// pylon: frame handling via grab() — geometry, payload consistency,
// chunk data, deterministic pattern (analogous to pypylon's
// grabresult_test.py, adapted to the synchronous grab() helper).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice, pickInt } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('pylon grab results', () => {
  it('grab reflects nodemap geometry and pixel format', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = pickInt(cam.getIntegerMin('Width'), cam.getIntegerMax('Width'), 64);
      const h = pickInt(cam.getIntegerMin('Height'), cam.getIntegerMax('Height'), 48);
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      const g = cam.grab();
      assert.equal(g.width, w);
      assert.equal(g.height, h);
      assert.equal(typeof g.pixelFormat, 'string');
      assert.equal(g.pixelType, g.pixelFormat);
      assert.equal(g.payloadSize, g.buffer.length);
      assert.ok(Buffer.isBuffer(g.buffer));
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('payload scales with bytes per pixel', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const entries = cam.getEnumEntries('PixelFormat');
    const origFormat = cam.getEnum('PixelFormat');
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = pickInt(cam.getIntegerMin('Width'), cam.getIntegerMax('Width'), 128);
      const h = pickInt(cam.getIntegerMin('Height'), cam.getIntegerMax('Height'), 64);
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      if (entries.includes('RGB8')) {
        cam.setEnum('PixelFormat', 'RGB8');
        const g = cam.grab();
        assert.equal(g.payloadSize, w * h * 3);
        assert.equal(g.buffer.length, g.payloadSize);
      } else {
        t.skip(`RGB8 not offered by this device (entries: ${entries.join(',')})`);
      }
    } finally {
      try { cam.setEnum('PixelFormat', origFormat); } catch { /* restore best effort */ }
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('delivers chunk data and deterministic frames', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      cam.setBoolean('ChunkModeActive', true);
      const a = cam.grab();
      const b = cam.grab();
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
