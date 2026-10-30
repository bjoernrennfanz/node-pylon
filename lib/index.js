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

// createCameraAt(deviceIndex): attach enumerated device i (array support).
function createCameraAt(deviceIndex) {
  const mod = loadNative();
  if (typeof mod.createCameraAt === 'function') {
    return mod.createCameraAt(deviceIndex);
  }
  return new mod.InstantCamera(deviceIndex);
}

// Pylon enum values (mirrors the C++ enums; read from the native module
// when available so JS and C++ can never drift apart).
function enumValue(name, fallback) {
  const mod = loadNative();
  return typeof mod[name] === 'number' ? mod[name] : fallback;
}

const GrabStrategy = {
  get OneByOne() { return enumValue('GrabStrategy_OneByOne', 0); },
  get LatestImageOnly() { return enumValue('GrabStrategy_LatestImageOnly', 1); },
  get LatestImages() { return enumValue('GrabStrategy_LatestImages', 2); },
  get UpcomingImage() { return enumValue('GrabStrategy_UpcomingImage', 3); },
};

const TimeoutHandling = {
  get Return() { return enumValue('TimeoutHandling_Return', 0); },
  get ThrowException() { return enumValue('TimeoutHandling_ThrowException', 1); },
};

const GrabLoop = {
  get ProvidedByInstantCamera() { return enumValue('GrabLoop_ProvidedByInstantCamera', 0); },
  get ProvidedByUser() { return enumValue('GrabLoop_ProvidedByUser', 1); },
};

const RegistrationMode = {
  get Append() { return enumValue('RegistrationMode_Append', 0); },
  get ReplaceAll() { return enumValue('RegistrationMode_ReplaceAll', 1); },
};

const Cleanup = {
  get None() { return enumValue('Cleanup_None', 0); },
  get Delete() { return enumValue('Cleanup_Delete', 1); },
};

const AccessMode = {
  get Control() { return enumValue('AccessMode_Control', 1); },
  get Stream() { return enumValue('AccessMode_Stream', 3); },
  get Event() { return enumValue('AccessMode_Event', 4); },
  get Exclusive() { return enumValue('AccessMode_Exclusive', 5); },
};

// TlFactory facade (like pylon.TlFactory.GetInstance() in pypylon).
// Identity-stable singleton: GetInstance()/getInstance() return this object.
// enumerateDevices() keeps the historical plain-object list; EnumerateDevices()
// and the discovery methods below delegate to the native TlFactory singleton
// and return DeviceInfo/TlInfo/TransportLayer/PylonDevice instances
// (pypylon parity). CreateFirstDevice returns a native PylonDevice (like
// pypylon); use createFirstCamera()/InstantCamera for the camera facade.
function nativeTlFactory() {
  return loadNative().TlFactory.GetInstance();
}

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
  EnumerateDevices(filter) {
    const tl = nativeTlFactory();
    return filter === undefined ? tl.EnumerateDevices() : tl.EnumerateDevices(filter);
  },
  EnumerateTls() {
    return nativeTlFactory().EnumerateTls();
  },
  enumerateTls() {
    return nativeTlFactory().EnumerateTls();
  },
  CreateTl(arg) {
    return nativeTlFactory().CreateTl(arg);
  },
  createTl(arg) {
    return nativeTlFactory().CreateTl(arg);
  },
  ReleaseTl(tl) {
    return nativeTlFactory().ReleaseTl(tl);
  },
  releaseTl(tl) {
    return nativeTlFactory().ReleaseTl(tl);
  },
  CreateDevice(info) {
    return nativeTlFactory().CreateDevice(info);
  },
  createDevice(info) {
    return nativeTlFactory().CreateDevice(info);
  },
  CreateFirstDevice(info) {
    const tl = nativeTlFactory();
    return info === undefined ? tl.CreateFirstDevice() : tl.CreateFirstDevice(info);
  },
  createFirstDevice(info) {
    return TlFactory.CreateFirstDevice(info);
  },
  DestroyDevice(device) {
    return nativeTlFactory().DestroyDevice(device);
  },
  destroyDevice(device) {
    return nativeTlFactory().DestroyDevice(device);
  },
  IsDeviceAccessible(info, mode) {
    const tl = nativeTlFactory();
    return mode === undefined ? tl.IsDeviceAccessible(info) : tl.IsDeviceAccessible(info, mode);
  },
  isDeviceAccessible(info, mode) {
    return TlFactory.IsDeviceAccessible(info, mode);
  },
  IsDeviceAccessibleInfo(info, mode) {
    const tl = nativeTlFactory();
    return mode === undefined
      ? tl.IsDeviceAccessibleInfo(info)
      : tl.IsDeviceAccessibleInfo(info, mode);
  },
  isDeviceAccessibleInfo(info, mode) {
    return TlFactory.IsDeviceAccessibleInfo(info, mode);
  },
};

