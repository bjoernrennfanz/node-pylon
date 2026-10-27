'use strict';
// Minimaler Smoke-Test ohne Kamera (Mock-Build). Lauf mit: npm test
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');

describe('node-pylon scaffold (mock)', () => {
  it('exposes version strings', () => {
    assert.match(pylon.version(), /scaffold/);
    assert.equal(typeof pylon.pylonVersion(), 'string');
  });

  it('flags mock build', () => {
    assert.equal(pylon.isMock(), true);
  });

  it('enumerateDevices returns an array without camera', () => {
    const devices = pylon.enumerateDevices();
    assert.ok(Array.isArray(devices));
    assert.equal(devices.length, 0);
  });

  it('grabMock returns deterministic Mono8 buffer', () => {
    const buf = pylon.grabMock(8, 4);
    assert.equal(buf.length, 32);
    assert.equal(buf[0], 0);
    assert.equal(buf[1], 1);
    assert.equal(buf[8], 1); // x=0,y=1 -> 1
  });
});
