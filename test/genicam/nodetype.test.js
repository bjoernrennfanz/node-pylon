'use strict';
// genicam: node types and access flags (analogous to pypylon's
// genicam node_test.py / INode checks).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam node types', () => {
  it('reports node types', (t) => {
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

  it('reports access flags', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.equal(cam.isReadable('Width'), true);
      assert.equal(cam.isWritable('Width'), true);
      assert.equal(cam.isWritable('DeviceModelName'), false);
      // PayloadSize writability is device-dependent (the emulator allows
      // writes); only the boolean shape is asserted here.
      assert.equal(typeof cam.isWritable('PayloadSize'), 'boolean');
    } finally {
      cam.close();
    }
  });

  it('rejects unknown nodes', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      for (const fn of ['getNodeType', 'isReadable', 'isWritable']) {
        assert.throws(() => cam[fn]('NoSuchNode12345'), /Unknown node/);
      }
      for (const fn of ['getInteger', 'getFloat', 'getBoolean', 'getString', 'getEnum']) {
        assert.throws(() => cam[fn]('NoSuchNode12345'), /Unknown node/);
      }
      assert.throws(() => cam.executeCommand('NoSuchCommand12345'), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('requires an open camera', (t) => {
    if (!requireDevice(t)) return;
    const cam = pylon.createFirstCamera();
    assert.throws(() => cam.getInteger('Width'), /not open/i);
    assert.throws(() => cam.getNodeType('Width'), /not open/i);
  });
});
