'use strict';
// pylon: FeaturePersistence save/load (analogous to pypylon's
// featurepersistence_test.py, 1:1 in logic).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

function tempFile() {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'nodepylon-'));
  return {
    dir,
    file: path.join(dir, 'features.pfs'),
    cleanup() {
      fs.rmSync(dir, { recursive: true, force: true });
    },
  };
}

describe('pylon FeaturePersistence', () => {
  it('save writes features to a file and load restores them', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    const tmp = tempFile();
    try {
      cam.setInteger('Width', cam.getIntegerMin('Width'));
      pylon.FeaturePersistence.save(tmp.file, cam);
      assert.ok(fs.statSync(tmp.file).size > 0);
      cam.setInteger('Width', cam.getIntegerMax('Width'));
      pylon.FeaturePersistence.load(tmp.file, cam);
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      pylon.FeaturePersistence.Save(tmp.file, cam);
      pylon.FeaturePersistence.Load(tmp.file, cam);
    } finally {
      tmp.cleanup();
      cam.close();
    }
  });

  it('load with validate=false restores without strict validation', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    const tmp = tempFile();
    try {
      cam.setInteger('Width', cam.getIntegerMin('Width'));
      pylon.FeaturePersistence.save(tmp.file, cam);
      cam.setInteger('Width', cam.getIntegerMax('Width'));
      pylon.FeaturePersistence.load(tmp.file, cam, false);
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
    } finally {
      tmp.cleanup();
      cam.close();
    }
  });

  it('saveToString serializes and loadFromString restores', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.setInteger('Width', cam.getIntegerMin('Width'));
      const state = pylon.FeaturePersistence.saveToString(cam);
      assert.equal(typeof state, 'string');
      assert.ok(state.length > 0);
      cam.setInteger('Width', cam.getIntegerMax('Width'));
      pylon.FeaturePersistence.loadFromString(state, cam);
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
      const again = pylon.FeaturePersistence.SaveToString(cam);
      pylon.FeaturePersistence.LoadFromString(again, cam);
    } finally {
      cam.close();
    }
  });

  it('loadFromString with validate=false restores without strict validation', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.setInteger('Width', cam.getIntegerMin('Width'));
      const state = pylon.FeaturePersistence.saveToString(cam);
      cam.setInteger('Width', cam.getIntegerMax('Width'));
      pylon.FeaturePersistence.loadFromString(state, cam, false);
      assert.equal(cam.getInteger('Width'), cam.getIntegerMin('Width'));
    } finally {
      cam.close();
    }
  });

  it('loading a missing file throws', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => pylon.FeaturePersistence.load(
        path.join(os.tmpdir(), 'nodepylon-does-not-exist.pfs'), cam));
    } finally {
      cam.close();
    }
  });
});
