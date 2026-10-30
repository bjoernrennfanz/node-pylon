// Implementation of the shared core (real pylon SDK only).
// Bridges call only the node_pylon::core::* functions declared in
// pylon_core.h and translate PylonError into JS exceptions.
#include "pylon_core.h"

#include <cstring>
#include <mutex>

#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>

namespace node_pylon {
namespace core {
namespace {

// Pylon runtime lifetime (mirrors src/pylon_lifecycle.h ref-counting, but
// lock-local to this core so the RN bridges need no N-API lifecycle).
std::mutex g_mutex;
int g_init_refs = 0;
int g_next_handle = 1;
std::map<CameraHandle, std::unique_ptr<Pylon::CInstantCamera>> g_cameras;

void EnsureInitializedLocked() {
  if (g_init_refs == 0) {
    try {
      Pylon::PylonInitialize();
    } catch (const Pylon::GenericException& ex) {
      throw PylonError(std::string("PylonInitialize failed: ") + ex.what());
    }
  }
  ++g_init_refs;
}

Pylon::CInstantCamera& LookupLocked(CameraHandle handle) {
  auto it = g_cameras.find(handle);
  if (it == g_cameras.end() || !it->second) {
    throw PylonError("unknown camera handle", -1);
  }
  return *it->second;
}

GenApi::INodeMap& NodeMapOf(Pylon::CInstantCamera& cam,
                            const std::string& name) {
  if (!cam.IsOpen()) {
    throw PylonError("camera is not open (nodemap needs an open camera)");
  }
  try {
    return cam.GetNodeMap();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetNodeMap(") + name +
                     ") failed: " + ex.what());
  }
}

GenApi::INode* LookupNode(GenApi::INodeMap& map, const std::string& name) {
  GenApi::INode* node = nullptr;
  try {
    node = map.GetNode(name.c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetNode(") + name + ") failed: " + ex.what());
  }
  if (node == nullptr) {
    throw PylonError("unknown node: " + name);
  }
  return node;
}

GrabImage ToGrabImage(Pylon::CGrabResultPtr& ptr) {
  GrabImage out;
  out.succeeded = ptr->GrabSucceeded();
  out.width = ptr->GetWidth();
  out.height = ptr->GetHeight();
  try {
    out.pixel_type_name = Pylon::CPixelTypeMapper::GetNameByPixelType(
        ptr->GetPixelType());
  } catch (const Pylon::GenericException&) {
    out.pixel_type_name = "";
  }
  out.payload_size = ptr->GetPayloadSize();
  out.timestamp = ptr->GetTimeStamp();
  out.block_id = static_cast<uint64_t>(ptr->GetBlockID());
  out.image_number = static_cast<uint64_t>(ptr->GetImageNumber());
  out.chunk_available = ptr->IsChunkDataAvailable();
  const uint8_t* data = static_cast<const uint8_t*>(ptr->GetBuffer());
  const size_t size = ptr->GetBufferSize();
  if (out.succeeded && data != nullptr && size > 0) {
    out.buffer.assign(data, data + size);
  }
  return out;
}

}  // namespace

std::vector<DeviceInfo> EnumerateDevices() {
  std::lock_guard<std::mutex> lock(g_mutex);
  EnsureInitializedLocked();
  std::vector<DeviceInfo> out;
  try {
    Pylon::DeviceInfoList_t devices;
    const int count =
        Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
    for (int i = 0; i < count; ++i) {
      const Pylon::CDeviceInfo& di = devices[i];
      DeviceInfo info;
      info.friendly_name = std::string(di.GetFriendlyName().c_str());
      info.model_name = std::string(di.GetModelName().c_str());
      info.serial_number = std::string(di.GetSerialNumber().c_str());
      info.vendor_name = std::string(di.GetVendorName().c_str());
      info.device_class = std::string(di.GetDeviceClass().c_str());
      out.push_back(info);
    }
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("EnumerateDevices failed: ") + ex.what());
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

const char* PixelTypeName(int pixel_type_value) {
  try {
    // CPixelTypeMapper caches names internally; copy into a rotating slot
    // would be needed for full safety — bridges copy the string
    // immediately, so a function-local static per call is sufficient here.
    static thread_local std::string slot;
    slot = Pylon::CPixelTypeMapper::GetNameByPixelType(
        static_cast<Pylon::EPixelType>(pixel_type_value));
    return slot.c_str();
  } catch (const Pylon::GenericException&) {
    return "";
  }
}

bool IsMonoPixelType(int pixel_type_value) {
  return Pylon::IsMono(static_cast<Pylon::EPixelType>(pixel_type_value));
}

bool IsColorPixelType(int pixel_type_value) {
  return Pylon::IsColorImage(static_cast<Pylon::EPixelType>(pixel_type_value));
}

CameraHandle OpenCamera(int device_index, int access_mode) {
  std::lock_guard<std::mutex> lock(g_mutex);
  // Forward-compatible validation (CInstantCamera::Open() itself opens with
  // the default mode — same contract as the Node InstantCamera facade).
  if (access_mode != 1 && access_mode != 3 && access_mode != 4 &&
      access_mode != 5) {
    throw PylonError(
        "Unknown access mode (Control/Stream/Event/Exclusive)");
  }
  EnsureInitializedLocked();
  auto camera = std::unique_ptr<Pylon::CInstantCamera>(
      new Pylon::CInstantCamera());
  try {
    if (device_index < 0) {
      Pylon::IPylonDevice* device =
          Pylon::CTlFactory::GetInstance().CreateFirstDevice();
      camera->Attach(device, Pylon::Cleanup_Delete);
    } else {
      Pylon::DeviceInfoList_t devices;
      const int count =
          Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
      if (device_index >= count) {
        throw PylonError("device index out of range: no device attached");
      }
      Pylon::IPylonDevice* device =
          Pylon::CTlFactory::GetInstance().CreateDevice(
              devices[device_index]);
      camera->Attach(device, Pylon::Cleanup_Delete);
    }
    camera->Open();
  } catch (const PylonError&) {
    throw;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("OpenCamera failed: ") + ex.what());
  }
  const CameraHandle handle = g_next_handle++;
  g_cameras[handle] = std::move(camera);
  return handle;
}

void CloseCamera(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  auto it = g_cameras.find(handle);
  if (it == g_cameras.end()) {
    throw PylonError("unknown camera handle", -1);
  }
  try {
    if (it->second->IsGrabbing()) it->second->StopGrabbing();
    if (it->second->IsOpen()) it->second->Close();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("CloseCamera failed: ") + ex.what());
  }
  g_cameras.erase(it);
}

bool IsCameraOpen(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  return LookupLocked(handle).IsOpen();
}

void StartGrabbing(CameraHandle handle, GrabStrategy strategy,
                   uint32_t max_images) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  if (!cam.IsOpen()) {
    throw PylonError("camera is not open");
  }
  try {
    const auto s = static_cast<Pylon::EGrabStrategy>(static_cast<int>(strategy));
    if (max_images == 0) {
      cam.StartGrabbing(s);
    } else {
      cam.StartGrabbing(max_images, s);
    }
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("StartGrabbing failed: ") + ex.what());
  }
}

