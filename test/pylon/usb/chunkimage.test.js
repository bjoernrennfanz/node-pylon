'use strict';
// pylon/usb: chunk images (mirrors pypylon's tests/pylon/usb/chunkimage_test.py).
// Hardware-only (skips without a USB camera; CI runs the emulated suite).
// Chunk payload parsing is not wrapped in the MVP, so this records the gap
// and skips on hardware; chunk flags are covered behaviorally via
// isChunkDataAvailable (emulated suite).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb chunk image (hardware only, MVP gap)', () => {
  it('grab with chunk mode enabled', (t) => {
    if (!requireUsb(t)) return;
    t.skip('chunk payload parsing is not wrapped in the MVP');
  });

  it('facade exposes no chunk-image parser (documents the gap)', () => {
    assert.equal(pylon.ChunkImage, undefined);
  });
});
