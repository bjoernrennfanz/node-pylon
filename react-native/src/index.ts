// JS facade for React Native (TurboModule, real pylon SDK only).
// Thin marshalling over the codegen spec (react-native/specs/NativePylon.js):
// every method delegates to the native module, which itself delegates to
// the shared pylon_core_* (no double wrapping). Requires the native module;
// without it, calls throw instead of falling back to fake values.
//
// Image buffers: the native layer returns the grab Object with `buffer` as
// Uint8Array (JSI ArrayBuffer moved zero-copy out of GrabImage::buffer).
// Bridges without ArrayBuffer support return a base64 string instead, which
// is decoded here — same Uint8Array contract either way. Register bytes
// travel base64-encoded in both directions for the same reason.
//
// Error mapping (mirrors lib/index.js behavior): GenICam wrong-type
// failures surface as TypeError, out-of-range/invalid-entry failures as
// RangeError, everything else as Error — so parity tests can assert the
// same classes as the Node suites.
import {TurboModuleRegistry} from 'react-native';
import type {Spec, DeviceInfo, GrabImage} from '../specs/NativePylon';

function translateError(err: unknown): never {
  const msg = err instanceof Error ? err.message : String(err);
  if (
    /node is not an? (Integer|Float|Boolean|String|Enumeration|Command|Category|Register)|has no value interface|is not a Selector|must be a (string|number|boolean|Uint8Array)/.test(
      msg,
    )
  ) {
    throw new TypeError(msg);
  }
  if (/out of range|invalid entry|Unknown access mode/i.test(msg)) {
    throw new RangeError(msg);
  }
  throw err instanceof Error ? err : new Error(msg);
}

function getModule(): Spec {
  const raw = TurboModuleRegistry.getEnforcing<Spec>('NativePylon');
  return new Proxy(raw, {
    get(target, prop, receiver) {
      const value = Reflect.get(target, prop, receiver);
      if (typeof value !== 'function') return value;
      return (...args: unknown[]) => {
        try {
          return (value as (...a: unknown[]) => unknown).apply(target, args);
        } catch (err) {
          return translateError(err);
        }
      };
    },
  });
}

function checkType(value: unknown, want: string, what: string): void {
  const ok =
    want === 'Uint8Array'
      ? value instanceof Uint8Array
      : typeof value === want;
  if (!ok) {
    throw new TypeError(`${what} must be a ${want}`);
  }
}

export function version(): string {
  return getModule().getVersion();
}

export function pylonVersion(): string {
  return getModule().getPylonVersion();
}

export function initialize(): void {
  // Balanced with terminate(): warms the transport layers via enumeration.
  getModule().enumerateDevices();
}

export function terminate(): void {
  // No-op over TurboModules (no process-lifetime runtime to shut down);
  // kept for lib/index.js call-shape parity.
}

export function enumerateDevices(): DeviceInfo[] {
  return getModule().enumerateDevices();
}

export function getPixelTypeName(pixelTypeValue: number): string {
  return getModule().getPixelTypeName(pixelTypeValue);
}

export function isMonoPixelType(pixelTypeValue: number): boolean {
  return getModule().isMonoPixelType(pixelTypeValue);
}

export function isColorPixelType(pixelTypeValue: number): boolean {
  return getModule().isColorPixelType(pixelTypeValue);
}

// Grab strategy / timeout handling (mirror pylon EGrabStrategy /
// ETimeoutHandling and lib/index.js GrabStrategy/TimeoutHandling).
export const GrabStrategy = {
  OneByOne: 0,
  LatestImageOnly: 1,
  LatestImages: 2,
  UpcomingImage: 3,
} as const;
export type GrabStrategyValue = (typeof GrabStrategy)[keyof typeof GrabStrategy];

export const TimeoutHandling = {
  Return: 0,
  ThrowException: 1,
} as const;
export type TimeoutHandlingValue =
  (typeof TimeoutHandling)[keyof typeof TimeoutHandling];

export const GrabLoop = {
  ProvidedByInstantCamera: 0,
  ProvidedByUser: 1,
} as const;

export const RegistrationMode = {
  Append: 0,
  ReplaceAll: 1,
} as const;

export const Cleanup = {
  None: 0,
  Delete: 1,
} as const;

export const AccessMode = {
  Control: 1,
  Stream: 3,
  Event: 4,
  Exclusive: 5,
} as const;
export type AccessModeValue = (typeof AccessMode)[keyof typeof AccessMode];

