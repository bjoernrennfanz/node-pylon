// TypeScript types for the JS facade (lib/index.js). Mirrors the pypylon API, MVP without DataProcessing.
export declare function version(): string;
export declare function pylonVersion(): string;
/** PylonInitialize (refcounted). Each initialize() reference needs a terminate(). */
export declare function initialize(): void;
/** PylonTerminate (refcounted). */
export declare function terminate(): void;

export interface PylonDeviceInfo {
  friendlyName: string;
  modelName: string;
  serialNumber: string;
  vendorName: string;
  deviceClass: string;
  [key: string]: unknown;
}

export declare function enumerateDevices(): PylonDeviceInfo[];
/** TlFactory.CreateFirstDevice — throws without a detected camera. */
export declare function createFirstCamera(): InstantCamera;
export declare function createFirst(): InstantCamera;
/** Attach enumerated device i (camera-array support). */
export declare function createCameraAt(deviceIndex: number): unknown;

export type GrabStrategyValue = number | 'OneByOne' | 'LatestImageOnly' | 'LatestImages' | 'UpcomingImage';
export type TimeoutHandlingValue = number | 'Return' | 'ThrowException';
export type GrabLoopValue = number | 'ProvidedByInstantCamera' | 'ProvidedByUser';
export type RegistrationModeValue = number | 'Append' | 'ReplaceAll';
export type AccessModeValue = number | 'Control' | 'Stream' | 'Event' | 'Exclusive';
export interface StartGrabbingOptions {
  strategy?: GrabStrategyValue;
  grabLoop?: GrabLoopValue;
  maxImages?: number;
}

export declare const GrabStrategy: {
  readonly OneByOne: number;
  readonly LatestImageOnly: number;
  readonly LatestImages: number;
  readonly UpcomingImage: number;
};
export declare const TimeoutHandling: {
  readonly Return: number;
  readonly ThrowException: number;
};
export declare const GrabLoop: {
  readonly ProvidedByInstantCamera: number;
  readonly ProvidedByUser: number;
};
export declare const RegistrationMode: {
  readonly Append: number;
  readonly ReplaceAll: number;
};
export declare const Cleanup: {
  readonly None: number;
  readonly Delete: number;
};
export declare const AccessMode: {
  readonly Control: number;
  readonly Stream: number;
  readonly Event: number;
  readonly Exclusive: number;
};

export type NodeTypeName = 'Integer' | 'Float' | 'Boolean' | 'String' | 'Enumeration' | 'Command';

export interface GrabResult {
  buffer: Buffer;
  width: number;
  height: number;
  /** pypylon naming (pixel type string). */
  pixelType: string;
  pixelFormat: string;
  payloadSize: number;
  timestamp: number;
  frameNumber: number;
  chunk: { modeActive: boolean; timestamp: number; frameID: number };
}

/** Result of retrieveResult (grab loop). */
export interface RetrieveResult {
  /** Mirrors CGrabResultPtr::IsValid(). */
  isValid: boolean;
  succeeded: boolean;
  /** Function accessor mirroring GrabSucceeded(). */
  grabSucceeded(): boolean;
  width: number;
  height: number;
  /** pypylon naming; mirrors pixelFormat. */
  pixelType: string;
  pixelFormat: string;
  payloadSize: number;
  buffer: Buffer;
  /** Mirrors GetBlockID(). */
  blockId: number;
  frameNumber: number;
  /** Mirrors GetTimeStamp(). */
  timestamp: number;
  errorCode?: number;
  errorDescription?: string;
  /** Set by InstantCameraArray.retrieveResult: source camera index. */
  cameraIndex?: number;
}

export type ConfigEventName =
  | 'OnAttached' | 'OnDetached' | 'OnDestroyed'
  | 'OnOpened' | 'OnClosed' | 'OnGrabStarted' | 'OnGrabStopped';
export type ConfigurationHandler = (event: ConfigEventName) => void;
export type ImageEventHandler = (result: RetrieveResult) => void;
export type CameraEventHandler = (result: RetrieveResult) => void;

