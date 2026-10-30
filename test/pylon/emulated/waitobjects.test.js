'use strict';
// pylon: WaitObjects multi-object waits (analogous to pypylon's
// waitobjects_test.py, adapted to the synchronous N-API surface).
// waitForAny/Ex return a plain bool/result, or a [result, index] pair when
// called with true as the last argument. The pypylon background-thread
// sections run sequentially here: Node timers cannot fire while a
// synchronous wait blocks the loop, so the stop-then-wait order is asserted
// instead (same SDK state transition, same outcome).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice } = require('../../helpers');

const SIGNAL_WAIT_TIMEOUT_MS = 5000;
const NEGATIVE_WAIT_TIMEOUT_MS = 250;

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('pylon WaitObjects', () => {
  it('default construction yields an empty container', () => {
    const container = new pylon.WaitObjects();
    assert.ok(container);
  });

  it('a wait object can be added to the container', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabResultWaitObject();
      const container = new pylon.WaitObjects();
      container.add(waitObject);
      assert.equal(container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS), true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('removeAll clears the container without error', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const waitObject = cam.getGrabStopWaitObject();
      const container = new pylon.WaitObjects();
      container.add(waitObject);
      container.removeAll();
      container.RemoveAll();
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAll returns false when not all objects are signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      container.add(cam.getGrabStopWaitObject());
      assert.equal(container.waitForAll(NEGATIVE_WAIT_TIMEOUT_MS), false);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAll returns true when all objects are signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const stopObject = cam.getGrabStopWaitObject();
      cam.stopGrabbing();
      const container = new pylon.WaitObjects();
      container.add(stopObject);
      assert.equal(container.waitForAll(SIGNAL_WAIT_TIMEOUT_MS), true);
      assert.equal(container.WaitForAll(SIGNAL_WAIT_TIMEOUT_MS), true);
    } finally {
      cam.close();
    }
  });

  it('waitForAllEx returns waitex_timeout when not all are signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      container.add(cam.getGrabStopWaitObject());
      assert.equal(container.waitForAllEx(NEGATIVE_WAIT_TIMEOUT_MS, false), pylon.waitex_timeout);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAllEx returns waitex_signaled when all are signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const stopObject = cam.getGrabStopWaitObject();
      cam.stopGrabbing();
      const container = new pylon.WaitObjects();
      container.add(stopObject);
      assert.equal(container.waitForAllEx(SIGNAL_WAIT_TIMEOUT_MS, false), pylon.waitex_signaled);
    } finally {
      cam.close();
    }
  });

  it('waitForAllEx with alertable returns timeout or alerted', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      const result = container.waitForAllEx(NEGATIVE_WAIT_TIMEOUT_MS, true);
      assert.ok(result === pylon.waitex_timeout || result === pylon.waitex_alerted);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny returns false when nothing is signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      assert.equal(container.waitForAny(NEGATIVE_WAIT_TIMEOUT_MS), false);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny returns true when at least one object is signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      container.add(cam.getGrabStopWaitObject());
      assert.equal(container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS), true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx returns waitex_timeout when nothing is signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      assert.equal(container.waitForAnyEx(NEGATIVE_WAIT_TIMEOUT_MS, false), pylon.waitex_timeout);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx returns waitex_signaled when an object is signaled', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      assert.equal(container.waitForAnyEx(SIGNAL_WAIT_TIMEOUT_MS, false), pylon.waitex_signaled);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx with alertable returns timeout or alerted', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      const result = container.waitForAnyEx(NEGATIVE_WAIT_TIMEOUT_MS, true);
      assert.ok(result === pylon.waitex_timeout || result === pylon.waitex_alerted);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny with null index returns a plain bool', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      const result = container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS, null);
      assert.equal(typeof result, 'boolean');
      assert.equal(result, true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx with null index returns a plain result', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      assert.equal(container.waitForAnyEx(SIGNAL_WAIT_TIMEOUT_MS, false, null), pylon.waitex_signaled);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny with index returns a [signaled, index] pair', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      container.add(cam.getGrabResultWaitObject());
      const result = container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS, true);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(result[0], true);
      assert.equal(result[1], 1);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx with index returns a [result, index] pair', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      container.add(cam.getGrabResultWaitObject());
      const result = container.waitForAnyEx(SIGNAL_WAIT_TIMEOUT_MS, false, true);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
      assert.equal(result[0], pylon.waitex_signaled);
      assert.equal(result[1], 1);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny without index returns a plain bool', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      const result = container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS);
      assert.equal(typeof result, 'boolean');
      assert.equal(result, true);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx without index returns a plain result', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      const result = container.waitForAnyEx(SIGNAL_WAIT_TIMEOUT_MS, false);
      assert.ok(!Array.isArray(result));
      assert.equal(result, pylon.waitex_signaled);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAny rejects an invalid index flag', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      assert.throws(() => container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS, 0), TypeError);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('waitForAnyEx rejects an invalid index flag', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabResultWaitObject());
      assert.throws(() => container.waitForAnyEx(SIGNAL_WAIT_TIMEOUT_MS, false, 0), TypeError);
      cam.stopGrabbing();
    } finally {
      cam.close();
    }
  });

  it('add rejects non-wait objects', () => {
    const container = new pylon.WaitObjects();
    assert.throws(() => container.add({}), TypeError);
    assert.throws(() => container.add(null), TypeError);
  });

  it('waitForAll succeeds after the grab stopped (sequential thread port)', (t) => {
    // Ports test_wait_for_all_from_thread: the stop signal comes from the
    // same thread (a synchronous wait blocks the JS loop, so no timer could
    // fire mid-wait); the SDK state transition asserted is identical.
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      cam.stopGrabbing();
      assert.equal(container.waitForAll(SIGNAL_WAIT_TIMEOUT_MS), true);
    } finally {
      cam.close();
    }
  });

  it('waitForAny succeeds after the grab stopped (sequential thread port)', (t) => {
    // Ports test_wait_for_any_from_thread, same sequential adaptation.
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      const container = new pylon.WaitObjects();
      container.add(cam.getGrabStopWaitObject());
      cam.stopGrabbing();
      assert.equal(container.waitForAny(SIGNAL_WAIT_TIMEOUT_MS), true);
    } finally {
      cam.close();
    }
  });
});
