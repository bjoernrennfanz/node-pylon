'use strict';
// genicam: consolidated nodemap behavior (types, access, errors).
// Consolidates the emulated pypylon nodemap-level tests that share one
// wrapper surface: node_test.py (INode type/access), nodemapwrapper_test.py,
// parameter_test.py + parametertestcase (read/write/guards), portparameter,
// categoryparameter, arrayparameter, placeholderparameter, enumentryparameter
// (all observed through the typed accessors), converter_test.py (float/int
// conversion errors surface as TypeError), error_test.py (unknown-node and
// access errors) and properties_test.py (read-only flags).
// The synthetic-XML genicam suites (alias, callback, cycledetector, poll,
// port, register, selector, streaming, swissknife, floattestsuite) run without
// a camera against hand-built XML and have no emulator counterpart here;
// they are documented as deviations in docs/testing.md.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam nodemap', () => {
  it('typed accessors agree on node types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.equal(cam.getNodeType('Width'), 'Integer');
      assert.equal(cam.getNodeType('ExposureTime'), 'Float');
      assert.equal(cam.getNodeType('PixelFormat'), 'Enumeration');
      assert.equal(cam.getNodeType('ChunkModeActive'), 'Boolean');
      assert.equal(cam.getNodeType('DeviceUserID'), 'String');
      assert.equal(cam.getNodeType('AcquisitionStart'), 'Command');
    } finally {
      cam.close();
    }
  });

  it('every accessor rejects unknown nodes', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const name = 'NoSuchNode12345';
      assert.throws(() => cam.getNodeType(name), /Unknown node/);
      assert.throws(() => cam.isReadable(name), /Unknown node/);
      assert.throws(() => cam.isWritable(name), /Unknown node/);
      assert.throws(() => cam.getInteger(name), /Unknown node/);
      assert.throws(() => cam.getIntegerMin(name), /Unknown node/);
      assert.throws(() => cam.getIntegerMax(name), /Unknown node/);
      assert.throws(() => cam.getFloat(name), /Unknown node/);
      assert.throws(() => cam.getBoolean(name), /Unknown node/);
      assert.throws(() => cam.getString(name), /Unknown node/);
      assert.throws(() => cam.getEnum(name), /Unknown node/);
      assert.throws(() => cam.getEnumEntries(name), /Unknown node/);
      assert.throws(() => cam.executeCommand(name), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('cross-type access throws TypeError', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      // Integer node through every other typed accessor.
      assert.throws(() => cam.getFloat('Width'), TypeError);
      assert.throws(() => cam.getBoolean('Width'), TypeError);
      assert.throws(() => cam.getString('Width'), TypeError);
      assert.throws(() => cam.getEnum('Width'), TypeError);
      assert.throws(() => cam.executeCommand('Width'), TypeError);
      // Float node through integer/boolean accessors.
      assert.throws(() => cam.getInteger('ExposureTime'), TypeError);
      assert.throws(() => cam.getBoolean('ExposureTime'), TypeError);
      // Enum node through numeric accessors.
      assert.throws(() => cam.getInteger('PixelFormat'), TypeError);
      assert.throws(() => cam.getFloat('PixelFormat'), TypeError);
      assert.throws(() => cam.tryMax('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('read-only nodes reject writes', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.equal(cam.isWritable('DeviceModelName'), false);
      assert.throws(() => cam.setString('DeviceModelName', 'x'), /not writable/);
    } finally {
      cam.close();
    }
  });

  it('set accessors validate argument types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setInteger('Width', 'wide'), TypeError);
      assert.throws(() => cam.setFloat('ExposureTime', 'long'), TypeError);
      assert.throws(() => cam.setBoolean('ChunkModeActive', 'yes'), TypeError);
      assert.throws(() => cam.setString('DeviceUserID', 42), TypeError);
      assert.throws(() => cam.setEnum('PixelFormat', 42), TypeError);
    } finally {
      cam.close();
    }
  });

  it('closed cameras reject nodemap access', (t) => {
    if (!requireDevice(t)) return;
    const cam = pylon.createFirstCamera();
    assert.throws(() => cam.getNodeType('Width'), /not open/i);
    assert.throws(() => cam.getInteger('Width'), /not open/i);
    assert.throws(() => cam.getFloat('ExposureTime'), /not open/i);
    assert.throws(() => cam.getBoolean('ChunkModeActive'), /not open/i);
    assert.throws(() => cam.getString('DeviceUserID'), /not open/i);
    assert.throws(() => cam.getEnum('PixelFormat'), /not open/i);
    assert.throws(() => cam.executeCommand('AcquisitionStart'), /not open/i);
  });
});
