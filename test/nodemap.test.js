'use strict';
// Nodemap/Parameter-API analog pypylon (GenICam-Typen).
// Laeuft gegen den Real-SDK-Build (Default, Emulator/Kamera) oder den
// Mock-Opt-in (NODE_PYLON_USE_MOCK=1). Lauf mit: npm test
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../lib');
const { isMockMode, requireDevice } = require('./helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('nodemap parameter API (ZAU-151)', () => {
  it('exposes device info', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const info = cam.getDeviceInfo();
    assert.equal(typeof info.serialNumber, 'string');
    if (isMockMode()) assert.equal(info.serialNumber, 'MOCK123456');
    assert.equal(typeof info.modelName, 'string');
    assert.equal(typeof info.friendlyName, 'string');
    cam.close();
  });

  it('reports node types and access flags', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    assert.equal(cam.getNodeType('Width'), 'Integer');
    assert.equal(cam.getNodeType('ExposureTime'), 'Float');
    assert.equal(cam.getNodeType('PixelFormat'), 'Enumeration');
    assert.equal(cam.getNodeType('ChunkModeActive'), 'Boolean');
    assert.equal(cam.getNodeType('DeviceUserID'), 'String');
    assert.equal(cam.getNodeType('AcquisitionStart'), 'Command');
    assert.equal(cam.isReadable('Width'), true);
    assert.equal(cam.isWritable('Width'), true);
    assert.equal(cam.isWritable('PayloadSize'), false);
    assert.equal(cam.isWritable('DeviceModelName'), false);
    assert.throws(() => cam.getNodeType('NoSuchNode'), /Unknown node/);
    cam.close();
  });

  it('reads/writes Integer with limits and tryMax', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    assert.equal(cam.getInteger('Width'), 640);
    assert.equal(cam.getIntegerMin('Width'), 64);
    assert.equal(cam.getIntegerMax('Width'), 1920);
    cam.setInteger('Width', 320);
    assert.equal(cam.getInteger('Width'), 320);
    assert.throws(() => cam.setInteger('Width', 99999), RangeError);
    assert.throws(() => cam.setInteger('PayloadSize', 10), /not writable/);
    assert.throws(() => cam.getInteger('ExposureTime'), TypeError);
    const max = cam.tryMax('Height');
    assert.equal(max, 1080);
    assert.equal(cam.getInteger('Height'), 1080);
    assert.throws(() => cam.tryMax('PixelFormat'), TypeError);
    cam.close();
  });

  it('reads/writes Float with limits', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    assert.equal(cam.getFloat('ExposureTime'), 5000.0);
    cam.setFloat('ExposureTime', 8000.5);
    assert.equal(cam.getFloat('ExposureTime'), 8000.5);
    assert.throws(() => cam.setFloat('ExposureTime', -1), RangeError);
    assert.throws(() => cam.getFloat('Width'), TypeError);
    assert.equal(cam.getFloatMin('Gain'), 0.0);
    assert.equal(cam.getFloatMax('Gain'), 24.0);
    cam.close();
  });

  it('reads/writes Boolean', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    assert.equal(cam.getBoolean('ChunkModeActive'), false);
    cam.setBoolean('ChunkModeActive', true);
    assert.equal(cam.getBoolean('ChunkModeActive'), true);
    assert.throws(() => cam.setBoolean('ChunkModeActive', 'yes'), TypeError);
    cam.close();
  });

  it('reads/writes String and Enum, executes Commands', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    assert.equal(cam.getString('DeviceModelName'), 'MockModel Mono');
    assert.throws(() => cam.setString('DeviceModelName', 'x'), /not writable/);
    cam.setString('DeviceUserID', 'ZAU-151');
    assert.equal(cam.getString('DeviceUserID'), 'ZAU-151');
    assert.deepEqual(cam.getEnumEntries('PixelFormat'), ['Mono8', 'Mono12', 'RGB8']);
    assert.equal(cam.getEnum('PixelFormat'), 'Mono8');
    cam.setEnum('PixelFormat', 'RGB8');
    assert.equal(cam.getEnum('PixelFormat'), 'RGB8');
    assert.throws(() => cam.setEnum('PixelFormat', 'YUV422'), RangeError);
    assert.equal(cam.executeCommand('AcquisitionStart'), true);
    assert.equal(cam.executeCommand('UserSetSave'), true);
    assert.throws(() => cam.executeCommand('NoSuchCommand'), /Unknown node/);
    cam.close();
  });
});
