'use strict';
// Ported 1:1 in logic from the pypylon emulated factory/transport/info suites:
//   tests/pylon/emulated/tlfactory_test.py (27 methods)
//   tests/pylon/emulated/devicefactory_test.py (5)
//   tests/pylon/emulated/deviceclass_test.py (9)
//   tests/pylon/emulated/device_test.py (4)
//   tests/pylon/emulated/deviceinfo_test.py (55)
//   tests/pylon/emulated/info_test.py (26)
//   tests/pylon/emulated/tlinfo_test.py (9)
//   tests/pylon/emulated/transportlayer_test.py (24)
//   tests/pylon/emulated/interface_test.py (22)
//   tests/pylon/emulated/interfaceinfo_test.py (5)
//   tests/pylon/emulated/container_test.py (3)
//
// Adaptation notes (logic-identical, JS-shaped):
// - pypylon tuples map to JS arrays (device lists, GetPropertyNames pairs,
//   IsDeviceAccessibleInfo results); assertions use Array.isArray/length.
// - Comparison operators (==, <) map to equals()/lessThan() methods.
// - isinstance/issubclass map to instanceof and prototype-chain checks;
//   the TlFactory facade object is behavioral (methods exist, identity
//   stable) while native TransportLayer/Interface instances carry the real
//   IDeviceFactory prototype chain.
// - `with` context managers (TransportLayer/Interface/NodeMap) map to
//   try/finally with CreateTl/ReleaseTl, createInterface/destroyInterface
//   and open/close; the exit path destroys exactly like __exit__.
// - The Python dict interface (__getitem__/__setitem__/in/iter/keys/values/
//   items/to_dict/update) maps to GetPropertyValue/keys/values/items/toDict/
//   update methods; missing keys report [false, sentinel] instead of
//   raising KeyError.
// - Device-dependent tests skip without a camera/emulator (requireDevice),
//   like the pypylon suites without PYLON_CAMEMU; device-independent
//   assertions (constants, empty-info semantics) always run.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../../lib');
const { requireDevice, deviceCount } = require('../../helpers');