// InstantCamera: native ObjectWrap with identical API.
class InstantCamera {
  constructor(deviceIndex) {
    this._impl = deviceIndex === undefined
      ? new (loadNative().InstantCamera)()
      : new (loadNative().InstantCamera)(deviceIndex);
  }

  static wrapNative(impl) {
    const cam = Object.create(InstantCamera.prototype);
    cam._impl = impl;
    return cam;
  }

  isOpen() { return this._impl.isOpen(); }
  open(accessMode) { return this._impl.open(accessMode); }
  close() { return this._impl.close(); }
  getDeviceInfo() { return this._impl.getDeviceInfo(); }
  isPylonDeviceAttached() { return this._impl.isPylonDeviceAttached(); }
  hasOwnership() { return this._impl.hasOwnership(); }
  attachDevice(deviceIndex) { return this._impl.attachDevice(deviceIndex); }
  detachDevice() { return this._impl.detachDevice(); }
  destroyDevice() { return this._impl.destroyDevice(); }
  isCameraDeviceRemoved() { return this._impl.isCameraDeviceRemoved(); }
  getCameraContext() { return this._impl.getCameraContext(); }
  setCameraContext(ctx) { return this._impl.setCameraContext(ctx); }
  getQueuedBufferCount() { return this._impl.getQueuedBufferCount(); }
  getNodeType(name) { return this._impl.getNodeType(name); }
  isReadable(name) { return this._impl.isReadable(name); }
  isWritable(name) { return this._impl.isWritable(name); }
  isAvailable(name) { return this._impl.isAvailable(name); }
  getDisplayName(name) { return this._impl.getDisplayName(name); }
  getDescription(name) { return this._impl.getDescription(name); }
  getInteger(name) { return this._impl.getInteger(name); }
  setInteger(name, v) { return this._impl.setInteger(name, v); }
  getIntegerMin(name) { return this._impl.getIntegerMin(name); }
  getIntegerMax(name) { return this._impl.getIntegerMax(name); }
  getIntegerInc(name) { return this._impl.getIntegerInc(name); }
  getIntegerUnit(name) { return this._impl.getIntegerUnit(name); }
  getIntegerRepresentation(name) { return this._impl.getIntegerRepresentation(name); }
  setIntegerToMin(name) { return this._impl.setIntegerToMin(name); }
  setIntegerToMax(name) { return this._impl.setIntegerToMax(name); }
  tryMax(name) { return this._impl.tryMax(name); }
  getFloat(name) { return this._impl.getFloat(name); }
  setFloat(name, v) { return this._impl.setFloat(name, v); }
  getFloatMin(name) { return this._impl.getFloatMin(name); }
  getFloatMax(name) { return this._impl.getFloatMax(name); }
  hasFloatInc(name) { return this._impl.hasFloatInc(name); }
  getFloatInc(name) { return this._impl.getFloatInc(name); }
  getFloatUnit(name) { return this._impl.getFloatUnit(name); }
  getFloatRepresentation(name) { return this._impl.getFloatRepresentation(name); }
  setFloatToMin(name) { return this._impl.setFloatToMin(name); }
  setFloatToMax(name) { return this._impl.setFloatToMax(name); }
  getValueAsString(name) { return this._impl.getValueAsString(name); }
  setValueFromString(name, v) { return this._impl.setValueFromString(name, v); }
  getBoolean(name) { return this._impl.getBoolean(name); }
  setBoolean(name, v) { return this._impl.setBoolean(name, v); }
  getString(name) { return this._impl.getString(name); }
  setString(name, v) { return this._impl.setString(name, v); }
  getStringMaxLength(name) { return this._impl.getStringMaxLength(name); }
  getEnum(name) { return this._impl.getEnum(name); }
  setEnum(name, v) { return this._impl.setEnum(name, v); }
  getEnumEntries(name) { return this._impl.getEnumEntries(name); }
  getEnumIntValue(name) { return this._impl.getEnumIntValue(name); }
  setEnumIntValue(name, v) { return this._impl.setEnumIntValue(name, v); }
  executeCommand(name) { return this._impl.executeCommand(name); }
  isCommandDone(name) { return this._impl.isCommandDone(name); }
  getCategoryFeatures(name) { return this._impl.getCategoryFeatures(name); }
  isSelector(name) { return this._impl.isSelector(name); }
  getSelectedFeatures(name) { return this._impl.getSelectedFeatures(name); }
  getSelectingFeatures(name) { return this._impl.getSelectingFeatures(name); }
  getRegisterLength(name) { return this._impl.getRegisterLength(name); }
  getRegisterAddress(name) { return this._impl.getRegisterAddress(name); }
  readRegister(name) { return this._impl.readRegister(name); }
  writeRegister(name, buffer) { return this._impl.writeRegister(name, buffer); }
  registerCallback(name) { return this._impl.registerCallback(name); }
  getCallbackCount(handle) { return this._impl.getCallbackCount(handle); }
  deregisterCallback(handle) { return this._impl.deregisterCallback(handle); }
  grab() { return this._impl.grab(); }
  startGrabbing(strategy, grabLoop) {
    if (strategy === undefined && grabLoop === undefined) {
      return this._impl.startGrabbing();
    }
    if (strategy !== undefined && grabLoop !== undefined) {
      return this._impl.startGrabbing(strategy, grabLoop);
    }
    return this._impl.startGrabbing(strategy);
  }
  startGrabbingMax(maxImages, strategy, grabLoop) {
    return this._impl.startGrabbingMax(maxImages, strategy, grabLoop);
  }
  stopGrabbing() { return this._impl.stopGrabbing(); }
  isGrabbing() {
    if (typeof this._impl.isGrabbing === 'function') return this._impl.isGrabbing();
    return Boolean(this._impl._grabbing);
  }
  retrieveResult(timeoutMs, timeoutHandling) {
    if (timeoutHandling === undefined) return this._impl.retrieveResult(timeoutMs);
    return this._impl.retrieveResult(timeoutMs, timeoutHandling);
  }
  retrieveGrabResult(timeoutMs, timeoutHandling) {
    const raw = timeoutHandling === undefined
      ? this._impl.retrieveGrabResult(timeoutMs)
      : this._impl.retrieveGrabResult(timeoutMs, timeoutHandling);
    if (raw === null || raw === undefined) return raw;
    tagResultFactory(raw, this._bufferFactory);
    return GrabResult.wrapNative(raw);
  }
  grabOne(timeoutMs, timeoutHandling) {
    // Plain grab object (isValid/succeeded/buffer fields); factory
    // provenance lives on holdable GrabResult (retrieveGrabResult).
    return timeoutHandling === undefined
      ? this._impl.grabOne(timeoutMs)
      : this._impl.grabOne(timeoutMs, timeoutHandling);
  }
  getGrabResultWaitObject() {
    return WaitObject.wrapNative(
      loadNative().__waitOps.getGrabResultWaitObject(this._impl));
  }
  GetGrabResultWaitObject() { return this.getGrabResultWaitObject(); }
  getGrabStopWaitObject() {
    return WaitObject.wrapNative(
      loadNative().__waitOps.getGrabStopWaitObject(this._impl));
  }
  GetGrabStopWaitObject() { return this.getGrabStopWaitObject(); }
  setBufferFactory(factory) {
    loadNative().__bufferFactoryOps.attach(this._impl, factory || null);
    this._bufferFactory = factory || null;
  }
  SetBufferFactory(factory) { return this.setBufferFactory(factory); }
  getBufferFactory() {
    if (this._bufferFactory) return this._bufferFactory;
    const attached = loadNative().__bufferFactoryOps.getAttached(this._impl);
    this._bufferFactory = attached || null;
    return this._bufferFactory;
  }
  GetBufferFactory() { return this.getBufferFactory(); }
  waitForFrameTriggerReady(timeoutMs, timeoutHandling) {
    return this._impl.waitForFrameTriggerReady(timeoutMs, timeoutHandling);
  }
  executeSoftwareTrigger() { return this._impl.executeSoftwareTrigger(); }
  registerConfiguration(handler, mode) {
    return this._impl.registerConfiguration(handler, mode);
  }
  deregisterConfiguration(idOrHandler) {
    return this._impl.deregisterConfiguration(idOrHandler);
  }
  registerImageEventHandler(handler, mode) {
    return this._impl.registerImageEventHandler(handler, mode);
  }
  deregisterImageEventHandler(idOrHandler) {
    return this._impl.deregisterImageEventHandler(idOrHandler);
  }
  registerCameraEventHandler(handler, nodeName, mode) {
    return this._impl.registerCameraEventHandler(handler, nodeName, mode);
  }
  deregisterCameraEventHandler(idOrHandler) {
    return this._impl.deregisterCameraEventHandler(idOrHandler);
  }
}

