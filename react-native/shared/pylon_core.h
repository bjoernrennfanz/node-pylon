// Shared C++ core for the Node addon (N-API) and the RN JSI/TurboModule.
// Pure C++17, NO N-API/JSI/V8 includes here — this file is written once
// and only adapted by both bridges (no double wrapping).
// Real pylon SDK only: enumeration, camera lifecycle, grab, nodemap and
// version come from the SDK; bridges must translate PylonError into
// JS exceptions (N-API: throw Napi::Error / JSI: jsi::JSError).
#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace node_pylon {
namespace core {

// A device mirroring the pylon TlFactory DeviceInfo (MVP fields).
struct DeviceInfo {
  std::string friendly_name;
  std::string model_name;
  std::string serial_number;
  std::string vendor_name;
  std::string device_class;
};

// FullDeviceInfo also carries the full name (like GetDeviceInfo in Node).
struct FullDeviceInfo : DeviceInfo {
  std::string full_name;
};

inline const char* kVersion = "0.2.0-core";

// Grab strategies / timeout handling (mirror Pylon::EGrabStrategy /
// Pylon::ETimeoutHandling so bridges and JS can never drift apart).
enum class GrabStrategy : int {
  OneByOne = 0,
  LatestImageOnly = 1,
  LatestImages = 2,
  UpcomingImage = 3,
};

enum class TimeoutHandling : int {
  Return = 0,
  ThrowException = 1,
};

// Thrown by every core function on pylon failure. Bridges translate
// `what()` (+ `code` for GenICam/pylon error codes) into JS exceptions.
struct PylonError : std::runtime_error {
  int code;
  explicit PylonError(const std::string& msg, int c = 0)
      : std::runtime_error(msg), code(c) {}
};

// Grabbed image (node-pylon GrabResult core fields). The pixel buffer is
// owned here; JSI bridges move it into a jsi::ArrayBuffer zero-copy
// (custom deleter), codegen-fallback bridges base64-encode it — either
// way no extra copy inside the core.
struct GrabImage {
  bool succeeded = false;
  uint32_t width = 0;
  uint32_t height = 0;
  std::string pixel_type_name;  // e.g. "Mono8" (see PixelTypeName())
  uint64_t payload_size = 0;
  uint64_t timestamp = 0;
  uint64_t block_id = 0;
  uint64_t image_number = 0;
  bool chunk_available = false;
  std::vector<uint8_t> buffer;
};

// Opaque camera handle owned by the core registry below. Bridges only
// pass the int across JSI; the CInstantCamera never crosses the ABI.
using CameraHandle = int;
constexpr CameraHandle kInvalidCamera = -1;

// --- Discovery / version (as before) ------------------------------------

// Real enumeration via the pylon transport layer factory. Empty without a
// camera or emulator (PYLON_CAMEMU=N provides emulator devices).
std::vector<DeviceInfo> EnumerateDevices();

// Real pylon version string from the SDK (e.g. "12.5.2.1565").
const char* PylonVersion();

// Pixel-type helpers (mirror EPixelType + CPixelTypeMapper names).
const char* PixelTypeName(int pixel_type_value);
bool IsMonoPixelType(int pixel_type_value);
bool IsColorPixelType(int pixel_type_value);

// --- Camera lifecycle ----------------------------------------------------
// All functions throw PylonError on failure (incl. unknown handle).

CameraHandle OpenCamera(int device_index, int access_mode = 1);
void CloseCamera(CameraHandle handle);
bool IsCameraOpen(CameraHandle handle);

// --- Grab loop (thread-safe; never blocks the JS loop from the bridge —
// bridges run these on a worker / CallInvoker + promise) ------------------

void StartGrabbing(CameraHandle handle,
                   GrabStrategy strategy = GrabStrategy::OneByOne,
                   uint32_t max_images = 0);  // 0 = unlimited
void StopGrabbing(CameraHandle handle);
bool IsGrabbing(CameraHandle handle);

// One-shot grab: opens the grab loop internally when needed and returns
// after the first image (or throws / returns !succeeded on timeout when
// handling == Return).
GrabImage GrabOne(CameraHandle handle, uint32_t timeout_ms,
                  TimeoutHandling handling = TimeoutHandling::ThrowException);

// Retrieve the next image from a running grab loop. Returns an image with
// succeeded=false on timeout when handling == Return.
GrabImage RetrieveResult(CameraHandle handle, uint32_t timeout_ms,
                         TimeoutHandling handling = TimeoutHandling::ThrowException);

// --- Nodemap basics (GenICam types mapped like src/node_map.*) ------------

std::string GetNodeType(CameraHandle handle, const std::string& name);
bool IsReadable(CameraHandle handle, const std::string& name);
bool IsWritable(CameraHandle handle, const std::string& name);
bool IsAvailable(CameraHandle handle, const std::string& name);

int64_t GetInteger(CameraHandle handle, const std::string& name);
void SetInteger(CameraHandle handle, const std::string& name, int64_t value);
double GetFloat(CameraHandle handle, const std::string& name);
void SetFloat(CameraHandle handle, const std::string& name, double value);
bool GetBoolean(CameraHandle handle, const std::string& name);
void SetBoolean(CameraHandle handle, const std::string& name, bool value);
std::string GetString(CameraHandle handle, const std::string& name);
void SetString(CameraHandle handle, const std::string& name,
               const std::string& value);
std::string GetEnum(CameraHandle handle, const std::string& name);
void SetEnum(CameraHandle handle, const std::string& name,
             const std::string& value);
std::vector<std::string> GetEnumEntries(CameraHandle handle,
                                        const std::string& name);
void ExecuteCommand(CameraHandle handle, const std::string& name);

// --- Nodemap properties (GenICam metadata, like src/node_map.*) -------------

int64_t GetIntegerMin(CameraHandle handle, const std::string& name);
int64_t GetIntegerMax(CameraHandle handle, const std::string& name);
int64_t GetIntegerInc(CameraHandle handle, const std::string& name);
std::string GetIntegerUnit(CameraHandle handle, const std::string& name);
std::string GetIntegerRepresentation(CameraHandle handle,
                                     const std::string& name);
void SetIntegerToMin(CameraHandle handle, const std::string& name);
void SetIntegerToMax(CameraHandle handle, const std::string& name);
int64_t TryMax(CameraHandle handle, const std::string& name);
double GetFloatMin(CameraHandle handle, const std::string& name);
double GetFloatMax(CameraHandle handle, const std::string& name);
bool HasFloatInc(CameraHandle handle, const std::string& name);
double GetFloatInc(CameraHandle handle, const std::string& name);
std::string GetFloatUnit(CameraHandle handle, const std::string& name);
std::string GetFloatRepresentation(CameraHandle handle,
                                   const std::string& name);
void SetFloatToMin(CameraHandle handle, const std::string& name);
void SetFloatToMax(CameraHandle handle, const std::string& name);
std::string GetDisplayName(CameraHandle handle, const std::string& name);
std::string GetDescription(CameraHandle handle, const std::string& name);
std::string GetValueAsString(CameraHandle handle, const std::string& name);
void SetValueFromString(CameraHandle handle, const std::string& name,
                        const std::string& value);
int64_t GetStringMaxLength(CameraHandle handle, const std::string& name);
int64_t GetEnumIntValue(CameraHandle handle, const std::string& name);
void SetEnumIntValue(CameraHandle handle, const std::string& name,
                     int64_t value);
bool IsCommandDone(CameraHandle handle, const std::string& name);
std::vector<std::string> GetCategoryFeatures(CameraHandle handle,
                                             const std::string& name);
bool IsSelector(CameraHandle handle, const std::string& name);
std::vector<std::string> GetSelectedFeatures(CameraHandle handle,
                                             const std::string& name);
std::vector<std::string> GetSelectingFeatures(CameraHandle handle,
                                              const std::string& name);
int64_t GetRegisterLength(CameraHandle handle, const std::string& name);
int64_t GetRegisterAddress(CameraHandle handle, const std::string& name);
std::vector<uint8_t> ReadRegister(CameraHandle handle,
                                 const std::string& name);
void WriteRegister(CameraHandle handle, const std::string& name,
                   const std::vector<uint8_t>& data);

// --- Camera device handling (like the Node InstantCamera facade) -----------

FullDeviceInfo GetCameraDeviceInfo(CameraHandle handle);
bool IsPylonDeviceAttached(CameraHandle handle);
bool HasOwnership(CameraHandle handle);
void AttachDevice(CameraHandle handle, int device_index);
void DetachDevice(CameraHandle handle);
void DestroyDevice(CameraHandle handle);
bool IsCameraDeviceRemoved(CameraHandle handle);
int64_t GetCameraContext(CameraHandle handle);
void SetCameraContext(CameraHandle handle, int64_t context);
int64_t GetQueuedBufferCount(CameraHandle handle);
// Discovery constants (device classes, TL types, property keys as strings;
// access modes and accessibility states as integers). Values come straight
// from the pylon SDK headers so bridges and JS can never drift apart.
struct DiscoveryConstants {
  std::map<std::string, std::string> strings;
  std::map<std::string, int64_t> integers;
};

DiscoveryConstants GetDiscoveryConstants();

}  // namespace core
}  // namespace node_pylon