function deviceFilter() {
  return [{ [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass }];
}

function withTransportLayer(fn) {
  const factory = pylon.TlFactory.GetInstance();
  const tl = factory.CreateTl(pylon.BaslerCamEmuDeviceClass);
  assert.ok(tl instanceof pylon.TransportLayer);
  try {
    return fn(tl);
  } finally {
    factory.ReleaseTl(tl);
  }
}

// ---------------------------------------------------------------------------
// deviceclass_test.py (9)
// ---------------------------------------------------------------------------

describe('pylon device classes', () => {
  it('matches the C++ DeviceClass.h string values', () => {
    assert.equal(pylon.BaslerCamEmuDeviceClass, 'BaslerCamEmu');
    assert.equal(pylon.BaslerIpCamDeviceClass, 'BaslerIPCam');
    assert.equal(pylon.BaslerCameraLinkDeviceClass, 'BaslerCameraLink');
    assert.equal(pylon.BaslerGenTlGevDeviceClass, 'BaslerGTC/Basler/GEV');
    assert.equal(pylon.BaslerGenTlU3vDeviceClass, 'BaslerGTC/Basler/U3V');
    assert.equal(pylon.BaslerGenTlCxpDeviceClass, 'BaslerGTC/Basler/CXP');
    assert.equal(pylon.BaslerGenTlBlazeDeviceClass,
      'BaslerGTC/Basler/GenTL_Producer_for_Basler_blaze_101_cameras');
    assert.equal(pylon.BaslerGenTlStaDeviceClass, 'BaslerGTC/Basler/basler_xw');
    assert.equal(pylon.BaslerUsbDeviceClass, 'BaslerUsb');
  });
});

// ---------------------------------------------------------------------------
// devicefactory_test.py (5)
// ---------------------------------------------------------------------------

describe('pylon device factory inheritance and enums', () => {
  it('TransportLayer derives from IDeviceFactory', () => {
    assert.ok(pylon.TransportLayer.prototype instanceof pylon.IDeviceFactory);
  });

  it('Interface derives from IDeviceFactory', () => {
    assert.ok(pylon.Interface.prototype instanceof pylon.IDeviceFactory);
  });

  it('TlFactory exposes the IDeviceFactory surface', () => {
    const factory = pylon.TlFactory.GetInstance();
    for (const method of ['EnumerateDevices', 'CreateDevice',
      'CreateFirstDevice', 'DestroyDevice', 'IsDeviceAccessible']) {
      assert.equal(typeof factory[method], 'function', method);
    }
  });

  it('device access mode enum values match the C++ integers', () => {
    assert.equal(pylon.DeviceAccessMode_Control, 0x1);
    assert.equal(pylon.DeviceAccessMode_Stream, 0x3);
    assert.equal(pylon.DeviceAccessMode_Event, 0x4);
    assert.equal(pylon.DeviceAccessMode_Exclusive, 0x5);
  });

  it('device accessibility enum values match the C++ integers', () => {
    assert.equal(pylon.Accessibility_Unknown, 0);
    assert.equal(pylon.Accessibility_Ok, 1);
    assert.equal(pylon.Accessibility_Opened, 2);
    assert.equal(pylon.Accessibility_OpenedExclusively, 3);
    assert.equal(pylon.Accessibility_NotReachable, 4);
  });
});

// ---------------------------------------------------------------------------
// info_test.py (26): CInfoBase contract on every concrete subclass
// ---------------------------------------------------------------------------

const INFO_CLASSES = ['DeviceInfo', 'TlInfo', 'InterfaceInfo'];

describe('pylon info base contract', () => {
  it('constructs an empty instance of every info class', () => {
    for (const name of INFO_CLASSES) {
      assert.ok(new pylon[name]() !== null, name);
    }
  });

  it('equal property sets compare equal on every info class', () => {
    for (const name of INFO_CLASSES) {
      const a = new pylon[name]();
      a.FriendlyName = 'F';
      a.VendorName = 'V';
      const b = new pylon[name]();
      b.FriendlyName = 'F';
      b.VendorName = 'V';
      assert.ok(a.equals(b), name);
    }
  });

  it('differing property sets compare unequal on every info class', () => {
    for (const name of INFO_CLASSES) {
      const a = new pylon[name]();
      a.FriendlyName = 'F';
      const b = new pylon[name]();
      b.FriendlyName = 'Other';
      assert.equal(a.equals(b), false, name);
    }
  });

  it('orders USB < GigE < CameraLink < GenTL < unknown < CamEmu', () => {
    const ordered = ['BaslerUsb', 'BaslerGigE', 'BaslerCameraLink',
      'BaslerGenTlConsumer', 'SomethingUnknown', 'BaslerCamEmu'];
    for (const name of INFO_CLASSES) {
      for (let i = 0; i < ordered.length - 1; i += 1) {
        const low = new pylon[name]();
        low.DeviceClass = ordered[i];
        const high = new pylon[name]();
        high.DeviceClass = ordered[i + 1];
        assert.equal(low.lessThan(high), true, `${name} ${ordered[i]}`);
        assert.equal(high.lessThan(low), false, `${name} ${ordered[i]}`);
      }
    }
  });

  it('same device class is not less in either direction', () => {
    for (const name of INFO_CLASSES) {
      const left = new pylon[name]();
      left.DeviceClass = 'BaslerUsb';
      const right = new pylon[name]();
      right.DeviceClass = 'BaslerUsb';
      assert.equal(left.lessThan(right), false, name);
      assert.equal(right.lessThan(left), false, name);
    }
  });

  it('CInfoBase properties round-trip on every info class', () => {
    const cases = [
      ['FriendlyName', 'Test Friendly'],
      ['FullName', 'Test Full Name'],
      ['VendorName', 'Test Vendor'],
      ['DeviceClass', 'BaslerUsb'],
      ['TLType', 'U3V'],
    ];
    for (const name of INFO_CLASSES) {
      for (const [prop, value] of cases) {
        const info = new pylon[name]();
        assert.equal(info[`Is${prop}Available`](), false, `${name}.${prop}`);
        assert.equal(info[prop], pylon[name].GetPropertyNotAvailable(), `${name}.${prop}`);
        info[prop] = value;
        assert.equal(info[`Is${prop}Available`](), true, `${name}.${prop}`);
        assert.equal(info[prop], value, `${name}.${prop}`);
        info[`Set${prop}`](`${value}_updated`);
        assert.equal(info[`Get${prop}`](), `${value}_updated`, `${name}.${prop}`);
      }
    }
  });

  it('GetPropertyNotAvailable sentinel is shared across subclasses', () => {
    const sentinel = pylon.DeviceInfo.GetPropertyNotAvailable();
    assert.equal(typeof sentinel, 'string');
    for (const name of INFO_CLASSES) {
      assert.equal(pylon[name].GetPropertyNotAvailable(), sentinel, name);
      assert.equal(new pylon[name]().GetPropertyNotAvailable(), sentinel, name);
    }
  });

  it('GetPropertyNames/keys list set properties', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.FriendlyName = 'F';
      info.VendorName = 'V';
      const [count, names] = info.GetPropertyNames();
      assert.equal(count, names.length, name);
      assert.ok(names.includes('FriendlyName'), name);
      assert.ok(names.includes('VendorName'), name);
      assert.deepEqual(info.keys(), names, name);
    }
  });

  it('GetPropertyAvailable distinguishes set and unknown keys', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.FriendlyName = 'F';
      assert.equal(info.GetPropertyAvailable('FriendlyName'), true, name);
      assert.equal(info.GetPropertyAvailable('NonExistentKey'), false, name);
    }
  });

  it('GetPropertyValue reports found state and value', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.VendorName = 'Basler';
      const [ok, value] = info.GetPropertyValue('VendorName');
      assert.equal(ok, true, name);
      assert.equal(String(value), 'Basler', name);
      const [missing] = info.GetPropertyValue('NonExistentKey');
      assert.equal(missing, false, name);
    }
  });

  it('SetPropertyValue is visible through the typed getter', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.SetPropertyValue('FriendlyName', 'FromGeneric');
      assert.equal(info.FriendlyName, 'FromGeneric', name);
    }
  });

  it('IsSubset matches and mismatches', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.FriendlyName = 'F';
      info.VendorName = 'V';
      const subset = new pylon[name]();
      subset.FriendlyName = 'F';
      assert.equal(info.IsSubset(subset), true, name);
      const other = new pylon[name]();
      other.FriendlyName = 'Other';
      assert.equal(info.IsSubset(other), false, name);
    }
  });

  it('IsUserProvided is true for constructed instances', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      assert.equal(typeof info.IsUserProvided(), 'boolean', name);
      assert.equal(info.IsUserProvided(), true, name);
    }
  });

  it('dict-style keys/values/items/toDict/update stay consistent', () => {
    for (const name of INFO_CLASSES) {
      const info = new pylon[name]();
      info.FriendlyName = 'F';
      info.VendorName = 'V';
      assert.ok(info.keys().includes('FriendlyName'), name);
      assert.ok(info.keys().includes('VendorName'), name);
      assert.deepEqual(info.items(), info.keys().map((k, i) => [k, info.values()[i]]), name);
      const dict = info.toDict();
      assert.equal(dict.FriendlyName, 'F', name);
      assert.equal(dict.VendorName, 'V', name);
      info.update({ FriendlyName: 'U', VendorName: 'W' });
      assert.equal(info.FriendlyName, 'U', name);
      assert.equal(info.VendorName, 'W', name);
    }
  });
});