void StopGrabbing(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    cam.StopGrabbing();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("StopGrabbing failed: ") + ex.what());
  }
}

bool IsGrabbing(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  return LookupLocked(handle).IsGrabbing();
}

GrabImage GrabOne(CameraHandle handle, uint32_t timeout_ms,
                  TimeoutHandling handling) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  if (!cam.IsOpen()) {
    throw PylonError("camera is not open");
  }
  try {
    const bool was_grabbing = cam.IsGrabbing();
    if (!was_grabbing) cam.StartGrabbing(Pylon::GrabStrategy_OneByOne);
    Pylon::CGrabResultPtr ptr;
    const auto h = handling == TimeoutHandling::ThrowException
                       ? Pylon::TimeoutHandling_ThrowException
                       : Pylon::TimeoutHandling_Return;
    const bool ok = cam.RetrieveResult(timeout_ms, ptr, h);
    if (!ok || !ptr.IsValid()) {
      GrabImage empty;
      empty.succeeded = false;
      return empty;
    }
    GrabImage img = ToGrabImage(ptr);
    if (!was_grabbing) cam.StopGrabbing();
    return img;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GrabOne failed: ") + ex.what());
  }
}

GrabImage RetrieveResult(CameraHandle handle, uint32_t timeout_ms,
                         TimeoutHandling handling) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  if (!cam.IsOpen()) {
    throw PylonError("camera is not open");
  }
  try {
    Pylon::CGrabResultPtr ptr;
    const auto h = handling == TimeoutHandling::ThrowException
                       ? Pylon::TimeoutHandling_ThrowException
                       : Pylon::TimeoutHandling_Return;
    const bool ok = cam.RetrieveResult(timeout_ms, ptr, h);
    if (!ok || !ptr.IsValid()) {
      GrabImage empty;
      empty.succeeded = false;
      return empty;
    }
    return ToGrabImage(ptr);
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("RetrieveResult failed: ") + ex.what());
  }
}

