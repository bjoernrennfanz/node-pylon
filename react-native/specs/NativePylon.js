/**
 * Codegen spec for the Pylon TurboModule (node-pylon API stand).
 * `npx @react-native-community/cli codegen` generates the per-platform
 * `AppSpecsJSI.h` scaffolds from it; the adapters under windows/macos/linux
 * only implement JSI marshalling and delegate to pylon_core_* in
 * react-native/shared/ (no double wrapping).
 *
 * Only codegen-compatible types are used here: string / number / boolean /
 * Array / Object. Grabbed images cross the bridge as Object
 * `{succeeded,width,height,pixelType,payloadSize,timestamp,blockId,buffer}`.
 * `buffer` is a Uint8Array (JSI ArrayBuffer, zero-copy from the core's
 * GrabImage::buffer) — bridges that cannot move an ArrayBuffer fall back
 * to a base64 string, which the JS facade decodes (see src/index.ts).
 *
 * Metro suffixes: optionally place `NativePylon.windows.js` / `.macos.js`
 * next to this file if platform-specific JS adjustments are needed
 * (see docs/rn-architecture.md, rnpm.haste.platforms).
 *
 * @format
 */

import type {TurboModule} from 'react-native';
import {TurboModuleRegistry} from 'react-native';

export interface DeviceInfo {
  friendlyName?: string;
  modelName?: string;
  serialNumber?: string;
  vendorName?: string;
  deviceClass?: string;
}

export interface GrabImage {
  succeeded?: boolean;
  width?: number;
  height?: number;
  pixelType?: string;
  payloadSize?: number;
  timestamp?: number;
  blockId?: number;
}

export interface Spec extends TurboModule {
  // Version / discovery (as before).
  getVersion(): string;
  getPylonVersion(): string;
  enumerateDevices(): Array<DeviceInfo>;
  // Pixel-type helpers.
  getPixelTypeName(pixelTypeValue: number): string;
  isMonoPixelType(pixelTypeValue: number): boolean;
  isColorPixelType(pixelTypeValue: number): boolean;
  // Camera lifecycle. Returns an opaque core handle (int); the
  // CInstantCamera never crosses the bridge. accessMode: 1 Control
  // (default), 3 Stream, 4 Event, 5 Exclusive (validated like Node).
  openCamera(deviceIndex: number, accessMode: number): number;
  closeCamera(handle: number): void;
  isCameraOpen(handle: number): boolean;
  // Grab loop. strategy: 0 OneByOne, 1 LatestImageOnly, 2 LatestImages,
  // 3 UpcomingImage (see GrabStrategy in src/index.ts). timeoutHandling:
  // 0 Return (timeout -> {succeeded:false}), 1 ThrowException.
  startGrabbing(handle: number, strategy: number, maxImages: number): void;
  stopGrabbing(handle: number): void;
  isGrabbing(handle: number): boolean;
  grabOne(handle: number, timeoutMs: number, timeoutHandling: number): Object;
  retrieveResult(
    handle: number,
    timeoutMs: number,
    timeoutHandling: number
  ): Object;
  // Nodemap basis (GenICam types like the Node InstantCamera facade).
  getNodeType(handle: number, name: string): string;
  isReadable(handle: number, name: string): boolean;
  isWritable(handle: number, name: string): boolean;
  isAvailable(handle: number, name: string): boolean;
  getInteger(handle: number, name: string): number;
  setInteger(handle: number, name: string, value: number): void;
  getFloat(handle: number, name: string): number;
  setFloat(handle: number, name: string, value: number): void;
  getBoolean(handle: number, name: string): boolean;
  setBoolean(handle: number, name: string, value: boolean): void;
  getString(handle: number, name: string): string;
  setString(handle: number, name: string, value: string): void;
  getEnum(handle: number, name: string): string;
  setEnum(handle: number, name: string, value: string): void;
  getEnumEntries(handle: number, name: string): Array<string>;
  executeCommand(handle: number, name: string): void;
  // Nodemap properties (GenICam metadata, like the Node InstantCamera).
  getIntegerMin(handle: number, name: string): number;
  getIntegerMax(handle: number, name: string): number;
  getIntegerInc(handle: number, name: string): number;
  getIntegerUnit(handle: number, name: string): string;
  getIntegerRepresentation(handle: number, name: string): string;
  setIntegerToMin(handle: number, name: string): void;
  setIntegerToMax(handle: number, name: string): void;
  tryMax(handle: number, name: string): number;
  getFloatMin(handle: number, name: string): number;
  getFloatMax(handle: number, name: string): number;
  hasFloatInc(handle: number, name: string): boolean;
  getFloatInc(handle: number, name: string): number;
  getFloatUnit(handle: number, name: string): string;
  getFloatRepresentation(handle: number, name: string): string;
  setFloatToMin(handle: number, name: string): void;
  setFloatToMax(handle: number, name: string): void;
  getDisplayName(handle: number, name: string): string;
  getDescription(handle: number, name: string): string;
  getValueAsString(handle: number, name: string): string;
  setValueFromString(handle: number, name: string, value: string): void;
  getStringMaxLength(handle: number, name: string): number;
  getEnumIntValue(handle: number, name: string): number;
  setEnumIntValue(handle: number, name: string, value: number): void;
  isCommandDone(handle: number, name: string): boolean;
  getCategoryFeatures(handle: number, name: string): Array<string>;
  isSelector(handle: number, name: string): boolean;
  getSelectedFeatures(handle: number, name: string): Array<string>;
  getSelectingFeatures(handle: number, name: string): Array<string>;
  // Register access. Bytes cross the bridge base64-encoded (codegen-safe);
  // the JS facade exposes Uint8Array (encode/decode both ways).
  getRegisterLength(handle: number, name: string): number;
  getRegisterAddress(handle: number, name: string): number;
  readRegister(handle: number, name: string): string;
  writeRegister(handle: number, name: string, dataBase64: string): void;
  // Camera device handling (like the Node InstantCamera facade).
  getDeviceInfo(handle: number): Object;
  isPylonDeviceAttached(handle: number): boolean;
  hasOwnership(handle: number): boolean;
  attachDevice(handle: number, deviceIndex: number): void;
  detachDevice(handle: number): void;
  destroyDevice(handle: number): void;
  isCameraDeviceRemoved(handle: number): boolean;
  getCameraContext(handle: number): number;
  setCameraContext(handle: number, context: number): void;
  getQueuedBufferCount(handle: number): number;
  // Discovery constants from the SDK (device classes, TL types, property
  // keys, access modes, accessibility) as {strings, integers} — values
  // come from the native layer so JS and C++ can never drift apart.
  getDiscoveryConstants(): Object;
}

const NativePylon = TurboModuleRegistry.getEnforcing<Spec>('NativePylon');

export default NativePylon;