// ---------------------------------------------------------------------------
// deviceinfo_test.py (55)
// ---------------------------------------------------------------------------

describe('pylon DeviceInfo', () => {
  it('constructs and copies', () => {
    const info = new pylon.DeviceInfo();
    assert.ok(info !== null);
    info.ModelName = 'TestModel';
    info.SerialNumber = '12345678';
    const copy = new pylon.DeviceInfo(info);
    assert.ok(copy.equals(info));
    assert.equal(copy.ModelName, 'TestModel');
    assert.equal(copy.SerialNumber, '12345678');
  });

  it('compares equal and orders by numeric serial', () => {
    const a = new pylon.DeviceInfo();
    a.ModelName = 'TestModel';
    assert.ok(a.equals(new pylon.DeviceInfo(a)));
    const low = new pylon.DeviceInfo();
    low.DeviceClass = 'BaslerUsb';
    low.SerialNumber = '1';
    const high = new pylon.DeviceInfo();
    high.DeviceClass = 'BaslerUsb';
    high.SerialNumber = '2';
    assert.equal(low.lessThan(high), true);
    assert.equal(high.lessThan(low), false);
  });

  it('round-trips the general properties', () => {
    const cases = [
      ['SerialNumber', '12345678', '87654321', '99999999', pylon.SerialNumberKey],
      ['UserDefinedName', 'MyCamera', 'UpdatedCamera', 'DictCamera', pylon.UserDefinedNameKey],
      ['ModelName', 'TestModel', 'UpdatedModel', 'DictModel', pylon.ModelNameKey],
      ['DeviceVersion', '1.0.0', '2.0.0', '3.0.0', pylon.DeviceVersionKey],
      ['DeviceFactory', 'BaslerGigE', 'BaslerUsb', 'BaslerCameraLink', pylon.DeviceFactoryKey],
      ['XMLSource', 'device://0', 'file://model.xml', 'device://1', pylon.XMLSourceKey],
      ['InterfaceID', 'eth0', 'eth1', 'eth2', pylon.InterfaceIDKey],
    ];
    for (const [prop, first, second, third, key] of cases) {
      const info = new pylon.DeviceInfo();
      assert.equal(info[`Is${prop}Available`](), false, prop);
      assert.equal(info[prop], pylon.DeviceInfo.GetPropertyNotAvailable(), prop);
      info[prop] = first;
      assert.equal(info[`Is${prop}Available`](), true, prop);
      assert.equal(info[prop], first, prop);
      info[`Set${prop}`](second);
      assert.equal(info[`Get${prop}`](), second, prop);
      info.SetPropertyValue(key, third);
      assert.equal(info[`Get${prop}`](), third, prop);
    }
  });

  it('round-trips the GigE properties', () => {
    const cases = [
      ['Address', '192.168.0.1:3956'],
      ['IpAddress', '192.168.0.1'],
      ['SubnetAddress', '192.168.0.0'],
      ['DefaultGateway', '192.168.0.254'],
      ['SubnetMask', '255.255.255.0'],
      ['PortNr', '3956'],
      ['MacAddress', '00:11:22:33:44:55'],
      ['Interface', '192.168.0.100'],
      ['IpConfigOptions', '6'],
      ['IpConfigCurrent', '4'],
    ];
    for (const [prop, value] of cases) {
      const info = new pylon.DeviceInfo();
      assert.equal(info[`Is${prop}Available`](), false, prop);
      info[prop] = value;
      assert.equal(info[`Is${prop}Available`](), true, prop);
      assert.equal(info[`Get${prop}`](), value, prop);
    }
  });

  it('GigE IP-config predicates throw without config', () => {
    const info = new pylon.DeviceInfo();
    for (const method of ['IsPersistentIpActive', 'IsDhcpActive',
      'IsAutoIpActive', 'IsPersistentIpSupported', 'IsDhcpSupported',
      'IsAutoIpSupported']) {
      assert.throws(() => info[method](), pylon.GenericException, method);
    }
  });

  it('USB read-only properties report unavailable with the sentinel', () => {
    for (const prop of ['DeviceGUID', 'ManufacturerInfo', 'DeviceIdx',
      'ProductId', 'VendorId', 'DriverKeyName', 'UsbDriverType',
      'TransferMode']) {
      const info = new pylon.DeviceInfo();
      assert.equal(info[`Is${prop}Available`](), false, prop);
      assert.equal(info[`Get${prop}`](), pylon.DeviceInfo.GetPropertyNotAvailable(), prop);
    }
  });

  it('round-trips the CameraLink properties', () => {
    const cases = [
      ['PortID', 'port-0'],
      ['DeviceID', 'device-0'],
      ['InitialBaudRate', '9600'],
      ['DeviceXMLFileOverride', 'override.xml'],
      ['DeviceSpecificString', 'specific'],
      ['PortSpecificString', 'port-specific'],
    ];
    for (const [prop, value] of cases) {
      const info = new pylon.DeviceInfo();
      info[`Set${prop}`](value);
      assert.equal(info[`Get${prop}`](), value, prop);
      assert.equal(info[`Is${prop}Available`](), true, prop);
    }
  });

  it('exposes the IProperties and dict surfaces', () => {
    const info = new pylon.DeviceInfo();
    info.ModelName = 'TestModel';
    assert.ok(info.keys().includes('ModelName'));
    assert.equal(info.GetPropertyAvailable('ModelName'), true);
    assert.equal(info.GetPropertyAvailable('NonExistentKey'), false);
    const [ok, value] = info.GetPropertyValue('ModelName');
    assert.equal(ok, true);
    assert.equal(String(value), 'TestModel');
    const [missing] = info.GetPropertyValue('NonExistentKey');
    assert.equal(missing, false);
    info.SetPropertyValue('ModelName', 'FromGeneric');
    assert.equal(info.ModelName, 'FromGeneric');
    const subset = new pylon.DeviceInfo();
    subset.ModelName = 'FromGeneric';
    assert.equal(info.IsSubset(subset), true);
    subset.ModelName = 'OtherModel';
    assert.equal(info.IsSubset(subset), false);
    assert.equal(info.IsUserProvided(), true);
    assert.equal(info.toDict().ModelName, 'FromGeneric');
    info.update({ ModelName: 'UpdatedModel', DeviceClass: 'BaslerUsb' });
    assert.equal(info.ModelName, 'UpdatedModel');
    assert.equal(info.DeviceClass, 'BaslerUsb');
  });
});

