// macOS adapter (react-native-macos, ObjC++/JSI, Fabric/TurboModule).
// Pure marshalling layer — logic lives in ../shared/pylon_core.{h,cpp}.
#if 0
#import <Foundation/Foundation.h>
#import <React/RCTBridgeModule.h>
// Codegen protocol: NativePylonSpec (generated). Implementation:
//   - (NSString *)getVersion;              -> core::kVersion
//   - (NSString *)getPylonVersion;         -> core::PylonVersion()
//   - (NSArray *)enumerateDevices;          -> core::EnumerateDevices()
// Registration via codegen (RCT_EXPORT_MODULE for legacy,
// TurboModule provider for the new architecture). No bridge JSON,
// JSI instead of the legacy bridge.
#endif
