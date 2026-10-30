'use strict';
// pylon: image/pixel-type/payload surface via grab() and retrieveResult().
// Covers the emulated pypylon image tests that map onto the MVP wrapper:
// image_test.py (IImage identity), pylonimage_test.py (geometry/pixels),
// pixeltype_test.py + pixeltypemapper_test.py (format names), payloadtype_test.py
// (payload size), imageformatconverter (payload scaling per format).
// Out of scope for the MVP wrapper and documented in docs/testing.md:
// PylonImageContainer/ImageDecompressor persistence helpers, GenDC parsing
// (pylondatacontainer_test.py) and DataProcessing recipes.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice, pickInt } = require('../../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

function bytesPerPixel(format) {
  if (format === 'RGB8') return 3;
  if (format === 'Mono12') return 2;
  return 1;
}

describe('pylon image attributes', () => {
  it('grab geometry matches the nodemap', (t) => {
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
      assert.equal(g.width, cam.getInteger('Width'));
      assert.equal(g.height, cam.getInteger('Height'));
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('pixel type mirrors the nodemap format', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const g = cam.grab();
      assert.equal(typeof g.pixelFormat, 'string');
      assert.ok(g.pixelFormat.length > 0);
      assert.equal(g.pixelType, g.pixelFormat);
      assert.equal(g.pixelFormat, cam.getEnum('PixelFormat'));
    } finally {
      cam.close();
    }
  });

  it('payload size matches buffer length and bytes per pixel', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    const origFormat = cam.getEnum('PixelFormat');
    try {
      const w = pickInt(cam.getIntegerMin('Width'), cam.getIntegerMax('Width'), 64);
      const h = pickInt(cam.getIntegerMin('Height'), cam.getIntegerMax('Height'), 48);
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      const g = cam.grab();
      assert.ok(Buffer.isBuffer(g.buffer));
      assert.equal(g.payloadSize, g.buffer.length);
      assert.equal(g.payloadSize, w * h * bytesPerPixel(g.pixelFormat));
    } finally {
      try { cam.setEnum('PixelFormat', origFormat); } catch { /* restore best effort */ }
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('frames carry timestamps, numbers and chunk data', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      cam.setBoolean('ChunkModeActive', true);
      const a = cam.grab();
      const b = cam.grab();
      assert.ok(a.timestamp > 0);
      assert.ok(b.timestamp >= a.timestamp);
      assert.equal(b.frameNumber, a.frameNumber + 1);
      assert.equal(a.chunk.modeActive, true);
      assert.equal(a.chunk.frameID, a.frameNumber);
      assert.equal(a.chunk.timestamp, a.timestamp);
      assert.notDeepEqual(a.buffer, b.buffer);
      cam.setBoolean('ChunkModeActive', false);
    } finally {
      cam.close();
    }
  });

  it('retrieveResult exposes the same image fields', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.TlFactory.GetInstance().CreateFirstDevice();
      camera.open();
      try {
        camera.startGrabbing();
        const res = camera.retrieveResult(10000);
        assert.ok(res !== null);
        assert.equal(res.succeeded, true);
        assert.equal(res.buffer.length, res.payloadSize);
        assert.equal(typeof res.pixelFormat, 'string');
        assert.equal(res.pixelType, res.pixelFormat);
      } finally {
        try { camera.stopGrabbing(); } catch { /* already stopped */ }
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});
