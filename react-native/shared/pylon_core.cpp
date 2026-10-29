// Implementation of the shared core (real pylon SDK only).
#include "pylon_core.h"

#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>

namespace node_pylon {
namespace core {

std::vector<DeviceInfo> EnumerateDevices() {
  std::vector<DeviceInfo> out;
  Pylon::DeviceInfoList_t devices;
  const int count =
      Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
  for (int i = 0; i < count; ++i) {
    const Pylon::CDeviceInfo& di = devices[i];
    DeviceInfo info;
    info.friendly_name = std::string(di.GetFriendlyName().c_str());
    info.model_name = std::string(di.GetModelName().c_str());
    info.serial_number = std::string(di.GetSerialNumber().c_str());
    out.push_back(info);
  }
  return out;
}

const char* PylonVersion() {
  static std::string cached;
  if (cached.empty()) {
    const char* v = Pylon::GetPylonVersionString();
    cached = v != nullptr ? v : "";
  }
  return cached.c_str();
}

}  // namespace core
}  // namespace node_pylon