std::string GetNodeType(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    switch (node->GetPrincipalInterfaceType()) {
      case GenApi::intfIInteger: return "Integer";
      case GenApi::intfIFloat: return "Float";
      case GenApi::intfIBoolean: return "Boolean";
      case GenApi::intfIString: return "String";
      case GenApi::intfIEnumeration: return "Enumeration";
      case GenApi::intfICommand: return "Command";
      case GenApi::intfICategory: return "Category";
      case GenApi::intfIRegister: return "Register";
      default: return "Unknown";
    }
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetNodeType(") + name + ") failed: " +
                     ex.what());
  }
}

bool IsReadable(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  return GenApi::IsReadable(LookupNode(NodeMapOf(cam, name), name));
}

bool IsWritable(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  return GenApi::IsWritable(LookupNode(NodeMapOf(cam, name), name));
}

bool IsAvailable(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  return GenApi::IsAvailable(LookupNode(NodeMapOf(cam, name), name));
}

int64_t GetInteger(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not an Integer: " + name);
  try {
    return static_cast<int64_t>(ptr->GetValue());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetInteger(") + name + ") failed: " +
                     ex.what());
  }
}

void SetInteger(CameraHandle handle, const std::string& name, int64_t value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not an Integer: " + name);
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    if (value < ptr->GetMin() || value > ptr->GetMax()) {
      throw PylonError("SetInteger(" + name + ") out of range");
    }
    ptr->SetValue(value);
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetInteger(") + name + ") failed: " +
                     ex.what());
  }
}

double GetFloat(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Float: " + name);
  try {
    return ptr->GetValue();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloat(") + name + ") failed: " +
                     ex.what());
  }
}

void SetFloat(CameraHandle handle, const std::string& name, double value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Float: " + name);
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    if (value < ptr->GetMin() || value > ptr->GetMax()) {
      throw PylonError("SetFloat(" + name + ") out of range");
    }
    ptr->SetValue(value);
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetFloat(") + name + ") failed: " +
                     ex.what());
  }
}

bool GetBoolean(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CBooleanPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Boolean: " + name);
  try {
    return ptr->GetValue();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetBoolean(") + name + ") failed: " +
                     ex.what());
  }
}