// Discovery constants from the SDK (device classes, TL types, property
// keys, access modes, accessibility). Values come from the native module
// at runtime so JS and C++ can never drift apart.
interface DiscoveryConstants {
  strings: {[key: string]: string};
  integers: {[key: string]: number};
}

let cachedConstants: DiscoveryConstants | null = null;

function discovery(): DiscoveryConstants {
  if (!cachedConstants) {
    cachedConstants = getModule().getDiscoveryConstants() as DiscoveryConstants;
  }
  return cachedConstants;
}

function constStr(name: string): string {
  return discovery().strings[name] ?? '';
}

function constInt(name: string): number {
  return discovery().integers[name] ?? 0;
}

export const BaslerCamEmuDeviceClass = {get value(): string { return constStr('BaslerCamEmuDeviceClass'); }};
export const BaslerIpCamDeviceClass = {get value(): string { return constStr('BaslerIpCamDeviceClass'); }};
export const BaslerCameraLinkDeviceClass = {get value(): string { return constStr('BaslerCameraLinkDeviceClass'); }};
export const BaslerUsbDeviceClass = {get value(): string { return constStr('BaslerUsbDeviceClass'); }};
export const BaslerGenTlGevDeviceClass = {get value(): string { return constStr('BaslerGenTlGevDeviceClass'); }};
export const BaslerGenTlU3vDeviceClass = {get value(): string { return constStr('BaslerGenTlU3vDeviceClass'); }};
export const BaslerGenTlCxpDeviceClass = {get value(): string { return constStr('BaslerGenTlCxpDeviceClass'); }};
export const BaslerGenTlBlazeDeviceClass = {get value(): string { return constStr('BaslerGenTlBlazeDeviceClass'); }};
export const BaslerGenTlStaDeviceClass = {get value(): string { return constStr('BaslerGenTlStaDeviceClass'); }};

// Plain-string snapshots (convenience; identical values to the getters).
export function discoveryStrings(): {[key: string]: string} {
  return {...discovery().strings};
}

export function discoveryIntegers(): {[key: string]: number} {
  return {...discovery().integers};
}

export type NodeTypeName =
  | 'Integer'
  | 'Float'
  | 'Boolean'
  | 'String'
  | 'Enumeration'
  | 'Command'
  | 'Category'
  | 'Register'
  | 'Unknown';

export interface PylonDeviceInfo {
  friendlyName: string;
  modelName: string;
  serialNumber: string;
  vendorName: string;
  deviceClass: string;
  fullName: string;
}

export interface ChunkData {
  modeActive: boolean;
  timestamp: number;
  frameID: number;
}

export interface GrabResult {
  succeeded: boolean;
  width: number;
  height: number;
  pixelType: string;
  payloadSize: number;
  timestamp: number;
  blockId: number;
  frameNumber: number;
  buffer: Uint8Array;
  chunk?: ChunkData;
}

type RawGrab = GrabImage & {
  buffer?: Uint8Array | string | number[] | null;
  frameNumber?: number;
  chunk?: ChunkData;
};

// Minimal base64 decoder (no Node Buffer / atob dependency — the facade
// must run on Hermes). Only used for bridges without ArrayBuffer support.
const B64_CHARS =
  'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

function decodeBase64(b64: string): Uint8Array {
  const rev = new Map<string, number>();
  for (let i = 0; i < B64_CHARS.length; i += 1) rev.set(B64_CHARS[i], i);
  let bits = 0;
  let nbits = 0;
  const out: number[] = [];
  for (const ch of b64) {
    if (ch === '=') break;
    const v = rev.get(ch);
    if (v === undefined) continue;
    bits = (bits << 6) | v;
    nbits += 6;
    if (nbits >= 8) {
      nbits -= 8;
      out.push((bits >> nbits) & 0xff);
    }
  }
  return Uint8Array.from(out);
}

function encodeBase64(bytes: Uint8Array): string {
  let out = '';
  let bits = 0;
  let nbits = 0;
  for (const byte of bytes) {
    bits = (bits << 8) | byte;
    nbits += 8;
    while (nbits >= 6) {
      nbits -= 6;
      out += B64_CHARS[(bits >> nbits) & 0x3f];
    }
  }
  if (nbits > 0) {
    out += B64_CHARS[(bits << (6 - nbits)) & 0x3f];
  }
  while (out.length % 4 !== 0) out += '=';
  return out;
}

