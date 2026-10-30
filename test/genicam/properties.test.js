'use strict';
// genicam: INode properties (IsAvailable, display name, description,
// inc/unit/representation, min/max helpers).
// Ports the logic of pypylon's properties_test.py, node_test.py (INode
// accessors) and floattestsuite/integer sections: every check runs against
// the emulator nodemap, values stay inside device limits.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam properties', () => {
  it('reports availability and naming', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      assert.equal(cam.isAvailable('Width'), true);
      assert.equal(typeof cam.isAvailable('ExposureTime'), 'boolean');
      const display = cam.getDisplayName('Width');
      assert.equal(typeof display, 'string');
      assert.ok(display.length > 0);
      const desc = cam.getDescription('Width');
      assert.equal(typeof desc, 'string');
      assert.throws(() => cam.isAvailable('NoSuchNode12345'), /Unknown node/);
      assert.throws(() => cam.getDisplayName('NoSuchNode12345'), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('integer inc/unit/representation and min/max helpers', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getInteger('Width');
    try {
      const inc = cam.getIntegerInc('Width');
      assert.ok(Number.isInteger(inc) && inc >= 1);
      assert.equal(typeof cam.getIntegerUnit('Width'), 'string');
      const repr = cam.getIntegerRepresentation('Width');
      assert.ok(['Linear', 'Logarithmic', 'PureNumber', 'HexNumber', 'Boolean', 'IPV4Address', 'MACAddress', '_UndefinedRepresentation'].includes(repr));
      cam.setIntegerToMin('Width');
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      cam.setIntegerToMax('Width');
      assert.equal(cam.getInteger('Width'), cam.getIntegerMax('Width'));
      assert.throws(() => cam.getIntegerInc('ExposureTime'), TypeError);
      assert.throws(() => cam.setIntegerToMin('ExposureTime'), TypeError);
    } finally {
      try { cam.setInteger('Width', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('float inc/unit/representation and min/max helpers', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    const orig = cam.getFloat('ExposureTime');
    try {
      assert.equal(typeof cam.hasFloatInc('ExposureTime'), 'boolean');
      assert.equal(typeof cam.getFloatUnit('ExposureTime'), 'string');
      const repr = cam.getFloatRepresentation('ExposureTime');
      assert.equal(typeof repr, 'string');
      assert.ok(repr.length > 0);
      cam.setFloatToMin('ExposureTime');
      assert.equal(cam.getFloat('ExposureTime'), cam.getFloatMin('ExposureTime'));
      cam.setFloatToMax('ExposureTime');
      assert.equal(cam.getFloat('ExposureTime'), cam.getFloatMax('ExposureTime'));
      assert.throws(() => cam.getFloatInc('Width'), TypeError);
    } finally {
      try { cam.setFloat('ExposureTime', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('string max length', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const max = cam.getStringMaxLength('DeviceUserID');
      assert.ok(Number.isInteger(max) && max > 0);
      assert.throws(() => cam.getStringMaxLength('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});
