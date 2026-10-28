// node-pylon N-API-Modul (ZAU-149 Scaffold + ZAU-151 Nodemap/Bild-Handling).
// ZAU-150-Anteil: PylonInitialize/Terminate, createFirstCamera (TlFactory).
// Regeln: N-API only (node-addon-api), kein SWIG/NAN, keine V8-Internals,
// keine C++-Exceptions ueber die N-API-Grenze (NAPI_DISABLE_CPP_EXCEPTIONS).
#include <napi.h>

#include "mock_camera.h"
#include "pylon_lifecycle.h"

// ---------------------------------------------------------------------------
// Mock-Schicht: deterministische Werte ohne Kamera/SDK.
// Echte pylon-Calls (TlFactory/InstantCamera) ersetzen die Mock-Pfade,
// sobald das PYLON SDK verfuegbar ist (ZAU-150). Die CameraWrap-API
// (Nodemap + Grab) bleibt dabei stabil.
// ---------------------------------------------------------------------------
namespace mock {
constexpr const char* kVersion = "0.2.0-core-mock";
constexpr const char* kPylonVersion = "mock-8.x";
}  // namespace mock

namespace {

Napi::String GetVersion(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), mock::kVersion);
}

Napi::String GetPylonVersion(const Napi::CallbackInfo& info) {
#if NODE_PYLON_MOCK
  return Napi::String::New(info.Env(), mock::kPylonVersion);
#else
  // Echter Build: Version direkt aus dem SDK.
  try {
    Pylon::String_t v = Pylon::GetPylonVersionString();
    return Napi::String::New(info.Env(), std::string(v.c_str()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
    return Napi::String::New(info.Env(), "");
  } catch (const std::exception& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
    return Napi::String::New(info.Env(), "");
  }
#endif
}

// PylonInitialize mit Refcount (ZAU-150). Balance: jedes initialize()
// braucht ein terminate(); jede InstantCamera haelt zusaetzlich eine Ref
// ueber ihre Lebensdauer (siehe CameraWrap-Konstruktor/Destruktor).
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

// Listet erkannte Geraete. Mock: genau ein synthetisches Geraet, damit
// DeviceInfo/Samples ohne Kamera funktionieren (analog pylon-Emulator).
Napi::Array EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Array arr = Napi::Array::New(env, 1);
  Napi::Object dev = Napi::Object::New(env);
  dev.Set("friendlyName", "Mock Camera (node-pylon)");
  dev.Set("modelName", "MockModel Mono");
  dev.Set("serialNumber", "MOCK0001");
  dev.Set("vendorName", "Basler (mock)");
  dev.Set("deviceClass", "MockDevice");
  arr.Set(uint32_t(0), dev);
  return arr;
}

// createFirstCamera(): JS-Kurzform fuer `new InstantCamera()`
// (TlFactory.CreateFirstDevice + Anbindung, analog pypylon).
Napi::Value CreateFirstCamera(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Object obj = node_pylon::CameraWrap::constructor.New({});
  if (env.IsExceptionPending()) return env.Null();
  return obj;
}

// Synthetischer Mono8-Frame als reiner Buffer (Legacy-Helper aus ZAU-149,
// bleibt aus Kompatibilitaet erhalten; neue Samples nutzen camera.grab()).
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
