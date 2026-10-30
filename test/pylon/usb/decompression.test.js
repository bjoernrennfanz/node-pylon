'use strict';
// pylon/usb: decompression on physical hardware (mirrors pypylon's
// tests/pylon/usb/decompression_test.py). Hardware-only: skips without a USB
// camera — GitHub CI only provides the CamEmu emulator, so CI runs the
// emulated suite (`npm run test:emulated`) instead. Full parity (compressed
// USB stream round-trip) needs a hardware stream with compression enabled;
// until then the wrapped ImageDecompressor surface is exercised.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb decompression (hardware only)', () => {
  it('ImageDecompressor reports a compression mode', (t) => {
    if (!requireUsb(t)) return;
    const mode = pylon.ImageDecompressor.getCompressionMode();
    assert.equal(typeof mode, 'number');
  });
});
