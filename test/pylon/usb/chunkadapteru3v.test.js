'use strict';
// pylon/usb: chunk adapter U3V (mirrors pypylon's
// tests/pylon/usb/chunkadapteru3v_test.py). Hardware-only (skips without a
// USB camera; CI runs the emulated suite). CChunkAdapterU3V is not wrapped
// in the MVP, so this records the gap and skips on hardware.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb chunk adapter (hardware only, MVP gap)', () => {
  it('chunk adapter U3V attach/configure', (t) => {
    if (!requireUsb(t)) return;
    t.skip('CChunkAdapterU3V is not wrapped in the MVP; chunk payloads are covered behaviorally via isChunkDataAvailable (emulated suite)');
  });

  it('facade exposes no ChunkAdapterU3V (documents the gap)', () => {
    assert.equal(pylon.ChunkAdapterU3V, undefined);
  });
});
