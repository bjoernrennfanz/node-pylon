// Shared C++ core for the Node addon (N-API) and the RN JSI/TurboModule.
// Pure C++17, NO N-API/JSI/V8 includes here — this file is written once
// and only adapted by both bridges (no double wrapping).
// Mock state without pylon SDK/camera; wire a real build behind
// NODE_PYLON_USE_REAL (PYLON_ROOT).
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

inline const char* kVersion = "0.1.0-rn-mock";
inline const char* kPylonVersionMock = "mock-8.x";

// Mock: empty list (no camera). Real: CTlFactory::EnumerateDevices.
std::vector<DeviceInfo> EnumerateDevices();

// Mock: Mono8 gradient w*h bytes. Real: grab result buffer, zero-copy.
// Returns false for invalid dimensions (0 or > 4096).
bool GrabMockFrame(uint32_t width, uint32_t height, std::vector<uint8_t>& out);

const char* PylonVersion();

}  // namespace core
}  // namespace node_pylon