export declare class InstantCamera {
  constructor(deviceIndex?: number);
  isOpen(): boolean;
  open(accessMode?: AccessModeValue): boolean;
  close(): boolean;
  getDeviceInfo(): PylonDeviceInfo;
  isPylonDeviceAttached(): boolean;
  hasOwnership(): boolean;
  attachDevice(deviceIndex?: number): boolean;
  detachDevice(): PylonDeviceInfo;
  destroyDevice(): void;
  isCameraDeviceRemoved(): boolean;
  getCameraContext(): number;
  setCameraContext(context: number): void;
  getQueuedBufferCount(): number;
  getNodeType(name: string): NodeTypeName;
  isReadable(name: string): boolean;
  isWritable(name: string): boolean;
  isAvailable(name: string): boolean;
  getDisplayName(name: string): string;
  getDescription(name: string): string;
  getInteger(name: string): number;
  setInteger(name: string, value: number): void;
  getIntegerMin(name: string): number;
  getIntegerMax(name: string): number;
  getIntegerInc(name: string): number;
  getIntegerUnit(name: string): string;
  getIntegerRepresentation(name: string): string;
  setIntegerToMin(name: string): void;
  setIntegerToMax(name: string): void;
  /** Sets Integer/Float to max and returns the value set. Throws for other types. */
  tryMax(name: string): number;
  getFloat(name: string): number;
  setFloat(name: string, value: number): void;
  getFloatMin(name: string): number;
  getFloatMax(name: string): number;
  hasFloatInc(name: string): boolean;
  getFloatInc(name: string): number;
  getFloatUnit(name: string): string;
  getFloatRepresentation(name: string): string;
  setFloatToMin(name: string): void;
  setFloatToMax(name: string): void;
  getValueAsString(name: string): string;
  setValueFromString(name: string, value: string): void;
  getBoolean(name: string): boolean;
  setBoolean(name: string, value: boolean): void;
  getString(name: string): string;
  setString(name: string, value: string): void;
  getStringMaxLength(name: string): number;
  getEnum(name: string): string;
  setEnum(name: string, value: string): void;
  getEnumEntries(name: string): string[];
  getEnumIntValue(name: string): number;
  setEnumIntValue(name: string, value: number): void;
  executeCommand(name: string): boolean;
  isCommandDone(name: string): boolean;
  getCategoryFeatures(name: string): string[];
  isSelector(name: string): boolean;
  getSelectedFeatures(name: string): string[];
  getSelectingFeatures(name: string): string[];
  getRegisterLength(name: string): number;
  getRegisterAddress(name: string): number;
  readRegister(name: string): Buffer;
  writeRegister(name: string, data: Buffer): void;
  registerCallback(name: string): number;
  getCallbackCount(handle: number): number;
  deregisterCallback(handle: number): void;
  grab(): GrabResult;
  // Grab loop (mirrors pypylon, synchronous with timeout).
  startGrabbing(strategy?: GrabStrategyValue | StartGrabbingOptions, grabLoop?: GrabLoopValue): void;
  startGrabbingMax(maxImages: number, strategy?: GrabStrategyValue, grabLoop?: GrabLoopValue): void;
  stopGrabbing(): void;
  isGrabbing(): boolean;
  /** Blocks until timeoutMs; null on timeout with Return handling. Throws when not grabbing. */
  retrieveResult(timeoutMs?: number, timeoutHandling?: TimeoutHandlingValue): RetrieveResult | null;
  /** Same acquisition as retrieveResult but returns the GrabResult wrapper. */
  retrieveGrabResult(timeoutMs?: number, timeoutHandling?: TimeoutHandlingValue): GrabResult | null;
  /** Single grab without an explicit grab loop. Must not be called while grabbing. */
  grabOne(timeoutMs?: number, timeoutHandling?: TimeoutHandlingValue): RetrieveResult | null;
  waitForFrameTriggerReady(timeoutMs?: number, timeoutHandling?: TimeoutHandlingValue): boolean;
  executeSoftwareTrigger(): void;
  /** Returns a numeric handler id; mode defaults to Append. */
  registerConfiguration(handler: ConfigurationHandler, mode?: RegistrationModeValue): number;
  deregisterConfiguration(idOrHandler: number | ConfigurationHandler): boolean;
  registerImageEventHandler(handler: ImageEventHandler, mode?: RegistrationModeValue): number;
  deregisterImageEventHandler(idOrHandler: number | ImageEventHandler): boolean;
  registerCameraEventHandler(handler: CameraEventHandler, nodeName?: string, mode?: RegistrationModeValue): number;
  deregisterCameraEventHandler(idOrHandler: number | CameraEventHandler): boolean;
  getGrabResultWaitObject(): WaitObject;
  GetGrabResultWaitObject(): WaitObject;
  getGrabStopWaitObject(): WaitObject;
  GetGrabStopWaitObject(): WaitObject;
  /** Attach a custom buffer factory (or null to detach the default). One attach per factory instance. */
  setBufferFactory(factory: BufferFactory | null): void;
  SetBufferFactory(factory: BufferFactory | null): void;
  getBufferFactory(): BufferFactory | null;
  GetBufferFactory(): BufferFactory | null;
}