function normalizeGrab(raw: unknown): GrabResult {
  const r = (raw ?? {}) as RawGrab;
  let buffer: Uint8Array;
  if (r.buffer instanceof Uint8Array) {
    buffer = r.buffer;
  } else if (typeof r.buffer === 'string') {
    // Base64 fallback from bridges without ArrayBuffer support.
    buffer = decodeBase64(r.buffer);
  } else if (Array.isArray(r.buffer)) {
    buffer = Uint8Array.from(r.buffer);
  } else {
    buffer = new Uint8Array(0);
  }
  const res: GrabResult = {
    succeeded: r.succeeded === true,
    width: r.width ?? 0,
    height: r.height ?? 0,
    pixelType: r.pixelType ?? '',
    payloadSize: r.payloadSize ?? buffer.length,
    timestamp: r.timestamp ?? 0,
    blockId: r.blockId ?? 0,
    frameNumber: r.frameNumber ?? 0,
    buffer,
  };
  if (r.chunk !== undefined) res.chunk = r.chunk;
  return res;
}

// InstantCamera: handle-based mirror of the Node InstantCamera facade
// (open/grab-loop/nodemap subset). The CInstantCamera lives in the shared
// core; only the int handle crosses the bridge.
export class InstantCamera {
  private handle: number = -1;
  private deviceIndex: number = -1;
  private accessMode: number = 1;

  constructor(deviceIndex?: number, accessMode?: number) {
    this.deviceIndex = deviceIndex ?? -1;
    this.accessMode = accessMode ?? AccessMode.Control;
    this.open();
  }

  static wrapHandle(handle: number): InstantCamera {
    const cam = Object.create(InstantCamera.prototype) as InstantCamera;
    cam.handle = handle;
    return cam;
  }

  getHandle(): number {
    return this.handle;
  }

  private needOpen(): void {
    if (this.handle < 0) {
      throw new Error('camera is not open');
    }
  }

  isOpen(): boolean {
    if (this.handle < 0) return false;
    return getModule().isCameraOpen(this.handle);
  }

  open(accessMode?: number): void {
    if (this.handle >= 0 && this.isOpen()) return;
    if (accessMode !== undefined) this.accessMode = accessMode;
    this.handle = getModule().openCamera(this.deviceIndex, this.accessMode);
  }

  close(): void {
    if (this.handle >= 0) {
      try {
        getModule().closeCamera(this.handle);
      } finally {
        this.handle = -1;
      }
    }
  }

  getDeviceInfo(): PylonDeviceInfo {
    const raw = getModule().getDeviceInfo(this.handle) as Record<string, unknown>;
    const str = (v: unknown) => (typeof v === 'string' ? v : '');
    const snake = (v: unknown) => (typeof v === 'string' ? v : '');
    return {
      friendlyName: str(raw.friendlyName ?? raw.friendly_name),
      modelName: str(raw.modelName ?? raw.model_name),
      serialNumber: str(raw.serialNumber ?? raw.serial_number),
      vendorName: str(raw.vendorName ?? raw.vendor_name),
      deviceClass: str(raw.deviceClass ?? raw.device_class),
      fullName: snake(raw.fullName ?? raw.full_name),
    };
  }

  isPylonDeviceAttached(): boolean {
    if (this.handle < 0) return false;
    return getModule().isPylonDeviceAttached(this.handle);
  }

  hasOwnership(): boolean {
    if (this.handle < 0) return false;
    return getModule().hasOwnership(this.handle);
  }

  attachDevice(deviceIndex?: number): void {
    getModule().attachDevice(this.handle, deviceIndex ?? this.deviceIndex);
  }

  detachDevice(): void {
    getModule().detachDevice(this.handle);
  }

  destroyDevice(): void {
    getModule().destroyDevice(this.handle);
  }

  isCameraDeviceRemoved(): boolean {
    if (this.handle < 0) return false;
    return getModule().isCameraDeviceRemoved(this.handle);
  }

  getCameraContext(): number {
    return getModule().getCameraContext(this.handle);
  }

  setCameraContext(context: number): void {
    checkType(context, 'number', 'camera context');
    getModule().setCameraContext(this.handle, context);
  }

  getQueuedBufferCount(): number {
    return getModule().getQueuedBufferCount(this.handle);
  }

  startGrabbing(
    strategy: GrabStrategyValue = GrabStrategy.OneByOne,
    maxImages = 0,
  ): void {
    this.needOpen();
    getModule().startGrabbing(this.handle, strategy, maxImages);
  }

