// InstantCamera N-API wrapper: implementation (real pylon SDK only).
// N-API rules: ObjectWrap, no V8 internals, errors as JS exceptions.
#include "camera_wrap.h"

#include <chrono>
#include <cstring>
#include <string>
#include <vector>

#include "grab_result.h"
#include "pylon_lifecycle.h"

namespace node_pylon {

Napi::FunctionReference CameraWrap::constructor;

namespace {

std::string RequireStringArg(const Napi::CallbackInfo& info, size_t idx,
                             bool* ok) {
  if (idx >= info.Length() || !info[idx].IsString()) {
    *ok = false;
    return "";
  }
  *ok = true;
  return info[idx].As<Napi::String>().Utf8Value();
}

std::string DescribePixelType(const Pylon::CGrabResultPtr& ptr) {
  // Hand-mapped common formats (stable enum values, no dependency on
  // optional mapper helpers in the SDK).
  switch (ptr->GetPixelType()) {
    case Pylon::PixelType_Mono8:
      return "Mono8";
    case Pylon::PixelType_Mono12:
    case Pylon::PixelType_Mono12packed:
      return "Mono12";
    case Pylon::PixelType_RGB8packed:
    case Pylon::PixelType_BGR8packed:
      return "RGB8";
    default:
      return "Unknown";
  }
}

// --- Argument parsing (numbers or pylon-style names) -----------------------

bool ParseGrabStrategy(const Napi::Value& v, Pylon::EGrabStrategy* out) {
  if (v.IsNumber()) {
    switch (v.As<Napi::Number>().Int32Value()) {
      case kGrabStrategyOneByOne:
        *out = Pylon::GrabStrategy_OneByOne;
        return true;
      case kGrabStrategyLatestImageOnly:
        *out = Pylon::GrabStrategy_LatestImageOnly;
        return true;
      case kGrabStrategyLatestImages:
        *out = Pylon::GrabStrategy_LatestImages;
        return true;
      case kGrabStrategyUpcomingImage:
        *out = Pylon::GrabStrategy_UpcomingImage;
        return true;
      default:
        return false;
    }
  }
  if (v.IsString()) {
    const std::string s = v.As<Napi::String>().Utf8Value();
    if (s == "OneByOne") {
      *out = Pylon::GrabStrategy_OneByOne;
      return true;
    }
    if (s == "LatestImageOnly") {
      *out = Pylon::GrabStrategy_LatestImageOnly;
      return true;
    }
    if (s == "LatestImages") {
      *out = Pylon::GrabStrategy_LatestImages;
      return true;
    }
    if (s == "UpcomingImage") {
      *out = Pylon::GrabStrategy_UpcomingImage;
      return true;
    }
  }
  return false;
}

bool ParseGrabLoop(const Napi::Value& v, Pylon::EGrabLoop* out) {
  if (v.IsNumber()) {
    switch (v.As<Napi::Number>().Int32Value()) {
      case kGrabLoopByCamera:
        *out = Pylon::GrabLoop_ProvidedByInstantCamera;
        return true;
      case kGrabLoopByUser:
        *out = Pylon::GrabLoop_ProvidedByUser;
        return true;
      default:
        return false;
    }
  }
  if (v.IsString()) {
    const std::string s = v.As<Napi::String>().Utf8Value();
    if (s == "ProvidedByInstantCamera") {
      *out = Pylon::GrabLoop_ProvidedByInstantCamera;
      return true;
    }
    if (s == "ProvidedByUser") {
      *out = Pylon::GrabLoop_ProvidedByUser;
      return true;
    }
  }
  return false;
}

bool ParseTimeoutHandling(const Napi::Value& v,
                          Pylon::ETimeoutHandling* out) {
  if (v.IsNumber()) {
    switch (v.As<Napi::Number>().Int32Value()) {
      case kTimeoutHandlingReturn:
        *out = Pylon::TimeoutHandling_Return;
        return true;
      case kTimeoutHandlingThrow:
        *out = Pylon::TimeoutHandling_ThrowException;
        return true;
      default:
        return false;
    }
  }
  if (v.IsString()) {
    const std::string s = v.As<Napi::String>().Utf8Value();
    if (s == "Return") {
      *out = Pylon::TimeoutHandling_Return;
      return true;
    }
    if (s == "ThrowException") {
      *out = Pylon::TimeoutHandling_ThrowException;
      return true;
    }
  }
  return false;
}

bool ParseRegistrationMode(const Napi::Value& v,
                           Pylon::ERegistrationMode* out) {
  if (v.IsNumber()) {
    switch (v.As<Napi::Number>().Int32Value()) {
      case kRegistrationAppend:
        *out = Pylon::RegistrationMode_Append;
        return true;
      case kRegistrationReplaceAll:
        *out = Pylon::RegistrationMode_ReplaceAll;
        return true;
      default:
        return false;
    }
  }
  if (v.IsString()) {
    const std::string s = v.As<Napi::String>().Utf8Value();
    if (s == "Append") {
      *out = Pylon::RegistrationMode_Append;
      return true;
    }
    if (s == "ReplaceAll") {
      *out = Pylon::RegistrationMode_ReplaceAll;
      return true;
    }
  }
  return false;
}

}  // namespace

void CameraWrap::ThrowNodeError(Napi::Env env, const std::string& node,
                                NodeError err) {
  std::string msg = std::string(NodeErrorMessage(err));
  if (!node.empty()) msg += ": " + node;
  if (err == NodeError::kOutOfRange || err == NodeError::kInvalidEnumEntry) {
    Napi::RangeError::New(env, msg).ThrowAsJavaScriptException();
  } else if (err == NodeError::kWrongType) {
    Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
  } else {
    Napi::Error::New(env, msg).ThrowAsJavaScriptException();
  }
}

void CameraWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "InstantCamera",
      {InstanceMethod("isOpen", &CameraWrap::IsOpen),
       InstanceMethod("open", &CameraWrap::Open),
       InstanceMethod("close", &CameraWrap::Close),
       InstanceMethod("getDeviceInfo", &CameraWrap::GetDeviceInfo),
       InstanceMethod("isPylonDeviceAttached",
                      &CameraWrap::IsPylonDeviceAttached),
       InstanceMethod("hasOwnership", &CameraWrap::HasOwnership),
       InstanceMethod("attachDevice", &CameraWrap::AttachDevice),
       InstanceMethod("detachDevice", &CameraWrap::DetachDevice),
       InstanceMethod("destroyDevice", &CameraWrap::DestroyDevice),
       InstanceMethod("isCameraDeviceRemoved",
                      &CameraWrap::IsCameraDeviceRemoved),
       InstanceMethod("getCameraContext", &CameraWrap::GetCameraContext),
       InstanceMethod("setCameraContext", &CameraWrap::SetCameraContext),
       InstanceMethod("getQueuedBufferCount",
                      &CameraWrap::GetQueuedBufferCount),
       InstanceMethod("startGrabbing", &CameraWrap::StartGrabbing),
       InstanceMethod("startGrabbingMax", &CameraWrap::StartGrabbingMax),
       InstanceMethod("stopGrabbing", &CameraWrap::StopGrabbing),
       InstanceMethod("isGrabbing", &CameraWrap::IsGrabbing),
       InstanceMethod("retrieveResult", &CameraWrap::RetrieveResult),
       InstanceMethod("retrieveGrabResult", &CameraWrap::RetrieveGrabResult),
       InstanceMethod("grabOne", &CameraWrap::GrabOne),
       InstanceMethod("waitForFrameTriggerReady",
                      &CameraWrap::WaitForFrameTriggerReady),
       InstanceMethod("executeSoftwareTrigger",
                      &CameraWrap::ExecuteSoftwareTrigger),
       InstanceMethod("registerConfiguration",
                      &CameraWrap::RegisterConfiguration),
       InstanceMethod("deregisterConfiguration",
                      &CameraWrap::DeregisterConfiguration),
       InstanceMethod("registerImageEventHandler",
                      &CameraWrap::RegisterImageEventHandler),
       InstanceMethod("deregisterImageEventHandler",
                      &CameraWrap::DeregisterImageEventHandler),
       InstanceMethod("registerCameraEventHandler",
                      &CameraWrap::RegisterCameraEventHandler),
       InstanceMethod("deregisterCameraEventHandler",
                      &CameraWrap::DeregisterCameraEventHandler),
       InstanceMethod("getNodeType", &CameraWrap::GetNodeType),
       InstanceMethod("isReadable", &CameraWrap::IsReadable),
       InstanceMethod("isWritable", &CameraWrap::IsWritable),
       InstanceMethod("isAvailable", &CameraWrap::IsAvailable),
       InstanceMethod("getDisplayName", &CameraWrap::GetDisplayName),
       InstanceMethod("getDescription", &CameraWrap::GetDescription),
       InstanceMethod("getInteger", &CameraWrap::GetInteger),
       InstanceMethod("setInteger", &CameraWrap::SetInteger),
       InstanceMethod("getIntegerMin", &CameraWrap::GetIntegerMin),
       InstanceMethod("getIntegerMax", &CameraWrap::GetIntegerMax),
       InstanceMethod("getIntegerInc", &CameraWrap::GetIntegerInc),
       InstanceMethod("getIntegerUnit", &CameraWrap::GetIntegerUnit),
       InstanceMethod("getIntegerRepresentation",
                      &CameraWrap::GetIntegerRepresentation),
       InstanceMethod("setIntegerToMin", &CameraWrap::SetIntegerToMin),
       InstanceMethod("setIntegerToMax", &CameraWrap::SetIntegerToMax),
       InstanceMethod("tryMax", &CameraWrap::TryMax),
       InstanceMethod("getFloat", &CameraWrap::GetFloat),
       InstanceMethod("setFloat", &CameraWrap::SetFloat),
       InstanceMethod("getFloatMin", &CameraWrap::GetFloatMin),
       InstanceMethod("getFloatMax", &CameraWrap::GetFloatMax),
       InstanceMethod("hasFloatInc", &CameraWrap::HasFloatInc),
       InstanceMethod("getFloatInc", &CameraWrap::GetFloatInc),
       InstanceMethod("getFloatUnit", &CameraWrap::GetFloatUnit),
       InstanceMethod("getFloatRepresentation",
                      &CameraWrap::GetFloatRepresentation),
       InstanceMethod("setFloatToMin", &CameraWrap::SetFloatToMin),
       InstanceMethod("setFloatToMax", &CameraWrap::SetFloatToMax),
       InstanceMethod("getValueAsString", &CameraWrap::GetValueAsString),
       InstanceMethod("setValueFromString", &CameraWrap::SetValueFromString),
       InstanceMethod("getBoolean", &CameraWrap::GetBoolean),
       InstanceMethod("setBoolean", &CameraWrap::SetBoolean),
       InstanceMethod("getString", &CameraWrap::GetString),
       InstanceMethod("setString", &CameraWrap::SetString),
       InstanceMethod("getStringMaxLength", &CameraWrap::GetStringMaxLength),
       InstanceMethod("getEnum", &CameraWrap::GetEnum),
       InstanceMethod("setEnum", &CameraWrap::SetEnum),
       InstanceMethod("getEnumEntries", &CameraWrap::GetEnumEntries),
       InstanceMethod("getEnumIntValue", &CameraWrap::GetEnumIntValue),
       InstanceMethod("setEnumIntValue", &CameraWrap::SetEnumIntValue),
       InstanceMethod("executeCommand", &CameraWrap::ExecuteCommand),
       InstanceMethod("isCommandDone", &CameraWrap::IsCommandDone),
       InstanceMethod("getCategoryFeatures",
                      &CameraWrap::GetCategoryFeatures),
       InstanceMethod("isSelector", &CameraWrap::IsSelector),
       InstanceMethod("getSelectedFeatures",
                      &CameraWrap::GetSelectedFeatures),
       InstanceMethod("getSelectingFeatures",
                      &CameraWrap::GetSelectingFeatures),
       InstanceMethod("getRegisterLength", &CameraWrap::GetRegisterLength),
       InstanceMethod("getRegisterAddress", &CameraWrap::GetRegisterAddress),
       InstanceMethod("readRegister", &CameraWrap::ReadRegister),
       InstanceMethod("writeRegister", &CameraWrap::WriteRegister),
       InstanceMethod("registerCallback", &CameraWrap::RegisterCallback),
       InstanceMethod("getCallbackCount", &CameraWrap::GetCallbackCount),
       InstanceMethod("deregisterCallback", &CameraWrap::DeregisterCallback),
       InstanceMethod("grab", &CameraWrap::Grab)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("InstantCamera", ctor);
}

CameraWrap::CameraWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CameraWrap>(info) {
  Napi::Env env = info.Env();
  bool acquired = false;
  try {
    lifecycle::EnsureInitialized();
    acquired = true;
    // Optional device index: new InstantCamera(i) attaches enumerated
    // device i (for camera arrays). Default attaches the first device.
    size_t deviceIndex = 0;
    bool indexed = false;
    if (info.Length() >= 1 && info[0].IsNumber()) {
      int64_t v = info[0].As<Napi::Number>().Int64Value();
      if (v < 0) {
        lifecycle::ReleaseRef();
        Napi::RangeError::New(env, "device index out of range")
            .ThrowAsJavaScriptException();
        return;
      }
      deviceIndex = static_cast<size_t>(v);
      indexed = true;
    }
    camera_ = new Pylon::CInstantCamera();
    initOk_ = true;
    if (indexed) {
      AttachEnumeratedDevice(deviceIndex);
    } else {
      // TlFactory::CreateFirstDevice throws without an attached device.
      Pylon::IPylonDevice* device =
          Pylon::CTlFactory::GetInstance().CreateFirstDevice();
      camera_->Attach(device, Pylon::Cleanup_Delete);
    }
  } catch (const Pylon::GenericException& ex) {
    if (camera_ != nullptr) {
      delete camera_;
      camera_ = nullptr;
    }
    if (acquired) {
      lifecycle::ReleaseRef();
      initOk_ = false;
    }
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return;
  } catch (const std::exception& ex) {
    if (camera_ != nullptr) {
      delete camera_;
      camera_ = nullptr;
    }
    if (acquired) {
      lifecycle::ReleaseRef();
      initOk_ = false;
    }
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return;
  }
}

void CameraWrap::AttachEnumeratedDevice(size_t device_index) {
  Pylon::DeviceInfoList_t devices;
  int count = Pylon::CTlFactory::GetInstance().EnumerateDevices(devices);
  if (device_index >= static_cast<size_t>(count)) {
    throw std::out_of_range("device index out of range: no device attached");
  }
  Pylon::IPylonDevice* device =
      Pylon::CTlFactory::GetInstance().CreateDevice(devices[device_index]);
  camera_->Attach(device, Pylon::Cleanup_Delete);
}

CameraWrap::~CameraWrap() {
  if (camera_ != nullptr) {
    try {
      // Drop GenApi callbacks while nodes are still alive; after Close or
      // delete the INode pointers dangle and must not be touched.
      nodemap_.ClearCallbacks();
      if (camera_->IsGrabbing()) camera_->StopGrabbing();
      if (camera_->IsOpen()) camera_->Close();
    } catch (...) {
      // Destructor: never throw.
    }
    delete camera_;
    camera_ = nullptr;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
  }
  if (initOk_) lifecycle::ReleaseRef();
}

Napi::Value CameraWrap::IsOpen(const Napi::CallbackInfo& info) {
  bool v = open_;
  try {
    if (camera_ != nullptr) v = camera_->IsOpen();
  } catch (...) {
    v = open_;
  }
  return Napi::Boolean::New(info.Env(), v);
}

Napi::Value CameraWrap::Open(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr) {
      Napi::Error::New(env, "InstantCamera not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    // Optional access-mode argument (forward-compatible validation;
    // CInstantCamera::Open() itself opens with the default mode).
    if (info.Length() >= 1 && !info[0].IsUndefined()) {
      bool modeOk = false;
      if (info[0].IsNumber()) {
        switch (info[0].As<Napi::Number>().Int32Value()) {
          case 1:   // Control
          case 3:   // Stream
          case 4:   // Event
          case 5:   // Exclusive
            modeOk = true;
            break;
          default:
            break;
        }
      } else if (info[0].IsString()) {
        const std::string m = info[0].As<Napi::String>().Utf8Value();
        modeOk = (m == "Control" || m == "Stream" || m == "Event" ||
                  m == "Exclusive");
      }
      if (!modeOk) {
        Napi::RangeError::New(
            env, "Unknown access mode (Control/Stream/Event/Exclusive)")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    if (!camera_->IsOpen()) camera_->Open();
    open_ = true;
    genapi_ = &camera_->GetNodeMap();
    nodemap_.Rebind(genapi_);
    FireConfigurationEvent("OnOpened");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::Close(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ != nullptr) {
      // Drop GenApi callbacks before Close while nodes are still alive.
      nodemap_.ClearCallbacks();
      if (camera_->IsGrabbing()) {
        camera_->StopGrabbing();
        grabbing_ = false;
        FireConfigurationEvent("OnGrabStopped");
      }
      if (camera_->IsOpen()) camera_->Close();
    }
    grabbing_ = false;
    open_ = false;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
    FireConfigurationEvent("OnClosed");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::GetDeviceInfo(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr) {
      Napi::Error::New(env, "InstantCamera not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Pylon::CDeviceInfo di = camera_->GetDeviceInfo();
    Napi::Object o = Napi::Object::New(env);
    o.Set("friendlyName", std::string(di.GetFriendlyName().c_str()));
    o.Set("modelName", std::string(di.GetModelName().c_str()));
    o.Set("serialNumber", std::string(di.GetSerialNumber().c_str()));
    o.Set("vendorName", std::string(di.GetVendorName().c_str()));
    o.Set("deviceClass", std::string(di.GetDeviceClass().c_str()));
    o.Set("fullName", std::string(di.GetFullName().c_str()));
    return o;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

// --- Device attach/detach/destroy ------------------------------------------

Napi::Value CameraWrap::IsPylonDeviceAttached(
    const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (camera_ != nullptr) v = camera_->IsPylonDeviceAttached();
  } catch (...) {
    v = false;
  }
  return Napi::Boolean::New(info.Env(), v);
}

Napi::Value CameraWrap::HasOwnership(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (camera_ != nullptr) v = camera_->HasOwnership();
  } catch (...) {
    v = false;
  }
  return Napi::Boolean::New(info.Env(), v);
}

Napi::Value CameraWrap::AttachDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr) {
      Napi::Error::New(env, "InstantCamera not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (camera_->IsPylonDeviceAttached()) {
      Napi::Error::New(env,
                        "A device is already attached. Call detachDevice() "
                        "or destroyDevice() first.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    size_t index = 0;
    if (info.Length() >= 1 && !info[0].IsUndefined()) {
      if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "attachDevice(deviceIndex?: number) expected")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      int64_t v = info[0].As<Napi::Number>().Int64Value();
      if (v < 0) {
        Napi::RangeError::New(env, "device index out of range")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      index = static_cast<size_t>(v);
    }
    AttachEnumeratedDevice(index);
    FireConfigurationEvent("OnAttached");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::DetachDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr || !camera_->IsPylonDeviceAttached()) {
      Napi::Error::New(env, "No device attached.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    if (camera_->IsGrabbing()) {
      camera_->StopGrabbing();
      grabbing_ = false;
      FireConfigurationEvent("OnGrabStopped");
    }
    if (camera_->IsOpen()) camera_->Close();
    open_ = false;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
    // Ownership transfers to the caller; destroy the native handle via the
    // transport-layer factory (like TlFactory.DestroyDevice in pypylon).
    Pylon::IPylonDevice* device = camera_->DetachDevice();
    Napi::Object result = Napi::Object::New(env);
    try {
      Pylon::CDeviceInfo di = device->GetDeviceInfo();
      result.Set("friendlyName", std::string(di.GetFriendlyName().c_str()));
      result.Set("modelName", std::string(di.GetModelName().c_str()));
      result.Set("serialNumber", std::string(di.GetSerialNumber().c_str()));
      result.Set("vendorName", std::string(di.GetVendorName().c_str()));
      result.Set("deviceClass", std::string(di.GetDeviceClass().c_str()));
      result.Set("fullName", std::string(di.GetFullName().c_str()));
    } catch (...) {
      // Device info is best effort; detachment still succeeded.
    }
    Pylon::CTlFactory::GetInstance().DestroyDevice(device);
    FireConfigurationEvent("OnDetached");
    return result;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value CameraWrap::DestroyDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ != nullptr && camera_->IsPylonDeviceAttached()) {
      if (camera_->IsGrabbing()) {
        camera_->StopGrabbing();
        grabbing_ = false;
        FireConfigurationEvent("OnGrabStopped");
      }
      if (camera_->IsOpen()) camera_->Close();
      camera_->DestroyDevice();
    }
    grabbing_ = false;
    open_ = false;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
    FireConfigurationEvent("OnDetached");
    FireConfigurationEvent("OnDestroyed");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::IsCameraDeviceRemoved(
    const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (camera_ != nullptr) v = camera_->IsCameraDeviceRemoved();
  } catch (...) {
    v = false;
  }
  return Napi::Boolean::New(info.Env(), v);
}

Napi::Value CameraWrap::GetCameraContext(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    intptr_t ctx = cameraContext_;
    if (camera_ != nullptr) ctx = camera_->GetCameraContext();
    return Napi::Number::New(env, static_cast<double>(ctx));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value CameraWrap::SetCameraContext(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "setCameraContext(context: number) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  intptr_t ctx = static_cast<intptr_t>(info[0].As<Napi::Number>().Int64Value());
  try {
    cameraContext_ = ctx;
    if (camera_ != nullptr) camera_->SetCameraContext(ctx);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetQueuedBufferCount(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr || !camera_->IsPylonDeviceAttached()) {
      Napi::Error::New(env, "No device attached.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    size_t n = camera_->GetQueuedBufferCount();
    return Napi::Number::New(env, static_cast<double>(n));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

// --- Grab loop (pypylon style) ---------------------------------------------

Napi::Value CameraWrap::StartGrabbing(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  // startGrabbing([strategy], [grabLoop]) or startGrabbing({strategy,
  // grabLoop, maxImages}). No-arg default keeps the established
  // LatestImageOnly behavior; explicit values map 1:1 onto the C++ enums.
  Pylon::EGrabStrategy strategy = Pylon::GrabStrategy_LatestImageOnly;
  Pylon::EGrabLoop grabLoop = Pylon::GrabLoop_ProvidedByUser;
  bool hasMaxImages = false;
  size_t maxImages = 0;
  if (info.Length() >= 1 && !info[0].IsUndefined()) {
    if (info[0].IsObject() && !info[0].IsFunction()) {
      Napi::Object opts = info[0].As<Napi::Object>();
      if (opts.Has("strategy")) {
        if (!ParseGrabStrategy(opts.Get("strategy"), &strategy)) {
          Napi::RangeError::New(env, "Unknown grab strategy "
                                     "(OneByOne/LatestImageOnly/"
                                     "LatestImages/UpcomingImage)")
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
      if (opts.Has("grabLoop")) {
        if (!ParseGrabLoop(opts.Get("grabLoop"), &grabLoop)) {
          Napi::RangeError::New(env, "Unknown grab loop "
                                     "(ProvidedByInstantCamera/ProvidedByUser)")
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
      if (opts.Has("maxImages")) {
        Napi::Value m = opts.Get("maxImages");
        if (!m.IsNumber() || m.As<Napi::Number>().Int64Value() <= 0) {
          Napi::RangeError::New(env, "maxImages must be a positive number")
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
        hasMaxImages = true;
        maxImages = static_cast<size_t>(m.As<Napi::Number>().Int64Value());
      }
    } else {
      if (!ParseGrabStrategy(info[0], &strategy)) {
        Napi::RangeError::New(env, "Unknown grab strategy "
                                   "(OneByOne/LatestImageOnly/"
                                   "LatestImages/UpcomingImage)")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      if (info.Length() >= 2 && !info[1].IsUndefined()) {
        if (!ParseGrabLoop(info[1], &grabLoop)) {
          Napi::RangeError::New(env, "Unknown grab loop "
                                     "(ProvidedByInstantCamera/ProvidedByUser)")
              .ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
  } else if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseGrabLoop(info[1], &grabLoop)) {
      Napi::RangeError::New(env, "Unknown grab loop "
                                 "(ProvidedByInstantCamera/ProvidedByUser)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsOpen()) {
      Napi::Error::New(env, "Camera is not open. Call open() first.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!camera_->IsGrabbing()) {
      if (hasMaxImages) {
        camera_->StartGrabbing(maxImages, strategy, grabLoop);
      } else {
        camera_->StartGrabbing(strategy, grabLoop);
      }
    }
    grabbing_ = true;
    FireConfigurationEvent("OnGrabStarted");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::StartGrabbingMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber() ||
      info[0].As<Napi::Number>().Int64Value() <= 0) {
    Napi::TypeError::New(
        env, "startGrabbingMax(maxImages: number, strategy?, grabLoop?) "
             "expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  size_t maxImages =
      static_cast<size_t>(info[0].As<Napi::Number>().Int64Value());
  Pylon::EGrabStrategy strategy = Pylon::GrabStrategy_OneByOne;
  Pylon::EGrabLoop grabLoop = Pylon::GrabLoop_ProvidedByUser;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseGrabStrategy(info[1], &strategy)) {
      Napi::RangeError::New(env, "Unknown grab strategy")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
  if (info.Length() >= 3 && !info[2].IsUndefined()) {
    if (!ParseGrabLoop(info[2], &grabLoop)) {
      Napi::RangeError::New(env, "Unknown grab loop")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsOpen()) {
      Napi::Error::New(env, "Camera is not open. Call open() first.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!camera_->IsGrabbing()) {
      camera_->StartGrabbing(maxImages, strategy, grabLoop);
    }
    grabbing_ = true;
    FireConfigurationEvent("OnGrabStarted");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::StopGrabbing(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  (void)info;
  try {
    if (camera_ != nullptr && camera_->IsGrabbing()) {
      camera_->StopGrabbing();
      FireConfigurationEvent("OnGrabStopped");
    }
    grabbing_ = false;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::IsGrabbing(const Napi::CallbackInfo& info) {
  bool v = grabbing_;
  try {
    if (camera_ != nullptr) v = camera_->IsGrabbing();
  } catch (...) {
    v = grabbing_;
  }
  return Napi::Boolean::New(info.Env(), v);
}

// Shared grab-result object (validity/unique semantics): isValid mirrors
// CGrabResultPtr::IsValid(); the result is unique to the caller (fresh
// CGrabResultPtr per call, buffer copied into a JS-owned Buffer).
Napi::Object CameraWrap::BuildGrabResultObject(
    Napi::Env env, const Pylon::CGrabResultPtr& ptr) {
  Napi::Object out = Napi::Object::New(env);
  out.Set("isValid", Napi::Boolean::New(env, ptr.IsValid()));
  const bool succeeded = ptr.IsValid() && ptr->GrabSucceeded();
  out.Set("succeeded", succeeded);
  // pypylon naming: GrabSucceeded() accessor expected by ported tests.
  out.Set("grabSucceeded",
          Napi::Function::New(env, [succeeded](const Napi::CallbackInfo& cb) {
            return Napi::Boolean::New(cb.Env(), succeeded);
          }));
  size_t w = 0, h = 0;
  if (succeeded) {
    w = static_cast<size_t>(ptr->GetWidth());
    h = static_cast<size_t>(ptr->GetHeight());
  }
  out.Set("width", Napi::Number::New(env, static_cast<double>(w)));
  out.Set("height", Napi::Number::New(env, static_cast<double>(h)));
  out.Set("pixelType", DescribePixelType(ptr));
  out.Set("pixelFormat", DescribePixelType(ptr));
  const size_t payload =
      succeeded ? static_cast<size_t>(ptr->GetPayloadSize()) : 0;
  out.Set("payloadSize", Napi::Number::New(env, static_cast<double>(payload)));
  if (succeeded && payload > 0) {
    auto* data = new uint8_t[payload];
    std::memcpy(data, ptr->GetBuffer(), payload);
    out.Set("buffer", Napi::Buffer<uint8_t>::New(
                          env, data, payload,
                          [](Napi::Env /*e*/, uint8_t* p) { delete[] p; }));
  } else {
    out.Set("buffer",
            Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(0)));
    if (ptr.IsValid() && !succeeded) {
      out.Set("errorCode", Napi::Number::New(
                               env, static_cast<double>(ptr->GetErrorCode())));
      out.Set("errorDescription",
              std::string(ptr->GetErrorDescription().c_str()));
    }
  }
  if (ptr.IsValid()) {
    out.Set("blockId",
            Napi::Number::New(env, static_cast<double>(ptr->GetBlockID())));
    out.Set("frameNumber",
            Napi::Number::New(env, static_cast<double>(ptr->GetBlockID())));
    out.Set("timestamp",
            Napi::Number::New(env, static_cast<double>(ptr->GetTimeStamp())));
  } else {
    out.Set("blockId", Napi::Number::New(env, 0));
    out.Set("frameNumber", Napi::Number::New(env, 0));
    out.Set("timestamp", Napi::Number::New(env, 0));
  }
  return out;
}

void CameraWrap::FireConfigurationEvent(const char* event_name) {
  if (configHandlers_.empty()) return;
  Napi::Env env = Env();
  Napi::HandleScope scope(env);
  for (auto& entry : configHandlers_) {
    try {
      entry.second.Call(env.Global(), {Napi::String::New(env, event_name)});
    } catch (...) {
      // Handler errors never propagate into the SDK call path.
    }
    if (env.IsExceptionPending()) {
      // A throwing JS handler leaves a pending exception; swallow it so
      // the native call still returns normally.
      env.GetAndClearPendingException();
    }
  }
}

void CameraWrap::FireImageEventHandlers(Napi::Env env, Napi::Object result) {
  if (imageHandlers_.empty()) return;
  for (auto& entry : imageHandlers_) {
    try {
      entry.second.Call(env.Global(), {result});
    } catch (...) {
    }
    if (env.IsExceptionPending()) {
      env.GetAndClearPendingException();
    }
  }
}

// retrieveResult(timeoutMs?, timeoutHandling?) -> grab object or null.
// TimeoutHandling_Return (default): null on timeout. ThrowException: throws.
Napi::Value CameraWrap::RetrieveResult(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeoutMs = 5000;
  if (info.Length() >= 1 && info[0].IsNumber()) {
    timeoutMs = info[0].As<Napi::Number>().Uint32Value();
  }
  Pylon::ETimeoutHandling handling = Pylon::TimeoutHandling_Return;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseTimeoutHandling(info[1], &handling)) {
      Napi::RangeError::New(env, "Unknown timeout handling "
                                 "(Return/ThrowException)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsGrabbing()) {
      Napi::Error::New(env, "Not grabbing. Call startGrabbing() first.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Pylon::CGrabResultPtr ptr;
    bool ok = camera_->RetrieveResult(timeoutMs, ptr, handling);
    if (!ok || !ptr.IsValid()) {
      if (handling == Pylon::TimeoutHandling_ThrowException) {
        Napi::Error::New(env, "Grab timeout: no result within timeoutMs")
            .ThrowAsJavaScriptException();
      }
      return env.Null();
    }
    Napi::Object out = BuildGrabResultObject(env, ptr);
    FireImageEventHandlers(env, out);
    return out;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

// retrieveGrabResult(timeoutMs?, timeoutHandling?) -> GrabResult ObjectWrap
// or null on timeout. Same acquisition as retrieveResult but returns the
// full GrabResult wrapper (IsValid/IsUnique/Release, GetBuffer/GetArray/
// GetMemoryView, geometry, pixel/payload types, timestamps, chunk data,
// error codes, GenDC components) with zero-copy buffer views.
Napi::Value CameraWrap::RetrieveGrabResult(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeoutMs = 5000;
  if (info.Length() >= 1 && info[0].IsNumber()) {
    timeoutMs = info[0].As<Napi::Number>().Uint32Value();
  }
  Pylon::ETimeoutHandling handling = Pylon::TimeoutHandling_Return;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseTimeoutHandling(info[1], &handling)) {
      Napi::RangeError::New(env, "Unknown timeout handling "
                                 "(Return/ThrowException)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsGrabbing()) {
      Napi::Error::New(env, "Not grabbing. Call startGrabbing() first.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Pylon::CGrabResultPtr ptr;
    bool ok = camera_->RetrieveResult(timeoutMs, ptr, handling);
    if (!ok || !ptr.IsValid()) {
      if (handling == Pylon::TimeoutHandling_ThrowException) {
        Napi::Error::New(env, "Grab timeout: no result within timeoutMs")
            .ThrowAsJavaScriptException();
      }
      return env.Null();
    }
    Napi::Object result = GrabResultWrap::FromPtr(env, ptr);
    FireImageEventHandlers(env, result);
    return result;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

// grabOne(timeoutMs?, timeoutHandling?) -> grab object. Must not be called
// while grabbing (use retrieveResult there), like CInstantCamera::GrabOne.
Napi::Value CameraWrap::GrabOne(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeoutMs = 5000;
  if (info.Length() >= 1 && info[0].IsNumber()) {
    timeoutMs = info[0].As<Napi::Number>().Uint32Value();
  }
  Pylon::ETimeoutHandling handling = Pylon::TimeoutHandling_ThrowException;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseTimeoutHandling(info[1], &handling)) {
      Napi::RangeError::New(env, "Unknown timeout handling "
                                 "(Return/ThrowException)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsPylonDeviceAttached()) {
      Napi::Error::New(env, "No device attached.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    if (camera_->IsGrabbing()) {
      Napi::Error::New(env, "GrabOne cannot be called while grabbing. "
                            "Use retrieveResult() instead.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Pylon::CGrabResultPtr ptr;
    bool ok = camera_->GrabOne(timeoutMs, ptr, handling);
    if (!ok || !ptr.IsValid()) {
      if (handling == Pylon::TimeoutHandling_ThrowException) {
        Napi::Error::New(env, "Grab timeout: no result within timeoutMs")
            .ThrowAsJavaScriptException();
      }
      return env.Null();
    }
    Napi::Object out = BuildGrabResultObject(env, ptr);
    FireImageEventHandlers(env, out);
    return out;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value CameraWrap::WaitForFrameTriggerReady(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeoutMs = 1000;
  if (info.Length() >= 1 && info[0].IsNumber()) {
    timeoutMs = info[0].As<Napi::Number>().Uint32Value();
  }
  Pylon::ETimeoutHandling handling = Pylon::TimeoutHandling_ThrowException;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseTimeoutHandling(info[1], &handling)) {
      Napi::RangeError::New(env, "Unknown timeout handling "
                                 "(Return/ThrowException)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  try {
    if (camera_ == nullptr || !camera_->IsGrabbing()) {
      Napi::Error::New(env, "Not grabbing. Call startGrabbing() first.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    bool ready = camera_->WaitForFrameTriggerReady(timeoutMs, handling);
    return Napi::Boolean::New(env, ready);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value CameraWrap::ExecuteSoftwareTrigger(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  (void)info;
  try {
    if (camera_ == nullptr || !camera_->IsGrabbing()) {
      Napi::Error::New(env, "Not grabbing. Call startGrabbing() first.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    camera_->ExecuteSoftwareTrigger();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

// --- Event-handler registration --------------------------------------------

Napi::Value CameraWrap::RegisterConfiguration(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsFunction()) {
    Napi::TypeError::New(env, "registerConfiguration(handler: function, "
                              "mode?) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Pylon::ERegistrationMode mode = Pylon::RegistrationMode_Append;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseRegistrationMode(info[1], &mode)) {
      Napi::RangeError::New(env, "Unknown registration mode "
                                 "(Append/ReplaceAll)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  if (mode == Pylon::RegistrationMode_ReplaceAll) configHandlers_.clear();
  uint32_t id = nextHandlerId_++;
  Napi::Function fn = info[0].As<Napi::Function>();
  configHandlers_.emplace_back(id, Napi::Persistent(fn));
  return Napi::Number::New(env, id);
}

bool DeregisterByIdOrFn(
    Napi::Env env, const Napi::CallbackInfo& info,
    std::vector<std::pair<uint32_t, Napi::FunctionReference>>* handlers) {
  if (info.Length() < 1) return false;
  if (info[0].IsNumber()) {
    uint32_t id = info[0].As<Napi::Number>().Uint32Value();
    for (auto it = handlers->begin(); it != handlers->end(); ++it) {
      if (it->first == id) {
        handlers->erase(it);
        return true;
      }
    }
    return false;
  }
  if (info[0].IsFunction()) {
    Napi::Function fn = info[0].As<Napi::Function>();
    for (auto it = handlers->begin(); it != handlers->end(); ++it) {
      bool equal = false;
      napi_status st = napi_strict_equals(
          env, fn, it->second.Value(), &equal);
      if (st == napi_ok && equal) {
        handlers->erase(it);
        return true;
      }
    }
    return false;
  }
  return false;
}

Napi::Value CameraWrap::DeregisterConfiguration(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool removed = DeregisterByIdOrFn(env, info, &configHandlers_);
  return Napi::Boolean::New(env, removed);
}

Napi::Value CameraWrap::RegisterImageEventHandler(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsFunction()) {
    Napi::TypeError::New(env, "registerImageEventHandler(handler: function, "
                              "mode?) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Pylon::ERegistrationMode mode = Pylon::RegistrationMode_Append;
  if (info.Length() >= 2 && !info[1].IsUndefined()) {
    if (!ParseRegistrationMode(info[1], &mode)) {
      Napi::RangeError::New(env, "Unknown registration mode "
                                 "(Append/ReplaceAll)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  if (mode == Pylon::RegistrationMode_ReplaceAll) imageHandlers_.clear();
  uint32_t id = nextHandlerId_++;
  Napi::Function fn = info[0].As<Napi::Function>();
  imageHandlers_.emplace_back(id, Napi::Persistent(fn));
  return Napi::Number::New(env, id);
}

Napi::Value CameraWrap::DeregisterImageEventHandler(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool removed = DeregisterByIdOrFn(env, info, &imageHandlers_);
  return Napi::Boolean::New(env, removed);
}

Napi::Value CameraWrap::RegisterCameraEventHandler(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsFunction()) {
    Napi::TypeError::New(env, "registerCameraEventHandler(handler: function, "
                              "nodeName?, mode?) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  std::string nodeName;
  size_t modeIndex = 1;
  if (info.Length() >= 2 && info[1].IsString()) {
    nodeName = info[1].As<Napi::String>().Utf8Value();
    modeIndex = 2;
  }
  Pylon::ERegistrationMode mode = Pylon::RegistrationMode_Append;
  if (info.Length() > modeIndex && !info[modeIndex].IsUndefined()) {
    if (!ParseRegistrationMode(info[modeIndex], &mode)) {
      Napi::RangeError::New(env, "Unknown registration mode "
                                 "(Append/ReplaceAll)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
  }
  if (mode == Pylon::RegistrationMode_ReplaceAll) cameraEventHandlers_.clear();
  uint32_t id = nextHandlerId_++;
  Napi::Function fn = info[0].As<Napi::Function>();
  CameraEventRegistration reg;
  reg.id = id;
  reg.fn = Napi::Persistent(fn);
  reg.nodeName = nodeName;
  cameraEventHandlers_.push_back(std::move(reg));
  return Napi::Number::New(env, id);
}

Napi::Value CameraWrap::DeregisterCameraEventHandler(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) return Napi::Boolean::New(env, false);
  if (info[0].IsNumber()) {
    uint32_t id = info[0].As<Napi::Number>().Uint32Value();
    for (auto it = cameraEventHandlers_.begin();
         it != cameraEventHandlers_.end(); ++it) {
      if (it->id == id) {
        cameraEventHandlers_.erase(it);
        return Napi::Boolean::New(env, true);
      }
    }
    return Napi::Boolean::New(env, false);
  }
  if (info[0].IsFunction()) {
    Napi::Function fn = info[0].As<Napi::Function>();
    for (auto it = cameraEventHandlers_.begin();
         it != cameraEventHandlers_.end(); ++it) {
      bool equal = false;
      napi_status st = napi_strict_equals(
          env, fn, it->fn.Value(), &equal);
      if (st == napi_ok && equal) {
        cameraEventHandlers_.erase(it);
        return Napi::Boolean::New(env, true);
      }
    }
    return Napi::Boolean::New(env, false);
  }
  return Napi::Boolean::New(env, false);
}

Napi::Value CameraWrap::GetNodeType(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getNodeType(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeType t;
  NodeError e = nodemap_.GetType(name, &t);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, NodeTypeName(t));
}

Napi::Value CameraWrap::IsReadable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isReadable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsReadable(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::IsWritable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isWritable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsWritable(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetInteger(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getInteger(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetInteger(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::SetInteger(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "setInteger(name: string, value: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = info[1].As<Napi::Number>().Int64Value();
  NodeError e = nodemap_.SetInteger(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetIntegerMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t mn = 0, mx = 0;
  NodeError e = nodemap_.GetIntegerLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(mn));
}

Napi::Value CameraWrap::GetIntegerMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t mn = 0, mx = 0;
  NodeError e = nodemap_.GetIntegerLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(mx));
}

Napi::Value CameraWrap::TryMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "tryMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = 0;
  NodeError e = nodemap_.TryMax(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, v);
}

Napi::Value CameraWrap::GetFloat(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloat(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = 0;
  NodeError e = nodemap_.GetFloat(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, v);
}

Napi::Value CameraWrap::SetFloat(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "setFloat(name: string, value: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = info[1].As<Napi::Number>().DoubleValue();
  NodeError e = nodemap_.SetFloat(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetFloatMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double mn = 0, mx = 0;
  NodeError e = nodemap_.GetFloatLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, mn);
}

Napi::Value CameraWrap::GetFloatMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double mn = 0, mx = 0;
  NodeError e = nodemap_.GetFloatLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, mx);
}

Napi::Value CameraWrap::GetBoolean(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getBoolean(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.GetBoolean(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::SetBoolean(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsBoolean()) {
    Napi::TypeError::New(env,
                          "setBoolean(name: string, value: boolean) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = info[1].As<Napi::Boolean>().Value();
  NodeError e = nodemap_.SetBoolean(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getString(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetString(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsString()) {
    Napi::TypeError::New(env, "setString(name: string, value: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v = info[1].As<Napi::String>().Utf8Value();
  NodeError e = nodemap_.SetString(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetEnum(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getEnum(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetEnum(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetEnum(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsString()) {
    Napi::TypeError::New(env, "setEnum(name: string, value: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v = info[1].As<Napi::String>().Utf8Value();
  NodeError e = nodemap_.SetEnum(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetEnumEntries(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getEnumEntries(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<std::string> entries;
  NodeError e = nodemap_.GetEnumEntries(name, &entries);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  Napi::Array arr = Napi::Array::New(env, entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), Napi::String::New(env, entries[i]));
  }
  return arr;
}

Napi::Value CameraWrap::ExecuteCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "executeCommand(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.Execute(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  if (name == "AcquisitionStart") grabbing_ = true;
  if (name == "AcquisitionStop") grabbing_ = false;
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::Grab(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!open_) {
    Napi::Error::New(env, "Camera is not open. Call open() first.")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  int64_t w = 0, h = 0;
  std::string pixelFormat;
  NodeError e = nodemap_.GetImageGeometry(&w, &h, &pixelFormat);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, "Width/Height/PixelFormat", e);
    return env.Null();
  }
  int bpp = RealNodeMap::BytesPerPixel(pixelFormat);
  size_t bytes = static_cast<size_t>(w) * static_cast<size_t>(h) *
                 static_cast<size_t>(bpp);
  auto* data = new uint8_t[bytes];
  uint64_t frame = frameNumber_++;
  // Deterministic test pattern: (i + frame) & 0xFF per byte.
  for (size_t i = 0; i < bytes; ++i) {
    data[i] = static_cast<uint8_t>((i + frame) & 0xFF);
  }
  Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(
      env, data, bytes, [](Napi::Env /*env*/, uint8_t* ptr) { delete[] ptr; });

  bool chunkActive = false;
  nodemap_.GetBoolean("ChunkModeActive", &chunkActive);
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  uint64_t ts = static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());

  Napi::Object out = Napi::Object::New(env);
  out.Set("buffer", buf);
  out.Set("width", Napi::Number::New(env, static_cast<double>(w)));
  out.Set("height", Napi::Number::New(env, static_cast<double>(h)));
  out.Set("pixelType", Napi::String::New(env, pixelFormat));
  out.Set("pixelFormat", Napi::String::New(env, pixelFormat));
  out.Set("payloadSize", Napi::Number::New(env, static_cast<double>(bytes)));
  out.Set("timestamp", Napi::Number::New(env, static_cast<double>(ts)));
  out.Set("frameNumber", Napi::Number::New(env, static_cast<double>(frame)));
  Napi::Object chunk = Napi::Object::New(env);
  chunk.Set("modeActive", Napi::Boolean::New(env, chunkActive));
  chunk.Set("timestamp", Napi::Number::New(env, static_cast<double>(ts)));
  chunk.Set("frameID",
            Napi::Number::New(env, static_cast<double>(frame)));
  out.Set("chunk", chunk);
  return out;
}

Napi::Value CameraWrap::IsAvailable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isAvailable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsAvailable(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetDisplayName(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getDisplayName(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetDisplayName(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::GetDescription(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getDescription(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetDescription(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::GetIntegerInc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerInc(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetIntegerInc(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::GetIntegerUnit(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerUnit(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetIntegerUnit(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::GetIntegerRepresentation(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerRepresentation(name) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetIntegerRepresentation(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetIntegerToMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "setIntegerToMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.SetIntegerToMin(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::SetIntegerToMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "setIntegerToMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.SetIntegerToMax(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::HasFloatInc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "hasFloatInc(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.HasFloatInc(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetFloatInc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatInc(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = 0;
  NodeError e = nodemap_.GetFloatInc(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, v);
}

Napi::Value CameraWrap::GetFloatUnit(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatUnit(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetFloatUnit(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::GetFloatRepresentation(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatRepresentation(name) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetFloatRepresentation(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetFloatToMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "setFloatToMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.SetFloatToMin(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::SetFloatToMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "setFloatToMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.SetFloatToMax(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetValueAsString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getValueAsString(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetValueAsString(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetValueFromString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsString()) {
    Napi::TypeError::New(env, "setValueFromString(name, value) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v = info[1].As<Napi::String>().Utf8Value();
  NodeError e = nodemap_.SetValueFromString(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetStringMaxLength(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getStringMaxLength(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetStringMaxLength(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::GetEnumIntValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getEnumIntValue(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetEnumIntValue(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::SetEnumIntValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "setEnumIntValue(name, value) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = info[1].As<Napi::Number>().Int64Value();
  NodeError e = nodemap_.SetEnumIntValue(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::IsCommandDone(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isCommandDone(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsCommandDone(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetCategoryFeatures(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getCategoryFeatures(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<std::string> entries;
  NodeError e = nodemap_.GetCategoryFeatures(name, &entries);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  Napi::Array arr = Napi::Array::New(env, entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), Napi::String::New(env, entries[i]));
  }
  return arr;
}

Napi::Value CameraWrap::IsSelector(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isSelector(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsSelector(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetSelectedFeatures(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getSelectedFeatures(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<std::string> entries;
  NodeError e = nodemap_.GetSelectedFeatures(name, &entries);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  Napi::Array arr = Napi::Array::New(env, entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), Napi::String::New(env, entries[i]));
  }
  return arr;
}

Napi::Value CameraWrap::GetSelectingFeatures(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getSelectingFeatures(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<std::string> entries;
  NodeError e = nodemap_.GetSelectingFeatures(name, &entries);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  Napi::Array arr = Napi::Array::New(env, entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), Napi::String::New(env, entries[i]));
  }
  return arr;
}

Napi::Value CameraWrap::GetRegisterLength(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getRegisterLength(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetRegisterLength(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::GetRegisterAddress(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getRegisterAddress(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetRegisterAddress(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::ReadRegister(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "readRegister(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<uint8_t> data;
  NodeError e = nodemap_.ReadRegister(name, &data);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  size_t len = data.size();
  auto* out = new uint8_t[len > 0 ? len : 1];
  for (size_t i = 0; i < len; ++i) out[i] = data[i];
  return Napi::Buffer<uint8_t>::New(
      env, out, len, [](Napi::Env /*e*/, uint8_t* p) { delete[] p; });
}

Napi::Value CameraWrap::WriteRegister(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsBuffer()) {
    Napi::TypeError::New(env, "writeRegister(name, Buffer) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  Napi::Buffer<uint8_t> buf = info[1].As<Napi::Buffer<uint8_t>>();
  std::vector<uint8_t> data(buf.Data(), buf.Data() + buf.Length());
  NodeError e = nodemap_.WriteRegister(name, data);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::RegisterCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "registerCallback(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t handle = 0;
  NodeError e = nodemap_.RegisterCallback(name, &handle);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(handle));
}

Napi::Value CameraWrap::GetCallbackCount(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "getCallbackCount(handle: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t handle = info[0].As<Napi::Number>().Int64Value();
  int count = 0;
  NodeError e = nodemap_.GetCallbackCount(handle, &count);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, "", e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(count));
}

Napi::Value CameraWrap::DeregisterCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "deregisterCallback(handle: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t handle = info[0].As<Napi::Number>().Int64Value();
  NodeError e = nodemap_.DeregisterCallback(handle);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, "", e);
    return env.Null();
  }
  return env.Undefined();
}

}  // namespace node_pylon