export declare class InstantCameraArray {
  constructor(size?: number);
  initialize(size: number): this;
  getSize(): number;
  getItem(index: number): InstantCamera;
  [Symbol.iterator](): Iterator<InstantCamera>;
  isPylonDeviceAttached(): boolean;
  isCameraDeviceRemoved(): boolean;
  isOpen(): boolean;
  open(): void;
  close(): void;
  destroyDevice(): void;
  isGrabbing(): boolean;
  startGrabbing(strategy?: GrabStrategyValue, grabLoop?: GrabLoopValue): void;
  stopGrabbing(): void;
  retrieveResult(timeoutMs?: number, timeoutHandling?: TimeoutHandlingValue): RetrieveResult | null;
}

export declare class AcquireContinuousConfiguration {
  static applyConfiguration(camera: InstantCamera): void;
  onOpened(camera: InstantCamera): void;
}

export declare class AcquireSingleFrameConfiguration {
  static applyConfiguration(camera: InstantCamera): void;
  onOpened(camera: InstantCamera): void;
}

export declare class SoftwareTriggerConfiguration {
  static applyConfiguration(camera: InstantCamera): void;
  onOpened(camera: InstantCamera): void;
}

export declare const ConfigurationHelper: {
  disableAllTriggers(camera: InstantCamera): void;
  disableCompression(camera: InstantCamera): void;
  disableGenDC(camera: InstantCamera): void;
  selectRangeComponent(camera: InstantCamera): void;
  probePacketSize(camera: InstantCamera): void;
};

export declare function createAndOpenCamera(): InstantCamera;

// GrabResult: native wrapper over CGrabResultPtr. Plain fields mirror the
// retrieveResult() object shape; methods expose validity, buffer views,
// chunk data and GenDC components.
export declare class GrabResult {
  // Plain data fields mirrored from the native wrapper (property access).
  // isValid/succeeded/grabSucceeded are methods (same names as the fields).
  valid: boolean;
  width: number;
  height: number;
  pixelType: string;
  pixelFormat: string;
  pixelTypeValue: number;
  payloadType: string;
  payloadSize: number;
  buffer: Buffer;
  timestamp: number;
  blockId: number;
  frameNumber: number;
  errorCode?: number;
  errorDescription?: string;
  isValid(): boolean;
  isUnique(): boolean;
  release(): void;
  succeeded(): boolean;
  grabSucceeded(): boolean;
  getWidth(): number;
  getHeight(): number;
  getOffsetX(): number;
  getOffsetY(): number;
  getPaddingX(): number;
  getPaddingY(): number;
  getPixelType(): string;
  getPixelTypeValue(): number;
  getPayloadType(): string;
  getPayloadSize(): number;
  getBufferSize(): number;
  getImageSize(): number;
  getTimestamp(): number;
  getBlockId(): number;
  getId(): number;
  getImageNumber(): number;
  getSkippedImages(): number;
  getErrorCode(): number;
  getErrorDescription(): string;
  isChunkDataAvailable(): boolean;
  hasCrc(): boolean;
  checkCrc(): boolean;
  getStride(): number | null;
  getBuffer(): Buffer;
  getArray(): Uint8Array;
  getMemoryView(): DataView;
  getDataComponentCount(): number;
  getDataComponent(index: number): DataComponentInfo;
  getDataComponentBuffer(index: number): Buffer;
  toObject(): Record<string, unknown>;
  /** Native buffer-context id from the allocating factory, or null without a custom factory. */
  getBufferContext(): number | null;
  GetBufferContext(): number | null;
  readonly bufferContext: number | null;
  /** Exact attached factory object, or null without a custom factory. */
  getBufferFactory(): BufferFactory | null;
  GetBufferFactory(): BufferFactory | null;
  readonly bufferFactory: BufferFactory | null;
}

