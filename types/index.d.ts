// TypeScript-Typen zur JS-Fassade (lib/index.js). Analog pypylon-API, MVP ohne DataProcessing.
export declare function version(): string;
export declare function pylonVersion(): string;
export declare function isMock(): boolean;

export interface PylonDeviceInfo {
  friendlyName?: string;
  modelName?: string;
  serialNumber?: string;
  [key: string]: unknown;
}

export declare function enumerateDevices(): PylonDeviceInfo[];

/** Synthetischer Mono8-Frame (width*height Bytes). Nur Mock/Scaffold; echter Grab folgt in ZAU-150/151. */
export declare function grabMock(width?: number, height?: number): Buffer;
