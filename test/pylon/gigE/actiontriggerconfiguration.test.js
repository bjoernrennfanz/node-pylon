'use strict';
// pylon/gigE: action trigger configuration (mirrors pypylon's
// tests/pylon/gigE/actiontriggerconfiguration_test.py). Hardware-only (skips
// without a GigE camera; CI runs the emulated suite). CActionTriggerConfiguration
// is not wrapped in the MVP, so this records the gap and skips on hardware.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireGigE } = require('../../helpers');

describe('pylon gigE action trigger (hardware only, MVP gap)', () => {
  it('action trigger configuration', (t) => {
    if (!requireGigE(t)) return;
    t.skip('CActionTriggerConfiguration is not wrapped in the MVP');
  });

  it('facade exposes no ActionTriggerConfiguration (documents the gap)', () => {
    assert.equal(pylon.ActionTriggerConfiguration, undefined);
  });
});
