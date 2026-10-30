'use strict';
// genicam: IValue string conversion + enum int values + command IsDone.
// Ports converter_test.py / swissknife evaluation logic (ToString/FromString
// round-trips), enumentryparameter int mapping, and command IsDone polling.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

function executableCommand(cam) {
  for (const name of ['UserSetSave', 'ForceFailedBuffer', 'AcquisitionStop']) {
    try {
      cam.getNodeType(name);
      return name;
    } catch {
      // Not offered — try the next one.
    }
  }
  return null;
}

describe('genicam value conversion', () => {
  it('ToString/FromString roundtrips integers and enums', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const origWidth = cam.getInteger('Width');
    const origPf = cam.getEnum('PixelFormat');
    try {
      const asString = cam.getValueAsString('Width');
      assert.equal(asString, String(origWidth));
      cam.setValueFromString('Width', String(cam.getIntegerMin('Width')));
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      const pfString = cam.getValueAsString('PixelFormat');
      assert.equal(pfString, origPf);
      assert.throws(() => cam.getValueAsString('NoSuchNode12345'), /Unknown node/);
    } finally {
      try { cam.setInteger('Width', origWidth); } catch { /* best effort */ }
      try { cam.setEnum('PixelFormat', origPf); } catch { /* best effort */ }
      cam.close();
    }
  });

  it('enum int values roundtrip', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getEnum('PixelFormat');
    const origInt = cam.getEnumIntValue('PixelFormat');
    try {
      assert.equal(typeof origInt, 'number');
      cam.setEnumIntValue('PixelFormat', origInt);
      assert.equal(cam.getEnum('PixelFormat'), orig);
      assert.throws(() => cam.getEnumIntValue('Width'), TypeError);
      assert.throws(() => cam.setEnumIntValue('Width', 0), TypeError);
    } finally {
      try { cam.setEnum('PixelFormat', orig); } catch { /* best effort */ }
      cam.close();
    }
  });

  it('command IsDone after execute', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const name = executableCommand(cam);
      if (name === null) {
        t.skip('no side-effect-free command offered by this device');
        return;
      }
      assert.equal(cam.executeCommand(name), true);
      assert.equal(cam.isCommandDone(name), true);
      assert.throws(() => cam.isCommandDone('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