void SetBoolean(CameraHandle handle, const std::string& name, bool value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CBooleanPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Boolean: " + name);
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    ptr->SetValue(value);
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetBoolean(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetString(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CStringPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a String: " + name);
  try {
    return std::string(ptr->GetValue().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetString(") + name + ") failed: " +
                     ex.what());
  }
}

void SetString(CameraHandle handle, const std::string& name,
               const std::string& value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CStringPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a String: " + name);
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    ptr->SetValue(value.c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetString(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetEnum(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not an Enumeration: " + name);
  try {
    return std::string(ptr->ToString().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetEnum(") + name + ") failed: " +
                     ex.what());
  }
}

void SetEnum(CameraHandle handle, const std::string& name,
             const std::string& value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not an Enumeration: " + name);
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    GenApi::StringList_t symbolics;
    ptr->GetSymbolics(symbolics);
    bool known = false;
    for (const auto& entry : symbolics) {
      if (std::string(entry.c_str()) == value) {
        known = true;
        break;
      }
    }
    if (!known) {
      throw PylonError("SetEnum(" + name + ") failed: invalid entry " + value);
    }
    ptr->FromString(value.c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetEnum(") + name + ") failed: " +
                     ex.what());
  }
}

std::vector<std::string> GetEnumEntries(CameraHandle handle,
                                        const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not an Enumeration: " + name);
  std::vector<std::string> out;
  try {
    GenApi::StringList_t symbolics;
    ptr->GetSymbolics(symbolics);
    for (const auto& s : symbolics) out.emplace_back(s.c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetEnumEntries(") + name + ") failed: " +
                     ex.what());
  }
  return out;
}

void ExecuteCommand(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CCommandPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Command: " + name);
  try {
    ptr->Execute();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("ExecuteCommand(") + name + ") failed: " +
                     ex.what());
  }
}

// --- Nodemap properties -----------------------------------------------------

namespace {

template <typename PtrT>
PtrT CheckedCast(GenApi::INode* node, const std::string& name,
                 const char* want) {
  PtrT ptr(node);
  if (!ptr.IsValid()) {
    throw PylonError(std::string("node is not a ") + want + ": " + name);
  }
  return ptr;
}

const char* RepresentationName(GenApi::ERepresentation r) {
  switch (r) {
    case GenApi::Linear: return "Linear";
    case GenApi::Logarithmic: return "Logarithmic";
    case GenApi::Boolean: return "Boolean";
    case GenApi::PureNumber: return "PureNumber";
    case GenApi::HexNumber: return "HexNumber";
    case GenApi::IPV4Address: return "IPV4Address";
    case GenApi::MACAddress: return "MACAddress";
    default: return "_UndefinedRepresentation";
  }
}

std::vector<std::string> ToStringVector(const GenApi::StringList_t& list) {
  std::vector<std::string> out;
  for (const auto& s : list) out.emplace_back(s.c_str());
  return out;
}

}  // namespace

#define NODEMAP_SCAFFOLD_INT(Op, Ret, Expr)                               \
  Ret Op(CameraHandle handle, const std::string& name) {                 \
    std::lock_guard<std::mutex> lock(g_mutex);                           \
    Pylon::CInstantCamera& cam = LookupLocked(handle);                   \
    GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);        \
    GenApi::CIntegerPtr ptr =                                             \
        CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");          \
    try {                                                                \
      return (Expr);                                                     \
    } catch (const Pylon::GenericException& ex) {                        \
      throw PylonError(std::string(#Op "(") + name + ") failed: " +      \
                       ex.what());                                       \
    }                                                                    \
  }

NODEMAP_SCAFFOLD_INT(GetIntegerMin, int64_t,
                     static_cast<int64_t>(ptr->GetMin()))
NODEMAP_SCAFFOLD_INT(GetIntegerMax, int64_t,
                     static_cast<int64_t>(ptr->GetMax()))
NODEMAP_SCAFFOLD_INT(GetIntegerInc, int64_t,
                     static_cast<int64_t>(ptr->GetInc()))

#undef NODEMAP_SCAFFOLD_INT

std::string GetIntegerUnit(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr =
      CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");
  try {
    return std::string(ptr->GetUnit().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetIntegerUnit(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetIntegerRepresentation(CameraHandle handle,
                                     const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr =
      CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");
  try {
    return RepresentationName(ptr->GetRepresentation());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetIntegerRepresentation(") + name +
                     ") failed: " + ex.what());
  }
}

void SetIntegerToMin(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr =
      CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");
  try {
    ptr->SetValue(ptr->GetMin());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetIntegerToMin(") + name + ") failed: " +
                     ex.what());
  }
}

void SetIntegerToMax(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr =
      CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");
  try {
    ptr->SetValue(ptr->GetMax());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetIntegerToMax(") + name + ") failed: " +
                     ex.what());
  }
}

int64_t TryMax(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CIntegerPtr ptr =
      CheckedCast<GenApi::CIntegerPtr>(node, name, "Integer");
  try {
    const int64_t max = static_cast<int64_t>(ptr->GetMax());
    ptr->SetValue(max);
    return max;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("TryMax(") + name + ") failed: " +
                     ex.what());
  }
}

double GetFloatMin(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return ptr->GetMin();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloatMin(") + name + ") failed: " +
                     ex.what());
  }
}

double GetFloatMax(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return ptr->GetMax();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloatMax(") + name + ") failed: " +
                     ex.what());
  }
}

