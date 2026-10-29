'use strict';
// genicam: Command nodes (analogous to pypylon's commandparameter tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam Command', () => {
  it('executes commands', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.equal(cam.executeCommand('AcquisitionStart'), true);
      assert.equal(cam.executeCommand('AcquisitionStop'), true);
    } finally {
      cam.close();
    }
  });

  it('rejects unknown commands', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.executeCommand('NoSuchCommand12345'), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.throws(() => cam.executeCommand('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
