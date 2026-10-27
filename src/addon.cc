// node-pylon N-API Scaffold (ZAU-149).
// Status: Mock-Build ohne pylon-SDK. Echte TlFactory/InstantCamera-Anbindung folgt in ZAU-150.
// Regeln: N-API only (node-addon-api), kein SWIG/NAN, keine V8-Internals,
// keine C++-Exceptions ueber die N-API-Grenze (NAPI_DISABLE_CPP_EXCEPTIONS).
#include <napi.h>

// ---------------------------------------------------------------------------
// Mock-Schicht: liefert deterministische Werte ohne Kamera/SDK.
// Wird durch echte pylon-Calls ersetzt, sobald PYLON SDK verfuegbar (ZAU-150).
// ---------------------------------------------------------------------------
namespace mock {
constexpr const char* kVersion = "0.1.0-scaffold-mock";
constexpr const char* kPylonVersion = "mock-8.x";
}  // namespace mock

Napi::String GetVersion(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), mock::kVersion);
}

Napi::String GetPylonVersion(const Napi::CallbackInfo& info) {
#if NODE_PYLON_MOCK
  return Napi::String::New(info.Env(), mock::kPylonVersion);
#else
  // ZAU-150: Pylon::GetPylonVersionString() hier einhaengen.
  return Napi::String::New(info.Env(), "pylon-linked");
#endif
}

// Listet erkannte Geraete. Mock: leere Liste (keine Kamera erforderlich).
Napi::Array EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
#if NODE_PYLON_MOCK
  return Napi::Array::New(env, 0);
#else
  // ZAU-150: CTlFactory::EnumerateDevices hier einhaengen.
  return Napi::Array::New(env, 0);
#endif
}

// Gibt einen synthetischen Graustufen-Frame als Buffer zurueck (Mock-Grab).
// Zero-copy-Prinzip wird in ZAU-151 mit echten GrabResults umgesetzt;
// hier bewusst einfacher Heap-Buffer als Platzhalter.
Napi::Value GrabMock(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t width = 64;
  size_t height = 48;
  if (info.Length() >= 2) {
    if (info[0].IsNumber()) width = info[0].As<Napi::Number>().Uint32Value();
    if (info[1].IsNumber()) height = info[1].As<Napi::Number>().Uint32Value();
  }
  if (width == 0 || height == 0 || width > 4096 || height > 4096) {
    Napi::TypeError::New(env, "width/height must be in 1..4096").ThrowAsJavaScriptException();
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
  exports.Set("enumerateDevices", Napi::Function::New(env, EnumerateDevices));
  exports.Set("grabMock", Napi::Function::New(env, GrabMock));
#if NODE_PYLON_MOCK
  exports.Set("mock", Napi::Boolean::New(env, true));
#else
  exports.Set("mock", Napi::Boolean::New(env, false));
#endif
  return exports;
}

NODE_API_MODULE(node_pylon, Init)
