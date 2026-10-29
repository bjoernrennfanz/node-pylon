'use strict';
// pylon: module and SDK version strings (analogous to pypylon's
// version/pylonVersion surface).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');

describe('pylon versions', () => {
  it('exposes module and SDK versions', () => {
    assert.equal(typeof pylon.version(), 'string');
    assert.ok(pylon.version().length > 0);
    assert.equal(typeof pylon.pylonVersion(), 'string');
    assert.match(pylon.pylonVersion(), /\d+\./);
  });

  it('initialize/terminate do not throw (refcounted)', () => {
    pylon.initialize();
    pylon.initialize();
    pylon.terminate();
    pylon.terminate();
  });
});
