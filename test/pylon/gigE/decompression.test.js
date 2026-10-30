'use strict';
// pylon/gigE: decompression on physical hardware (mirrors pypylon's
// tests/pylon/gigE/decompression_test.py). Hardware-only: skips without a
// GigE camera — GitHub CI only provides the CamEmu emulator, so CI runs the
// emulated suite (`npm run test:emulated`) instead. Full parity (compressed
// GigE stream round-trip) needs a hardware stream with compression enabled;
// until then the wrapped ImageDecompressor surface is exercised.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE decompression (hardware only)', () => {
  it('ImageDecompressor reports a compression mode', (t) => {
    if (!requireGigE(t)) return;
    const mode = pylon.ImageDecompressor.getCompressionMode();
    assert.equal(typeof mode, 'number');
  });
});
