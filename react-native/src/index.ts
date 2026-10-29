// JS facade for React Native (TurboModule with mock fallback).
// Usage in the sample: import {enumerateDevices, grabMock} from '../src'.
import {TurboModuleRegistry} from 'react-native';
import type {Spec, DeviceInfo} from '../../specs/NativePylon';

function getModule(): Spec | null {
  try {
    return TurboModuleRegistry.get<Spec>('NativePylon') ?? null;
  } catch {
    return null;
  }
}

export function version(): string {
  return getModule()?.getVersion() ?? '0.1.0-rn-mock';
}

export function pylonVersion(): string {
  return getModule()?.getPylonVersion() ?? 'mock-8.x';
}

export function isMock(): boolean {
  return getModule()?.isMock() ?? true;
}

export function enumerateDevices(): DeviceInfo[] {
  return getModule()?.enumerateDevices() ?? [];
}

/** Mock grab as base64 (works without a camera). Real grab -> TypedArray. */
export function grabMock(width = 64, height = 48): string {
  const m = getModule();
  if (m) return m.grabMock(width, height);
  const bytes = width * height;
  const buf = new Uint8Array(bytes);
  for (let y = 0; y < height; y++)
    for (let x = 0; x < width; x++) buf[y * width + x] = (x + y) & 0xff;
  let bin = '';
  for (let i = 0; i < buf.length; i++) bin += String.fromCharCode(buf[i]);
  // btoa is available in RN/Hermes; Node fallback via Buffer.
  const g = globalThis as {btoa?: (s: string) => string};
  if (typeof g.btoa === 'function') return g.btoa(bin);
  // eslint-disable-next-line @typescript-eslint/no-require-imports
  return (require('buffer') as typeof import('buffer')).Buffer.from(bin, 'binary').toString('base64');
}

export type {DeviceInfo};
