'use strict';

// Reiner JS-Mock der N-API-CameraWrap (Opt-in, kein Default).
// Semantik-identisch zu src/mock_nodemap.cc + src/mock_camera.cc, nur fuer
// Offline-Arbeit ohne SDK/Compiler. Aktivierung: NODE_PYLON_USE_MOCK=1
// (siehe lib/index.js, docs/installation.md). Standard ist der Real-SDK-Build.

const NODE_TYPES = ['Integer', 'Float', 'Boolean', 'String', 'Enumeration', 'Command'];

function bytesPerPixel(pixelFormat) {
  if (pixelFormat === 'RGB8') return 3;
  if (pixelFormat === 'Mono12') return 2;
  return 1;
}

function mockDeviceInfo() {
  return {
    friendlyName: 'Mock Camera (node-pylon)',
    modelName: 'MockModel Mono',
    serialNumber: 'MOCK123456',
    vendorName: 'Basler (mock)',
    deviceClass: 'MockDevice',
  };
}

class MockInstantCamera {
  constructor() {
    this._open = false;
    this._grabbing = false;
    this._frameNumber = 0;
    this._nodes = {
      Width: { type: 'Integer', value: 640, min: 64, max: 1920, readable: true, writable: true },
      Height: { type: 'Integer', value: 480, min: 48, max: 1080, readable: true, writable: true },
      PayloadSize: { type: 'Integer', value: 640 * 480, min: 0, max: 1920 * 1080 * 3, readable: true, writable: false },
      ExposureTime: { type: 'Float', value: 5000.0, min: 10.0, max: 100000.0, readable: true, writable: true },
      Gain: { type: 'Float', value: 0.0, min: 0.0, max: 24.0, readable: true, writable: true },
      ChunkModeActive: { type: 'Boolean', value: false, readable: true, writable: true },
      AcquisitionFrameRateEnable: { type: 'Boolean', value: false, readable: true, writable: true },
      DeviceUserID: { type: 'String', value: '', readable: true, writable: true, maxLen: 64 },
      DeviceModelName: { type: 'String', value: 'MockModel Mono', readable: true, writable: false, maxLen: 64 },
      DeviceSerialNumber: { type: 'String', value: 'MOCK123456', readable: true, writable: false, maxLen: 64 },
      PixelFormat: { type: 'Enumeration', value: 'Mono8', entries: ['Mono8', 'Mono12', 'RGB8'], readable: true, writable: true },
      TestPattern: { type: 'Enumeration', value: 'Off', entries: ['Off', 'SensorTestPattern'], readable: true, writable: true },
      AcquisitionStart: { type: 'Command', available: true, executeCount: 0 },
      AcquisitionStop: { type: 'Command', available: true, executeCount: 0 },
      UserSetSave: { type: 'Command', available: true, executeCount: 0 },
    };
  }

  _lookup(name) {
    const n = this._nodes[name];
    if (!n) throw new Error(`Unknown node: ${name}`);
    return n;
  }

  _assertType(name, expected) {
    const n = this._lookup(name);
    if (n.type !== expected) throw new TypeError(`Node '${name}' is not a ${expected}`);
    return n;
  }

  isOpen() { return this._open; }
  open() { this._open = true; return true; }
  close() { this._open = false; this._grabbing = false; return true; }

  // ZAU-150 Grab-Loop (pypylon-nah). retrieveResult wirft ohne Grabbing
  // und liefert sonst { succeeded, width, height, pixelFormat,
  // payloadSize, buffer } — semantisch identisch zum nativen Mock.
  startGrabbing() {
    if (!this._open) throw new Error('Camera is not open. Call open() first.');
    this._grabbing = true;
  }

  stopGrabbing() { this._grabbing = false; }

  isGrabbing() { return this._grabbing; }

  retrieveResult(timeoutMs = 5000) {
    if (!this._grabbing) throw new Error('Not grabbing. Call startGrabbing() first.');
    const w = this._nodes.Width.value;
    const h = this._nodes.Height.value;
    const pixelFormat = this._nodes.PixelFormat.value;
    const bytes = w * h * bytesPerPixel(pixelFormat);
    const buffer = Buffer.allocUnsafe(bytes);
    const frame = this._frameNumber++;
    for (let i = 0; i < bytes; i++) buffer[i] = (i + frame) & 0xff;
    return { succeeded: true, width: w, height: h, pixelType: pixelFormat, pixelFormat, payloadSize: bytes, buffer };
  }

  getDeviceInfo() { return mockDeviceInfo(); }

  getNodeType(name) { return this._lookup(name).type; }

  isReadable(name) {
    const n = this._lookup(name);
    if (n.type === 'Command') return false;
    return n.readable;
  }

  isWritable(name) {
    const n = this._lookup(name);
    if (n.type === 'Command') return n.available;
    return n.writable;
  }

  getInteger(name) {
    const n = this._assertType(name, 'Integer');
    if (!n.readable) throw new Error(`Node is not readable: ${name}`);
    if (name === 'PayloadSize') {
      return this._nodes.Width.value * this._nodes.Height.value * bytesPerPixel(this._nodes.PixelFormat.value);
    }
    return n.value;
  }

