'use strict';
// RN parity: version/discovery/device-info/constants, mirrored in logic
// from test/pylon/emulated/version.test.js, deviceinfo.test.js,
// discovery.test.js (portable parts) and the device-independent constant
// assertions of factory-transport.test.js (device classes, access modes,
// accessibility states, TL types, property keys).
//
// Not ported (documented in RN_COVERAGE_GAPS): the TlFactory/
// TransportLayer/Interface object model (CreateTl/ReleaseTl, property
// dicts, comparisons, filtered enumeration) — the device-independent
// constant values ARE ported via getDiscoveryConstants().
const { describe, it, before } = require('node:test');
const assert = require('node:assert/strict');
const { loadFacade } = require('./rn-stub-core');

let f = null;
let SKIP = null;

before(async () => {
  try {
    ({ facade: f } = await loadFacade());
  } catch (e) {
    if (e && e.code === 'RN_NO_STRIP') SKIP = e.message;
    else throw e;
  }
});

describe('rn-parity version', () => {
  it('reports version strings', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    assert.equal(typeof f.version(), 'string');
    assert.ok(f.version().length > 0);
    assert.match(f.pylonVersion(), /\d+\./);
  });

  it('initialize/terminate are balanced and safe', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    f.initialize();
    f.initialize();
    f.terminate();
    f.terminate();
  });
});

describe('rn-parity device info', () => {
  it('exposes non-empty device fields', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const cam = f.createFirstCamera();
    try {
      const info = cam.getDeviceInfo();
      for (const key of [
        'friendlyName',
        'modelName',
        'serialNumber',
        'vendorName',
        'deviceClass',
      ]) {
        assert.equal(typeof info[key], 'string', key);
        assert.ok(info[key].length > 0, key);
      }
      assert.equal(typeof info.fullName, 'string');
      assert.ok(info.fullName.length > 0);
    } finally {
      cam.close();
    }
  });
});

describe('rn-parity discovery', () => {
  it('enumerates a stable device list', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const a = f.enumerateDevices();
    const b = f.enumerateDevices();
    assert.ok(a.length > 0);
    assert.equal(a.length, b.length);
    assert.deepEqual(
      a.map((d) => d.serialNumber),
      b.map((d) => d.serialNumber),
    );
  });

  it('matches enumeration against the opened camera', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const devs = f.enumerateDevices();
    const cam = f.createCameraAt(0);
    try {
      assert.equal(cam.getDeviceInfo().serialNumber, devs[0].serialNumber);
      assert.equal(cam.getDeviceInfo().modelName, devs[0].modelName);
    } finally {
      cam.close();
    }
  });

  it('deviceClass names the emulator', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const devs = f.enumerateDevices();
    assert.equal(typeof devs[0].deviceClass, 'string');
    assert.ok(devs[0].deviceClass.length > 0);
  });
});

describe('rn-parity discovery constants', () => {
  it('matches the C++ DeviceClass.h string values', (t) => {
    if (SKIP) { t.skip(SKIP); return; }
    const s = f.discoveryStrings();
    assert.equal(s.BaslerCamEmuDeviceClass, 'BaslerCamEmu');
    assert.equal(s.BaslerIpCamDeviceClass, 'BaslerIPCam');
    assert.equal(s.BaslerCameraLinkDeviceClass, 'BaslerCameraLink');
    assert.equal(s.BaslerGenTlGevDeviceClass, 'BaslerGTC/Basler/GEV');
    assert.equal(s.BaslerGenTlU3vDeviceClass, 'BaslerGTC/Basler/U3V');
    assert.equal(s.BaslerGenTlCxpDeviceClass, 'BaslerGTC/Basler/CXP');
    assert.equal(
      s.BaslerGenTlBlazeDeviceClass,
      'BaslerGTC/Basler/GenTL_Producer_for_Basler_blaze_101_cameras',
    );
    assert.equal(s.BaslerGenTlStaDeviceClass, 'BaslerGTC/Basler/basler_xw');
    assert.equal(s.BaslerUsbDeviceClass, 'BaslerUsb');
  });

  it('matches device access mode integers', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const n = f.discoveryIntegers();
    assert.equal(n.DeviceAccessMode_Control, 0x1);
    assert.equal(n.DeviceAccessMode_Stream, 0x3);
    assert.equal(n.DeviceAccessMode_Event, 0x4);
    assert.equal(n.DeviceAccessMode_Exclusive, 0x5);
  });

  it('matches accessibility integers', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const n = f.discoveryIntegers();
    assert.equal(n.Accessibility_Unknown, 0);
    assert.equal(n.Accessibility_Ok, 1);
    assert.equal(n.Accessibility_Opened, 2);
    assert.equal(n.Accessibility_OpenedExclusively, 3);
    assert.equal(n.Accessibility_NotReachable, 4);
  });

  it('exposes transport-layer type strings', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const s = f.discoveryStrings();
    for (const key of [
      'TLTypeGigE',
      'TLTypeUSB',
      'TLTypeCL',
      'TLTypeCamEmu',
      'TLTypeCXP',
      'TLTypeCustom',
    ]) {
      assert.equal(typeof s[key], 'string', key);
      assert.ok(s[key].length > 0, key);
    }
  });

  it('exposes property key strings', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const s = f.discoveryStrings();
    for (const key of [
      'FriendlyNameKey',
      'FullNameKey',
      'VendorNameKey',
      'DeviceClassKey',
      'TLTypeKey',
      'InterfaceIDKey',
      'SerialNumberKey',
      'UserDefinedNameKey',
      'ModelNameKey',
      'DeviceVersionKey',
      'DeviceFactoryKey',
      'XMLSourceKey',
      'FileNameKey',
      'InfoIDKey',
      'VersionKey',
    ]) {
      assert.equal(typeof s[key], 'string', key);
      assert.ok(s[key].length > 0, key);
    }
  });

  it('uses the constants for device filtering', (t) => {

    if (SKIP) { t.skip(SKIP); return; }
    const s = f.discoveryStrings();
    const emu = f
      .enumerateDevices()
      .filter((d) => d.deviceClass === s.BaslerCamEmuDeviceClass);
    assert.equal(emu.length, f.enumerateDevices().length);
  });
});