bool HasFloatInc(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return ptr->HasInc();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("HasFloatInc(") + name + ") failed: " +
                     ex.what());
  }
}

double GetFloatInc(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return ptr->GetInc();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloatInc(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetFloatUnit(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return std::string(ptr->GetUnit().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloatUnit(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetFloatRepresentation(CameraHandle handle,
                                   const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    return RepresentationName(ptr->GetRepresentation());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetFloatRepresentation(") + name +
                     ") failed: " + ex.what());
  }
}

void SetFloatToMin(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    ptr->SetValue(ptr->GetMin());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetFloatToMin(") + name + ") failed: " +
                     ex.what());
  }
}

void SetFloatToMax(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CFloatPtr ptr =
      CheckedCast<GenApi::CFloatPtr>(node, name, "Float");
  try {
    ptr->SetValue(ptr->GetMax());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetFloatToMax(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetDisplayName(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    return std::string(node->GetDisplayName().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetDisplayName(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetDescription(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    return std::string(node->GetDescription().c_str());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetDescription(") + name + ") failed: " +
                     ex.what());
  }
}

std::string GetValueAsString(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    GenApi::CValuePtr ptr(node);
    if (!ptr.IsValid()) {
      throw PylonError("node has no value interface: " + name);
    }
    return std::string(ptr->ToString().c_str());
  } catch (const PylonError&) {
    throw;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetValueAsString(") + name + ") failed: " +
                     ex.what());
  }
}

void SetValueFromString(CameraHandle handle, const std::string& name,
                        const std::string& value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    GenApi::CValuePtr ptr(node);
    if (!ptr.IsValid()) {
      throw PylonError("node has no value interface: " + name);
    }
    if (!GenApi::IsWritable(node)) {
      throw PylonError("node is not writable: " + name);
    }
    ptr->FromString(value.c_str());
  } catch (const PylonError&) {
    throw;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetValueFromString(") + name +
                     ") failed: " + ex.what());
  }
}

int64_t GetStringMaxLength(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CStringPtr ptr =
      CheckedCast<GenApi::CStringPtr>(node, name, "String");
  try {
    return static_cast<int64_t>(ptr->GetMaxLength());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetStringMaxLength(") + name + ") failed: " +
                     ex.what());
  }
}

int64_t GetEnumIntValue(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CEnumerationPtr ptr =
      CheckedCast<GenApi::CEnumerationPtr>(node, name, "Enumeration");
  try {
    return static_cast<int64_t>(ptr->GetIntValue());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetEnumIntValue(") + name + ") failed: " +
                     ex.what());
  }
}

void SetEnumIntValue(CameraHandle handle, const std::string& name,
                     int64_t value) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CEnumerationPtr ptr =
      CheckedCast<GenApi::CEnumerationPtr>(node, name, "Enumeration");
  if (!GenApi::IsWritable(node)) {
    throw PylonError("node is not writable: " + name);
  }
  try {
    ptr->SetIntValue(value);
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetEnumIntValue(") + name + ") failed: " +
                     ex.what());
  }
}