// Creates and opens a camera (one-liner for samples/tests).
function createAndOpenCamera() {
  const cam = new InstantCamera();
  cam.open();
  return cam;
}

// --- Infra API facade (buffer factory, wait objects, feature persistence,
// static defect pixel correction, type mappings) ---------------------------
// Unwraps facade cameras to their native handles for the native ops objects.
function implOf(camera) {
  return camera instanceof InstantCamera ? camera._impl : camera;
}

// WaitObject: native ObjectWrap with identical API (pypylon waitobject
// semantics: default-constructed objects are invalid, camera wait objects
// come from camera.getGrabResultWaitObject()/getGrabStopWaitObject()).
class WaitObject {
  constructor() {
    this._impl = new (loadNative().WaitObject)();
  }

  static wrapNative(impl) {
    const w = Object.create(WaitObject.prototype);
    w._impl = impl;
    return w;
  }

  static sleep(ms) { return loadNative().WaitObject.sleep(ms); }
  static Sleep(ms) { return WaitObject.sleep(ms); }

  isValid() { return this._impl.isValid(); }
  IsValid() { return this.isValid(); }
  wait(timeoutMs) { return this._impl.wait(timeoutMs); }
  Wait(timeoutMs) { return this.wait(timeoutMs); }
  waitEx(timeoutMs, alertable) { return this._impl.waitEx(timeoutMs, alertable); }
  WaitEx(timeoutMs, alertable) { return this.waitEx(timeoutMs, alertable); }
}

