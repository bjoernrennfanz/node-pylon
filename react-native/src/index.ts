// JS facade for React Native (TurboModule, real pylon SDK only).
// Requires the native module; without it, calls throw instead of falling
// back to fake values.
import {TurboModuleRegistry} from 'react-native';
import type {Spec, DeviceInfo} from '../../specs/NativePylon';

function getModule(): Spec {
  return TurboModuleRegistry.getEnforcing<Spec>('NativePylon');
}

export function version(): string {
  return getModule().getVersion();
}

export function pylonVersion(): string {
  return getModule().getPylonVersion();
}

export function enumerateDevices(): DeviceInfo[] {
  return getModule().enumerateDevices();
}
