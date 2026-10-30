// Smoke test for the shared React-Native core: runs the REAL
// node_pylon::core::* functions against the REAL pylon SDK and the camera
// emulator (PYLON_CAMEMU=2) — no stubs, no mocks, no JS involved.
// Built and executed by scripts/smoke-pylon-core.js in the `rn-build`
// CI job. Prints one PASS line per check; any failure prints FAIL and
// returns nonzero.
#include <iostream>
#include <string>

#include "pylon_core.h"

namespace {

int g_failed = 0;

void Check(bool ok, const std::string& name, const std::string& detail) {
  if (ok) {
    std::cout << "PASS " << name << " (" << detail << ")" << std::endl;
  } else {
    std::cout << "FAIL " << name << " (" << detail << ")" << std::endl;
    ++g_failed;
  }
}

}  // namespace

int main() {
  using namespace node_pylon::core;
  try {
    const std::string pv = PylonVersion();
    Check(!pv.empty() && pv.find('.') != std::string::npos, "pylon-version",
          pv);

    const std::vector<DeviceInfo> devs = EnumerateDevices();
    Check(!devs.empty(), "enumerate-devices",
          std::to_string(devs.size()) + " device(s)");
    if (!devs.empty()) {
      Check(!devs[0].serial_number.empty(), "device-serial",
            devs[0].serial_number);
    }

    const DiscoveryConstants dc = GetDiscoveryConstants();
    Check(dc.strings.count("BaslerCamEmuDeviceClass") != 0 &&
              dc.strings.at("BaslerCamEmuDeviceClass") == "BaslerCamEmu",
          "discovery-constant",
          dc.strings.count("BaslerCamEmuDeviceClass") != 0
              ? dc.strings.at("BaslerCamEmuDeviceClass")
              : "<missing>");
    Check(dc.integers.count("DeviceAccessMode_Control") != 0 &&
              dc.integers.at("DeviceAccessMode_Control") == 1,
          "access-mode-constant", "Control==1");

    const CameraHandle cam = OpenCamera(-1);
    Check(cam != kInvalidCamera, "open-camera",
          "handle=" + std::to_string(cam));
    Check(IsCameraOpen(cam), "is-open", "true");
    Check(IsPylonDeviceAttached(cam), "device-attached", "true");

    const FullDeviceInfo info = GetCameraDeviceInfo(cam);
    Check(!info.serial_number.empty(), "camera-serial", info.serial_number);

    Check(GetNodeType(cam, "Width") == "Integer", "nodetype-width",
          GetNodeType(cam, "Width"));
    const int64_t w = GetInteger(cam, "Width");
    const int64_t mn = GetIntegerMin(cam, "Width");
    const int64_t mx = GetIntegerMax(cam, "Width");
    Check(w > 0 && mn <= w && w <= mx, "integer-range",
          "Width=" + std::to_string(w) + " [" + std::to_string(mn) + "," +
              std::to_string(mx) + "]");
    const std::string fmt = GetEnum(cam, "PixelFormat");
    Check(!fmt.empty() && !GetEnumEntries(cam, "PixelFormat").empty(),
          "pixel-format", fmt);

    StartGrabbing(cam);
    Check(IsGrabbing(cam), "start-grabbing", "true");
    const GrabImage img =
        RetrieveResult(cam, 5000, TimeoutHandling::ThrowException);
    Check(img.succeeded, "grab-succeeded", fmt);
    Check(img.width > 0 && img.height > 0, "grab-geometry",
          std::to_string(img.width) + "x" + std::to_string(img.height));
    Check(!img.buffer.empty() && img.buffer.size() == img.payload_size,
          "grab-buffer", std::to_string(img.buffer.size()) + " bytes");
    // Timestamps may start at 0 on the emulator (like the Node grabresult
    // suite asserts: >= 0 and non-decreasing across grabs).
    const GrabImage img2 =
        RetrieveResult(cam, 5000, TimeoutHandling::ThrowException);
    Check(img.succeeded && img2.succeeded, "grab-twice", "ok");
    Check(img.timestamp >= 0 && img2.timestamp >= img.timestamp,
          "grab-timestamp",
          std::to_string(img.timestamp) + "->" +
              std::to_string(img2.timestamp));
    StopGrabbing(cam);
    Check(!IsGrabbing(cam), "stop-grabbing", "false");

    CloseCamera(cam);
    Check(true, "close-camera", "ok");
  } catch (const PylonError& ex) {
    std::cout << "FAIL exception: " << ex.what() << std::endl;
    ++g_failed;
  } catch (const std::exception& ex) {
    std::cout << "FAIL std-exception: " << ex.what() << std::endl;
    ++g_failed;
  }
  if (g_failed == 0) {
    std::cout << "SMOKE OK" << std::endl;
    return 0;
  }
  std::cout << "SMOKE FAILED (" << g_failed << ")" << std::endl;
  return 1;
}