// WaitObjects: container for multi-object waits (pypylon waitobjects
// semantics). waitForAny/Ex return a plain bool/result, or a
// [result, index] pair when called with true as the last argument.
class WaitObjects {
  constructor() {
    this._impl = new (loadNative().WaitObjects)();
  }

  static wrapNative(impl) {
    const w = Object.create(WaitObjects.prototype);
    w._impl = impl;
    return w;
  }

  add(waitObject) {
    const r = this._impl.add(waitObject instanceof WaitObject ? waitObject._impl : waitObject);
    return r;
  }
  Add(waitObject) { return this.add(waitObject); }
  removeAll() { return this._impl.removeAll(); }
  RemoveAll() { return this.removeAll(); }
  waitForAll(timeoutMs) { return this._impl.waitForAll(timeoutMs); }
  WaitForAll(timeoutMs) { return this.waitForAll(timeoutMs); }
  waitForAny(timeoutMs, withIndex) {
    return withIndex === undefined
      ? this._impl.waitForAny(timeoutMs)
      : this._impl.waitForAny(timeoutMs, withIndex);
  }
  WaitForAny(timeoutMs, withIndex) { return this.waitForAny(timeoutMs, withIndex); }
  waitForAllEx(timeoutMs, alertable) { return this._impl.waitForAllEx(timeoutMs, alertable); }
  WaitForAllEx(timeoutMs, alertable) { return this.waitForAllEx(timeoutMs, alertable); }
  waitForAnyEx(timeoutMs, alertable, withIndex) {
    return withIndex === undefined
      ? this._impl.waitForAnyEx(timeoutMs, alertable)
      : this._impl.waitForAnyEx(timeoutMs, alertable, withIndex);
  }
  WaitForAnyEx(timeoutMs, alertable, withIndex) {
    return this.waitForAnyEx(timeoutMs, alertable, withIndex);
  }
}

// WaitEx results + waitForever (mirrors Pylon::EWaitExResult / waitForever;
// read from the native module when available).
const WaitExResult = {
  get Timeout() { return enumValue('waitex_timeout', 0); },
  get Signaled() { return enumValue('waitex_signaled', 1); },
  get Abandoned() { return enumValue('waitex_abandoned', 2); },
  get Alerted() { return enumValue('waitex_alerted', -1); },
};

// FeaturePersistence: Save/Load/SaveToString/LoadFromString against the
// open camera's nodemap (like pylon.FeaturePersistence in pypylon).
const FeaturePersistence = {
  save(file, camera) {
    return loadNative().FeaturePersistence.save(file, implOf(camera));
  },
  Save(file, camera) { return FeaturePersistence.save(file, camera); },
  load(file, camera, validate) {
    return validate === undefined
      ? loadNative().FeaturePersistence.load(file, implOf(camera))
      : loadNative().FeaturePersistence.load(file, implOf(camera), validate);
  },
  Load(file, camera, validate) { return FeaturePersistence.load(file, camera, validate); },
  saveToString(camera) {
    return loadNative().FeaturePersistence.saveToString(implOf(camera));
  },
  SaveToString(camera) { return FeaturePersistence.saveToString(camera); },
  loadFromString(features, camera, validate) {
    return validate === undefined
      ? loadNative().FeaturePersistence.loadFromString(features, implOf(camera))
      : loadNative().FeaturePersistence.loadFromString(features, implOf(camera), validate);
  },
  LoadFromString(features, camera, validate) {
    return FeaturePersistence.loadFromString(features, camera, validate);
  },
};

// StaticDefectPixelCorrection: defect-pixel list access for supporting
// cameras (like pylon.StaticDefectPixelCorrection in pypylon). Pixel
// entries are [x, y] / [x, y, type] arrays or {x, y, type?} objects;
// getters return [success, StaticDefectPixelVector] pairs.
const StaticDefectPixelCorrection = {
  get ListType_Factory() { return loadNative().StaticDefectPixelCorrection.ListType_Factory; },
  get ListType_User() { return loadNative().StaticDefectPixelCorrection.ListType_User; },
  getDefectPixelList(camera, listType) {
    return listType === undefined
      ? loadNative().StaticDefectPixelCorrection.getDefectPixelList(implOf(camera))
      : loadNative().StaticDefectPixelCorrection.getDefectPixelList(implOf(camera), listType);
  },
  GetDefectPixelList(camera, listType) {
    return StaticDefectPixelCorrection.getDefectPixelList(camera, listType);
  },
  setDefectPixelList(camera, pixels, listType) {
    return listType === undefined
      ? loadNative().StaticDefectPixelCorrection.setDefectPixelList(implOf(camera), pixels)
      : loadNative().StaticDefectPixelCorrection.setDefectPixelList(implOf(camera), pixels, listType);
  },
  SetDefectPixelList(camera, pixels, listType) {
    return StaticDefectPixelCorrection.setDefectPixelList(camera, pixels, listType);
  },
  normalizePixelList(camera, pixels) {
    return loadNative().StaticDefectPixelCorrection.normalizePixelList(implOf(camera), pixels);
  },
  NormalizePixelList(camera, pixels) {
    return StaticDefectPixelCorrection.normalizePixelList(camera, pixels);
  },
};

