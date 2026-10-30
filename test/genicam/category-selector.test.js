'use strict';
// genicam: categories, selectors, aliases and poll semantics.
// Ports categoryparameter_test.py (Root lists Width), selector logic
// (IsSelector + selected/selecting features), alias resolution (nodes
// reachable by their standard names), and poll-style re-reads.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice } = require('../helpers');

function freshCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('genicam category and selector', () => {
  it('Root category lists standard features', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const features = cam.getCategoryFeatures('Root');
      assert.ok(Array.isArray(features) && features.length > 0);
      assert.ok(features.includes('Width'));
      assert.throws(() => cam.getCategoryFeatures('Width'), TypeError);
      assert.throws(() => cam.getCategoryFeatures('NoSuchNode12345'), /Unknown node/);
    } finally {
      cam.close();
    }
  });

  it('selector flags and feature lists', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      // Width is a plain value node, never a selector.
      assert.equal(cam.isSelector('Width'), false);
      assert.throws(() => cam.isSelector('NoSuchNode12345'), /Unknown node/);
      // Probe common nodes for a real selector; skip when the emulator
      // exposes none under these names.
      const candidates = ['PixelFormat', 'ExposureTime', 'Gain', 'TriggerSelector', 'ComponentSelector'];
      let selector = null;
      for (const name of candidates) {
        try {
          if (cam.isSelector(name)) {
            selector = name;
            break;
          }
        } catch {
          // Node absent on this device — keep probing.
        }
      }
      if (selector === null) {
        t.skip('no selector node among probed names on this device');
        return;
      }
      const selected = cam.getSelectedFeatures(selector);
      const selecting = cam.getSelectingFeatures(selector);
      assert.ok(Array.isArray(selected));
      assert.ok(Array.isArray(selecting));
    } finally {
      cam.close();
    }
  });

  it('alias and poll: repeated reads stay consistent', (t) => {
    if (!requireDevice(t)) return;
    const cam = freshCamera();
    try {
      const first = cam.getInteger('Width');
      const second = cam.getInteger('Width');
      assert.equal(first, second);
      assert.equal(cam.getValueAsString('Width'), String(first));
    } finally {
      cam.close();
    }
  });
});