export interface DataComponentInfo {
  index: number;
  componentIndex: number;
  componentType: number;
  componentTypeName: string;
  width: number;
  height: number;
  offsetX: number;
  offsetY: number;
  pixelType: string;
  pixelTypeValue: number;
  dataSize: number;
  timestamp: number;
  sourceId: number;
}

// PylonImage: reusable image buffer with persistence.
export declare class PylonImage {
  static create(pixelType: string | number, width: number, height: number, paddingX?: number): PylonImage;
  isValid(): boolean;
  isUnique(): boolean;
  release(): void;
  copyImage(source: PylonImage | GrabResult): void;
  attachGrabResult(grab: GrabResult): void;
  getWidth(): number;
  getHeight(): number;
  getPaddingX(): number;
  getOrientation(): 'TopDown' | 'BottomUp';
  getPixelType(): string;
  getPixelTypeValue(): number;
  getImageSize(): number;
  getAllocatedBufferSize(): number;
  getStride(): number | null;
  isSupportedPixelType(pixelType: string | number): boolean;
  getBuffer(): Buffer;
  getArray(): Uint8Array;
  getMemoryView(): DataView;
  save(format: ImageFileFormatName, filename: string): void;
  load(filename: string): void;
  canSaveWithoutConversion(format: ImageFileFormatName): boolean;
  toObject(): Record<string, unknown>;
}

export type ImageFileFormatName = 'Bmp' | 'Tiff' | 'Jpeg' | 'Png' | 'Raw' | 'Dng';

// ImageFormatConverter: pixel-format conversion.
export declare class ImageFormatConverter {
  setOutputPixelFormat(pixelType: string | number): void;
  getOutputPixelFormat(): string;
  setOutputBitAlignment(alignment: 'LsbAligned' | 'MsbAligned'): void;
  getOutputBitAlignment(): 'LsbAligned' | 'MsbAligned';
  convert(destination: PylonImage, source: PylonImage | GrabResult): void;
  getBufferSizeForConversion(source: PylonImage | GrabResult): number;
  imageHasDestinationFormat(source: PylonImage | GrabResult): boolean;
  static isSupportedInputFormat(pixelType: string | number): boolean;
  static isSupportedOutputFormat(pixelType: string | number): boolean;
}

export interface CompressionInfo {
  hasCompressedImage: boolean;
  lossy: boolean;
  width: number;
  height: number;
  offsetX: number;
  offsetY: number;
  decompressedImageSize: number;
  decompressedPayloadSize: number;
  compressionStatus: number;
}

// ImageDecompressor: compression info and decompression.
export declare class ImageDecompressor {
  describe(source: PylonImage | GrabResult): CompressionInfo;
  decompress(destination: PylonImage, source: PylonImage | GrabResult): CompressionInfo;
  static getCompressionMode(): string;
}

export declare const PixelType: Record<string, number>;
export declare const PayloadType: Record<string, number>;
export declare const ImageFileFormat: Record<ImageFileFormatName, ImageFileFormatName>;
export declare function getPixelTypeByName(name: string): number;
export declare function getNameByPixelType(value: number): string;
export declare function isMono(pixelType: string | number): boolean;
export declare function isColor(pixelType: string | number): boolean;
export declare function isPacked(pixelType: string | number): boolean;

export interface TlFactoryInstance {
  GetInstance(): TlFactoryInstance;
  getInstance(): TlFactoryInstance;
  /** Historical plain-object device list (unchanged behavior). */
  enumerateDevices(): PylonDeviceInfo[];
  /** DeviceInfo instances, optional DeviceInfo/dict filter (pypylon parity). */
  EnumerateDevices(filter?: DeviceInfo | Record<string, string> | Array<DeviceInfo | Record<string, string>>): DeviceInfo[];
  /** Transport-layer infos (pypylon EnumerateTls parity, array not tuple). */
  EnumerateTls(): TlInfo[];
  enumerateTls(): TlInfo[];
  /** TransportLayer by device class (null when unknown) or TlInfo. */
  CreateTl(deviceClassOrInfo: string | TlInfo): TransportLayer | null;
  createTl(deviceClassOrInfo: string | TlInfo): TransportLayer | null;
  ReleaseTl(tl: TransportLayer): void;
  releaseTl(tl: TransportLayer): void;
  CreateDevice(info: DeviceInfo | Record<string, string>): PylonDevice;
  createDevice(info: DeviceInfo | Record<string, string>): PylonDevice;
  /** PylonDevice (pypylon parity); throws without a detected camera. */
  CreateFirstDevice(info?: DeviceInfo | Record<string, string>): PylonDevice;
  createFirstDevice(info?: DeviceInfo | Record<string, string>): PylonDevice;
  DestroyDevice(device: PylonDevice): void;
  destroyDevice(device: PylonDevice): void;
  IsDeviceAccessible(info: DeviceInfo | Record<string, string>, mode?: number): boolean;
  isDeviceAccessible(info: DeviceInfo | Record<string, string>, mode?: number): boolean;
  /** [accessible, state] (tuple parity: array, not tuple). */
  IsDeviceAccessibleInfo(info: DeviceInfo | Record<string, string>, mode?: number): [boolean, number];
  isDeviceAccessibleInfo(info: DeviceInfo | Record<string, string>, mode?: number): [boolean, number];
}

