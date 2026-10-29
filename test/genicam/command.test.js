'use strict';
// genicam: Command nodes (analogous to pypylon's commandparameter tests).
// Executes a side-effect-free standard command; the emulator rejects the
// acquisition commands outside a running grab, so those stay out of the
// success path (like in pypylon's emulated command tests).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice, hasNode } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

function executableCommand(cam) {
  for (const name of ['UserSetSave', 'ForceFailedBuffer', 'AcquisitionStop']) {
    if (hasNode(cam, name)) return name;
  }
  return null;
}

describe('genicam Command', () => {
  it('executes commands', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const name = executableCommand(cam);
      if (name === null) {
        t.skip('no side-effect-free command offered by this device');
        return;
      }
      assert.equal(cam.executeCommand(name), true);
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