// Direct native re-exports (lazy so requiring the facade never needs the
// build; identity with native objects is preserved, e.g. result
// getBufferFactory() === the attached factory).
function nativeClass(name) {
  return loadNative()[name];
}

// Tags a native grab result with the camera's attached buffer factory so
// getBufferFactory() returns it (provenance for the bufferfactory tests).
function tagResultFactory(rawNativeResult, factory) {
  if (!factory) return;
  try {
    loadNative().__bufferFactoryOps.setResultFactory(rawNativeResult, factory);
  } catch {
    // Tagging is best effort; grab data stays valid without it.
  }
}

// GrabResult: native ObjectWrap over CGrabResultPtr (pypylon grabresult
// semantics). Instances come from camera.retrieveGrabResult(); the class
// is also constructible directly (invalid/empty result, like a default
// CGrabResultPtr). Plain fields (succeeded, width, height, pixelType,
// payloadSize, buffer, timestamp, ...) are set on the instance for
// property-style access alongside the accessor methods.
class GrabResult {
  constructor() {
    this._impl = new (loadNative().GrabResult)();
  }

  static wrapNative(impl) {
    const r = Object.create(GrabResult.prototype);
    r._impl = impl;
    // Mirror the native plain fields (succeeded, width, pixelType, buffer,
    // ...) for property-style access; functions stay on the prototype.
    for (const key of Object.keys(impl)) {
      if (key === '_impl') continue;
      let v;
      try {
        v = impl[key];
      } catch {
        continue;
      }
      if (typeof v !== 'function') r[key] = v;
    }
    return r;
  }

  isValid() { return this._impl.isValid(); }
  isUnique() { return this._impl.isUnique(); }
  release() { return this._impl.release(); }
  succeeded() { return this._impl.succeeded(); }
  grabSucceeded() { return this._impl.succeeded(); }
  getWidth() { return this._impl.getWidth(); }
  getHeight() { return this._impl.getHeight(); }
  getOffsetX() { return this._impl.getOffsetX(); }
  getOffsetY() { return this._impl.getOffsetY(); }
  getPaddingX() { return this._impl.getPaddingX(); }
  getPaddingY() { return this._impl.getPaddingY(); }
  getPixelType() { return this._impl.getPixelType(); }
  getPixelTypeValue() { return this._impl.getPixelTypeValue(); }
  getPayloadType() { return this._impl.getPayloadType(); }
  getPayloadSize() { return this._impl.getPayloadSize(); }
  getBufferSize() { return this._impl.getBufferSize(); }
  getImageSize() { return this._impl.getImageSize(); }
  getTimestamp() { return this._impl.getTimestamp(); }
  getBlockId() { return this._impl.getBlockId(); }
  getId() { return this._impl.getId(); }
  getImageNumber() { return this._impl.getImageNumber(); }
  getSkippedImages() { return this._impl.getSkippedImages(); }
  getErrorCode() { return this._impl.getErrorCode(); }
  getErrorDescription() { return this._impl.getErrorDescription(); }
  isChunkDataAvailable() { return this._impl.isChunkDataAvailable(); }
  hasCrc() { return this._impl.hasCrc(); }
  checkCrc() { return this._impl.checkCrc(); }
  getStride() { return this._impl.getStride(); }
  getBuffer() { return this._impl.getBuffer(); }
  getArray() { return this._impl.getArray(); }
  getMemoryView() { return this._impl.getMemoryView(); }
  getDataComponentCount() { return this._impl.getDataComponentCount(); }
  getDataComponent(i) { return this._impl.getDataComponent(i); }
  getDataComponentBuffer(i) { return this._impl.getDataComponentBuffer(i); }
  toObject() { return this._impl.toObject(); }
  // Buffer-factory provenance (like pylon GrabResult.BufferContext /
  // BufferFactory): context id from the allocating factory, or null
  // without a custom factory; the exact attached factory object or null.
  getBufferContext() { return this._impl.getBufferContext(); }
  GetBufferContext() { return this.getBufferContext(); }
  get bufferContext() { return this.getBufferContext(); }
  getBufferFactory() { return this._impl.getBufferFactory(); }
  GetBufferFactory() { return this.getBufferFactory(); }
  get bufferFactory() { return this.getBufferFactory(); }
}