export declare const TlFactory: TlFactoryInstance;

// --- Infra APIs (buffer factory, wait objects, feature persistence,
// static defect pixel correction, type mappings) ---------------------------

/** malloc-backed custom buffer factory with allocation statistics. */
export declare class BufferFactory {
  /** Buffers allocated through this factory (SDK grab engine). */
  allocationCount(): number;
  /** Buffers freed through this factory. */
  freeCount(): number;
  /** Payload size of the most recent allocation. */
  lastAllocationSize(): number;
  /** True once pylon destroyed the factory (replace/detach/camera destroy). */
  isReleased(): boolean;
}

/** Platform wait object (single event). Default-constructed objects are invalid. */
export declare class WaitObject {
  static sleep(ms: number): void;
  static Sleep(ms: number): void;
  isValid(): boolean;
  IsValid(): boolean;
  /** Blocks up to timeoutMs; true when signaled, false on timeout. */
  wait(timeoutMs: number): boolean;
  Wait(timeoutMs: number): boolean;
  /** Interruptible wait; returns a WaitExResult code. */
  waitEx(timeoutMs: number, alertable?: boolean): number;
  WaitEx(timeoutMs: number, alertable?: boolean): number;
}

/** Container for multi-object waits. */
export declare class WaitObjects {
  /** Adds a wait object; returns its index. */
  add(waitObject: WaitObject): number;
  Add(waitObject: WaitObject): number;
  removeAll(): void;
  RemoveAll(): void;
  waitForAll(timeoutMs: number): boolean;
  WaitForAll(timeoutMs: number): boolean;
  waitForAny(timeoutMs: number, withIndex?: null | true): boolean | [boolean, number];
  WaitForAny(timeoutMs: number, withIndex?: null | true): boolean | [boolean, number];
  waitForAllEx(timeoutMs: number, alertable?: boolean): number;
  WaitForAllEx(timeoutMs: number, alertable?: boolean): number;
  waitForAnyEx(timeoutMs: number, alertable?: boolean, withIndex?: null | true): number | [number, number];
  WaitForAnyEx(timeoutMs: number, alertable?: boolean, withIndex?: null | true): number | [number, number];
}

export declare const WaitExResult: {
  readonly Timeout: number;
  readonly Signaled: number;
  readonly Abandoned: number;
  readonly Alerted: number;
};
export declare const waitex_timeout: number;
export declare const waitex_signaled: number;
export declare const waitex_abandoned: number;
export declare const waitex_alerted: number;
export declare const waitForever: number;

/** Feature persistence against the open camera's nodemap. */
export declare const FeaturePersistence: {
  save(file: string, camera: InstantCamera): void;
  Save(file: string, camera: InstantCamera): void;
  load(file: string, camera: InstantCamera, validate?: boolean): void;
  Load(file: string, camera: InstantCamera, validate?: boolean): void;
  saveToString(camera: InstantCamera): string;
  SaveToString(camera: InstantCamera): string;
  loadFromString(features: string, camera: InstantCamera, validate?: boolean): void;
  LoadFromString(features: string, camera: InstantCamera, validate?: boolean): void;
};

export interface DefectPixel {
  x: number;
  y: number;
  type: number;
}
export type DefectPixelEntry = [number, number] | [number, number, number] | DefectPixel;