bool IsCommandDone(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CCommandPtr ptr =
      CheckedCast<GenApi::CCommandPtr>(node, name, "Command");
  try {
    return ptr->IsDone();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("IsCommandDone(") + name + ") failed: " +
                     ex.what());
  }
}

std::vector<std::string> GetCategoryFeatures(CameraHandle handle,
                                             const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CCategoryPtr ptr =
      CheckedCast<GenApi::CCategoryPtr>(node, name, "Category");
  try {
    GenApi::FeatureList_t features;
    ptr->GetFeatures(features);
    std::vector<std::string> out;
    for (auto* f : features) {
      if (f == nullptr) continue;
      // Feature lists hold IValue*; the name lives on INode.
      auto* inode = dynamic_cast<GenApi::INode*>(f);
      if (inode != nullptr) {
        out.emplace_back(inode->GetName().c_str());
      }
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetCategoryFeatures(") + name +
                     ") failed: " + ex.what());
  }
}

bool IsSelector(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  try {
    GenApi::CSelectorPtr ptr(node);
    if (!ptr.IsValid()) return false;
    return ptr->IsSelector();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("IsSelector(") + name + ") failed: " +
                     ex.what());
  }
}

std::vector<std::string> GetSelectedFeatures(CameraHandle handle,
                                             const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CSelectorPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Selector: " + name);
  try {
    GenApi::FeatureList_t features;
    ptr->GetSelectedFeatures(features);
    std::vector<std::string> out;
    for (auto* f : features) {
      if (f == nullptr) continue;
      // Feature lists hold IValue*; the name lives on INode.
      auto* inode = dynamic_cast<GenApi::INode*>(f);
      if (inode != nullptr) {
        out.emplace_back(inode->GetName().c_str());
      }
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetSelectedFeatures(") + name +
                     ") failed: " + ex.what());
  }
}

std::vector<std::string> GetSelectingFeatures(CameraHandle handle,
                                              const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CSelectorPtr ptr(node);
  if (!ptr.IsValid()) throw PylonError("node is not a Selector: " + name);
  try {
    GenApi::FeatureList_t features;
    ptr->GetSelectingFeatures(features);
    std::vector<std::string> out;
    for (auto* f : features) {
      if (f == nullptr) continue;
      // Feature lists hold IValue*; the name lives on INode.
      auto* inode = dynamic_cast<GenApi::INode*>(f);
      if (inode != nullptr) {
        out.emplace_back(inode->GetName().c_str());
      }
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetSelectingFeatures(") + name +
                     ") failed: " + ex.what());
  }
}

int64_t GetRegisterLength(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CRegisterPtr ptr =
      CheckedCast<GenApi::CRegisterPtr>(node, name, "Register");
  try {
    return static_cast<int64_t>(ptr->GetLength());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetRegisterLength(") + name + ") failed: " +
                     ex.what());
  }
}

int64_t GetRegisterAddress(CameraHandle handle, const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CRegisterPtr ptr =
      CheckedCast<GenApi::CRegisterPtr>(node, name, "Register");
  try {
    return static_cast<int64_t>(ptr->GetAddress());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetRegisterAddress(") + name +
                     ") failed: " + ex.what());
  }
}

std::vector<uint8_t> ReadRegister(CameraHandle handle,
                                 const std::string& name) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CRegisterPtr ptr =
      CheckedCast<GenApi::CRegisterPtr>(node, name, "Register");
  try {
    const int64_t len = ptr->GetLength();
    std::vector<uint8_t> out(static_cast<size_t>(len));
    ptr->Get(out.data(), len);
    return out;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("ReadRegister(") + name + ") failed: " +
                     ex.what());
  }
}

void WriteRegister(CameraHandle handle, const std::string& name,
                   const std::vector<uint8_t>& data) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  GenApi::INode* node = LookupNode(NodeMapOf(cam, name), name);
  GenApi::CRegisterPtr ptr =
      CheckedCast<GenApi::CRegisterPtr>(node, name, "Register");
  try {
    ptr->Set(data.data(), static_cast<int64_t>(data.size()));
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("WriteRegister(") + name + ") failed: " +
                     ex.what());
  }
}

