'use strict';
// pylon/usb: camera events during grab (mirrors pypylon's
// tests/pylon/usb/grabcameraevents_test.py). Hardware-only (skips without a
// USB camera; CI runs the emulated suite). Device-node event delivery via
// an event-grab thread is a follow-up, so this records the gap and skips.
const { describe, it } = require('node:test');
const { requireUsb } = require('../../helpers');

describe('pylon usb grab camera events (hardware only, MVP gap)', () => {
  it('exposure-end / frame-trigger events during grab', (t) => {
    if (!requireUsb(t)) return;
    t.skip('device-node event delivery via an event-grab thread is a follow-up');
  });
});
