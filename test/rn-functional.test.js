'use strict';
// RN functional tests: exercise the real TS facade (react-native/src/index.ts)
// against an emulator-like in-test core stub (CamEmu semantics: 2 devices,
// typed nodemap, synthetic Mono8 frames). No camera, SDK or compiler needed.
//
// The stub stands in for the JSI/TurboModule layer only (it is test harness,
// not shipped API): it implements the exact codegen-spec surface and mirrors
// pylon error semantics. The facade under test is the unmodified shipped
// file, loaded via Node type stripping.
const { test, before } = require('node:test');
const assert = require('node:assert/strict');
const { loadFacade } = require('./rn-stub-core');

// --- Load the shipped facade with the stubbed TurboModule -----------------

let facade = null;
let mock = null;
let SKIP = null;

before(async () => {
  try {
    ({ facade, mock } = await loadFacade());
  } catch (e) {
    if (e && e.code === 'RN_NO_STRIP') SKIP = e.message;
    else throw e;
  }
});

async function F(t) {
  if (SKIP) t.skip(SKIP);
  if (!facade) ({ facade, mock } = await loadFacade());
  return facade;
}


test('rn-functional: version + discovery (emulator: 2 devices)', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  assert.equal(f.version(), '0.2.0-core');
  assert.equal(f.pylonVersion(), '12.5.2.1565');
  const devs = f.enumerateDevices();
  assert.equal(devs.length, 2);
  assert.equal(devs[0].modelName, 'Emulator');
  assert.equal(devs[0].serialNumber, 'EMU000001');
  assert.equal(devs[1].serialNumber, 'EMU000002');
});

test('rn-functional: pixel-type helpers', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  assert.equal(f.getPixelTypeName(0x01080001), 'Mono8');
  assert.equal(f.isMonoPixelType(0x01080001), true);
  assert.equal(f.isColorPixelType(0x01080001), false);
  assert.equal(f.isColorPixelType(0x02180014), true);
});

test('rn-functional: camera lifecycle open/close', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createFirstCamera();
  assert.equal(cam.isOpen(), true);
  cam.close();
  assert.equal(cam.isOpen(), false);
  assert.throws(() => f.createCameraAt(99), /device index out of range/);
});

test('rn-functional: grabOne returns Mono8 image with buffer', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(0);
  try {
    const res = cam.grabOne(5000);
    assert.equal(res.succeeded, true);
    assert.equal(res.width, 640);
    assert.equal(res.height, 480);
    assert.equal(res.pixelType, 'Mono8');
    assert.ok(res.buffer instanceof Uint8Array);
    assert.equal(res.buffer.length, 640 * 480);
    assert.equal(res.buffer[0], 0);
    assert.equal(res.buffer[47], 47);
  } finally {
    cam.close();
  }
});

test('rn-functional: grab loop start/retrieve/stop', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(1);
  try {
    assert.equal(cam.isGrabbing(), false);
    cam.startGrabbing(f.GrabStrategy.OneByOne);
    assert.equal(cam.isGrabbing(), true);
    const a = cam.retrieveResult(1000);
    const b = cam.retrieveResult(1000);
    assert.equal(a.succeeded, true);
    assert.equal(b.succeeded, true);
    assert.ok(b.blockId > a.blockId);
    cam.stopGrabbing();
    assert.equal(cam.isGrabbing(), false);
  } finally {
    cam.close();
  }
});

