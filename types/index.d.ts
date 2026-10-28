// TypeScript-Typen zur JS-Fassade (lib/index.js). Analog pypylon-API, MVP ohne DataProcessing.
export declare function version(): string;
export declare function pylonVersion(): string;
export declare function isMock(): boolean;
export declare function isNative(): boolean;
/** PylonInitialize (Refcount). Jede initialize()-Referenz braucht ein terminate(). */
export declare function initialize(): void;
/** PylonTerminate (Refcount). */
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
/** TlFactory.CreateFirstDevice — wirft ohne erkannte Kamera eine Exception. */
export declare function createFirstCamera(): InstantCamera;
export declare function createFirst(): InstantCamera;

/** Synthetischer Mono8-Frame (width*height Bytes). Legacy-Helper; neue Codepfade nutzen InstantCamera.grab(). */
export declare function grabMock(width?: number, height?: number): Buffer;

export type NodeTypeName = 'Integer' | 'Float' | 'Boolean' | 'String' | 'Enumeration' | 'Command';

export interface GrabResult {
  buffer: Buffer;
  width: number;
  height: number;
  /** pypylon-Naming (PixelTypeToString); identisch zu pixelFormat im Mock. */
  pixelType: string;
  pixelFormat: string;
  payloadSize: number;
  timestamp: number;
  frameNumber: number;
  chunk: { modeActive: boolean; timestamp: number; frameID: number };
}

/** Ergebnis von retrieveResult (ZAU-150 Grab-Loop). */
export interface RetrieveResult {
  succeeded: boolean;
  width: number;
  height: number;
  /** pypylon-Naming; identisch zu pixelFormat im Mock. */
  pixelType: string;
  pixelFormat: string;
  payloadSize: number;
  buffer: Buffer;
  errorCode?: number;
  errorDescription?: string;
}

export declare class InstantCamera {
  constructor();
  isOpen(): boolean;
  open(): boolean;
  close(): boolean;
  getDeviceInfo(): PylonDeviceInfo;
  getNodeType(name: string): NodeTypeName;
  isReadable(name: string): boolean;
  isWritable(name: string): boolean;
  getInteger(name: string): number;
  setInteger(name: string, value: number): void;
  getIntegerMin(name: string): number;
  getIntegerMax(name: string): number;
  /** Setzt Integer/Float auf Max und gibt den gesetzten Wert zurueck. Wirft bei anderen Typen. */
  tryMax(name: string): number;
  getFloat(name: string): number;
  setFloat(name: string, value: number): void;
  getFloatMin(name: string): number;
  getFloatMax(name: string): number;
  getBoolean(name: string): boolean;
  setBoolean(name: string, value: boolean): void;
  getString(name: string): string;
  setString(name: string, value: string): void;
  getEnum(name: string): string;
  setEnum(name: string, value: string): void;
  getEnumEntries(name: string): string[];
  executeCommand(name: string): boolean;
  grab(): GrabResult;
  // ZAU-150 Grab-Loop (pypylon-nah, synchron mit Timeout).
  startGrabbing(): void;
  stopGrabbing(): void;
  isGrabbing(): boolean;
  /** Blockiert bis timeoutMs; null bei Timeout. Wirft, wenn nicht gegrabbt wird. */
  retrieveResult(timeoutMs?: number): RetrieveResult | null;
}

export declare function createAndOpenCamera(): InstantCamera;

export interface TlFactoryInstance {
  GetInstance(): TlFactoryInstance;
  getInstance(): TlFactoryInstance;
  enumerateDevices(): PylonDeviceInfo[];
  EnumerateDevices(): PylonDeviceInfo[];
  createFirstDevice(): InstantCamera;
  CreateFirstDevice(): InstantCamera;
}

export declare const TlFactory: TlFactoryInstance;
