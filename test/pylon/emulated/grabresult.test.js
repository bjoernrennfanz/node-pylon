'use strict';
// pylon: GrabResult 1:1 semantics (ports pypylon emulated grabresult logic).
// Covers IsValid/IsUnique/Release, GetBuffer/GetArray/GetMemoryView,
// width/height/pixel type/payload size/timestamp/chunk data/error codes and
// GenDC component awareness against the SDK emulator.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice, pickInt } = require('../../helpers');

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('grab result', () => {
  it('exposes validity and release semantics', (t) => {
    if (!requireDevice(t)) return;
    const empty = new pylon.GrabResult();
    assert.equal(empty.isValid(), false);
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        assert.ok(grab instanceof pylon.GrabResult);
        assert.equal(grab.isValid(), true);
        assert.equal(grab.succeeded(), true);
        assert.equal(grab.grabSucceeded(), true);
        // Fresh result is uniquely owned before buffer views are taken.
        // (Buffer views share ownership, so check uniqueness first.)
        assert.equal(typeof grab.isUnique(), 'boolean');
        grab.release();
        assert.equal(grab.isValid(), false);
        assert.equal(grab.succeeded(), false);
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('is unique before shared buffer views exist', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        // No buffer view taken yet on this fresh wrapper: unique owner.
        assert.equal(grab.isUnique(), true);
        const buf = grab.getBuffer();
        assert.ok(Buffer.isBuffer(buf));
        // Plain fields stay consistent with the accessors.
        assert.equal(grab.width, grab.getWidth());
        assert.equal(grab.height, grab.getHeight());
        assert.equal(grab.pixelType, grab.getPixelType());
        assert.equal(grab.succeeded(), true);
        assert.equal(grab.valid, true);
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('reports geometry, pixel type and payload size', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = pickInt(cam.getIntegerMin('Width'), cam.getIntegerMax('Width'), 64);
      const h = pickInt(cam.getIntegerMin('Height'), cam.getIntegerMax('Height'), 48);
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        assert.equal(grab.getWidth(), w);
        assert.equal(grab.getHeight(), h);
        assert.equal(grab.getPixelType(), cam.getEnum('PixelFormat'));
        assert.ok(grab.getPixelTypeValue() !== pylon.PixelType.Undefined);
        assert.equal(grab.getPayloadType(), 'Image');
        assert.ok(grab.getPayloadSize() > 0);
        assert.ok(grab.getBufferSize() >= grab.getPayloadSize());
        assert.ok(grab.getImageSize() > 0);
        // pypylon documents: CamEmu reports 0 timestamp for all frames
        // (and 0 as placeholder BlockID since pylon 12.4), so identifiers
        // are asserted non-decreasing, not positive, like upstream.
        assert.ok(grab.getTimestamp() >= 0);
        assert.ok(grab.getBlockId() >= 0);
        assert.ok(grab.getImageNumber() >= 0);
        const second = cam.retrieveGrabResult(10000);
        assert.ok(second !== null);
        assert.ok(second.getTimestamp() >= grab.getTimestamp());
        assert.ok(second.getBlockId() >= grab.getBlockId());
        assert.ok(second.getImageNumber() >= grab.getImageNumber());
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });

  it('shares one zero-copy buffer across views', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const buf = grab.getBuffer();
        const arr = grab.getArray();
        const view = grab.getMemoryView();
        assert.ok(Buffer.isBuffer(buf));
        assert.ok(arr instanceof Uint8Array);
        assert.ok(view instanceof DataView);
        assert.equal(arr.byteLength, buf.length);
        assert.equal(view.byteLength, buf.length);
        assert.equal(buf.length, grab.getBufferSize());
        // Same memory: mutating through one view is visible in the other.
        const first = buf[0];
        buf[0] = (first + 1) & 0xff;
        assert.equal(arr[0], buf[0]);
        assert.equal(view.getUint8(0), buf[0]);
        buf[0] = first;
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('reports chunk data and crc state', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        assert.equal(typeof grab.isChunkDataAvailable(), 'boolean');
        assert.equal(typeof grab.hasCrc(), 'boolean');
        assert.equal(typeof grab.checkCrc(), 'boolean');
        // Stride is a number when the SDK reports it, else null.
        const stride = grab.getStride();
        assert.ok(stride === null || (typeof stride === 'number' && stride > 0));
        assert.equal(typeof grab.getErrorCode(), 'number');
        assert.equal(typeof grab.getErrorDescription(), 'string');
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('exposes GenDC component awareness', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const count = grab.getDataComponentCount();
        assert.ok(Number.isInteger(count) && count >= 0);
        if (count === 0) {
          // Plain image payload: no components, indexed access throws.
          assert.throws(() => grab.getDataComponent(0));
          assert.throws(() => grab.getDataComponentBuffer(0));
        } else {
          const comp = grab.getDataComponent(0);
          assert.equal(typeof comp.componentTypeName, 'string');
          assert.ok(comp.dataSize > 0);
          const cbuf = grab.getDataComponentBuffer(0);
          assert.ok(Buffer.isBuffer(cbuf));
          assert.equal(cbuf.length, comp.dataSize);
        }
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('throws when grabbing without a grab loop', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      assert.throws(() => cam.retrieveGrabResult(100));
    } finally {
      cam.close();
    }
  });

  it('carries matching fields on retrieveResult', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const res = cam.retrieveResult(10000);
        assert.ok(res !== null);
        assert.equal(res.isValid, true);
        assert.equal(typeof res.grabSucceeded, 'function');
        assert.equal(res.grabSucceeded(), true);
        assert.ok(typeof res.blockId === 'number');
        assert.ok(typeof res.timestamp === 'number' && res.timestamp >= 0);
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });
});
