// macOS-Adapter (react-native-macos, ObjC++/JSI, Fabric/TurboModule).
// Reine Marshalling-Schicht — Logik liegt in ../shared/pylon_core.{h,cpp}.
#if 0
#import <Foundation/Foundation.h>
#import <React/RCTBridgeModule.h>
// Codegen-Protokoll: NativePylonSpec (generiert). Implementierung:
//   - (NSString *)getVersion;              -> core::kVersion
//   - (NSString *)getPylonVersion;         -> core::PylonVersion()
//   - (BOOL)isMock;                        -> YES im Mock (ZAU-150: SDK-Check)
//   - (NSArray *)enumerateDevices;          -> core::EnumerateDevices()
//   - (NSString *)grabMock:(double)w h:(double)h; -> base64(core::GrabMockFrame)
// Registrierung erfolgt ueber Codegen (RCT_EXPORT_MODULE bei Legacy,
// TurboModule-Provider bei New Architecture). Kein Bridge-JSON,
// JSI statt Legacy-Bridge.
#endif
