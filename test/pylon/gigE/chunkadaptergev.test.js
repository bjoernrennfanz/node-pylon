'use strict';
// pylon/gigE: chunk adapter GeV (mirrors pypylon's
// tests/pylon/gigE/chunkadaptergev_test.py). Hardware-only (skips without a
// GigE camera; CI runs the emulated suite). CChunkAdapterGeV is not wrapped
// in the MVP, so this records the gap and skips on hardware.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE chunk adapter (hardware only, MVP gap)', () => {
  it('chunk adapter GeV attach/configure', (t) => {
    if (!requireGigE(t)) return;
    t.skip('CChunkAdapterGeV is not wrapped in the MVP; chunk payloads are covered behaviorally via isChunkDataAvailable (emulated suite)');
  });

  it('facade exposes no ChunkAdapterGeV (documents the gap)', () => {
    assert.equal(pylon.ChunkAdapterGeV, undefined);
  });
});
