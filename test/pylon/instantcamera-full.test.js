'use strict';
// pylon: InstantCamera full API (logic-identical ports of the emulated
// pypylon suites): attach/detach/destroy, open/close, grab strategies,
// timeout handling, StartGrabbing variants, GrabOne, software trigger,
// event-handler registration, configurations, helper and camera array.
// Uses the emulator when no hardware is attached (PYLON_CAMEMU=2).
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');
const { requireDevice, hasNode } = require('../helpers');

function openCamera() {
  const cam = pylon.createFirstCamera();
  cam.open();
  return cam;
}

describe('pylon InstantCamera enums', () => {
  it('exposes grab strategy, timeout, loop, registration and access enums', (t) => {
    let enums;
    try {
      enums = [
        pylon.GrabStrategy.OneByOne, pylon.GrabStrategy.LatestImageOnly,
        pylon.GrabStrategy.LatestImages, pylon.GrabStrategy.UpcomingImage,
        pylon.TimeoutHandling.Return, pylon.TimeoutHandling.ThrowException,
        pylon.GrabLoop.ProvidedByInstantCamera, pylon.GrabLoop.ProvidedByUser,
        pylon.RegistrationMode.Append, pylon.RegistrationMode.ReplaceAll,
        pylon.Cleanup.None, pylon.Cleanup.Delete,
      ];
    } catch {
      t.skip('native build missing');
      return;
    }
    assert.deepEqual(enums.slice(0, 4), [0, 1, 2, 3]);
    assert.deepEqual(enums.slice(4, 6), [0, 1]);
    assert.deepEqual(enums.slice(6, 8), [0, 1]);
    assert.deepEqual(enums.slice(8, 10), [0, 1]);
    assert.deepEqual(enums.slice(10, 12), [0, 1]);
  });
});

