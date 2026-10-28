/**
 * Codegen-Spec fuer das Pylon-TurboModule (ZAU-153).
 * `npx @react-native-community/cli codegen` erzeugt daraus pro Plattform
 * die `AppSpecsJSI.h`-Scaffolds; die Adapter unter windows/macos/linux
 * implementieren nur JSI-Marshalling und delegieren an pylon_core_*.
 *
 * Metro-Suffixe: optional `NativePylon.windows.js` / `.macos.js` neben
 * dieser Datei ablegen, falls plattformspezifische JS-Anpassungen noetig
 * (siehe docs/rn-architecture.md, rnpm.haste.platforms).
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
  // Sync-Mock fuer das Sample; echter Grab wird Promise<GrabResult> (ZAU-150/151,
  // CallInvoker-Thread, kein Block des JS-Loops).
  grabMock(width: number, height: number): string;
}

const NativePylon = TurboModuleRegistry.getEnforcing<Spec>('NativePylon');

export default NativePylon;
