'use strict';
// RN parity: GenICam nodemap suites, mirrored 1:1 in logic from
// test/genicam/*.test.js (integer, float, boolean, string, enumeration,
// command, nodemap, nodetype, properties, register, value-string,
// category-selector). Not ported: callback (needs the event-emitter
// bridge, see RN_COVERAGE_GAPS).
//
// Runs against the emulator-like core stub (no camera/SDK needed); the
// stub mirrors CamEmu node semantics and pylon_core_* error texts, and the
// facade maps them to TypeError/RangeError exactly like lib/index.js.
const { describe, it, before } = require('node:test');
const assert = require('node:assert/strict');
const { loadFacade } = require('./rn-stub-core');

let f = null;

before(async () => {
  ({ facade: f } = await loadFacade());
});

function freshCamera() {
  const cam = new f.InstantCamera();
  cam.open();
  return cam;
}

describe('rn-parity genicam Integer', () => {
  it('reads values, limits and writes roundtrip', () => {
    const cam = freshCamera();
    const orig = cam.getInteger('Width');
    try {
      const v = cam.getInteger('Width');
      assert.equal(typeof v, 'number');
      const mn = cam.getIntegerMin('Width');
      const mx = cam.getIntegerMax('Width');
      assert.ok(mn <= v && v <= mx);
      const target = v === mn ? Math.min(mx, mn + 64) : mn;
      cam.setInteger('Width', target);
      assert.equal(cam.getInteger('Width'), target);
      cam.setInteger('Width', orig);
      assert.equal(cam.getInteger('Width'), orig);
    } finally {
      try { cam.setInteger('Width', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('rejects out-of-range writes', () => {
    const cam = freshCamera();
    try {
      const mx = cam.getIntegerMax('Width');
      assert.throws(() => cam.setInteger('Width', mx + 1), RangeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getInteger('ExposureTime'), TypeError);
      assert.throws(() => cam.getInteger('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('tryMax sets the maximum', () => {
    const cam = freshCamera();
    const orig = cam.getInteger('Height');
    try {
      const max = cam.tryMax('Height');
      assert.equal(max, cam.getIntegerMax('Height'));
      assert.equal(cam.getInteger('Height'), max);
      assert.throws(() => cam.tryMax('PixelFormat'), TypeError);
    } finally {
      try { cam.setInteger('Height', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });
});

describe('rn-parity genicam Float', () => {
  it('reads values, limits and writes roundtrip', () => {
    const cam = freshCamera();
    const orig = cam.getFloat('ExposureTime');
    try {
      const v = cam.getFloat('ExposureTime');
      assert.equal(typeof v, 'number');
      assert.ok(v > 0);
      const mn = cam.getFloatMin('ExposureTime');
      const mx = cam.getFloatMax('ExposureTime');
      assert.ok(mn <= v && v <= mx);
      const target = Math.min(mx, Math.max(mn, orig === mn ? mn + (mx - mn) / 2 : mn));
      cam.setFloat('ExposureTime', target);
      assert.equal(cam.getFloat('ExposureTime'), target);
      cam.setFloat('ExposureTime', orig);
      assert.equal(cam.getFloat('ExposureTime'), orig);
    } finally {
      try { cam.setFloat('ExposureTime', orig); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('rejects out-of-range writes', () => {
    const cam = freshCamera();
    try {
      const mx = cam.getFloatMax('ExposureTime');
      assert.throws(() => cam.setFloat('ExposureTime', mx * 2 + 1), RangeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getFloat('Width'), TypeError);
      assert.throws(() => cam.getFloat('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam Boolean', () => {
  it('roundtrips ChunkModeActive and restores', () => {
    const cam = freshCamera();
    const orig = cam.getBoolean('ChunkModeActive');
    try {
      assert.equal(typeof orig, 'boolean');
      cam.setBoolean('ChunkModeActive', !orig);
      assert.equal(cam.getBoolean('ChunkModeActive'), !orig);
      cam.setBoolean('ChunkModeActive', orig);
      assert.equal(cam.getBoolean('ChunkModeActive'), orig);
    } finally {
      try { cam.setBoolean('ChunkModeActive', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setBoolean('ChunkModeActive', 'yes'), TypeError);
      assert.throws(() => cam.getBoolean('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam String', () => {
  it('reads the model name, rejects writes to it', () => {
    const cam = freshCamera();
    try {
      const model = cam.getString('DeviceModelName');
      assert.equal(typeof model, 'string');
      assert.ok(model.length > 0);
      assert.throws(() => cam.setString('DeviceModelName', 'x'), /not writable/);
    } finally {
      cam.close();
    }
  });

  it('roundtrips DeviceUserID and restores', () => {
    const cam = freshCamera();
    const orig = cam.getString('DeviceUserID');
    try {
      cam.setString('DeviceUserID', 'node-pylon-test');
      assert.equal(cam.getString('DeviceUserID'), 'node-pylon-test');
      cam.setString('DeviceUserID', orig);
      assert.equal(cam.getString('DeviceUserID'), orig);
    } finally {
      try { cam.setString('DeviceUserID', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setString('DeviceUserID', 42), TypeError);
      assert.throws(() => cam.getString('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam Enumeration', () => {
  it('reads value and entries, writes roundtrip', () => {
    const cam = freshCamera();
    const orig = cam.getEnum('PixelFormat');
    try {
      const entries = cam.getEnumEntries('PixelFormat');
      assert.ok(Array.isArray(entries));
      assert.ok(entries.length > 0);
      assert.ok(entries.includes(orig));
      for (const entry of entries.slice(0, 2)) {
        cam.setEnum('PixelFormat', entry);
        assert.equal(cam.getEnum('PixelFormat'), entry);
      }
      cam.setEnum('PixelFormat', orig);
      assert.equal(cam.getEnum('PixelFormat'), orig);
    } finally {
      try { cam.setEnum('PixelFormat', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('rejects invalid entries', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setEnum('PixelFormat', 'NotARealEntry12345'), RangeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getEnum('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam Command', () => {
  it('executes AcquisitionStart and reports done', () => {
    const cam = freshCamera();
    try {
      cam.executeCommand('AcquisitionStart');
      assert.equal(cam.isCommandDone('AcquisitionStart'), true);
    } finally {
      cam.close();
    }
  });

  it('rejects unknown commands', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.executeCommand('NoSuchCommand12345'), /unknown node/);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.executeCommand('Width'), TypeError);
      assert.throws(() => cam.isCommandDone('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam nodemap', () => {
  it('reports exact node types', () => {
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

  it('reports unknown nodes on every getter', () => {
    const cam = freshCamera();
    try {
      for (const get of [
        () => cam.getNodeType('NoSuchNode12345'),
        () => cam.getInteger('NoSuchNode12345'),
        () => cam.getFloat('NoSuchNode12345'),
        () => cam.getBoolean('NoSuchNode12345'),
        () => cam.getString('NoSuchNode12345'),
        () => cam.getEnum('NoSuchNode12345'),
        () => cam.executeCommand('NoSuchNode12345'),
        () => cam.getIntegerMin('NoSuchNode12345'),
        () => cam.getFloatMax('NoSuchNode12345'),
        () => cam.getEnumEntries('NoSuchNode12345'),
        () => cam.getValueAsString('NoSuchNode12345'),
        () => cam.getCategoryFeatures('NoSuchNode12345'),
      ]) {
        assert.throws(get, /unknown node/);
      }
    } finally {
      cam.close();
    }
  });

  it('rejects cross-type access', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getFloat('Width'), TypeError);
      assert.throws(() => cam.getBoolean('Width'), TypeError);
      assert.throws(() => cam.getString('Width'), TypeError);
      assert.throws(() => cam.getEnum('Width'), TypeError);
      assert.throws(() => cam.executeCommand('Width'), TypeError);
      assert.throws(() => cam.tryMax('PixelFormat'), TypeError);
      assert.throws(() => cam.getInteger('ExposureTime'), TypeError);
      assert.throws(() => cam.getBoolean('ExposureTime'), TypeError);
      assert.throws(() => cam.getInteger('PixelFormat'), TypeError);
      assert.throws(() => cam.getFloat('PixelFormat'), TypeError);
    } finally {
      cam.close();
    }
  });

  it('rejects wrong JS value types on setters', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.setInteger('Width', 'x'), TypeError);
      assert.throws(() => cam.setFloat('ExposureTime', 'x'), TypeError);
      assert.throws(() => cam.setBoolean('ChunkModeActive', 1), TypeError);
      assert.throws(() => cam.setString('DeviceUserID', 42), TypeError);
      assert.throws(() => cam.setEnum('PixelFormat', 42), TypeError);
    } finally {
      cam.close();
    }
  });

  it('rejects access on a closed camera', () => {
    const cam = freshCamera();
    cam.close();
    assert.throws(() => cam.grabOne(100), /not open/);
    assert.throws(() => cam.startGrabbing(), /not open/);
  });

  it('DeviceModelName is read-only', () => {
    const cam = freshCamera();
    try {
      assert.equal(cam.isWritable('DeviceModelName'), false);
      assert.throws(() => cam.setString('DeviceModelName', 'x'), /not writable/);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam nodetype', () => {
  it('maps principal interface types', () => {
    const cam = freshCamera();
    try {
      assert.equal(cam.getNodeType('Width'), 'Integer');
      assert.equal(cam.getNodeType('ExposureTime'), 'Float');
      assert.equal(cam.getNodeType('ChunkModeActive'), 'Boolean');
      assert.equal(cam.getNodeType('DeviceUserID'), 'String');
      assert.equal(cam.getNodeType('PixelFormat'), 'Enumeration');
      assert.equal(cam.getNodeType('AcquisitionStart'), 'Command');
    } finally {
      cam.close();
    }
  });

  it('reports readable/writable flags', () => {
    const cam = freshCamera();
    try {
      assert.equal(cam.isReadable('Width'), true);
      assert.equal(cam.isWritable('Width'), true);
      assert.equal(cam.isWritable('DeviceModelName'), false);
      assert.equal(typeof cam.isWritable('PayloadSize'), 'boolean');
    } finally {
      cam.close();
    }
  });

  it('reports unknown nodes', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getNodeType('NoSuchNode12345'), /unknown node/);
    } finally {
      cam.close();
    }
  });

  it('reads values through the typed accessors', () => {
    const cam = freshCamera();
    try {
      assert.equal(typeof cam.getInteger('Width'), 'number');
      assert.equal(typeof cam.getFloat('ExposureTime'), 'number');
      assert.equal(typeof cam.getBoolean('ChunkModeActive'), 'boolean');
      assert.equal(typeof cam.getString('DeviceUserID'), 'string');
      assert.equal(typeof cam.getEnum('PixelFormat'), 'string');
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam properties', () => {
  it('exposes integer metadata and min/max writes', () => {
    const cam = freshCamera();
    const orig = cam.getInteger('Width');
    try {
      const inc = cam.getIntegerInc('Width');
      assert.equal(typeof inc, 'number');
      assert.ok(inc >= 1);
      assert.equal(typeof cam.getIntegerUnit('Width'), 'string');
      assert.equal(cam.getIntegerRepresentation('Width'), 'Linear');
      assert.equal(cam.getIntegerMin('Width'), cam.getIntegerMin('Width'));
      cam.setIntegerToMin('Width');
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      cam.setIntegerToMax('Width');
      assert.equal(cam.getInteger('Width'), cam.getIntegerMax('Width'));
      cam.setInteger('Width', orig);
      assert.equal(cam.getInteger('Width'), orig);
    } finally {
      try { cam.setInteger('Width', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('exposes float metadata and min/max writes', () => {
    const cam = freshCamera();
    const orig = cam.getFloat('ExposureTime');
    try {
      assert.equal(typeof cam.hasFloatInc('ExposureTime'), 'boolean');
      assert.equal(typeof cam.getFloatUnit('ExposureTime'), 'string');
      assert.ok(cam.getFloatUnit('ExposureTime').length > 0);
      assert.equal(typeof cam.getFloatRepresentation('ExposureTime'), 'string');
      const mn = cam.getFloatMin('ExposureTime');
      const mx = cam.getFloatMax('ExposureTime');
      assert.ok(mn <= orig && orig <= mx);
      cam.setFloatToMin('ExposureTime');
      assert.equal(cam.getFloat('ExposureTime'), mn);
      cam.setFloatToMax('ExposureTime');
      assert.equal(cam.getFloat('ExposureTime'), mx);
      cam.setFloat('ExposureTime', orig);
      assert.equal(cam.getFloat('ExposureTime'), orig);
    } finally {
      try { cam.setFloat('ExposureTime', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('exposes string max length', () => {
    const cam = freshCamera();
    try {
      const maxLen = cam.getStringMaxLength('DeviceUserID');
      assert.equal(typeof maxLen, 'number');
      assert.ok(maxLen > 0);
    } finally {
      cam.close();
    }
  });

  it('rejects metadata on wrong types', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getIntegerInc('ExposureTime'), TypeError);
      assert.throws(() => cam.setIntegerToMin('ExposureTime'), TypeError);
      assert.throws(() => cam.getFloatInc('Width'), TypeError);
      assert.throws(() => cam.getStringMaxLength('Width'), TypeError);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam register', () => {
  it('rejects register access on non-register nodes', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getRegisterLength('Width'), TypeError);
      assert.throws(() => cam.getRegisterAddress('Width'), TypeError);
      assert.throws(() => cam.readRegister('Width'), TypeError);
      assert.throws(() => cam.getRegisterLength('NoSuchNode12345'), /unknown node/);
    } finally {
      cam.close();
    }
  });

  it('reads and writes a register as bytes', () => {
    const cam = freshCamera();
    try {
      const len = cam.getRegisterLength('TestRegister');
      assert.ok(len > 0);
      assert.equal(typeof cam.getRegisterAddress('TestRegister'), 'number');
      const data = cam.readRegister('TestRegister');
      assert.ok(data instanceof Uint8Array);
      assert.equal(data.length, len);
      const orig = Array.from(data);
      const flipped = Uint8Array.from(orig.map((b) => b ^ 0xff));
      cam.writeRegister('TestRegister', flipped);
      assert.deepEqual(Array.from(cam.readRegister('TestRegister')), Array.from(flipped));
      cam.writeRegister('TestRegister', Uint8Array.from(orig));
      assert.deepEqual(Array.from(cam.readRegister('TestRegister')), orig);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam value-string', () => {
  it('converts integers to/from string', () => {
    const cam = freshCamera();
    const orig = cam.getInteger('Width');
    try {
      assert.equal(cam.getValueAsString('Width'), String(orig));
      cam.setValueFromString('Width', String(cam.getIntegerMin('Width')));
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      cam.setValueFromString('Width', String(orig));
      assert.equal(cam.getInteger('Width'), orig);
    } finally {
      try { cam.setInteger('Width', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('converts enums to/from string and int', () => {
    const cam = freshCamera();
    const orig = cam.getEnum('PixelFormat');
    try {
      assert.equal(cam.getValueAsString('PixelFormat'), orig);
      const intValue = cam.getEnumIntValue('PixelFormat');
      assert.equal(typeof intValue, 'number');
      cam.setEnumIntValue('PixelFormat', intValue);
      assert.equal(cam.getEnum('PixelFormat'), orig);
      cam.setValueFromString('PixelFormat', orig);
      assert.equal(cam.getEnum('PixelFormat'), orig);
      assert.throws(() => cam.getEnumIntValue('Width'), TypeError);
      assert.throws(() => cam.setEnumIntValue('Width', 0), TypeError);
    } finally {
      try { cam.setEnum('PixelFormat', orig); } catch { /* restore */ }
      cam.close();
    }
  });

  it('executes commands and reports done', () => {
    const cam = freshCamera();
    try {
      cam.executeCommand('AcquisitionStart');
      assert.equal(cam.isCommandDone('AcquisitionStart'), true);
      assert.throws(() => cam.isCommandDone('Width'), TypeError);
      assert.throws(() => cam.getValueAsString('NoSuchNode12345'), /unknown node/);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity genicam category-selector', () => {
  it('lists Root category features', () => {
    const cam = freshCamera();
    try {
      const features = cam.getCategoryFeatures('Root');
      assert.ok(Array.isArray(features));
      assert.ok(features.length > 0);
      const img = cam.getCategoryFeatures('ImageFormatControl');
      assert.ok(img.includes('Width'));
    } finally {
      cam.close();
    }
  });

  it('rejects categories on wrong types and unknown nodes', () => {
    const cam = freshCamera();
    try {
      assert.throws(() => cam.getCategoryFeatures('Width'), TypeError);
      assert.throws(() => cam.getCategoryFeatures('NoSuchNode12345'), /unknown node/);
    } finally {
      cam.close();
    }
  });

  it('distinguishes selectors from plain nodes', () => {
    const cam = freshCamera();
    try {
      assert.equal(cam.isSelector('Width'), false);
      const v0 = cam.getInteger('Width');
      const v1 = cam.getInteger('Width');
      assert.equal(v0, v1);
      assert.equal(cam.getValueAsString('Width'), String(v0));
      if (cam.isSelector('TriggerSelector')) {
        const selected = cam.getSelectedFeatures('TriggerSelector');
        assert.ok(Array.isArray(selected));
        const selecting = cam.getSelectingFeatures('TriggerSelector');
        assert.ok(Array.isArray(selecting));
      }
    } finally {
      cam.close();
    }
  });
});
