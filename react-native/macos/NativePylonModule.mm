// macOS adapter (react-native-macos, ObjC++/JSI, Fabric/TurboModule).
// Pure marshalling layer — logic lives in ../shared/pylon_core.{h,cpp}.
#if 0
#import <Foundation/Foundation.h>
#import <React/RCTBridgeModule.h>
// Codegen protocol: NativePylonSpec (generated). Implementation:
//   - (NSString *)getVersion;              -> core::kVersion
//   - (NSString *)getPylonVersion;         -> core::PylonVersion()
//   - (BOOL)isMock;                        -> YES in mock (SDK check in real build)
//   - (NSArray *)enumerateDevices;          -> core::EnumerateDevices()
//   - (NSString *)grabMock:(double)w h:(double)h; -> base64(core::GrabMockFrame)
// Registration via codegen (RCT_EXPORT_MODULE for legacy,
// TurboModule provider for the new architecture). No bridge JSON,
// JSI instead of the legacy bridge.
#endif