// --- Camera device handling -------------------------------------------------

FullDeviceInfo GetCameraDeviceInfo(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    const Pylon::CDeviceInfo di = cam.GetDeviceInfo();
    FullDeviceInfo info;
    info.friendly_name = std::string(di.GetFriendlyName().c_str());
    info.model_name = std::string(di.GetModelName().c_str());
    info.serial_number = std::string(di.GetSerialNumber().c_str());
    info.vendor_name = std::string(di.GetVendorName().c_str());
    info.device_class = std::string(di.GetDeviceClass().c_str());
    info.full_name = std::string(di.GetFullName().c_str());
    return info;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetCameraDeviceInfo failed: ") + ex.what());
  }
}

bool IsPylonDeviceAttached(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  return LookupLocked(handle).IsPylonDeviceAttached();
}

bool HasOwnership(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  return LookupLocked(handle).HasOwnership();
}

void AttachDevice(CameraHandle handle, int device_index) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    Pylon::DeviceInfoList_t devices;
    const int count =
        Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
    if (device_index < 0 || device_index >= count) {
      throw PylonError("device index out of range: no device attached");
    }
    Pylon::IPylonDevice* device =
        Pylon::CTlFactory::GetInstance().CreateDevice(devices[device_index]);
    cam.Attach(device, Pylon::Cleanup_Delete);
  } catch (const PylonError&) {
    throw;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("AttachDevice failed: ") + ex.what());
  }
}

void DetachDevice(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    if (!cam.IsPylonDeviceAttached()) {
      throw PylonError("no device attached");
    }
    if (cam.IsOpen()) cam.Close();
    Pylon::IPylonDevice* device = cam.DetachDevice();
    Pylon::CTlFactory::GetInstance().DestroyDevice(device);
  } catch (const PylonError&) {
    throw;
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("DetachDevice failed: ") + ex.what());
  }
}

void DestroyDevice(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    cam.DestroyDevice();
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("DestroyDevice failed: ") + ex.what());
  }
}

bool IsCameraDeviceRemoved(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  try {
    return LookupLocked(handle).IsCameraDeviceRemoved();
  } catch (...) {
    return false;
  }
}

int64_t GetCameraContext(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    return static_cast<int64_t>(cam.GetCameraContext());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetCameraContext failed: ") + ex.what());
  }
}

void SetCameraContext(CameraHandle handle, int64_t context) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  try {
    cam.SetCameraContext(static_cast<intptr_t>(context));
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("SetCameraContext failed: ") + ex.what());
  }
}

int64_t GetQueuedBufferCount(CameraHandle handle) {
  std::lock_guard<std::mutex> lock(g_mutex);
  Pylon::CInstantCamera& cam = LookupLocked(handle);
  if (!cam.IsPylonDeviceAttached()) {
    throw PylonError("no device attached");
  }
  try {
    return static_cast<int64_t>(cam.GetQueuedBufferCount());
  } catch (const Pylon::GenericException& ex) {
    throw PylonError(std::string("GetQueuedBufferCount failed: ") +
                     ex.what());
  }
}

namespace {

std::string ConstStr(const char* s) { return s != nullptr ? s : ""; }
std::string ConstStr(const Pylon::String_t& s) {
  return std::string(s.c_str());
}

}  // namespace