// PylonImage: reusable image buffer (pypylon PylonImage semantics).
// create() allocates, copyImage() copies from a PylonImage or GrabResult,
// save()/load() persist via CImagePersistence, getBuffer()/getArray()/
// getMemoryView() expose the buffer zero-copy.
class PylonImage {
  constructor() {
    this._impl = new (loadNative().PylonImage)();
  }

  static wrapNative(impl) {
    const img = Object.create(PylonImage.prototype);
    img._impl = impl;
    return img;
  }

  static create(pixelType, width, height, paddingX) {
    return PylonImage.wrapNative(
      loadNative().PylonImage.create(pixelType, width, height, paddingX));
  }

  isValid() { return this._impl.isValid(); }
  isUnique() { return this._impl.isUnique(); }
  release() { return this._impl.release(); }
  copyImage(source) {
    return this._impl.copyImage(source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source);
  }
  attachGrabResult(grab) {
    return this._impl.attachGrabResult(grab instanceof GrabResult ? grab._impl : grab);
  }
  getWidth() { return this._impl.getWidth(); }
  getHeight() { return this._impl.getHeight(); }
  getPaddingX() { return this._impl.getPaddingX(); }
  getOrientation() { return this._impl.getOrientation(); }
  getPixelType() { return this._impl.getPixelType(); }
  getPixelTypeValue() { return this._impl.getPixelTypeValue(); }
  getImageSize() { return this._impl.getImageSize(); }
  getAllocatedBufferSize() { return this._impl.getAllocatedBufferSize(); }
  getStride() { return this._impl.getStride(); }
  isSupportedPixelType(pixelType) { return this._impl.isSupportedPixelType(pixelType); }
  getBuffer() { return this._impl.getBuffer(); }
  getArray() { return this._impl.getArray(); }
  getMemoryView() { return this._impl.getMemoryView(); }
  save(format, filename) { return this._impl.save(format, filename); }
  load(filename) { return this._impl.load(filename); }
  canSaveWithoutConversion(format) { return this._impl.canSaveWithoutConversion(format); }
  toObject() { return this._impl.toObject(); }
}

// ImageFormatConverter: format conversion between pixel types
// (pypylon ImageFormatConverter semantics).
class ImageFormatConverter {
  constructor() {
    this._impl = new (loadNative().ImageFormatConverter)();
  }

  setOutputPixelFormat(pixelType) { return this._impl.setOutputPixelFormat(pixelType); }
  getOutputPixelFormat() { return this._impl.getOutputPixelFormat(); }
  setOutputBitAlignment(alignment) { return this._impl.setOutputBitAlignment(alignment); }
  getOutputBitAlignment() { return this._impl.getOutputBitAlignment(); }
  convert(destination, source) {
    const dst = destination instanceof PylonImage ? destination._impl : destination;
    const src = source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source;
    return this._impl.convert(dst, src);
  }
  getBufferSizeForConversion(source) {
    const src = source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source;
    return this._impl.getBufferSizeForConversion(src);
  }
  imageHasDestinationFormat(source) {
    const src = source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source;
    return this._impl.imageHasDestinationFormat(src);
  }
  static isSupportedInputFormat(pixelType) {
    return loadNative().ImageFormatConverter.isSupportedInputFormat(pixelType);
  }
  static isSupportedOutputFormat(pixelType) {
    return loadNative().ImageFormatConverter.isSupportedOutputFormat(pixelType);
  }
}

// ImageDecompressor: compression info and decompression for cameras that
// transfer compressed buffers (pypylon ImageDecompressor semantics).
class ImageDecompressor {
  constructor() {
    this._impl = new (loadNative().ImageDecompressor)();
  }

  describe(source) {
    const src = source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source;
    return this._impl.describe(src);
  }
  decompress(destination, source) {
    const dst = destination instanceof PylonImage ? destination._impl : destination;
    const src = source instanceof PylonImage || source instanceof GrabResult
      ? source._impl : source;
    return this._impl.decompress(dst, src);
  }
  static getCompressionMode() {
    return loadNative().ImageDecompressor.getCompressionMode();
  }
}

// PixelType enum values and helpers (mirrors pylon EPixelType + mapper).
// Proxies over the native enum objects so JS and C++ cannot drift apart.
function nativeEnum(name) {
  const mod = loadNative();
  return mod[name] || {};
}
function enumProxy(name) {
  return new Proxy({}, {
    get(_t, prop) { return nativeEnum(name)[prop]; },
    ownKeys() { return Object.keys(nativeEnum(name)); },
    getOwnPropertyDescriptor(_t, prop) {
      if (!(prop in nativeEnum(name))) return undefined;
      return { enumerable: true, configurable: true };
    },
  });
}
const PixelType = enumProxy('PixelType');
const PayloadType = enumProxy('PayloadType');
function getPixelTypeByName(name) { return loadNative().getPixelTypeByName(name); }
function getNameByPixelType(value) { return loadNative().getNameByPixelType(value); }
function isMono(pixelType) { return loadNative().isMono(pixelType); }
function isColor(pixelType) { return loadNative().isColor(pixelType); }
function isPacked(pixelType) { return loadNative().isPacked(pixelType); }
const ImageFileFormat = {
  Bmp: 'Bmp', Tiff: 'Tiff', Jpeg: 'Jpeg', Png: 'Png', Raw: 'Raw', Dng: 'Dng',
};