  startGrabbingMax(
    maxImages: number,
    strategy: GrabStrategyValue = GrabStrategy.OneByOne,
  ): void {
    this.needOpen();
    getModule().startGrabbing(this.handle, strategy, maxImages);
  }

  stopGrabbing(): void {
    if (this.handle < 0) return;
    getModule().stopGrabbing(this.handle);
  }

  isGrabbing(): boolean {
    if (this.handle < 0) return false;
    return getModule().isGrabbing(this.handle);
  }

  grabOne(timeoutMs = 5000, timeoutHandling: TimeoutHandlingValue = TimeoutHandling.ThrowException): GrabResult {
    this.needOpen();
    return normalizeGrab(
      getModule().grabOne(this.handle, timeoutMs, timeoutHandling),
    );
  }

  retrieveResult(timeoutMs = 5000, timeoutHandling: TimeoutHandlingValue = TimeoutHandling.ThrowException): GrabResult | null {
    this.needOpen();
    const raw = getModule().retrieveResult(
      this.handle,
      timeoutMs,
      timeoutHandling,
    ) as RawGrab | null;
    if (raw === null || raw === undefined) return null;
    if (timeoutHandling === TimeoutHandling.Return && raw.succeeded !== true) {
      return null;
    }
    return normalizeGrab(raw);
  }

  getNodeType(name: string): NodeTypeName {
    return getModule().getNodeType(this.handle, name) as NodeTypeName;
  }

  isReadable(name: string): boolean {
    return getModule().isReadable(this.handle, name);
  }

  isWritable(name: string): boolean {
    return getModule().isWritable(this.handle, name);
  }

  isAvailable(name: string): boolean {
    return getModule().isAvailable(this.handle, name);
  }

  getInteger(name: string): number {
    return getModule().getInteger(this.handle, name);
  }

  setInteger(name: string, value: number): void {
    checkType(value, 'number', 'integer value');
    getModule().setInteger(this.handle, name, value);
  }

  getIntegerMin(name: string): number {
    return getModule().getIntegerMin(this.handle, name);
  }

  getIntegerMax(name: string): number {
    return getModule().getIntegerMax(this.handle, name);
  }

  getIntegerInc(name: string): number {
    return getModule().getIntegerInc(this.handle, name);
  }

  getIntegerUnit(name: string): string {
    return getModule().getIntegerUnit(this.handle, name);
  }

  getIntegerRepresentation(name: string): string {
    return getModule().getIntegerRepresentation(this.handle, name);
  }

  setIntegerToMin(name: string): void {
    getModule().setIntegerToMin(this.handle, name);
  }

  setIntegerToMax(name: string): void {
    getModule().setIntegerToMax(this.handle, name);
  }

  tryMax(name: string): number {
    return getModule().tryMax(this.handle, name);
  }

  getFloat(name: string): number {
    return getModule().getFloat(this.handle, name);
  }

  setFloat(name: string, value: number): void {
    checkType(value, 'number', 'float value');
    getModule().setFloat(this.handle, name, value);
  }

  getFloatMin(name: string): number {
    return getModule().getFloatMin(this.handle, name);
  }

  getFloatMax(name: string): number {
    return getModule().getFloatMax(this.handle, name);
  }

  hasFloatInc(name: string): boolean {
    return getModule().hasFloatInc(this.handle, name);
  }

  getFloatInc(name: string): number {
    return getModule().getFloatInc(this.handle, name);
  }

  getFloatUnit(name: string): string {
    return getModule().getFloatUnit(this.handle, name);
  }

  getFloatRepresentation(name: string): string {
    return getModule().getFloatRepresentation(this.handle, name);
  }

  setFloatToMin(name: string): void {
    getModule().setFloatToMin(this.handle, name);
  }

  setFloatToMax(name: string): void {
    getModule().setFloatToMax(this.handle, name);
  }

  getDisplayName(name: string): string {
    return getModule().getDisplayName(this.handle, name);
  }

  getDescription(name: string): string {
    return getModule().getDescription(this.handle, name);
  }

  getValueAsString(name: string): string {
    return getModule().getValueAsString(this.handle, name);
  }

  setValueFromString(name: string, value: string): void {
    checkType(value, 'string', 'string value');
    getModule().setValueFromString(this.handle, name, value);
  }

  getBoolean(name: string): boolean {
    return getModule().getBoolean(this.handle, name);
  }

  setBoolean(name: string, value: boolean): void {
    checkType(value, 'boolean', 'boolean value');
    getModule().setBoolean(this.handle, name, value);
  }