DiscoveryConstants GetDiscoveryConstants() {
  DiscoveryConstants out;
  // Device classes (Pylon::Basler*DeviceClass).
  out.strings["BaslerCamEmuDeviceClass"] =
      ConstStr(Pylon::BaslerCamEmuDeviceClass);
  out.strings["BaslerIpCamDeviceClass"] =
      ConstStr(Pylon::BaslerIpCamDeviceClass);
  out.strings["BaslerCameraLinkDeviceClass"] =
      ConstStr(Pylon::BaslerCameraLinkDeviceClass);
  out.strings["BaslerUsbDeviceClass"] = ConstStr(Pylon::BaslerUsbDeviceClass);
  out.strings["BaslerGenTlGevDeviceClass"] =
      ConstStr(Pylon::BaslerGenTlGevDeviceClass);
  out.strings["BaslerGenTlU3vDeviceClass"] =
      ConstStr(Pylon::BaslerGenTlU3vDeviceClass);
  out.strings["BaslerGenTlCxpDeviceClass"] =
      ConstStr(Pylon::BaslerGenTlCxpDeviceClass);
  out.strings["BaslerGenTlBlazeDeviceClass"] =
      ConstStr(Pylon::BaslerGenTlBlazeDeviceClass);
  out.strings["BaslerGenTlStaDeviceClass"] =
      ConstStr(Pylon::BaslerGenTlStaDeviceClass);
  // Transport-layer types (Pylon::TLType::*).
  out.strings["TLTypeGigE"] = ConstStr(Pylon::TLType::TLTypeGigE);
  out.strings["TLTypeUSB"] = ConstStr(Pylon::TLType::TLTypeUSB);
  out.strings["TLTypeCL"] = ConstStr(Pylon::TLType::TLTypeCL);
  out.strings["TLTypeCamEmu"] = ConstStr(Pylon::TLType::TLTypeCamEmu);
  out.strings["TLTypeCXP"] = ConstStr(Pylon::TLType::TLTypeCXP);
  out.strings["TLTypeCustom"] = ConstStr(Pylon::TLType::TLTypeCustom);
  // Property keys (Pylon::Key::*).
  out.strings["FriendlyNameKey"] = ConstStr(Pylon::Key::FriendlyNameKey);
  out.strings["FullNameKey"] = ConstStr(Pylon::Key::FullNameKey);
  out.strings["VendorNameKey"] = ConstStr(Pylon::Key::VendorNameKey);
  out.strings["DeviceClassKey"] = ConstStr(Pylon::Key::DeviceClassKey);
  out.strings["TLTypeKey"] = ConstStr(Pylon::Key::TLTypeKey);
  out.strings["InterfaceIDKey"] = ConstStr(Pylon::Key::InterfaceIDKey);
  out.strings["SerialNumberKey"] = ConstStr(Pylon::Key::SerialNumberKey);
  out.strings["UserDefinedNameKey"] =
      ConstStr(Pylon::Key::UserDefinedNameKey);
  out.strings["ModelNameKey"] = ConstStr(Pylon::Key::ModelNameKey);
  out.strings["DeviceVersionKey"] = ConstStr(Pylon::Key::DeviceVersionKey);
  out.strings["DeviceFactoryKey"] = ConstStr(Pylon::Key::DeviceFactoryKey);
  out.strings["XMLSourceKey"] = ConstStr(Pylon::Key::XMLSourceKey);
  out.strings["FileNameKey"] = ConstStr(Pylon::Key::FileNameKey);
  out.strings["InfoIDKey"] = ConstStr(Pylon::Key::InfoIDKey);
  out.strings["VersionKey"] = ConstStr(Pylon::Key::VersionKey);
  // Device access modes (pylon/DeviceAccessMode.h: Pylon::Control etc.).
  out.integers["DeviceAccessMode_Control"] = Pylon::Control;
  out.integers["DeviceAccessMode_Stream"] = Pylon::Stream;
  out.integers["DeviceAccessMode_Event"] = Pylon::Event;
  out.integers["DeviceAccessMode_Exclusive"] = Pylon::Exclusive;
  // Accessibility states (Pylon::Accessibility_*).
  out.integers["Accessibility_Unknown"] = Pylon::Accessibility_Unknown;
  out.integers["Accessibility_Ok"] = Pylon::Accessibility_Ok;
  out.integers["Accessibility_Opened"] = Pylon::Accessibility_Opened;
  out.integers["Accessibility_OpenedExclusively"] =
      Pylon::Accessibility_OpenedExclusively;
  out.integers["Accessibility_NotReachable"] =
      Pylon::Accessibility_NotReachable;
  return out;
}

}  // namespace core
}  // namespace node_pylon