/** Static defect pixel correction (ace 2 cameras with user-list support). */
export declare const StaticDefectPixelCorrection: {
  readonly ListType_Factory: number;
  readonly ListType_User: number;
  getDefectPixelList(camera: InstantCamera, listType?: number): [boolean, StaticDefectPixelVector];
  GetDefectPixelList(camera: InstantCamera, listType?: number): [boolean, StaticDefectPixelVector];
  setDefectPixelList(camera: InstantCamera, pixels: DefectPixelEntry[] | StaticDefectPixelVector, listType?: number): [boolean, StaticDefectPixelVector];
  SetDefectPixelList(camera: InstantCamera, pixels: DefectPixelEntry[] | StaticDefectPixelVector, listType?: number): [boolean, StaticDefectPixelVector];
  normalizePixelList(camera: InstantCamera, pixels: DefectPixelEntry[] | StaticDefectPixelVector): [boolean, StaticDefectPixelVector];
  NormalizePixelList(camera: InstantCamera, pixels: DefectPixelEntry[] | StaticDefectPixelVector): [boolean, StaticDefectPixelVector];
};

export declare class StaticDefectPixel {
  constructor();
  constructor(x: number, y: number, type?: number);
  X: number;
  x: number;
  Y: number;
  y: number;
  Type: number;
  type: number;
}

export declare class StaticDefectPixelVector {
  readonly length: number;
  size(): number;
  capacity(): number;
  empty(): boolean;
  reserve(n: number): void;
  resize(n: number): void;
  clear(): void;
  swap(other: StaticDefectPixelVector): void;
  at(index: number): StaticDefectPixel;
  push(pixel: DefectPixelEntry | StaticDefectPixel): void;
  toArray(): DefectPixel[];
  [Symbol.iterator](): Iterator<DefectPixel>;
}

export declare const StaticDefectPixelType_Reserved: number;

/** DeviceInfo with settable string properties (String_t mapping). */
export declare class DeviceInfo {
  constructor();
  constructor(other: DeviceInfo);
  FriendlyName: string;
  FullName: string;
  VendorName: string;
  DeviceClass: string;
  TLType: string;
  SerialNumber: string;
  UserDefinedName: string;
  ModelName: string;
  DeviceVersion: string;
  DeviceFactory: string;
  XMLSource: string;
  InterfaceID: string;
  Address: string;
  IpAddress: string;
  SubnetAddress: string;
  DefaultGateway: string;
  SubnetMask: string;
  PortNr: string;
  MacAddress: string;
  Interface: string;
  IpConfigOptions: string;
  IpConfigCurrent: string;
  PortID: string;
  DeviceID: string;
  InitialBaudRate: string;
  DeviceXMLFileOverride: string;
  DeviceSpecificString: string;
  PortSpecificString: string;
  readonly DeviceGUID: string;
  readonly ManufacturerInfo: string;
  readonly DeviceIdx: string;
  readonly ProductId: string;
  readonly VendorId: string;
  readonly DriverKeyName: string;
  readonly UsbDriverType: string;
  readonly TransferMode: string;
  /** camelCase aliases for the discovery fields. */
  friendlyName: string;
  fullName: string;
  vendorName: string;
  deviceClass: string;
  serialNumber: string;
  modelName: string;
  tlType: string;
  GetFriendlyName(): string;
  SetFriendlyName(value: string): void;
  IsFriendlyNameAvailable(): boolean;
  // (Same Get/Set/IsAvailable triplets exist for every property above.)
  /** Returns [count, names] (StringList_t mapping). */
  GetPropertyNames(): [number, string[]];
  GetPropertyAvailable(name: string): boolean;
  /** Returns [found, value] (tuple parity). */
  GetPropertyValue(name: string): [boolean, string];
  SetPropertyValue(name: string, value: string): void;
  IsSubset(other: DeviceInfo): boolean;
  IsUserProvided(): boolean;
  GetPropertyNotAvailable(): string;
  static GetPropertyNotAvailable(): string;
  /** operator== parity. */
  equals(other: DeviceInfo): boolean;
  /** CDeviceInfo::operator< parity. */
  lessThan(other: DeviceInfo): boolean;
  keys(): string[];
  values(): string[];
  items(): Array<[string, string]>;
  toDict(): Record<string, string>;
  update(dict: Record<string, string>): void;
  IsPersistentIpActive(): boolean;
  IsDhcpActive(): boolean;
  IsAutoIpActive(): boolean;
  IsPersistentIpSupported(): boolean;
  IsDhcpSupported(): boolean;
  IsAutoIpSupported(): boolean;
}

