// Geteilter C++-Core fuer Node-Addon (N-API) und RN-JSI/TurboModule (ZAU-153).
// Reines C++17, KEINE N-API-/JSI-/V8-Includes hier — diese Datei wird einmal
// geschrieben und von beiden Bruecken nur adaptiert (kein Doppel-Wrapping).
// Mock-Stand ohne pylon-SDK/Kamera; echte Anbindung ab ZAU-150 hinter
// NODE_PYLON_USE_REAL (PYLON_ROOT) verdrahten.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace node_pylon {
namespace core {

// Ein Geraet analog pylon TlFactory DeviceInfo (MVP-Felder).
struct DeviceInfo {
  std::string friendly_name;
  std::string model_name;
  std::string serial_number;
};

inline const char* kVersion = "0.1.0-rn-mock";
inline const char* kPylonVersionMock = "mock-8.x";

// Mock: leere Liste (keine Kamera). Real: CTlFactory::EnumerateDevices (ZAU-150).
std::vector<DeviceInfo> EnumerateDevices();

// Mock: Mono8-Gradient w*h Bytes. Real: GrabResult-Puffer, zero-copy (ZAU-151).
// Gibt false zurueck bei ungültigen Massen (0 oder > 4096).
bool GrabMockFrame(uint32_t width, uint32_t height, std::vector<uint8_t>& out);

const char* PylonVersion();

}  // namespace core
}  // namespace node_pylon
