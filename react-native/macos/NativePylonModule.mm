// macOS adapter (react-native-macos, ObjC++/JSI, Fabric/TurboModule).
// Pure marshalling layer — camera/grab/nodemap logic lives in
// ../shared/pylon_core.{h,cpp}; no second grab path here.
#if 0
#import <Foundation/Foundation.h>
#import <React/RCTBridgeModule.h>
// Codegen protocol: NativePylonSpec (generated). Implementation:
//   Version / discovery:
//   - (NSString *)getVersion;              -> core::kVersion
//   - (NSString *)getPylonVersion;         -> core::PylonVersion()
//   - (NSArray *)enumerateDevices;          -> core::EnumerateDevices()
//   Pixel-type helpers:
//   - (NSString *)getPixelTypeName:(double)v; -> core::PixelTypeName
//   - (BOOL)isMonoPixelType:(double)v;       -> core::IsMonoPixelType
//   - (BOOL)isColorPixelType:(double)v;      -> core::IsColorPixelType
//   Camera lifecycle (opaque int handle; CInstantCamera stays in core):
//   - (double)openCamera:(double)deviceIndex; -> core::OpenCamera
//   - (void)closeCamera:(double)handle;       -> core::CloseCamera
//   - (BOOL)isCameraOpen:(double)handle;
//   Grab loop (background queue via CallInvoker; timeouts honored):
//   - (void)startGrabbing:(double)h strategy:(double)s maxImages:(double)m;
//   - (void)stopGrabbing:(double)h; - (BOOL)isGrabbing:(double)h;
//   - (NSDictionary *)grabOne:(double)h timeoutMs:(double)t timeoutHandling:(double)th;
//     -> core::GrabOne; buffer moved zero-copy into an ArrayBuffer
//        (fallback: base64 string, decoded by the JS facade).
//   - (NSDictionary *)retrieveResult:...;  -> core::RetrieveResult
//   Properties: core::GetIntegerMin/Max/Inc/Unit/Representation,
//   SetIntegerToMin/Max, TryMax, GetFloatMin/Max, HasFloatInc, GetFloatInc,
//   GetFloatUnit/Representation, SetFloatToMin/Max, GetDisplayName,
//   GetDescription, GetValueAsString/SetValueFromString, GetStringMaxLength,
//   GetEnumIntValue/SetEnumIntValue, IsCommandDone, GetCategoryFeatures,
//   IsSelector, GetSelectedFeatures, GetSelectingFeatures,
//   GetRegisterLength/Address, Read/WriteRegister (base64 on the bridge).
//   Device: GetCameraDeviceInfo, IsPylonDeviceAttached, HasOwnership,
//   Attach/Detach/DestroyDevice, IsCameraDeviceRemoved,
//   Get/SetCameraContext, GetQueuedBufferCount, GetDiscoveryConstants.
// Registration via codegen (RCT_EXPORT_MODULE for legacy,
// TurboModule provider for the new architecture). No bridge JSON,
// JSI instead of the legacy bridge.
#endif