  getString(name: string): string {
    return getModule().getString(this.handle, name);
  }

  setString(name: string, value: string): void {
    checkType(value, 'string', 'string value');
    getModule().setString(this.handle, name, value);
  }

  getStringMaxLength(name: string): number {
    return getModule().getStringMaxLength(this.handle, name);
  }

  getEnum(name: string): string {
    return getModule().getEnum(this.handle, name);
  }

  setEnum(name: string, value: string): void {
    checkType(value, 'string', 'enum value');
    getModule().setEnum(this.handle, name, value);
  }

  getEnumEntries(name: string): string[] {
    return getModule().getEnumEntries(this.handle, name);
  }

  getEnumIntValue(name: string): number {
    return getModule().getEnumIntValue(this.handle, name);
  }

  setEnumIntValue(name: string, value: number): void {
    checkType(value, 'number', 'enum int value');
    getModule().setEnumIntValue(this.handle, name, value);
  }

  executeCommand(name: string): void {
    getModule().executeCommand(this.handle, name);
  }

  isCommandDone(name: string): boolean {
    return getModule().isCommandDone(this.handle, name);
  }

  getCategoryFeatures(name: string): string[] {
    return getModule().getCategoryFeatures(this.handle, name);
  }

  isSelector(name: string): boolean {
    return getModule().isSelector(this.handle, name);
  }

  getSelectedFeatures(name: string): string[] {
    return getModule().getSelectedFeatures(this.handle, name);
  }

  getSelectingFeatures(name: string): string[] {
    return getModule().getSelectingFeatures(this.handle, name);
  }

  getRegisterLength(name: string): number {
    return getModule().getRegisterLength(this.handle, name);
  }

  getRegisterAddress(name: string): number {
    return getModule().getRegisterAddress(this.handle, name);
  }

  readRegister(name: string): Uint8Array {
    return decodeBase64(getModule().readRegister(this.handle, name));
  }

  writeRegister(name: string, data: Uint8Array): void {
    checkType(data, 'Uint8Array', 'register data');
    getModule().writeRegister(this.handle, name, encodeBase64(data));
  }

  executeSoftwareTrigger(): void {
    // Software trigger = AcquisitionStart command on trigger-configured
    // cameras (see SoftwareTriggerConfiguration); no dedicated core call
    // needed, same effect as the Node helper.
    this.executeCommand('AcquisitionStart');
  }
}

export function createFirstCamera(): InstantCamera {
  return new InstantCamera(-1);
}

export function createCameraAt(deviceIndex: number): InstantCamera {
  return new InstantCamera(deviceIndex);
}

// InstantCameraArray: fixed-size set of cameras, one per device index
// (like the Node InstantCameraArray: initialize/open/close/start/stop,
// round-robin retrieveResult reporting the source camera index).
export class InstantCameraArray {
  private cameras: InstantCamera[] = [];

  constructor(size?: number) {
    if (size !== undefined) this.initialize(size);
  }

  initialize(size: number): this {
    this.destroyDevice();
    this.cameras = [];
    for (let i = 0; i < size; i += 1) {
      this.cameras.push(new InstantCamera(i));
    }
    return this;
  }

  getSize(): number {
    return this.cameras.length;
  }

  getItem(index: number): InstantCamera {
    if (index < 0 || index >= this.cameras.length) {
      throw new RangeError('camera index out of range');
    }
    return this.cameras[index];
  }

  isOpen(): boolean {
    if (this.cameras.length === 0) return false;
    return this.cameras.every(c => c.isOpen());
  }

  open(): void {
    for (const c of this.cameras) c.open();
  }

  close(): void {
    for (const c of this.cameras) c.close();
  }

  destroyDevice(): void {
    for (const c of this.cameras) {
      try {
        c.destroyDevice();
      } catch {
        // Best effort.
      }
    }
    this.cameras = [];
  }

  isGrabbing(): boolean {
    return this.cameras.some(c => c.isGrabbing());
  }

  startGrabbing(
    strategy: GrabStrategyValue = GrabStrategy.OneByOne,
    maxImages = 0,
  ): void {
    for (const c of this.cameras) c.startGrabbing(strategy, maxImages);
  }

  stopGrabbing(): void {
    for (const c of this.cameras) {
      try {
        c.stopGrabbing();
      } catch {
        // Best effort.
      }
    }
  }

