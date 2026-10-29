'use strict';

// JS facade over the native N-API module (mirrors the pypylon top-level API).
//
// Default is the real build against Basler pylon SDK 26.09
// (`build/Release/node_pylon.node`, `npm run rebuild`). The pure JS mock
// (`lib/mock.js`) is only an explicit opt-in for offline work
// without SDK/compiler, enabled via NODE_PYLON_USE_MOCK=1:
//   NODE_PYLON_USE_MOCK=1 npm run rebuild:mock
//   NODE_PYLON_USE_MOCK=1 node samples/list-devices.js
// Without a native module and without opt-in, loading throws a clear error
// instead of silently falling back to the mock.
let native = null;
let useNative = false;
let useMock = false;

function isMockOptIn() {
  return process.env.NODE_PYLON_USE_MOCK === '1';
}

function loadNative() {
  if (native) return native;
  try {
    // eslint-disable-next-line import/no-dynamic-require, global-require
    native = require('../build/Release/node_pylon.node');
    useNative = true;
    useMock = false;
  } catch (err) {
    if (!isMockOptIn()) {
      throw new Error(
        'node-pylon: native module build/Release/node_pylon.node not found. ' +
        'Build against the real pylon SDK 26.09 with `npm run rebuild` ' +
        '(needs PYLON_SDK_BASE_URL, see docs/installation.md). ' +
        'Mock fallback is opt-in only: NODE_PYLON_USE_MOCK=1 with `npm run rebuild:mock`. ' +
        `Original error: ${err.message}`
      );
    }
    native = require('./mock');
    useNative = false;
    useMock = true;
  }
  return native;
}

function isNative() {
  loadNative();
  return useNative;
}

function version() {
  loadNative();
  if (useNative) return native.version();
  return '0.2.0-core-mock(js-fallback)';
}

function pylonVersion() {
  loadNative();
  if (useNative) return native.pylonVersion();
  return 'mock-8.x(js-fallback)';
}

function isMock() {
  loadNative();
  if (useNative) return Boolean(native.mock);
  return true;
}

function mockActive() {
  loadNative();
  return useMock;
}

function enumerateDevices() {
  return loadNative().enumerateDevices();
}

function initialize() {
  loadNative();
  if (useNative && typeof native.initialize === 'function') return native.initialize();
  return require('./mock').initialize();
}

function terminate() {
  loadNative();
  if (useNative && typeof native.terminate === 'function') return native.terminate();
  return require('./mock').terminate();
}

function createFirstCamera() {
  loadNative();
  if (useNative && typeof native.createFirstCamera === 'function') {
    return native.createFirstCamera();
  }
  if (useNative && typeof native.InstantCamera === 'function') {
    return new native.InstantCamera();
  }
  const mock = require('./mock');
  return mock.createFirstCamera();
}

// TlFactory facade (mirrors pylon.TlFactory.GetInstance() in pypylon).
// The native TlFactory calls (EnumerateDevices/CreateFirstDevice) run
// in the addon; this object gives them the pypylon-like shape.
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

function grabMock(width = 64, height = 48) {
  return loadNative().grabMock(width, height);
}

// InstantCamera: native (ObjectWrap) or mock fallback (opt-in only)
// with an identical API.
class InstantCamera {
  constructor() {
    loadNative();
    if (useNative && typeof native.InstantCamera === 'function') {
      this._impl = new native.InstantCamera();
      this._isNative = true;
    } else {
      this._impl = new native.MockInstantCamera();
      this._isNative = false;
    }
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

// Creates and opens a camera (one line for samples/tests).
function createAndOpenCamera() {
  const cam = new InstantCamera();
  cam.open();
  return cam;
}

module.exports = {
  version,
  pylonVersion,
  isMock,
  isNative,
  mockActive,
  initialize,
  terminate,
  enumerateDevices,
  createFirstCamera,
  createFirst: createFirstCamera,
  TlFactory,
  grabMock,
  InstantCamera,
  createAndOpenCamera,
};
