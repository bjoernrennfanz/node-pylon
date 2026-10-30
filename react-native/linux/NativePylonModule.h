// Linux adapter (community port react-native-linux, JSI/TurboModule).
// Pure marshalling layer — camera/grab/nodemap logic lives in
// ../shared/pylon_core.{h,cpp}.
// If the Linux fork under evaluation does not support codegen/TurboModules:
// fallback to Electron + Node addon (build/Release/node_pylon.node) from this
// repo (see docs/rn-architecture.md). No double wrapping:
// only call pylon_core_* here, no second grab path.
#pragma once
// #include "../shared/pylon_core.h"
// namespace node_pylon::rn::linux {
// bool RegisterPylonTurboModule(/* facebook::jsi::Runtime&, CallInvoker */);
//
// Method map (each line = one spec method -> one core call):
//   getVersion/getPylonVersion/enumerateDevices -> core::kVersion /
//     core::PylonVersion() / core::EnumerateDevices()
//   getPixelTypeName/isMonoPixelType/isColorPixelType -> core::PixelTypeName /
//     core::IsMonoPixelType / core::IsColorPixelType
//   openCamera/closeCamera/isCameraOpen -> core::OpenCamera/CloseCamera/
//     core::IsCameraOpen (opaque int handle)
//   startGrabbing/stopGrabbing/isGrabbing -> core::StartGrabbing/
//     core::StopGrabbing/core::IsGrabbing (background thread, timeouts)
//   grabOne/retrieveResult -> core::GrabOne/core::RetrieveResult; buffer
//     moved zero-copy into a jsi::ArrayBuffer (fallback: base64 string,
//     decoded by the JS facade) inside the returned object
//     {succeeded,width,height,pixelType,payloadSize,timestamp,blockId,buffer}
//   Properties: core::GetIntegerMin/Max/Inc/Unit/Representation,
//   SetIntegerToMin/Max, TryMax, GetFloatMin/Max, HasFloatInc, GetFloatInc,
//   GetFloatUnit/Representation, SetFloatToMin/Max, GetDisplayName,
//   GetDescription, GetValueAsString/SetValueFromString, GetStringMaxLength,
//   GetEnumIntValue/SetEnumIntValue, IsCommandDone, GetCategoryFeatures,
//   IsSelector, GetSelectedFeatures, GetSelectingFeatures,
//   GetRegisterLength/Address, Read/WriteRegister (base64 on the bridge).
//   Device: core::GetCameraDeviceInfo, IsPylonDeviceAttached, HasOwnership,
//   Attach/Detach/DestroyDevice, IsCameraDeviceRemoved,
//   Get/SetCameraContext, GetQueuedBufferCount, GetDiscoveryConstants.
// }  // namespace node_pylon::rn::linux