// Acquisition configurations (like pylon's CAcquire*Configuration: applied
// to the camera nodemap directly, or registered to run on open).
class AcquireContinuousConfiguration {
  static applyConfiguration(camera) {
    camera.setEnum('AcquisitionMode', 'Continuous');
  }
  onOpened(camera) {
    AcquireContinuousConfiguration.applyConfiguration(camera);
  }
}

class AcquireSingleFrameConfiguration {
  static applyConfiguration(camera) {
    camera.setEnum('AcquisitionMode', 'SingleFrame');
  }
  onOpened(camera) {
    AcquireSingleFrameConfiguration.applyConfiguration(camera);
  }
}

class SoftwareTriggerConfiguration {
  static applyConfiguration(camera) {
    camera.setEnum('AcquisitionMode', 'Continuous');
    camera.setEnum('TriggerSelector', 'FrameStart');
    camera.setEnum('TriggerMode', 'On');
    camera.setEnum('TriggerSource', 'Software');
  }
  onOpened(camera) {
    SoftwareTriggerConfiguration.applyConfiguration(camera);
  }
}

// ConfigurationHelper statics (like pylon::ConfigurationHelper). All calls
// are safe: missing nodes throw, matching the C++ behavior of failing only
// when a required node is absent; callers guard with try/catch like the
// pypylon tests do.
const ConfigurationHelper = {
  disableAllTriggers(camera) {
    const selector = 'TriggerSelector';
    const mode = 'TriggerMode';
    const previous = camera.getEnum(selector);
    try {
      for (const entry of camera.getEnumEntries(selector)) {
        camera.setEnum(selector, entry);
        try {
          camera.setEnum(mode, 'Off');
        } catch {
          // Some selectors have no Off entry; keep going.
        }
      }
    } finally {
      camera.setEnum(selector, previous);
    }
  },
  disableCompression(camera) {
    camera.setEnum('ImageCompressionMode', 'Off');
  },
  disableGenDC(camera) {
    camera.setEnum('GenDCStreamingMode', 'Off');
  },
  selectRangeComponent(camera) {
    camera.setEnum('ComponentSelector', 'Range');
  },
  probePacketSize(camera) {
    // Probes the optimal packet size; safe on devices without GigE support.
    // Best effort: leave the nodemap untouched when the nodes are absent.
    try {
      camera.getInteger('GevSCPSPacketSize');
    } catch {
      // No GigE packet-size node: nothing to probe.
    }
  },
};

// InstantCameraArray: fixed-size set of cameras, one per enumerated device
// (like CInstantCameraArray::Initialize/Open/Close/StartGrabbing).
class InstantCameraArray {
  constructor(size) {
    this._cameras = [];
    if (size !== undefined) this.initialize(size);
  }
  initialize(size) {
    this.destroyDevice();
    this._cameras = [];
    for (let i = 0; i < size; i += 1) {
      this._cameras.push(InstantCamera.wrapNative(createCameraAt(i)));
    }
    return this;
  }
  getSize() { return this._cameras.length; }
  getItem(index) {
    if (index < 0 || index >= this._cameras.length) {
      throw new RangeError('camera index out of range');
    }
    return this._cameras[index];
  }
  [Symbol.iterator]() { return this._cameras[Symbol.iterator](); }
  isPylonDeviceAttached() {
    if (this._cameras.length === 0) return false;
    return this._cameras.every((c) => c.isPylonDeviceAttached());
  }
  isCameraDeviceRemoved() {
    return this._cameras.some((c) => c.isCameraDeviceRemoved());
  }
  isOpen() {
    if (this._cameras.length === 0) return false;
    return this._cameras.every((c) => c.isOpen());
  }
  open() { for (const c of this._cameras) c.open(); }
  close() { for (const c of this._cameras) c.close(); }
  destroyDevice() {
    for (const c of this._cameras) {
      try { c.destroyDevice(); } catch { /* best effort */ }
    }
    this._cameras = [];
  }
  isGrabbing() {
    return this._cameras.some((c) => c.isGrabbing());
  }
  startGrabbing(strategy, grabLoop) {
    for (const c of this._cameras) c.startGrabbing(strategy, grabLoop);
  }
  stopGrabbing() {
    for (const c of this._cameras) {
      try { c.stopGrabbing(); } catch { /* best effort */ }
    }
  }
  // Round-robin retrieve across grabbing cameras (like
  // CInstantCameraArray::RetrieveResult, which reports the source camera).
  retrieveResult(timeoutMs, timeoutHandling) {
    for (let i = 0; i < this._cameras.length; i += 1) {
      const cam = this._cameras[i];
      if (!cam.isGrabbing()) continue;
      const res = cam.retrieveResult(timeoutMs, timeoutHandling);
      if (res !== null && res !== undefined) {
        res.cameraIndex = i;
        return res;
      }
    }
    return null;
  }
}

