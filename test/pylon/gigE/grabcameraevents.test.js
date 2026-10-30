'use strict';
// pylon/gigE: camera events during grab (mirrors pypylon's
// tests/pylon/gigE/grabcameraevents_test.py). Hardware-only (skips without a
// GigE camera; CI runs the emulated suite). Device-node event delivery via
// an event-grab thread is a follow-up, so this records the gap and skips.
const { describe, it } = require('node:test');
const { requireGigE } = require('../../helpers');

describe('pylon gigE grab camera events (hardware only, MVP gap)', () => {
  it('exposure-end / frame-trigger events during grab', (t) => {
    if (!requireGigE(t)) return;
    t.skip('device-node event delivery via an event-grab thread is a follow-up');
  });
});
