'use strict';
// pylon: WaitObject single-object waits (analogous to pypylon's
// waitobject_test.py, adapted to the synchronous N-API surface).
// Timeout semantics are logic-identical: Wait returns bool, WaitEx returns
// a WaitExResult code, Sleep blocks the calling thread.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice } = require('../../helpers');

const SIGNAL_WAIT_TIMEOUT_MS = 5000;
const NEGATIVE_WAIT_TIMEOUT_MS = 250;
const WAIT_EX_NEGATIVE_TIMEOUT_MS = 50;
const WAIT_ZERO_MAX_ELAPSED_MS = 500;
const SLEEP_DURATION_MS = 200;
const SLEEP_MIN_RATIO = 0.8;
const SLEEP_MAX_OVERHEAD_MS = 500;

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('pylon WaitObject', () => {
  it('default construction is not valid', () => {
    const waitObject = new pylon.WaitObject();
    assert.equal(waitObject.isValid(), false);
    assert.equal(waitObject.IsValid(), false);
  });

  it('grab result wait object from a grabbing camera is valid', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabResultWaitObject();
      assert.equal(waitObject.isValid(), true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('grab result wait object signals when an image is ready', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabResultWaitObject();
      assert.equal(waitObject.wait(SIGNAL_WAIT_TIMEOUT_MS), true);
      assert.equal(waitObject.Wait(SIGNAL_WAIT_TIMEOUT_MS), true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('grab stop wait object is valid without grabbing', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      const waitObject = cam.getGrabStopWaitObject();
      assert.equal(waitObject.isValid(), true);
    } finally {
      cam.close();
    }
  });

  it('grab stop wait object is not signaled while grabbing', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      assert.equal(waitObject.wait(NEGATIVE_WAIT_TIMEOUT_MS), false);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('grab stop wait object signals after grabbing stops', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      cam.stopGrabbing();
      assert.equal(waitObject.wait(SIGNAL_WAIT_TIMEOUT_MS), true);
    } finally {
      cam.close();
    }
  });

  it('wait returns false when the timeout elapses', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      assert.equal(waitObject.wait(NEGATIVE_WAIT_TIMEOUT_MS), false);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('wait with zero timeout returns immediately', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      const start = Date.now();
      waitObject.wait(0);
      assert.ok(Date.now() - start < WAIT_ZERO_MAX_ELAPSED_MS);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitEx returns waitex_timeout when the timeout elapses', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      assert.equal(waitObject.waitEx(WAIT_EX_NEGATIVE_TIMEOUT_MS, false), pylon.waitex_timeout);
      assert.equal(waitObject.WaitEx(WAIT_EX_NEGATIVE_TIMEOUT_MS, false), pylon.waitex_timeout);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitEx returns waitex_signaled when the object is signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      cam.stopGrabbing();
      assert.equal(waitObject.waitEx(SIGNAL_WAIT_TIMEOUT_MS, false), pylon.waitex_signaled);
    } finally {
      cam.close();
    }
  });

  it('waitEx with alertable returns timeout or alerted', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      const result = waitObject.waitEx(WAIT_EX_NEGATIVE_TIMEOUT_MS, true);
      assert.ok(result === pylon.waitex_timeout || result === pylon.waitex_alerted);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('sleep suspends the calling thread for approximately the given time', () => {
    const start = Date.now();
    pylon.WaitObject.sleep(SLEEP_DURATION_MS);
    const elapsed = Date.now() - start;
    assert.ok(elapsed >= SLEEP_DURATION_MS * SLEEP_MIN_RATIO);
    assert.ok(elapsed < SLEEP_DURATION_MS + SLEEP_MAX_OVERHEAD_MS);
    pylon.WaitObject.Sleep(10);
  });

  it('waitex result constants exist and are distinct', () => {
    for (const name of ['waitex_timeout', 'waitex_signaled', 'waitex_abandoned', 'waitex_alerted']) {
      assert.equal(typeof pylon[name], 'number');
    }
    const constants = [pylon.waitex_timeout, pylon.waitex_signaled, pylon.waitex_abandoned, pylon.waitex_alerted];
    assert.equal(new Set(constants).size, constants.length);
    assert.equal(pylon.waitex_timeout, 0);
    assert.equal(pylon.waitex_signaled, 1);
    assert.equal(pylon.waitex_alerted, -1);
    assert.equal(pylon.WaitExResult.Timeout, pylon.waitex_timeout);
    assert.equal(pylon.WaitExResult.Signaled, pylon.waitex_signaled);
    assert.equal(pylon.WaitExResult.Abandoned, pylon.waitex_abandoned);
    assert.equal(pylon.WaitExResult.Alerted, pylon.waitex_alerted);
  });

  it('waitForever constant is available', () => {
    assert.equal(pylon.waitForever, 0xffffffff);
  });

  it('wait validates its timeout argument', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      assert.throws(() => waitObject.wait('soon'), TypeError);
      assert.throws(() => waitObject.wait(-1), TypeError);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });
});
