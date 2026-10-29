'use strict';
// Image handling — grab() attributes (Width/Height/PixelFormat),
// payload consistency, chunk data, zero-copy buffer.
// Runs against the real-SDK build (default, emulator/camera) or the
// mock opt-in (NODE_PYLON_USE_MOCK=1). Run with: npm test
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { requireDevice } = require('./helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('image handling', () => {
  it('grab reflects nodemap geometry and pixel format', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    cam.setInteger('Width', 64);
    cam.setInteger('Height', 48);
    cam.setEnum('PixelFormat', 'Mono8');
    const g = cam.grab();
    assert.equal(g.width, 64);
    assert.equal(g.height, 48);
    assert.equal(g.pixelFormat, 'Mono8');
    assert.equal(g.pixelType, 'Mono8');
    assert.equal(g.payloadSize, 64 * 48);
    assert.ok(Buffer.isBuffer(g.buffer));
    assert.equal(g.buffer.length, 64 * 48);
    cam.close();
  });

  it('payload scales with bytes per pixel (RGB8)', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    cam.setInteger('Width', 128);
    cam.setInteger('Height', 64);
    cam.setEnum('PixelFormat', 'RGB8');
    const g = cam.grab();
    assert.equal(g.payloadSize, 128 * 64 * 3);
    assert.equal(g.buffer.length, g.payloadSize);
    // PayloadSize nodemap matches the grab.
    assert.equal(cam.getInteger('PayloadSize'), g.payloadSize);
    cam.close();
  });

  it('delivers chunk data and deterministic frames', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    cam.setBoolean('ChunkModeActive', true);
    const a = cam.grab();
    const b = cam.grab();
    assert.equal(b.frameNumber, a.frameNumber + 1);
    assert.equal(a.chunk.modeActive, true);
    assert.equal(a.chunk.frameID, a.frameNumber);
    assert.ok(a.timestamp > 0);
    assert.notDeepEqual(a.buffer, b.buffer); // frame counter in pattern
    cam.close();
  });

  it('grabMock stays backward compatible', () => {
    const buf = pylon.grabMock(8, 4);
    assert.equal(buf.length, 32);
    assert.equal(buf[0], 0);
    assert.equal(buf[1], 1);
    assert.equal(buf[8], 1); // x=0,y=1 -> 1
  });
});
