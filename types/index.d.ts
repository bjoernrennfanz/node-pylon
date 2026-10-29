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
  succeeded: boolean;
  width: number;
  height: number;
  /** pypylon naming; mirrors pixelFormat. */
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
  /** Sets Integer/Float to max and returns the value set. Throws for other types. */
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
  // Grab loop (mirrors pypylon, synchronous with timeout).
  startGrabbing(): void;
  stopGrabbing(): void;
  isGrabbing(): boolean;
  /** Blocks until timeoutMs; null on timeout. Throws when not grabbing. */
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
