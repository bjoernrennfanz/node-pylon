/**
 * Codegen spec for the Pylon TurboModule.
 * `npx @react-native-community/cli codegen` generates the per-platform
 * `AppSpecsJSI.h` scaffolds from it; the adapters under windows/macos/linux
 * only implement JSI marshalling and delegate to pylon_core_*.
 *
 * Metro suffixes: optionally place `NativePylon.windows.js` / `.macos.js` next
 * to this file if platform-specific JS adjustments are needed
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
}

export interface Spec extends TurboModule {
  getVersion(): string;
  getPylonVersion(): string;
  isMock(): boolean;
  enumerateDevices(): Array<DeviceInfo>;
  // Sync mock for the sample; the real grab becomes Promise<GrabResult>
  // (CallInvoker thread, never blocks the JS loop).
  grabMock(width: number, height: number): string;
}

const NativePylon = TurboModuleRegistry.getEnforcing<Spec>('NativePylon');

export default NativePylon;
