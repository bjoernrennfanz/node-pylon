// Linux-Adapter (Community-Port react-native-linux, JSI/TurboModule).
// Reine Marshalling-Schicht — Logik liegt in ../shared/pylon_core.{h,cpp}.
// Falls der evaluierte Linux-Fork kein Codegen/TurboModules traegt:
// Fallback Electron + Node-Addon (build/Release/node_pylon.node) aus diesem
// Repo nutzen (siehe docs/rn-architecture.md). Kein Doppel-Wrapping:
// auch hier nur pylon_core_* aufrufen, kein zweiter Grab-Pfad.
#pragma once
// #include "../shared/pylon_core.h"
// namespace node_pylon::rn::linux {
// bool RegisterPylonTurboModule(/* facebook::jsi::Runtime&, CallInvoker */);
// }  // namespace node_pylon::rn::linux