// ---------------------------------------------------------------------------
// tlinfo_test.py (9)
// ---------------------------------------------------------------------------

describe('pylon TlInfo', () => {
  it('constructs an empty TlInfo', () => {
    assert.ok(new pylon.TlInfo() !== null);
  });

  it('compares equal and unequal by TlInfo properties', () => {
    const a = new pylon.TlInfo();
    a.FileName = 'producer.cti';
    a.InfoID = 'Basler/GenTL/1.0';
    a.ModelName = 'pylon GenTL Producer';
    a.Version = '1.0.0';
    const b = new pylon.TlInfo();
    b.FileName = 'producer.cti';
    b.InfoID = 'Basler/GenTL/1.0';
    b.ModelName = 'pylon GenTL Producer';
    b.Version = '1.0.0';
    assert.ok(a.equals(b));
    b.FileName = 'other.cti';
    assert.equal(a.equals(b), false);
  });

  it('orders USB below GigE and ties on the same class', () => {
    const usb = new pylon.TlInfo();
    usb.DeviceClass = 'BaslerUsb';
    const gige = new pylon.TlInfo();
    gige.DeviceClass = 'BaslerGigE';
    assert.equal(usb.lessThan(gige), true);
    assert.equal(gige.lessThan(usb), false);
    const left = new pylon.TlInfo();
    left.DeviceClass = 'BaslerGigE';
    const right = new pylon.TlInfo();
    right.DeviceClass = 'BaslerGigE';
    assert.equal(left.lessThan(right), false);
    assert.equal(right.lessThan(left), false);
  });

  it('round-trips FileName/InfoID/ModelName/Version', () => {
    const cases = [
      ['FileName', 'producer.cti', 'updated.cti', 'dict.cti', pylon.FileNameKey],
      ['InfoID', 'Basler/GenTL/1.0', 'Basler/GenTL/2.0', 'Basler/GenTL/3.0', pylon.InfoIDKey],
      ['ModelName', 'pylon GenTL Producer', 'Updated Producer', 'Dict Producer', pylon.ModelNameKey],
      ['Version', '1.0.0', '2.0.0', '3.0.0', pylon.VersionKey],
    ];
    for (const [prop, first, second, third, key] of cases) {
      const info = new pylon.TlInfo();
      assert.equal(info[`Is${prop}Available`](), false, prop);
      assert.equal(info[prop], pylon.TlInfo.GetPropertyNotAvailable(), prop);
      info[prop] = first;
      assert.equal(info[`Is${prop}Available`](), true, prop);
      assert.equal(info[prop], first, prop);
      info[`Set${prop}`](second);
      assert.equal(info[`Get${prop}`](), second, prop);
      info.SetPropertyValue(key, third);
      assert.equal(info[`Get${prop}`](), third, prop);
    }
  });
});

