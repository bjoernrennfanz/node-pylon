// node-pylon N-API module (pypylon-style top-level API, real SDK only).
// Rules: N-API only (node-addon-api), no SWIG/NAN, no V8 internals,
// no C++ exceptions across the N-API boundary (NAPI_DISABLE_CPP_EXCEPTIONS).
#include <napi.h>

#include <string>

#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>

#include "camera_wrap.h"
#include "grab_result.h"
#include "image_pipeline.h"
#include "pylon_image.h"
#include "pylon_lifecycle.h"
// Infra API wrappers (buffer factory, wait objects, feature persistence,
// static defect pixel correction, type mappings / DeviceInfo / exceptions).
#include "buffer_factory.h"
#include "defect_pixel.h"
#include "device_info.h"
#include "discovery.h"
#include "error_map.h"
#include "feature_persistence.h"
#include "wait_wrap.h"

namespace {

Napi::String GetVersion(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), "0.2.0-core");
}

Napi::String GetPylonVersion(const Napi::CallbackInfo& info) {
  // Real version from the SDK (pylon 26.09: e.g. "12.5.2.1565").
  const char* v = nullptr;
  try {
    v = Pylon::GetPylonVersionString();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
    return Napi::String::New(info.Env(), "");
  } catch (const std::exception& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
    return Napi::String::New(info.Env(), "");
  }
  return Napi::String::New(info.Env(), v != nullptr ? v : "");
}

// PylonInitialize with refcount. Every initialize() needs a terminate();
// each InstantCamera additionally holds one reference for its lifetime
// (see CameraWrap constructor/destructor).
Napi::Value PylonInitialize(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    node_pylon::lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value PylonTerminate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    node_pylon::lifecycle::ReleaseRef();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

// Detected devices via the pylon transport layer factory.
// Empty without camera/emulator; PYLON_CAMEMU=N provides emulator devices.
Napi::Array EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  // Balanced init/term reference so enumeration works without a prior
  // initialize() call.
  try {
    node_pylon::lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return Napi::Array::New(env, 0);
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return Napi::Array::New(env, 0);
  }
  Pylon::DeviceInfoList_t deviceList;
  int count = 0;
  try {
    count = Pylon::CTlFactory::GetInstance().EnumerateDevices(deviceList);
  } catch (const Pylon::GenericException& ex) {
    node_pylon::lifecycle::ReleaseRef();
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return Napi::Array::New(env, 0);
  } catch (const std::exception& ex) {
    node_pylon::lifecycle::ReleaseRef();
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return Napi::Array::New(env, 0);
  }
  node_pylon::lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    const Pylon::CDeviceInfo& di = deviceList[i];
    Napi::Object dev = Napi::Object::New(env);
    dev.Set("friendlyName", std::string(di.GetFriendlyName().c_str()));
    dev.Set("modelName", std::string(di.GetModelName().c_str()));
    dev.Set("serialNumber", std::string(di.GetSerialNumber().c_str()));
    dev.Set("vendorName", std::string(di.GetVendorName().c_str()));
    dev.Set("deviceClass", std::string(di.GetDeviceClass().c_str()));
    dev.Set("fullName", std::string(di.GetFullName().c_str()));
    arr.Set(static_cast<uint32_t>(i), dev);
  }
  return arr;
}

// createFirstCamera(): short form for `new InstantCamera()`
// (TlFactory.CreateFirstDevice + attach, like pypylon).
Napi::Value CreateFirstCamera(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Object obj = node_pylon::CameraWrap::constructor.New({});
  if (env.IsExceptionPending()) return env.Null();
  return obj;
}

// createCameraAt(index): attach enumerated device i (camera-array support).
Napi::Value CreateCameraAt(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "createCameraAt(deviceIndex: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Object obj = node_pylon::CameraWrap::constructor.New({info[0]});
  if (env.IsExceptionPending()) return env.Null();
  return obj;
}

void ExportEnum(Napi::Env env, Napi::Object exports, const char* name,
                int32_t value) {
  exports.Set(name, Napi::Number::New(env, value));
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, GetVersion));
  exports.Set("pylonVersion", Napi::Function::New(env, GetPylonVersion));
  exports.Set("initialize", Napi::Function::New(env, PylonInitialize));
  exports.Set("terminate", Napi::Function::New(env, PylonTerminate));
  exports.Set("enumerateDevices", Napi::Function::New(env, EnumerateDevices));
  exports.Set("createFirstCamera",
              Napi::Function::New(env, CreateFirstCamera));
  exports.Set("createCameraAt", Napi::Function::New(env, CreateCameraAt));
  // Grab strategies (CInstantCamera::EGrabStrategy).
  ExportEnum(env, exports, "GrabStrategy_OneByOne", 0);
  ExportEnum(env, exports, "GrabStrategy_LatestImageOnly", 1);
  ExportEnum(env, exports, "GrabStrategy_LatestImages", 2);
  ExportEnum(env, exports, "GrabStrategy_UpcomingImage", 3);
  // Timeout handling (ETimeoutHandling).
  ExportEnum(env, exports, "TimeoutHandling_Return", 0);
  ExportEnum(env, exports, "TimeoutHandling_ThrowException", 1);
  // Grab loop (EGrabLoop).
  ExportEnum(env, exports, "GrabLoop_ProvidedByInstantCamera", 0);
  ExportEnum(env, exports, "GrabLoop_ProvidedByUser", 1);
  // Registration mode (ERegistrationMode) and cleanup (ECleanup).
  ExportEnum(env, exports, "RegistrationMode_Append", 0);
  ExportEnum(env, exports, "RegistrationMode_ReplaceAll", 1);
  ExportEnum(env, exports, "Cleanup_None", 0);
  ExportEnum(env, exports, "Cleanup_Delete", 1);
  // Device access modes (EDeviceAccessMode bits).
  ExportEnum(env, exports, "AccessMode_Control", 1);
  ExportEnum(env, exports, "AccessMode_Stream", 3);
  ExportEnum(env, exports, "AccessMode_Event", 4);
  ExportEnum(env, exports, "AccessMode_Exclusive", 5);
  node_pylon::CameraWrap::Init(env, exports);
  node_pylon::GrabResultWrap::Init(env, exports);
  node_pylon::PylonImageWrap::Init(env, exports);
  node_pylon::InitImagePipeline(env, exports);
  // Infra APIs first create the exception hierarchy (later wrappers throw
  // translated errors through it).
  node_pylon::errormap::Init(env, exports);
  node_pylon::DeviceInfoWrap::Init(env, exports);
  node_pylon::InitDiscovery(env, exports);
  node_pylon::InitWaitApi(env, exports);
  node_pylon::feature::Init(env, exports);
  node_pylon::defect::Init(env, exports);
  node_pylon::bufferfactory::Init(env, exports);
  return exports;
}

}  // namespace

NODE_API_MODULE(node_pylon, Init)