/** TlInfo (transport-layer info, CInfoBase + FileName/InfoID/ModelName/Version). */
export declare class TlInfo {
  constructor();
  constructor(other: TlInfo);
  FriendlyName: string;
  FullName: string;
  VendorName: string;
  DeviceClass: string;
  TLType: string;
  FileName: string;
  InfoID: string;
  ModelName: string;
  Version: string;
  GetPropertyNames(): [number, string[]];
  GetPropertyAvailable(name: string): boolean;
  GetPropertyValue(name: string): [boolean, string];
  SetPropertyValue(name: string, value: string): void;
  IsSubset(other: TlInfo): boolean;
  IsUserProvided(): boolean;
  GetPropertyNotAvailable(): string;
  static GetPropertyNotAvailable(): string;
  equals(other: TlInfo): boolean;
  lessThan(other: TlInfo): boolean;
  keys(): string[];
  values(): string[];
  items(): Array<[string, string]>;
  toDict(): Record<string, string>;
  update(dict: Record<string, string>): void;
}

/** InterfaceInfo (CInfoBase + InterfaceID). */
export declare class InterfaceInfo {
  constructor();
  constructor(other: InterfaceInfo);
  FriendlyName: string;
  FullName: string;
  VendorName: string;
  DeviceClass: string;
  TLType: string;
  InterfaceID: string;
  GetPropertyNames(): [number, string[]];
  GetPropertyAvailable(name: string): boolean;
  GetPropertyValue(name: string): [boolean, string];
  SetPropertyValue(name: string, value: string): void;
  IsSubset(other: InterfaceInfo): boolean;
  IsUserProvided(): boolean;
  GetPropertyNotAvailable(): string;
  static GetPropertyNotAvailable(): string;
  equals(other: InterfaceInfo): boolean;
  /** CInterfaceInfo::operator< parity. */
  lessThan(other: InterfaceInfo): boolean;
  keys(): string[];
  values(): string[];
  items(): Array<[string, string]>;
  toDict(): Record<string, string>;
  update(dict: Record<string, string>): void;
}

/** IDeviceFactory base (TlFactory/TransportLayer/Interface derive from it). */
export declare class IDeviceFactory {}

/** Transport-layer handle (CreateTl/ReleaseTl parity). */
export declare class TransportLayer extends IDeviceFactory {
  getTlInfo(): TlInfo;
  enumerateDevices(filter?: DeviceInfo | Record<string, string> | Array<DeviceInfo | Record<string, string>>): DeviceInfo[];
  enumerateInterfaces(): InterfaceInfo[];
  createInterface(info: InterfaceInfo): Interface;
  destroyInterface(iface: Interface): void;
  createDevice(info: DeviceInfo | Record<string, string>): PylonDevice;
  createFirstDevice(info?: DeviceInfo | Record<string, string>): PylonDevice;
  destroyDevice(device: PylonDevice): void;
  isDeviceAccessible(info: DeviceInfo | Record<string, string>, mode?: number): boolean;
  isDeviceAccessibleInfo(info: DeviceInfo | Record<string, string>, mode?: number): [boolean, number];
  /** GenApi TL nodemap node count (null-safe, CamEmu has nodes). */
  getNodeMapNumNodes(): number;
  release(): void;
}

/** Interface handle (frame grabber / network card). */
export declare class Interface extends IDeviceFactory {
  getInterfaceInfo(): InterfaceInfo;
  enumerateDevices(filter?: DeviceInfo | Record<string, string> | Array<DeviceInfo | Record<string, string>>): DeviceInfo[];
  open(): void;
  close(): void;
  isOpen(): boolean;
  /** 0 when the interface exposes no nodemap (CamEmu default interface). */
  getNodeMapNumNodes(): number;
  createDevice(info: DeviceInfo | Record<string, string>): PylonDevice;
  createFirstDevice(info?: DeviceInfo | Record<string, string>): PylonDevice;
  destroyDevice(device: PylonDevice): void;
  isDeviceAccessible(info: DeviceInfo | Record<string, string>, mode?: number): boolean;
  isDeviceAccessibleInfo(info: DeviceInfo | Record<string, string>, mode?: number): [boolean, number];
  destroy(): void;
}