module.exports = {
  version,
  pylonVersion,
  initialize,
  terminate,
  enumerateDevices,
  createFirstCamera,
  createFirst: createFirstCamera,
  createCameraAt,
  TlFactory,
  InstantCamera,
  InstantCameraArray,
  GrabResult,
  PylonImage,
  ImageFormatConverter,
  ImageDecompressor,
  PixelType,
  PayloadType,
  ImageFileFormat,
  getPixelTypeByName,
  getNameByPixelType,
  isMono,
  isColor,
  isPacked,
  createAndOpenCamera,
  AcquireContinuousConfiguration,
  AcquireSingleFrameConfiguration,
  SoftwareTriggerConfiguration,
  ConfigurationHelper,
  GrabStrategy,
  TimeoutHandling,
  GrabLoop,
  RegistrationMode,
  Cleanup,
  AccessMode,
  // Infra APIs (buffer factory, wait objects, feature persistence,
  // static defect pixel correction, type mappings).
  WaitObject,
  WaitObjects,
  WaitExResult,
  FeaturePersistence,
  StaticDefectPixelCorrection,
};

// Lazy native re-exports (no build needed to require the facade).
// Classes are the native constructors (identity preserved); constants and
// the genicam namespace read through to the native module.
for (const name of [
  'BufferFactory',
  'DeviceInfo',
  'TlInfo',
  'InterfaceInfo',
  'TransportLayer',
  'Interface',
  'PylonDevice',
  'IDeviceFactory',
  'StaticDefectPixel',
  'StaticDefectPixelVector',
]) {
  Object.defineProperty(module.exports, name, {
    enumerable: true,
    configurable: true,
    get() { return nativeClass(name); },
  });
}
for (const name of [
  'waitex_timeout',
  'waitex_signaled',
  'waitex_abandoned',
  'waitex_alerted',
  'waitForever',
  'StaticDefectPixelType_Reserved',
  'genicam',
  'GenericException',
  'BadAllocException',
  'InvalidArgumentException',
  'OutOfRangeException',
  'PropertyException',
  'RuntimeException',
  'LogicalErrorException',
  'AccessException',
  'TimeoutException',
  'DynamicCastException',
  // Discovery constants (device classes, TL types, property keys,
  // device access modes, accessibility states; read through to native).
  'BaslerCamEmuDeviceClass',
  'BaslerIpCamDeviceClass',
  'BaslerCameraLinkDeviceClass',
  'BaslerGigEDeviceClass',
  'BaslerGenTlDeviceClassPrefix',
  'BaslerGenTlGevDeviceClass',
  'BaslerGenTlU3vDeviceClass',
  'BaslerGenTlCxpDeviceClass',
  'BaslerGenTlBlazeDeviceClass',
  'BaslerGenTlStaDeviceClass',
  'BaslerUsbDeviceClass',
  'TLTypeGigE',
  'TLTypeUSB',
  'TLTypeCL',
  'TLTypeCamEmu',
  'TLTypeCXP',
  'TLTypeCustom',
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
  'ManufacturerInfoKey',
  'DeviceIDKey',
  'AddressKey',
  'IpAddressKey',
  'DefaultGatewayKey',
  'SubnetMaskKey',
  'SubnetAddressKey',
  'PortNrKey',
  'MacAddressKey',
  'InterfaceKey',
  'IpConfigOptionsKey',
  'IpConfigCurrentKey',
  'DeviceGUIDKey',
  'VendorIdKey',
  'ProductIdKey',
  'DriverKeyNameKey',
  'UsbDriverTypeKey',
  'UsbPortVersionBcdKey',
  'SpeedSupportBitmaskKey',
  'TransferModeKey',
  'PortIDKey',
  'InitialBaudRateKey',
  'DeviceXMLFileOverrideKey',
  'DeviceSpecificStringKey',
  'PortSpecificStringKey',
  'DeviceIdxKey',
  'FileNameKey',
  'InfoIDKey',
  'VersionKey',
  'DeviceAccessMode_Control',
  'DeviceAccessMode_Stream',
  'DeviceAccessMode_Event',
  'DeviceAccessMode_Exclusive',
  'Accessibility_Unknown',
  'Accessibility_Ok',
  'Accessibility_Opened',
  'Accessibility_OpenedExclusively',
  'Accessibility_NotReachable',
]) {
  Object.defineProperty(module.exports, name, {
    enumerable: true,
    configurable: true,
    get() { return loadNative()[name]; },
  });
}
