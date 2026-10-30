'use strict';
// pylon/gigE: GigE transport layer on physical hardware (mirrors pypylon's
// tests/pylon/gigE/gigetransportlayer_test.py). Hardware-only: skips without
// a GigE camera — GitHub CI only provides the CamEmu emulator, so CI runs
// the emulated suite (`npm run test:emulated`) instead.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE transport layer (hardware only)', () => {
  it('GigE transport layer is enumerated', (t) => {
    if (!requireGigE(t)) return;
    const tls = pylon.TlFactory.EnumerateTls();
    const names = tls.map((tl) => String(tl.GetTLType
      ? tl.GetTLType() : (tl.tlType || tl.TLType || '')));
    assert.ok(names.some((n) => n.includes('GigE')),
      `expected a GigE TL among: ${names.join(', ')}`);
  });

  it('EnumerateDevices finds the GigE camera via DeviceClass filter', (t) => {
    if (!requireGigE(t)) return;
    const devices = pylon.TlFactory.EnumerateDevices({ DeviceClass: 'BaslerGigE' });
    assert.ok(devices.length >= 1, 'expected at least one GigE device');
  });
});
