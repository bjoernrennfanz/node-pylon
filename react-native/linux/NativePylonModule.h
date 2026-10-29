// Linux adapter (community port react-native-linux, JSI/TurboModule).
// Pure marshalling layer — logic lives in ../shared/pylon_core.{h,cpp}.
// If the Linux fork under evaluation does not support codegen/TurboModules:
// fallback to Electron + Node addon (build/Release/node_pylon.node) from this
// repo (see docs/rn-architecture.md). No double wrapping:
// only call pylon_core_* here, no second grab path.
#pragma once
// #include "../shared/pylon_core.h"
// namespace node_pylon::rn::linux {
// bool RegisterPylonTurboModule(/* facebook::jsi::Runtime&, CallInvoker */);
// }  // namespace node_pylon::rn::linux
