'use strict';
// API surface: lib exports must match types/index.d.ts.
// Run with: npm test (no camera, mock build).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const pylon = require('../lib');

const EXPECTED = ['version', 'pylonVersion', 'isMock', 'isNative', 'mockActive', 'enumerateDevices', 'grabMock',
  'initialize', 'terminate', 'createFirstCamera', 'createFirst'];
const EXPECTED_OBJECTS = ['TlFactory', 'InstantCamera'];

describe('node-pylon api surface', () => {
  it('exports all documented functions', () => {
    for (const name of EXPECTED) assert.equal(typeof pylon[name], 'function', name);
  });

  it('exports TlFactory and InstantCamera', () => {
    for (const name of EXPECTED_OBJECTS) assert.ok(pylon[name], name);
    assert.equal(typeof pylon.TlFactory.GetInstance, 'function');
    assert.equal(typeof pylon.TlFactory.CreateFirstDevice, 'function');
  });

  it('d.ts declares all lib exports', () => {
    const dts = fs.readFileSync(path.join(__dirname, '..', 'types', 'index.d.ts'), 'utf8');
    for (const name of EXPECTED) assert.match(dts, new RegExp(`function ${name}\\b`), name);
  });

  it('return types match docs', () => {
    assert.equal(typeof pylon.version(), 'string');
    assert.equal(typeof pylon.pylonVersion(), 'string');
    assert.equal(typeof pylon.isMock(), 'boolean');
    assert.ok(Array.isArray(pylon.enumerateDevices()));
    const buf = pylon.grabMock(4, 3);
    assert.ok(Buffer.isBuffer(buf));
    assert.equal(buf.length, 12);
  });
});
