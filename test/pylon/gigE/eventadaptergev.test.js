'use strict';
// pylon/gigE: event adapter GeV (mirrors pypylon's
// tests/pylon/gigE/eventadaptergev_test.py). Hardware-only (skips without a
// GigE camera; CI runs the emulated suite). CEventAdapterGeV / device-node
// event delivery via an event-grab thread is a follow-up (see emulated
// instantcamera-full handler docs), so this records the gap and skips.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE event adapter (hardware only, MVP gap)', () => {
  it('event adapter GeV attach/receive', (t) => {
    if (!requireGigE(t)) return;
    t.skip('CEventAdapterGeV is not wrapped in the MVP');
  });

  it('facade exposes no EventAdapterGeV (documents the gap)', () => {
    assert.equal(pylon.EventAdapterGeV, undefined);
  });
});