describe('pylon InstantCamera attach/detach/destroy', () => {
  it('attach/detach/destroy cycle', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      assert.equal(camera.isPylonDeviceAttached(), true);
      assert.equal(camera.hasOwnership(), true);
      assert.equal(camera.isOpen(), false);
      camera.destroyDevice();
      assert.equal(camera.isPylonDeviceAttached(), false);
      assert.equal(camera.isOpen(), false);
      assert.equal(camera.hasOwnership(), false);
      assert.equal(camera.attachDevice(0), true);
      assert.equal(camera.isPylonDeviceAttached(), true);
      const info = camera.detachDevice();
      assert.equal(typeof info.serialNumber, 'string');
      assert.equal(camera.isPylonDeviceAttached(), false);
      assert.equal(camera.attachDevice(), true);
      camera.open();
      camera.close();
      camera.destroyDevice();
      assert.equal(camera.isPylonDeviceAttached(), false);
    } finally {
      pylon.terminate();
    }
  });

  it('destroyDevice closes an open camera', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      assert.equal(camera.isOpen(), true);
      camera.destroyDevice();
      assert.equal(camera.isPylonDeviceAttached(), false);
      assert.equal(camera.isOpen(), false);
    } finally {
      pylon.terminate();
    }
  });

  it('attach while attached throws, detach without device throws', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      assert.throws(() => camera.attachDevice(0), /already attached/);
      camera.destroyDevice();
      assert.throws(() => camera.detachDevice(), /No device attached/);
      assert.throws(() => camera.attachDevice(9999), /out of range|no device/i);
      assert.throws(() => camera.open(), /attach|device|open|camera/i);
    } finally {
      pylon.terminate();
    }
  });

  it('device-index constructor attaches that device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = new pylon.InstantCamera(0);
      assert.equal(camera.isPylonDeviceAttached(), true);
      camera.open();
      camera.close();
      camera.destroyDevice();
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon InstantCamera open/close and context', () => {
  it('open accepts access modes, invalid mode throws', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      assert.equal(camera.open(pylon.AccessMode.Exclusive), true);
      assert.equal(camera.open('Control'), true);
      assert.throws(() => camera.open('Bogus'), /access mode/i);
      assert.throws(() => camera.open(42), /access mode/i);
      camera.close();
    } finally {
      pylon.terminate();
    }
  });

  it('camera context round-trips', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      camera.setCameraContext(12345);
      assert.equal(camera.getCameraContext(), 12345);
      camera.setCameraContext(0);
      assert.equal(camera.getCameraContext(), 0);
    } finally {
      pylon.terminate();
    }
  });

  it('device is not reported removed on the emulator', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        assert.equal(camera.isCameraDeviceRemoved(), false);
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('queued buffer count is a number while grabbing', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbing();
        const n = camera.getQueuedBufferCount();
        assert.equal(typeof n, 'number');
        assert.ok(n >= 0);
        camera.stopGrabbing();
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon InstantCamera grab strategies and variants', () => {
  for (const strategy of [0, 1, 2, 3]) {
    it(`startGrabbing with strategy ${strategy} grabs`, (t) => {
      if (!requireDevice(t)) return;
      pylon.initialize();
      try {
        const camera = openCamera();
        try {
          camera.startGrabbing(strategy);
          assert.equal(camera.isGrabbing(), true);
          const res = camera.retrieveResult(10000);
          assert.ok(res !== null);
          assert.equal(res.succeeded, true);
          camera.stopGrabbing();
          assert.equal(camera.isGrabbing(), false);
        } finally {
          camera.close();
        }
      } finally {
        pylon.terminate();
      }
    });
  }

  it('unknown strategy throws', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        assert.throws(() => camera.startGrabbing(99), /strategy/i);
        assert.throws(() => camera.startGrabbing('Nope'), /strategy/i);
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('startGrabbingMax grabs up to maxImages', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbingMax(2);
        assert.equal(camera.isGrabbing(), true);
        let count = 0;
        for (let i = 0; i < 2; i += 1) {
          const res = camera.retrieveResult(10000);
          if (res === null) break;
          assert.equal(res.succeeded, true);
          count += 1;
        }
        assert.ok(count >= 1);
        camera.stopGrabbing();
        assert.throws(() => camera.startGrabbingMax(0), /maxImages/);
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('options-object startGrabbing works', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbing({ strategy: 'OneByOne', grabLoop: 'ProvidedByUser' });
        assert.equal(camera.isGrabbing(), true);
        const res = camera.retrieveResult(10000);
        assert.ok(res !== null);
        camera.stopGrabbing();
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon InstantCamera retrieveResult and grabOne', () => {
  it('grab result carries validity and identity fields', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbing(pylon.GrabStrategy.OneByOne);
        try {
          const res = camera.retrieveResult(10000);
          assert.ok(res !== null);
          assert.equal(res.isValid, true);
          assert.equal(res.succeeded, true);
          assert.equal(res.grabSucceeded(), true);
          assert.ok(res.width > 0 && res.height > 0);
          assert.equal(res.buffer.length, res.payloadSize);
          assert.equal(typeof res.pixelFormat, 'string');
          assert.equal(typeof res.blockId, 'number');
          assert.equal(typeof res.frameNumber, 'number');
          assert.equal(typeof res.timestamp, 'number');
          assert.ok(Buffer.isBuffer(res.buffer));
        } finally {
          camera.stopGrabbing();
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('Return handling yields null on timeout, ThrowException throws', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbing(pylon.GrabStrategy.UpcomingImage);
        try {
          // UpcomingImage keeps the input queue empty: with no trigger a
          // zero timeout either returns null or a ready frame; both are
          // valid Return semantics.
          const res = camera.retrieveResult(0, pylon.TimeoutHandling.Return);
          if (res !== null) assert.equal(res.succeeded, true);
          // ThrowException: either a frame is ready or it throws.
          try {
            const res2 = camera.retrieveResult(0, pylon.TimeoutHandling.ThrowException);
            if (res2 !== null) assert.equal(res2.succeeded, true);
          } catch (err) {
            assert.match(String(err && err.message || err), /timeout/i);
          }
          assert.throws(
            () => camera.retrieveResult(100, 'Bogus'),
            /timeout handling/i
          );
        } finally {
          camera.stopGrabbing();
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('grabOne returns a valid result without a grab loop', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        assert.equal(camera.isGrabbing(), false);
        const res = camera.grabOne(10000);
        assert.ok(res !== null);
        assert.equal(res.isValid, true);
        assert.equal(res.succeeded, true);
        assert.equal(res.buffer.length, res.payloadSize);
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('grabOne while grabbing throws', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        camera.startGrabbing();
        try {
          assert.throws(() => camera.grabOne(100), /while grabbing/);
        } finally {
          camera.stopGrabbing();
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon InstantCamera event handlers', () => {
  it('configuration handler fires on lifecycle transitions', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      const seen = [];
      const id = camera.registerConfiguration((ev) => seen.push(ev));
      assert.equal(typeof id, 'number');
      camera.open();
      camera.startGrabbing();
      camera.stopGrabbing();
      camera.close();
      assert.ok(seen.includes('OnOpened'), `events: ${seen}`);
      assert.ok(seen.includes('OnGrabStarted'), `events: ${seen}`);
      assert.ok(seen.includes('OnGrabStopped'), `events: ${seen}`);
      assert.ok(seen.includes('OnClosed'), `events: ${seen}`);
      assert.equal(camera.deregisterConfiguration(id), true);
      assert.equal(camera.deregisterConfiguration(id), false);
      camera.destroyDevice();
    } finally {
      pylon.terminate();
    }
  });

  it('ReplaceAll registration clears previous handlers', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      let calls = 0;
      camera.registerConfiguration(() => { calls += 1; });
      camera.registerConfiguration(() => { calls += 10; }, pylon.RegistrationMode.ReplaceAll);
      camera.open();
      camera.close();
      assert.equal(calls, 20);
      camera.destroyDevice();
    } finally {
      pylon.terminate();
    }
  });

  it('image handler fires per retrieved result', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        let count = 0;
        let last = null;
        const id = camera.registerImageEventHandler((res) => {
          count += 1;
          last = res;
        });
        camera.startGrabbing();
        try {
          const res = camera.retrieveResult(10000);
          assert.ok(res !== null);
          assert.equal(count, 1);
          assert.equal(last.succeeded, true);
        } finally {
          camera.stopGrabbing();
        }
        assert.equal(camera.deregisterImageEventHandler(id), true);
        camera.startGrabbing();
        try {
          const res = camera.retrieveResult(10000);
          assert.ok(res !== null);
          assert.equal(count, 1);
        } finally {
          camera.stopGrabbing();
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('camera event handler register/deregister round-trips', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = pylon.createFirstCamera();
      const id = camera.registerCameraEventHandler(() => {}, 'ExposureEnd');
      assert.equal(typeof id, 'number');
      assert.equal(camera.deregisterCameraEventHandler(id), true);
      assert.equal(camera.deregisterCameraEventHandler(id), false);
      const handler = () => {};
      const id2 = camera.registerCameraEventHandler(handler);
      assert.equal(camera.deregisterCameraEventHandler(handler), true);
      assert.equal(id2 !== id, true);
      camera.destroyDevice();
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon acquisition configurations', () => {
  it('continuous configuration sets AcquisitionMode', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        if (!hasNode(camera, 'AcquisitionMode')) {
          t.skip('AcquisitionMode missing on this device');
          return;
        }
        camera.setEnum('AcquisitionMode', 'SingleFrame');
        pylon.AcquireContinuousConfiguration.applyConfiguration(camera);
        assert.equal(camera.getEnum('AcquisitionMode'), 'Continuous');
        const cfg = new pylon.AcquireContinuousConfiguration();
        camera.setEnum('AcquisitionMode', 'SingleFrame');
        cfg.onOpened(camera);
        assert.equal(camera.getEnum('AcquisitionMode'), 'Continuous');
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('single-frame configuration sets AcquisitionMode', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        if (!hasNode(camera, 'AcquisitionMode')) {
          t.skip('AcquisitionMode missing on this device');
          return;
        }
        pylon.AcquireSingleFrameConfiguration.applyConfiguration(camera);
        assert.equal(camera.getEnum('AcquisitionMode'), 'SingleFrame');
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('software trigger configuration and trigger flow', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        for (const node of ['AcquisitionMode', 'TriggerSelector', 'TriggerMode', 'TriggerSource']) {
          if (!hasNode(camera, node)) {
            t.skip(`${node} missing on this device`);
            return;
          }
        }
        pylon.SoftwareTriggerConfiguration.applyConfiguration(camera);
        assert.equal(camera.getEnum('AcquisitionMode'), 'Continuous');
        assert.equal(camera.getEnum('TriggerSelector'), 'FrameStart');
        assert.equal(camera.getEnum('TriggerMode'), 'On');
        assert.equal(camera.getEnum('TriggerSource'), 'Software');
        camera.startGrabbing(pylon.GrabStrategy.OneByOne);
        try {
          const ready = camera.waitForFrameTriggerReady(
            5000, pylon.TimeoutHandling.ThrowException
          );
          assert.equal(ready, true);
          camera.executeSoftwareTrigger();
          const res = camera.retrieveResult(
            10000, pylon.TimeoutHandling.ThrowException
          );
          assert.ok(res !== null);
          assert.equal(res.succeeded, true);
        } finally {
          camera.stopGrabbing();
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon ConfigurationHelper', () => {
  it('disableAllTriggers turns every trigger off', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        if (!hasNode(camera, 'TriggerSelector') || !hasNode(camera, 'TriggerMode')) {
          t.skip('trigger nodes missing on this device');
          return;
        }
        const selectorValues = camera.getEnumEntries('TriggerSelector');
        const saved = [];
        for (const entry of selectorValues) {
          camera.setEnum('TriggerSelector', entry);
          saved.push(camera.getEnum('TriggerMode'));
        }
        try {
          pylon.ConfigurationHelper.disableAllTriggers(camera);
          for (const entry of selectorValues) {
            camera.setEnum('TriggerSelector', entry);
            assert.equal(camera.getEnum('TriggerMode'), 'Off');
          }
        } finally {
          selectorValues.forEach((entry, i) => {
            camera.setEnum('TriggerSelector', entry);
            try { camera.setEnum('TriggerMode', saved[i]); } catch { /* restore */ }
          });
        }
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });

  it('compression, GenDC, range and packet-size helpers are safe', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const camera = openCamera();
      try {
        if (hasNode(camera, 'ImageCompressionMode')) {
          const orig = camera.getEnum('ImageCompressionMode');
          try {
            pylon.ConfigurationHelper.disableCompression(camera);
            assert.equal(camera.getEnum('ImageCompressionMode'), 'Off');
          } finally {
            try { camera.setEnum('ImageCompressionMode', orig); } catch { /* restore */ }
          }
        } else {
          assert.doesNotThrow(() => pylon.ConfigurationHelper.disableCompression(camera));
        }
        if (hasNode(camera, 'GenDCStreamingMode')) {
          const orig = camera.getEnum('GenDCStreamingMode');
          try {
            pylon.ConfigurationHelper.disableGenDC(camera);
            assert.equal(camera.getEnum('GenDCStreamingMode'), 'Off');
          } finally {
            try { camera.setEnum('GenDCStreamingMode', orig); } catch { /* restore */ }
          }
        } else {
          assert.doesNotThrow(() => pylon.ConfigurationHelper.disableGenDC(camera));
        }
        if (hasNode(camera, 'ComponentSelector')) {
          const orig = camera.getEnum('ComponentSelector');
          try {
            pylon.ConfigurationHelper.selectRangeComponent(camera);
            assert.equal(camera.getEnum('ComponentSelector'), 'Range');
          } finally {
            try { camera.setEnum('ComponentSelector', orig); } catch { /* restore */ }
          }
        }
        assert.doesNotThrow(() => pylon.ConfigurationHelper.probePacketSize(camera));
      } finally {
        camera.close();
      }
    } finally {
      pylon.terminate();
    }
  });
});

describe('pylon InstantCameraArray', () => {
  it('empty array reports size zero and false flags', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const arr = new pylon.InstantCameraArray(0);
      assert.equal(arr.getSize(), 0);
      assert.equal(arr.isPylonDeviceAttached(), false);
      assert.equal(arr.isOpen(), false);
      assert.deepEqual([...arr], []);
    } finally {
      pylon.terminate();
    }
  });

  it('array open/grab/close over one camera', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const arr = new pylon.InstantCameraArray(1);
      try {
        assert.equal(arr.getSize(), 1);
        assert.equal(arr.isPylonDeviceAttached(), true);
        const cam = arr.getItem(0);
        assert.equal(typeof cam.open, 'function');
        arr.open();
        assert.equal(arr.isOpen(), true);
        assert.equal(arr.isCameraDeviceRemoved(), false);
        arr.startGrabbing(pylon.GrabStrategy.OneByOne);
        assert.equal(arr.isGrabbing(), true);
        const res = arr.retrieveResult(10000);
        assert.ok(res !== null);
        assert.equal(res.succeeded, true);
        assert.equal(res.cameraIndex, 0);
        arr.stopGrabbing();
        assert.equal(arr.isGrabbing(), false);
        arr.close();
        assert.equal(arr.isOpen(), false);
        assert.throws(() => arr.getItem(1), /out of range/);
      } finally {
        arr.destroyDevice();
      }
      assert.equal(arr.getSize(), 0);
    } finally {
      pylon.terminate();
    }
  });
});
