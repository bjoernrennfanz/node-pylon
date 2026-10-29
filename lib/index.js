'use strict';

// JS facade over the native N-API module (pypylon-style top-level API).
// Always the real Basler pylon SDK 26.09 build
// (build/Release/node_pylon.node); without it, loading throws a clear
// error pointing at the SDK setup.
let native = null;

function loadNative() {
  if (native) return native;
  try {
    // eslint-disable-next-line import/no-dynamic-require, global-require
    native = require('../build/Release/node_pylon.node');
  } catch (err) {
    throw new Error(
      'node-pylon: native module build/Release/node_pylon.node not found. ' +
      'Build against the real pylon SDK 26.09 with `npm run rebuild` ' +
      '(needs PYLON_SDK_BASE_URL, see docs/installation.md). ' +
      `Original error: ${err.message}`
    );
  }
  return native;
}

function version() {
  return loadNative().version();
}

function pylonVersion() {
  return loadNative().pylonVersion();
}

function enumerateDevices() {
  return loadNative().enumerateDevices();
}

function initialize() {
  return loadNative().initialize();
}

function terminate() {
  return loadNative().terminate();
}

function createFirstCamera() {
  const mod = loadNative();
  if (typeof mod.createFirstCamera === 'function') {
    return mod.createFirstCamera();
  }
  return new mod.InstantCamera();
}

// TlFactory facade (like pylon.TlFactory.GetInstance() in pypylon).
// The native TlFactory calls (EnumerateDevices/CreateFirstDevice) run in
// the addon; this object gives them the pypylon-like shape.
const TlFactory = {
  GetInstance() {
    return TlFactory;
  },
  getInstance() {
    return TlFactory;
  },
  enumerateDevices() {
    return enumerateDevices();
  },
  EnumerateDevices() {
    return enumerateDevices();
  },
  createFirstDevice() {
    return createFirstCamera();
  },
  CreateFirstDevice() {
    return createFirstCamera();
  },
};

// InstantCamera: native ObjectWrap with identical API.
class InstantCamera {
  constructor() {
    this._impl = new (loadNative().InstantCamera)();
  }

  isOpen() { return this._impl.isOpen(); }
  open() { return this._impl.open(); }
  close() { return this._impl.close(); }
  getDeviceInfo() { return this._impl.getDeviceInfo(); }
  getNodeType(name) { return this._impl.getNodeType(name); }
  isReadable(name) { return this._impl.isReadable(name); }
  isWritable(name) { return this._impl.isWritable(name); }
  getInteger(name) { return this._impl.getInteger(name); }
  setInteger(name, v) { return this._impl.setInteger(name, v); }
  getIntegerMin(name) { return this._impl.getIntegerMin(name); }
  getIntegerMax(name) { return this._impl.getIntegerMax(name); }
  tryMax(name) { return this._impl.tryMax(name); }
  getFloat(name) { return this._impl.getFloat(name); }
  setFloat(name, v) { return this._impl.setFloat(name, v); }
  getFloatMin(name) { return this._impl.getFloatMin(name); }
  getFloatMax(name) { return this._impl.getFloatMax(name); }
  getBoolean(name) { return this._impl.getBoolean(name); }
  setBoolean(name, v) { return this._impl.setBoolean(name, v); }
  getString(name) { return this._impl.getString(name); }
  setString(name, v) { return this._impl.setString(name, v); }
  getEnum(name) { return this._impl.getEnum(name); }
  setEnum(name, v) { return this._impl.setEnum(name, v); }
  getEnumEntries(name) { return this._impl.getEnumEntries(name); }
  executeCommand(name) { return this._impl.executeCommand(name); }
  grab() { return this._impl.grab(); }
  startGrabbing() { return this._impl.startGrabbing(); }
  stopGrabbing() { return this._impl.stopGrabbing(); }
  isGrabbing() {
    if (typeof this._impl.isGrabbing === 'function') return this._impl.isGrabbing();
    return Boolean(this._impl._grabbing);
  }
  retrieveResult(timeoutMs) { return this._impl.retrieveResult(timeoutMs); }
}

// Creates and opens a camera (one-liner for samples/tests).
function createAndOpenCamera() {
  const cam = new InstantCamera();
  cam.open();
  return cam;
}

module.exports = {
  version,
  pylonVersion,
  initialize,
  terminate,
  enumerateDevices,
  createFirstCamera,
  createFirst: createFirstCamera,
  TlFactory,
  InstantCamera,
  createAndOpenCamera,
};
