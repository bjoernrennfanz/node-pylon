'use strict';
// pylon/usb: event adapter U3V (mirrors pypylon's
// tests/pylon/usb/eventadapteru3v_test.py). Hardware-only (skips without a
// USB camera; CI runs the emulated suite). CEventAdapterU3V / device-node
// event delivery via an event-grab thread is a follow-up (see emulated
// instantcamera-full handler docs), so this records the gap and skips.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireUsb } = require('../../helpers');

describe('pylon usb event adapter (hardware only, MVP gap)', () => {
  it('event adapter U3V attach/receive', (t) => {
    if (!requireUsb(t)) return;
    t.skip('CEventAdapterU3V is not wrapped in the MVP');
  });

  it('facade exposes no EventAdapterU3V (documents the gap)', () => {
    assert.equal(pylon.EventAdapterU3V, undefined);
  });
});
