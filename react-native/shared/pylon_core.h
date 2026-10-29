// Shared C++ core for the Node addon (N-API) and the RN JSI/TurboModule.
// Pure C++17, NO N-API/JSI/V8 includes here — this file is written once
// and only adapted by both bridges (no double wrapping).
// Real pylon SDK only: device enumeration and version come from
// the SDK; callers must have called PylonInitialize (the N-API bridge does
// this per InstantCamera via pylon_lifecycle.h).
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace node_pylon {
namespace core {

// A device mirroring the pylon TlFactory DeviceInfo (MVP fields).
struct DeviceInfo {
  std::string friendly_name;
  std::string model_name;
  std::string serial_number;
};

inline const char* kVersion = "0.2.0-core";

// Real enumeration via the pylon transport layer factory. Empty without a
// camera or emulator (PYLON_CAMEMU=N provides emulator devices).
std::vector<DeviceInfo> EnumerateDevices();

// Real pylon version string from the SDK (e.g. "12.5.2.1565").
const char* PylonVersion();

}  // namespace core
}  // namespace node_pylon