/** Low-level device handle (CreateDevice/DestroyDevice parity). */
export declare class PylonDevice {
  getDeviceInfo(): DeviceInfo;
  open(mode?: number): void;
  close(): void;
  isOpen(): boolean;
  destroy(): void;
}

// Discovery constants (device classes, TL types, property keys,
// device access modes, accessibility states).
export declare const BaslerCamEmuDeviceClass: string;
export declare const BaslerIpCamDeviceClass: string;
export declare const BaslerCameraLinkDeviceClass: string;
export declare const BaslerGigEDeviceClass: string;
export declare const BaslerGenTlDeviceClassPrefix: string;
export declare const BaslerGenTlGevDeviceClass: string;
export declare const BaslerGenTlU3vDeviceClass: string;
export declare const BaslerGenTlCxpDeviceClass: string;
export declare const BaslerGenTlBlazeDeviceClass: string;
export declare const BaslerGenTlStaDeviceClass: string;
export declare const BaslerUsbDeviceClass: string;
export declare const TLTypeGigE: string;
export declare const TLTypeUSB: string;
export declare const TLTypeCL: string;
export declare const TLTypeCamEmu: string;
export declare const TLTypeCXP: string;
export declare const TLTypeCustom: string;
export declare const FriendlyNameKey: string;
export declare const FullNameKey: string;
export declare const VendorNameKey: string;
export declare const DeviceClassKey: string;
export declare const TLTypeKey: string;
export declare const InterfaceIDKey: string;
export declare const SerialNumberKey: string;
export declare const UserDefinedNameKey: string;
export declare const ModelNameKey: string;
export declare const DeviceVersionKey: string;
export declare const DeviceFactoryKey: string;
export declare const XMLSourceKey: string;
export declare const ManufacturerInfoKey: string;
export declare const DeviceIDKey: string;
export declare const AddressKey: string;
export declare const IpAddressKey: string;
export declare const DefaultGatewayKey: string;
export declare const SubnetMaskKey: string;
export declare const SubnetAddressKey: string;
export declare const PortNrKey: string;
export declare const MacAddressKey: string;
export declare const InterfaceKey: string;
export declare const IpConfigOptionsKey: string;
export declare const IpConfigCurrentKey: string;
export declare const DeviceGUIDKey: string;
export declare const VendorIdKey: string;
export declare const ProductIdKey: string;
export declare const DriverKeyNameKey: string;
export declare const UsbDriverTypeKey: string;
export declare const UsbPortVersionBcdKey: string;
export declare const SpeedSupportBitmaskKey: string;
export declare const TransferModeKey: string;
export declare const PortIDKey: string;
export declare const InitialBaudRateKey: string;
export declare const DeviceXMLFileOverrideKey: string;
export declare const DeviceSpecificStringKey: string;
export declare const PortSpecificStringKey: string;
export declare const DeviceIdxKey: string;
export declare const FileNameKey: string;
export declare const InfoIDKey: string;
export declare const VersionKey: string;
export declare const DeviceAccessMode_Control: number;
export declare const DeviceAccessMode_Stream: number;
export declare const DeviceAccessMode_Event: number;
export declare const DeviceAccessMode_Exclusive: number;
export declare const Accessibility_Unknown: number;
export declare const Accessibility_Ok: number;
export declare const Accessibility_Opened: number;
export declare const Accessibility_OpenedExclusively: number;
export declare const Accessibility_NotReachable: number;

/** GenICam exception hierarchy (identical objects on pylon and genicam). */
export declare class GenericException extends Error {}
export declare class BadAllocException extends GenericException {}
export declare class InvalidArgumentException extends GenericException {}
export declare class OutOfRangeException extends GenericException {}
export declare class PropertyException extends GenericException {}
export declare class RuntimeException extends GenericException {}
export declare class LogicalErrorException extends GenericException {}
export declare class AccessException extends GenericException {}
export declare class TimeoutException extends GenericException {}
export declare class DynamicCastException extends GenericException {}
export declare const genicam: {
  GenericException: typeof GenericException;
  BadAllocException: typeof BadAllocException;
  InvalidArgumentException: typeof InvalidArgumentException;
  OutOfRangeException: typeof OutOfRangeException;
  PropertyException: typeof PropertyException;
  RuntimeException: typeof RuntimeException;
  LogicalErrorException: typeof LogicalErrorException;
  AccessException: typeof AccessException;
  TimeoutException: typeof TimeoutException;
  DynamicCastException: typeof DynamicCastException;
};