  retrieveResult(
    timeoutMs = 5000,
    timeoutHandling: TimeoutHandlingValue = TimeoutHandling.ThrowException,
  ): (GrabResult & {cameraIndex: number}) | null {
    for (let i = 0; i < this.cameras.length; i += 1) {
      const cam = this.cameras[i];
      if (!cam.isGrabbing()) continue;
      const res = cam.retrieveResult(timeoutMs, timeoutHandling);
      if (res !== null && res !== undefined) {
        return {...res, cameraIndex: i};
      }
    }
    return null;
  }
}

// Acquisition configurations (like lib/index.js: applied to the camera
// nodemap directly, or called after open).
export const AcquireContinuousConfiguration = {
  applyConfiguration(camera: InstantCamera): void {
    camera.setEnum('AcquisitionMode', 'Continuous');
  },
};

export const AcquireSingleFrameConfiguration = {
  applyConfiguration(camera: InstantCamera): void {
    camera.setEnum('AcquisitionMode', 'SingleFrame');
  },
};

export const SoftwareTriggerConfiguration = {
  applyConfiguration(camera: InstantCamera): void {
    camera.setEnum('AcquisitionMode', 'Continuous');
    camera.setEnum('TriggerSelector', 'FrameStart');
    camera.setEnum('TriggerMode', 'On');
    camera.setEnum('TriggerSource', 'Software');
  },
};

// ConfigurationHelper subset (safe: absent/unwritable nodes on this device
// make them a no-op instead of throwing — same contract as trySetEnum in
// lib/index.js).
function isReadableSafe(camera: InstantCamera, node: string): boolean {
  try {
    return camera.isReadable(node);
  } catch {
    return false;
  }
}

export const ConfigurationHelper = {
  disableAllTriggers(camera: InstantCamera): void {
    const selector = 'TriggerSelector';
    const previous = camera.getEnum(selector);
    try {
      for (const entry of camera.getEnumEntries(selector)) {
        camera.setEnum(selector, entry);
        try {
          camera.setEnum('TriggerMode', 'Off');
        } catch {
          // Some selectors have no Off entry; keep going.
        }
      }
    } finally {
      camera.setEnum(selector, previous);
    }
  },
  disableCompression(camera: InstantCamera): void {
    if (isReadableSafe(camera, 'ImageCompressionMode')) {
      camera.setEnum('ImageCompressionMode', 'Off');
    }
  },
  disableGenDC(camera: InstantCamera): void {
    if (isReadableSafe(camera, 'GenDCStreamingMode')) {
      camera.setEnum('GenDCStreamingMode', 'Off');
    }
  },
  selectRangeComponent(camera: InstantCamera): void {
    if (isReadableSafe(camera, 'ComponentSelector')) {
      camera.setEnum('ComponentSelector', 'Range');
    }
  },
  probePacketSize(camera: InstantCamera): void {
    // Probes the optimal packet size; safe on devices without GigE support.
    try {
      camera.getInteger('GevSCPSPacketSize');
    } catch {
      // No GigE packet-size node: nothing to probe.
    }
  },
};

// Documented gaps vs lib/index.js (node-pylon core). Everything not listed
// here is covered by the facade above or the constants:
// - TlFactory/TransportLayer/Interface object model (EnumerateDevices with
//   filters, CreateTl/ReleaseTl, property dicts, comparisons): needs a
//   discovery object API on the bridge — device-independent constants ARE
//   ported (see discoveryStrings/discoveryIntegers). Follow-up scope.
// - WaitObject/WaitObjects, FeaturePersistence, StaticDefectPixelCorrection,
//   BufferFactory: need N-API ObjectWrap lifetime or file I/O with no
//   codegen-compatible shape yet. Node-only im MVP.
// - PylonImage/ImageFormatConverter/ImageDecompressor pipeline: reusable
//   image buffers with format conversion; no codegen shape yet. Node-only.
// - Chunk data triple (ChunkAdapter GeV/U3V) beyond frameNumber/timestamp:
//   GenDC/chunk parsing stays Node-only; frame sequencing IS ported.
// - Event/callback handlers (image/camera/config/callback events):
//   TurboModule events need a separate event-emitter bridge. Next step.
export const RN_COVERAGE_GAPS: readonly string[] = [
  'TlFactory/TransportLayer/Interface object model',
  'WaitObject/WaitObjects',
  'FeaturePersistence',
  'StaticDefectPixelCorrection',
  'BufferFactory',
  'PylonImage/ImageFormatConverter/ImageDecompressor',
  'ChunkAdapter triple (GeV/U3V)',
  'event/callback handlers',
];

export type {DeviceInfo, GrabImage};
