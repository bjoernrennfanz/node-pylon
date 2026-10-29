// Implementation of the shared core (mock). See pylon_core.h.
#include "pylon_core.h"

namespace node_pylon {
namespace core {

std::vector<DeviceInfo> EnumerateDevices() {
#if defined(NODE_PYLON_USE_REAL)
  // Real build: hook in Pylon::CTlFactory::EnumerateDevices here.
  // Intentionally still empty until SDK origin/license is clarified (owner: management).
  return {};
#else
  return {};
#endif
}

bool GrabMockFrame(uint32_t width, uint32_t height, std::vector<uint8_t>& out) {
  if (width == 0 || height == 0 || width > 4096 || height > 4096) return false;
  out.resize(static_cast<size_t>(width) * height);
  for (uint32_t y = 0; y < height; ++y)
    for (uint32_t x = 0; x < width; ++x)
      out[static_cast<size_t>(y) * width + x] = static_cast<uint8_t>((x + y) & 0xFF);
  return true;
}

const char* PylonVersion() {
#if defined(NODE_PYLON_USE_REAL)
  return "pylon-linked";  // Real build: Pylon::GetPylonVersionString()
#else
  return kPylonVersionMock;
#endif
}

}  // namespace core
}  // namespace node_pylon