test('rn-functional: nodemap typed roundtrips', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(0);
  try {
    cam.setInteger('Width', 320);
    assert.equal(cam.getInteger('Width'), 320);
    cam.setFloat('ExposureTime', 7500.5);
    assert.equal(cam.getFloat('ExposureTime'), 7500.5);
    cam.setBoolean('ChunkModeActive', true);
    assert.equal(cam.getBoolean('ChunkModeActive'), true);
    cam.setString('DeviceUserID', 'rn-test');
    assert.equal(cam.getString('DeviceUserID'), 'rn-test');
    cam.setEnum('AcquisitionMode', 'SingleFrame');
    assert.equal(cam.getEnum('AcquisitionMode'), 'SingleFrame');
    assert.deepEqual(cam.getEnumEntries('AcquisitionMode'), [
      'Continuous',
      'SingleFrame',
    ]);
    cam.executeCommand('AcquisitionStart');
    assert.equal(cam.getNodeType('ExposureTime'), 'Float');
    assert.equal(cam.isReadable('Width'), true);
    assert.equal(cam.isWritable('Width'), true);
    assert.equal(cam.isAvailable('Width'), true);
  } finally {
    cam.close();
  }
});

test('rn-functional: nodemap errors translate to JS exceptions', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(0);
  try {
    assert.throws(() => cam.getInteger('NoSuchNode'), /unknown node/);
    assert.throws(() => cam.getInteger('ExposureTime'), /not an Integer/);
    assert.throws(() => cam.setEnum('AcquisitionMode', 'Bogus'), /invalid entry/);
    assert.throws(() => cam.getFloat('Width'), /not a Float/);
  } finally {
    cam.close();
  }
});

test('rn-functional: timeout Return yields invalid result, bad handle throws', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(0);
  try {
    const res = cam.grabOne(0, f.TimeoutHandling.Return);
    assert.equal(res.succeeded, false);
    assert.ok(res.buffer instanceof Uint8Array);
    const closed = f.createCameraAt(0);
    closed.close();
    assert.throws(() => closed.grabOne(100), /not open/);
  } finally {
    cam.close();
  }
});

test('rn-functional: base64 fallback decodes to identical bytes', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  mock.state.returnBase64 = true;
  try {
    const cam = f.createCameraAt(0);
    try {
      const res = cam.grabOne(5000);
      assert.equal(res.succeeded, true);
      assert.ok(res.buffer instanceof Uint8Array);
      assert.equal(res.buffer.length, 640 * 480);
      assert.equal(res.buffer[0], 0);
      assert.equal(res.buffer[47], 47);
    } finally {
      cam.close();
    }
  } finally {
    mock.state.returnBase64 = false;
  }
});

test('rn-functional: acquisition configurations apply to nodemap', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  const cam = f.createCameraAt(0);
  try {
    f.AcquireContinuousConfiguration.applyConfiguration(cam);
    assert.equal(cam.getEnum('AcquisitionMode'), 'Continuous');
    f.AcquireSingleFrameConfiguration.applyConfiguration(cam);
    assert.equal(cam.getEnum('AcquisitionMode'), 'SingleFrame');
    f.SoftwareTriggerConfiguration.applyConfiguration(cam);
    assert.equal(cam.getEnum('TriggerSelector'), 'FrameStart');
    assert.equal(cam.getEnum('TriggerMode'), 'On');
    assert.equal(cam.getEnum('TriggerSource'), 'Software');
    f.ConfigurationHelper.disableAllTriggers(cam);
    assert.equal(cam.getEnum('TriggerSelector'), 'FrameStart');
    assert.equal(cam.getEnum('TriggerMode'), 'Off');
    f.ConfigurationHelper.disableCompression(cam);
    f.ConfigurationHelper.disableGenDC(cam);
  } finally {
    cam.close();
  }
});

test('rn-functional: coverage gaps are documented, not silent', async (t) => {
  if (SKIP) { t.skip(SKIP); return; }
  const f = await F(t);
  assert.ok(Array.isArray(f.RN_COVERAGE_GAPS));
  assert.ok(f.RN_COVERAGE_GAPS.length > 0);
  for (const gap of ['WaitObject', 'FeaturePersistence', 'event']) {
    assert.ok(
      f.RN_COVERAGE_GAPS.some((g) => g.includes(gap)),
      `gap missing: ${gap}`,
    );
  }
});
