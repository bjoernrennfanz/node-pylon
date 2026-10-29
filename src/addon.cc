// node-pylon N-API module (mirrors the pypylon top-level API).
// Default: real build against Basler pylon SDK 26.09 (NODE_PYLON_MOCK=0).
// Mock only as an explicit opt-in (NODE_PYLON_MOCK=1, no SDK).
// Rules: N-API only (node-addon-api), no SWIG/NAN, no V8 internals,
// no C++ exceptions across the N-API boundary (NAPI_DISABLE_CPP_EXCEPTIONS).
#include <napi.h>

#include <string>

#include "mock_camera.h"
#include "pylon_lifecycle.h"

#if !NODE_PYLON_MOCK
#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>
#endif

// ---------------------------------------------------------------------------
// Versions: mock state vs. real SDK build are distinguishable.
// ---------------------------------------------------------------------------
namespace mock {
constexpr const char* kVersionMock = "0.2.0-core-mock";
constexpr const char* kVersionReal = "0.2.0-core";
constexpr const char* kPylonVersionMock = "mock-8.x";
}  // namespace mock

namespace {

Napi::String GetVersion(const Napi::CallbackInfo& info) {
#if NODE_PYLON_MOCK
  return Napi::String::New(info.Env(), mock::kVersionMock);
#else
  return Napi::String::New(info.Env(), mock::kVersionReal);
#endif
}

Napi::String GetPylonVersion(const Napi::CallbackInfo& info) {
#if NODE_PYLON_MOCK
  return Napi::String::New(info.Env(), mock::kPylonVersionMock);
#else
// Real version straight from the SDK (pylon 26.09: e.g. "12.5.2.1565").
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
#endif
}

// PylonInitialize with refcount. Balance: every initialize()
// needs a terminate(); each InstantCamera additionally holds one ref
// for its lifetime (see CameraWrap constructor/destructor).
Napi::Value PylonInitialize(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    node_pylon::lifecycle::EnsureInitialized();
  }
#if !NODE_PYLON_MOCK
  catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
#endif
  catch (const std::exception& ex) {
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

// Lists detected devices. Real build: real enumeration via the
// pylon TlFactory (empty list without camera/emulator). Mock opt-in:
// exactly one synthetic device (mirrors the pylon emulator).
Napi::Array EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
#if NODE_PYLON_MOCK
  Napi::Array arr = Napi::Array::New(env, 1);
  Napi::Object dev = Napi::Object::New(env);
  dev.Set("friendlyName", "Mock Camera (node-pylon)");
  dev.Set("modelName", "MockModel Mono");
  dev.Set("serialNumber", "MOCK123456");
  dev.Set("vendorName", "Basler (mock)");
  dev.Set("deviceClass", "MockDevice");
  arr.Set(uint32_t(0), dev);
  return arr;
#else
  // Real TlFactory enumeration. PylonInitialize is refcounted in this
  // module (see pylon_lifecycle.h): take a balanced reference around the
  // call so enumerateDevices() also works without a prior initialize().
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
#endif
}

// createFirstCamera(): JS shorthand for `new InstantCamera()`
// (TlFactory.CreateFirstDevice + wiring, mirrors pypylon).
Napi::Value CreateFirstCamera(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Object obj = node_pylon::CameraWrap::constructor.New({});
  if (env.IsExceptionPending()) return env.Null();
  return obj;
}

// Synthetic Mono8 frame as a plain buffer (legacy helper,
// kept for compatibility; new samples use camera.grab()).
Napi::Value GrabMock(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t width = 64;
  size_t height = 48;
  if (info.Length() >= 2) {
    if (info[0].IsNumber()) width = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsNumber()) height = info[1].As<Napi::Number>().Uint32Value();
  }
  if (width == 0 || height == 0 || width > 4096 || height > 4096) {
    Napi::TypeError::New(env, "width/height must be in 1..4096")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  size_t bytes = width * height;  // Mono8
  auto data = new uint8_t[bytes];
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      data[y * width + x] = static_cast<uint8_t>((x + y) & 0xFF);
    }
  }
  // Buffer uebernimmt Ownership (Finalizer gibt Speicher frei).
  return Napi::Buffer<uint8_t>::New(
      env, data, bytes, [](Napi::Env /*env*/, uint8_t* ptr) { delete[] ptr; });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, GetVersion));
  exports.Set("pylonVersion", Napi::Function::New(env, GetPylonVersion));
  exports.Set("initialize", Napi::Function::New(env, PylonInitialize));
  exports.Set("terminate", Napi::Function::New(env, PylonTerminate));
  exports.Set("enumerateDevices", Napi::Function::New(env, EnumerateDevices));
  exports.Set("createFirstCamera",
              Napi::Function::New(env, CreateFirstCamera));
  exports.Set("grabMock", Napi::Function::New(env, GrabMock));
#if NODE_PYLON_MOCK
  exports.Set("mock", Napi::Boolean::New(env, true));
#else
  exports.Set("mock", Napi::Boolean::New(env, false));
#endif
  node_pylon::CameraWrap::Init(env, exports);
  return exports;
}

}  // namespace

NODE_API_MODULE(node_pylon, Init)
