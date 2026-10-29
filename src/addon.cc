// node-pylon N-API module (pypylon-style top-level API, real SDK only).
// Rules: N-API only (node-addon-api), no SWIG/NAN, no V8 internals,
// no C++ exceptions across the N-API boundary (NAPI_DISABLE_CPP_EXCEPTIONS).
#include <napi.h>

#include <string>

#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>

#include "camera_wrap.h"
#include "pylon_lifecycle.h"

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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, GetVersion));
  exports.Set("pylonVersion", Napi::Function::New(env, GetPylonVersion));
  exports.Set("initialize", Napi::Function::New(env, PylonInitialize));
  exports.Set("terminate", Napi::Function::New(env, PylonTerminate));
  exports.Set("enumerateDevices", Napi::Function::New(env, EnumerateDevices));
  exports.Set("createFirstCamera",
              Napi::Function::New(env, CreateFirstCamera));
  node_pylon::CameraWrap::Init(env, exports);
  return exports;
}

}  // namespace

NODE_API_MODULE(node_pylon, Init)