  setInteger(name, v) {
    const n = this._assertType(name, 'Integer');
    if (!Number.isInteger(v)) throw new TypeError(`setInteger('${name}') expects an integer`);
    if (!n.writable) throw new Error(`Node is not writable: ${name}`);
    if (v < n.min || v > n.max) throw new RangeError(`Value out of range: ${name} (expected ${n.min}..${n.max})`);
    n.value = v;
  }

  getIntegerMin(name) { return this._assertType(name, 'Integer').min; }
  getIntegerMax(name) { return this._assertType(name, 'Integer').max; }

  tryMax(name) {
    const n = this._lookup(name);
    if (n.type === 'Integer' || n.type === 'Float') {
      if (!n.writable) throw new Error(`Node is not writable: ${name}`);
      n.value = n.max;
      return n.max;
    }
    throw new TypeError(`Node '${name}' is not a Integer`);
  }

  getFloat(name) {
    const n = this._assertType(name, 'Float');
    if (!n.readable) throw new Error(`Node is not readable: ${name}`);
    return n.value;
  }

  setFloat(name, v) {
    const n = this._assertType(name, 'Float');
    if (typeof v !== 'number') throw new TypeError(`setFloat('${name}') expects a number`);
    if (!n.writable) throw new Error(`Node is not writable: ${name}`);
    if (v < n.min || v > n.max) throw new RangeError(`Value out of range: ${name} (expected ${n.min}..${n.max})`);
    n.value = v;
  }

  getFloatMin(name) { return this._assertType(name, 'Float').min; }
  getFloatMax(name) { return this._assertType(name, 'Float').max; }

  getBoolean(name) {
    const n = this._assertType(name, 'Boolean');
    if (!n.readable) throw new Error(`Node is not readable: ${name}`);
    return n.value;
  }

  setBoolean(name, v) {
    const n = this._assertType(name, 'Boolean');
    if (typeof v !== 'boolean') throw new TypeError(`setBoolean('${name}') expects a boolean`);
    if (!n.writable) throw new Error(`Node is not writable: ${name}`);
    n.value = v;
  }

  getString(name) {
    const n = this._assertType(name, 'String');
    if (!n.readable) throw new Error(`Node is not readable: ${name}`);
    return n.value;
  }

  setString(name, v) {
    const n = this._assertType(name, 'String');
    if (typeof v !== 'string') throw new TypeError(`setString('${name}') expects a string`);
    if (!n.writable) throw new Error(`Node is not writable: ${name}`);
    if (v.length > n.maxLen) throw new RangeError(`Value out of range: ${name} (max ${n.maxLen} chars)`);
    n.value = v;
  }

  getEnum(name) {
    const n = this._assertType(name, 'Enumeration');
    if (!n.readable) throw new Error(`Node is not readable: ${name}`);
    return n.value;
  }

  setEnum(name, v) {
    const n = this._assertType(name, 'Enumeration');
    if (!n.writable) throw new Error(`Node is not writable: ${name}`);
    if (!n.entries.includes(v)) throw new RangeError(`Invalid enumeration entry: ${name} (expected one of ${n.entries.join(', ')})`);
    n.value = v;
  }

  getEnumEntries(name) {
    return [...this._assertType(name, 'Enumeration').entries];
  }

  executeCommand(name) {
    const n = this._assertType(name, 'Command');
    if (!n.available) throw new Error(`Command execution failed: ${name}`);
    n.executeCount += 1;
    if (name === 'AcquisitionStart') this._grabbing = true;
    if (name === 'AcquisitionStop') this._grabbing = false;
    return true;
  }

  grab() {
    const w = this._nodes.Width.value;
    const h = this._nodes.Height.value;
    const pixelFormat = this._nodes.PixelFormat.value;
    const bpp = bytesPerPixel(pixelFormat);
    const bytes = w * h * bpp;
    const buffer = Buffer.allocUnsafe(bytes);
    const frame = this._frameNumber++;
    for (let i = 0; i < bytes; i++) buffer[i] = (i + frame) & 0xff;
    const timestamp = Number(process.hrtime.bigint());
    return {
      buffer,
      width: w,
      height: h,
      pixelType: pixelFormat,
      pixelFormat,
      payloadSize: bytes,
      timestamp,
      frameNumber: frame,
      chunk: { modeActive: this._nodes.ChunkModeActive.value, timestamp, frameID: frame },
    };
  }
}

function enumerateDevices() {
  return [mockDeviceInfo()];
}

// Pylon-Lifecycle als Refcount-No-op (Mock; echte SDK-Calls nur nativ).
// Jede initialize()-Referenz braucht ein terminate().
let initCount = 0;

function initialize() {
  initCount += 1;
}

function terminate() {
  initCount = Math.max(0, initCount - 1);
}

function createFirstCamera() {
  return new MockInstantCamera();
}

function grabMock(width = 64, height = 48) {
  if (!Number.isInteger(width) || !Number.isInteger(height) || width < 1 || height < 1 || width > 4096 || height > 4096) {
    throw new TypeError('width/height must be in 1..4096');
  }
  const buffer = Buffer.allocUnsafe(width * height);
  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) buffer[y * width + x] = (x + y) & 0xff;
  }
  return buffer;
}

module.exports = {
  NODE_TYPES,
  MockInstantCamera,
  mockDeviceInfo,
  enumerateDevices,
  initialize,
  terminate,
  createFirstCamera,
  grabMock,
  bytesPerPixel,
};