// ---------------------------------------------------------------------------
// interfaceinfo_test.py (5)
// ---------------------------------------------------------------------------

describe('pylon InterfaceInfo', () => {
  it('constructs and copies', () => {
    const info = new pylon.InterfaceInfo();
    assert.ok(info !== null);
    info.InterfaceID = 'eth0';
    info.FriendlyName = 'Test Interface';
    const copy = new pylon.InterfaceInfo(info);
    assert.equal(copy.InterfaceID, 'eth0');
    assert.equal(copy.FriendlyName, 'Test Interface');
  });

  it('compares equal and unequal by InterfaceID', () => {
    const info = new pylon.InterfaceInfo();
    info.InterfaceID = 'eth0';
    assert.ok(info.equals(new pylon.InterfaceInfo(info)));
    const other = new pylon.InterfaceInfo();
    other.InterfaceID = 'eth1';
    assert.equal(info.equals(other), false);
  });

  it('round-trips InterfaceID through property, legacy and dict styles', () => {
    const info = new pylon.InterfaceInfo();
    assert.equal(info.IsInterfaceIDAvailable(), false);
    assert.equal(info.InterfaceID, pylon.InterfaceInfo.GetPropertyNotAvailable());
    info.InterfaceID = 'eth0';
    assert.equal(info.IsInterfaceIDAvailable(), true);
    assert.equal(info.InterfaceID, 'eth0');
    info.SetInterfaceID('eth1');
    assert.equal(info.GetInterfaceID(), 'eth1');
    info.SetPropertyValue(pylon.InterfaceIDKey, 'eth2');
    assert.equal(info.GetInterfaceID(), 'eth2');
  });
});

// ---------------------------------------------------------------------------
// tlfactory_test.py (27)
// ---------------------------------------------------------------------------

describe('pylon TlFactory', () => {
  it('GetInstance returns the singleton factory', () => {
    const factory = pylon.TlFactory.GetInstance();
    assert.equal(factory, pylon.TlFactory);
    assert.equal(factory, pylon.TlFactory.getInstance());
  });

  it('EnumerateTls returns a non-empty array of TlInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const infos = pylon.TlFactory.GetInstance().EnumerateTls();
      assert.ok(Array.isArray(infos));
      for (const entry of infos) {
        assert.ok(entry instanceof pylon.TlInfo);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateTls includes the emulator transport layer', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const infos = pylon.TlFactory.GetInstance().EnumerateTls();
      const classes = infos.map((entry) => entry.DeviceClass);
      assert.ok(classes.includes(pylon.BaslerCamEmuDeviceClass));
    } finally {
      pylon.terminate();
    }
  });

  it('CreateTl with an unknown device class returns null', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const tl = pylon.TlFactory.GetInstance().CreateTl('NonExistentTransportLayer_XYZ');
      assert.strictEqual(tl, null);
    } finally {
      pylon.terminate();
    }
  });

  it('CreateTl by device class returns a TransportLayer', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const tl = factory.CreateTl(pylon.BaslerCamEmuDeviceClass);
      try {
        assert.ok(tl instanceof pylon.TransportLayer);
      } finally {
        factory.ReleaseTl(tl);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('CreateTl by TlInfo returns a TransportLayer', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const emu = factory.EnumerateTls()
        .find((entry) => entry.DeviceClass === pylon.BaslerCamEmuDeviceClass);
      assert.ok(emu);
      const tl = factory.CreateTl(emu);
      try {
        assert.ok(tl instanceof pylon.TransportLayer);
      } finally {
        factory.ReleaseTl(tl);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('released TransportLayer exposes devices via try/finally like `with`', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        assert.ok(tl instanceof pylon.TransportLayer);
        assert.equal(tl.enumerateDevices().length, deviceCount());
      });
      withTransportLayer((tl) => {
        assert.ok(tl instanceof pylon.TransportLayer);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices returns an array', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      assert.ok(Array.isArray(pylon.TlFactory.GetInstance().EnumerateDevices()));
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices finds the emulated cameras', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const devices = pylon.TlFactory.GetInstance().EnumerateDevices(deviceFilter());
      assert.equal(devices.length, deviceCount());
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices entries are DeviceInfo instances', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      for (const entry of pylon.TlFactory.GetInstance().EnumerateDevices()) {
        assert.ok(entry instanceof pylon.DeviceInfo);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices filters by DeviceInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        assert.ok(devices.length > 0);
        const filtered = pylon.TlFactory.GetInstance().EnumerateDevices([devices[0]]);
        assert.equal(filtered.length, 1);
        assert.equal(filtered[0].SerialNumber, devices[0].SerialNumber);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices filters by dict', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        assert.ok(devices.length > 0);
        const filtered = pylon.TlFactory.GetInstance().EnumerateDevices([{
          [pylon.SerialNumberKey]: devices[0].SerialNumber,
          [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass,
        }]);
        assert.equal(filtered.length, 1);
        assert.equal(filtered[0].SerialNumber, devices[0].SerialNumber);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices with a DeviceClass dict returns all emulated cameras', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const filtered = pylon.TlFactory.GetInstance()
        .EnumerateDevices([{ [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass }]);
      assert.equal(filtered.length, deviceCount());
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices accepts a mixed DeviceInfo/dict filter list', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      if (deviceCount() < 2) return;
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        const filtered = pylon.TlFactory.GetInstance().EnumerateDevices([
          devices[0],
          {
            [pylon.SerialNumberKey]: devices[1].SerialNumber,
            [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass,
          },
        ]);
        const serials = filtered.map((entry) => entry.SerialNumber);
        assert.ok(serials.includes(devices[0].SerialNumber));
        assert.ok(serials.includes(devices[1].SerialNumber));
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices with a wrong filter type throws TypeError', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      assert.throws(
        () => pylon.TlFactory.GetInstance().EnumerateDevices(['not_a_device_info']),
        TypeError);
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices with a non-string dict value throws TypeError', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      assert.throws(
        () => pylon.TlFactory.GetInstance().EnumerateDevices([{ SerialNumber: 42 }]),
        TypeError);
    } finally {
      pylon.terminate();
    }
  });

  it('CreateDevice returns a PylonDevice', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      const device = factory.CreateDevice(devices[0]);
      try {
        assert.ok(device instanceof pylon.PylonDevice);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('CreateFirstDevice with a filter returns a PylonDevice', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const device = factory.CreateFirstDevice(deviceFilter()[0]);
      try {
        assert.ok(device instanceof pylon.PylonDevice);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('CreateFirstDevice without a filter returns a PylonDevice', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const device = factory.CreateFirstDevice();
      try {
        assert.ok(device instanceof pylon.PylonDevice);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('DestroyDevice releases the device for re-creation', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      const device = factory.CreateDevice(devices[0]);
      factory.DestroyDevice(device);
      const again = factory.CreateDevice(devices[0]);
      try {
        assert.ok(again instanceof pylon.PylonDevice);
      } finally {
        factory.DestroyDevice(again);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessible is true for an available device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      assert.equal(factory.IsDeviceAccessible(devices[0]), true);
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessibleInfo returns an [accessible, state] array', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      const result = factory.IsDeviceAccessibleInfo(
        devices[0], pylon.DeviceAccessMode_Control);
      assert.ok(Array.isArray(result));
      assert.equal(result.length, 2);
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessibleInfo reports Ok for an available device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      let [accessible, state] = factory.IsDeviceAccessibleInfo(
        devices[0], pylon.DeviceAccessMode_Control);
      assert.equal(accessible, true);
      assert.equal(state, pylon.Accessibility_Ok);
      [accessible, state] = factory.IsDeviceAccessibleInfo(devices[0]);
      assert.equal(accessible, true);
      assert.equal(state, pylon.Accessibility_Ok);
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessibleInfo stays Ok with an exclusively opened device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const devices = factory.EnumerateDevices(deviceFilter());
      const device = factory.CreateDevice(devices[0]);
      device.open(pylon.DeviceAccessMode_Exclusive);
      try {
        // CamEmu reports reachable devices as accessible/Ok (pypylon parity:
        // the emulated test asserts True/Ok for the exclusive probe).
        const [accessible, state] = factory.IsDeviceAccessibleInfo(
          devices[0], pylon.DeviceAccessMode_Exclusive);
        assert.equal(accessible, true);
        assert.equal(state, pylon.Accessibility_Ok);
      } finally {
        device.close();
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });
});

// ---------------------------------------------------------------------------
// device_test.py (4)
// ---------------------------------------------------------------------------

describe('pylon PylonDevice', () => {
  it('CreateFirstDevice returns a PylonDevice', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const device = factory.CreateFirstDevice(deviceFilter()[0]);
      try {
        assert.ok(device instanceof pylon.PylonDevice);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('DeviceInfo is a non-null DeviceInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const device = factory.CreateFirstDevice(deviceFilter()[0]);
      try {
        const info = device.getDeviceInfo();
        assert.ok(info !== null);
        assert.ok(info instanceof pylon.DeviceInfo);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });

  it('DeviceInfo device class matches the emulator', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      const device = factory.CreateFirstDevice(deviceFilter()[0]);
      try {
        assert.equal(device.getDeviceInfo().DeviceClass,
          pylon.BaslerCamEmuDeviceClass);
      } finally {
        factory.DestroyDevice(device);
      }
    } finally {
      pylon.terminate();
    }
  });
});

// ---------------------------------------------------------------------------
// transportlayer_test.py (24)
// ---------------------------------------------------------------------------

describe('pylon TransportLayer', () => {
  it('TransportLayer derives from IDeviceFactory', () => {
    assert.ok(pylon.TransportLayer.prototype instanceof pylon.IDeviceFactory);
  });

  it('getTlInfo returns a TlInfo for the emulator', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const info = tl.getTlInfo();
        assert.ok(info instanceof pylon.TlInfo);
        assert.equal(info.DeviceClass, pylon.BaslerCamEmuDeviceClass);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('transport layer nodemap holds nodes', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        assert.ok(tl.getNodeMapNumNodes() > 0);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices returns an array of DeviceInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        assert.ok(Array.isArray(devices));
        assert.equal(devices.length, deviceCount());
        for (const entry of devices) {
          assert.ok(entry instanceof pylon.DeviceInfo);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices filters by DeviceInfo and dict', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        assert.ok(devices.length > 0);
        const byInfo = tl.enumerateDevices([devices[0]]);
        assert.equal(byInfo.length, 1);
        assert.equal(byInfo[0].SerialNumber, devices[0].SerialNumber);
        const byDict = tl.enumerateDevices([{
          [pylon.SerialNumberKey]: devices[0].SerialNumber,
          [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass,
        }]);
        assert.equal(byDict.length, 1);
        assert.equal(byDict[0].SerialNumber, devices[0].SerialNumber);
        assert.throws(() => tl.enumerateDevices(['not_a_device_info']), TypeError);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateInterfaces returns an array of InterfaceInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const ifaces = tl.enumerateInterfaces();
        assert.ok(Array.isArray(ifaces));
        for (const entry of ifaces) {
          assert.ok(entry instanceof pylon.InterfaceInfo);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('createInterface yields an Interface and destroys on exit', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const infos = tl.enumerateInterfaces();
        assert.ok(infos.length > 0);
        const iface = tl.createInterface(infos[0]);
        try {
          assert.ok(iface instanceof pylon.Interface);
        } finally {
          tl.destroyInterface(iface);
        }
        const again = tl.createInterface(infos[0]);
        try {
          assert.ok(again !== null);
        } finally {
          tl.destroyInterface(again);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('interface nodemap of the emulator reports zero nodes', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const infos = tl.enumerateInterfaces();
        assert.ok(infos.length > 0);
        const iface = tl.createInterface(infos[0]);
        try {
          // Like `with interface:` in pypylon, entering the nodemap scope
          // opens the interface; exit closes it again.
          iface.open();
          try {
            assert.equal(iface.getNodeMapNumNodes(), 0);
          } finally {
            iface.close();
          }
        } finally {
          tl.destroyInterface(iface);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('CreateDevice/CreateFirstDevice/DestroyDevice round-trip', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        const device = tl.createDevice(devices[0]);
        try {
          assert.ok(device instanceof pylon.PylonDevice);
        } finally {
          tl.destroyDevice(device);
        }
        const first = tl.createFirstDevice(deviceFilter()[0]);
        try {
          assert.ok(first instanceof pylon.PylonDevice);
        } finally {
          tl.destroyDevice(first);
        }
        const any = tl.createFirstDevice();
        try {
          assert.ok(any instanceof pylon.PylonDevice);
        } finally {
          tl.destroyDevice(any);
        }
        const recreated = tl.createDevice(devices[0]);
        tl.destroyDevice(recreated);
        const twice = tl.createDevice(devices[0]);
        try {
          assert.ok(twice instanceof pylon.PylonDevice);
        } finally {
          tl.destroyDevice(twice);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessible reports Ok for an available device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        assert.equal(tl.isDeviceAccessible(devices[0]), true);
        const [accessible, state] = tl.isDeviceAccessibleInfo(
          devices[0], pylon.DeviceAccessMode_Control);
        assert.equal(accessible, true);
        assert.equal(state, pylon.Accessibility_Ok);
        const [accessibleDefault, stateDefault] = tl.isDeviceAccessibleInfo(devices[0]);
        assert.equal(accessibleDefault, true);
        assert.equal(stateDefault, pylon.Accessibility_Ok);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessibleInfo stays Ok with an exclusively opened device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withTransportLayer((tl) => {
        const devices = tl.enumerateDevices();
        const device = tl.createDevice(devices[0]);
        device.open(pylon.DeviceAccessMode_Exclusive);
        try {
          const [accessible, state] = tl.isDeviceAccessibleInfo(
            devices[0], pylon.DeviceAccessMode_Exclusive);
          assert.equal(accessible, true);
          assert.equal(state, pylon.Accessibility_Ok);
        } finally {
          device.close();
          tl.destroyDevice(device);
        }
      });
    } finally {
      pylon.terminate();
    }
  });
});

// ---------------------------------------------------------------------------
// interface_test.py (22)
// ---------------------------------------------------------------------------

describe('pylon Interface', () => {
  function withInterface(fn) {
    return withTransportLayer((tl) => {
      const infos = tl.enumerateInterfaces();
      assert.ok(infos.length > 0, 'no interfaces for the emulator TL');
      const iface = tl.createInterface(infos[0]);
      try {
        return fn(tl, iface);
      } finally {
        tl.destroyInterface(iface);
      }
    });
  }

  it('Interface derives from IDeviceFactory', () => {
    assert.ok(pylon.Interface.prototype instanceof pylon.IDeviceFactory);
  });

  it('EnumerateDevices returns an array of DeviceInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const devices = iface.enumerateDevices();
        assert.ok(Array.isArray(devices));
        assert.equal(devices.length, deviceCount());
        for (const entry of devices) {
          assert.ok(entry instanceof pylon.DeviceInfo);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices filters by DeviceInfo and dict', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const devices = iface.enumerateDevices();
        assert.ok(devices.length > 0);
        const byInfo = iface.enumerateDevices([devices[0]]);
        assert.equal(byInfo.length, 1);
        assert.equal(byInfo[0].SerialNumber, devices[0].SerialNumber);
        const byDict = iface.enumerateDevices([{
          [pylon.SerialNumberKey]: devices[0].SerialNumber,
          [pylon.DeviceClassKey]: pylon.BaslerCamEmuDeviceClass,
        }]);
        assert.equal(byDict.length, 1);
        assert.equal(byDict[0].SerialNumber, devices[0].SerialNumber);
        assert.throws(() => iface.enumerateDevices(['not_a_device_info']), TypeError);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('getInterfaceInfo returns an InterfaceInfo for the emulator', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const info = iface.getInterfaceInfo();
        assert.ok(info instanceof pylon.InterfaceInfo);
        assert.equal(info.DeviceClass, pylon.BaslerCamEmuDeviceClass);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('open/close/isOpen transitions', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        assert.equal(iface.isOpen(), false);
        iface.open();
        try {
          assert.equal(iface.isOpen(), true);
        } finally {
          iface.close();
        }
        assert.equal(iface.isOpen(), false);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('open/close around a nodemap section closes on exit and on error', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        // Like `with interface.NodeMap:`: enter opens, exit closes.
        assert.equal(iface.isOpen(), false);
        iface.open();
        try {
          assert.equal(iface.isOpen(), true);
          assert.equal(typeof iface.getNodeMapNumNodes(), 'number');
        } finally {
          iface.close();
        }
        assert.equal(iface.isOpen(), false);
        try {
          iface.open();
          try {
            throw new Error('intentional error');
          } finally {
            iface.close();
          }
        } catch {
          // Intentional: the interface still closes on exception.
        }
        assert.equal(iface.isOpen(), false);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('CreateDevice/CreateFirstDevice/DestroyDevice round-trip', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const devices = iface.enumerateDevices();
        const device = iface.createDevice(devices[0]);
        try {
          assert.ok(device instanceof pylon.PylonDevice);
        } finally {
          iface.destroyDevice(device);
        }
        const first = iface.createFirstDevice(deviceFilter()[0]);
        try {
          assert.ok(first instanceof pylon.PylonDevice);
        } finally {
          iface.destroyDevice(first);
        }
        const any = iface.createFirstDevice();
        try {
          assert.ok(any instanceof pylon.PylonDevice);
        } finally {
          iface.destroyDevice(any);
        }
        const recreated = iface.createDevice(devices[0]);
        iface.destroyDevice(recreated);
        const twice = iface.createDevice(devices[0]);
        try {
          assert.ok(twice instanceof pylon.PylonDevice);
        } finally {
          iface.destroyDevice(twice);
        }
      });
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessible reports Ok for an available device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const devices = iface.enumerateDevices();
        assert.equal(iface.isDeviceAccessible(devices[0]), true);
        const [accessible, state] = iface.isDeviceAccessibleInfo(
          devices[0], pylon.DeviceAccessMode_Control);
        assert.equal(accessible, true);
        assert.equal(state, pylon.Accessibility_Ok);
        const [accessibleDefault, stateDefault] = iface.isDeviceAccessibleInfo(devices[0]);
        assert.equal(accessibleDefault, true);
        assert.equal(stateDefault, pylon.Accessibility_Ok);
      });
    } finally {
      pylon.terminate();
    }
  });

  it('IsDeviceAccessibleInfo stays Ok with an exclusively opened device', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      withInterface((_tl, iface) => {
        const devices = iface.enumerateDevices();
        const device = iface.createDevice(devices[0]);
        device.open(pylon.DeviceAccessMode_Exclusive);
        try {
          const [accessible, state] = iface.isDeviceAccessibleInfo(
            devices[0], pylon.DeviceAccessMode_Exclusive);
          assert.equal(accessible, true);
          assert.equal(state, pylon.Accessibility_Ok);
        } finally {
          device.close();
          iface.destroyDevice(device);
        }
      });
    } finally {
      pylon.terminate();
    }
  });
});

// ---------------------------------------------------------------------------
// container_test.py (3)
// ---------------------------------------------------------------------------

describe('pylon discovery containers', () => {
  it('EnumerateTls returns a non-empty array of TlInfo', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const infos = pylon.TlFactory.GetInstance().EnumerateTls();
      assert.ok(Array.isArray(infos));
      assert.ok(infos.length > 0);
      assert.ok(infos[0] instanceof pylon.TlInfo);
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateInterfaces returns an array of InterfaceInfo per TL', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const factory = pylon.TlFactory.GetInstance();
      for (const tlInfo of factory.EnumerateTls()) {
        let tl = null;
        try {
          tl = factory.CreateTl(tlInfo);
        } catch {
          continue;
        }
        if (tl === null) continue;
        try {
          const ifaces = tl.enumerateInterfaces();
          assert.ok(Array.isArray(ifaces));
          assert.ok(ifaces.length > 0);
          assert.ok(ifaces[0] instanceof pylon.InterfaceInfo);
        } finally {
          factory.ReleaseTl(tl);
        }
      }
    } finally {
      pylon.terminate();
    }
  });

  it('EnumerateDevices returns an array of DeviceInfo when present', (t) => {
    if (!requireDevice(t)) return;
    pylon.initialize();
    try {
      const devices = pylon.TlFactory.GetInstance().EnumerateDevices();
      assert.ok(Array.isArray(devices));
      if (devices.length > 0) {
        assert.ok(devices[0] instanceof pylon.DeviceInfo);
      }
    } finally {
      pylon.terminate();
    }
  });
});
