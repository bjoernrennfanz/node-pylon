// FeaturePersistence N-API wrapper: implementation.
#include "feature_persistence.h"

#include <string>

#include <pylon/PylonIncludes.h>

#include "camera_wrap.h"
#include "error_map.h"

namespace node_pylon {
namespace feature {

namespace {

// Unwraps the native camera and returns its open nodemap, or nullptr (with
// a pending JS exception) when the argument is not an open camera.
GenApi::INodeMap* RequireNodeMap(Napi::Env env, const Napi::Value& camera) {
  if (!camera.IsObject() ||
      !camera.As<Napi::Object>().InstanceOf(CameraWrap::constructor.Value())) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  CameraWrap* wrap =
      CameraWrap::Unwrap(camera.As<Napi::Object>());
  GenApi::INodeMap* map = wrap->InfraNodeMap();
  if (map == nullptr) {
    Napi::Error::New(env, "Camera is not open. Call open() first.")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  return map;
}

bool ParseValidate(const Napi::CallbackInfo& info, size_t idx, bool* out) {
  Napi::Env env = info.Env();
  if (idx >= info.Length() || info[idx].IsUndefined()) {
    *out = true;
    return true;
  }
  if (!info[idx].IsBoolean()) {
    Napi::TypeError::New(env, "validate flag (boolean) expected")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = info[idx].As<Napi::Boolean>().Value();
  return true;
}

Napi::Value Save(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString()) {
    Napi::TypeError::New(env, "save(file: string, camera) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  GenApi::INodeMap* map = RequireNodeMap(env, info[1]);
  if (map == nullptr) return env.Undefined();
  const std::string file = info[0].As<Napi::String>().Utf8Value();
  try {
    Pylon::CFeaturePersistence::Save(file.c_str(), map);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value Load(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString()) {
    Napi::TypeError::New(env, "load(file: string, camera, validate?) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  bool validate = true;
  if (!ParseValidate(info, 2, &validate)) return env.Undefined();
  GenApi::INodeMap* map = RequireNodeMap(env, info[1]);
  if (map == nullptr) return env.Undefined();
  const std::string file = info[0].As<Napi::String>().Utf8Value();
  try {
    Pylon::CFeaturePersistence::Load(file.c_str(), map, validate);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value SaveToString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "saveToString(camera) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  GenApi::INodeMap* map = RequireNodeMap(env, info[0]);
  if (map == nullptr) return env.Null();
  try {
    Pylon::String_t features;
    Pylon::CFeaturePersistence::SaveToString(features, map);
    return Napi::String::New(env, std::string(features.c_str()));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value LoadFromString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString()) {
    Napi::TypeError::New(env,
                          "loadFromString(features: string, camera, validate?) "
                          "expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  bool validate = true;
  if (!ParseValidate(info, 2, &validate)) return env.Undefined();
  GenApi::INodeMap* map = RequireNodeMap(env, info[1]);
  if (map == nullptr) return env.Undefined();
  const std::string features = info[0].As<Napi::String>().Utf8Value();
  try {
    Pylon::CFeaturePersistence::LoadFromString(features.c_str(), map, validate);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

}  // namespace

void Init(Napi::Env env, Napi::Object exports) {
  Napi::Object api = Napi::Object::New(env);
  api.Set("save", Napi::Function::New(env, Save));
  api.Set("Save", Napi::Function::New(env, Save));
  api.Set("load", Napi::Function::New(env, Load));
  api.Set("Load", Napi::Function::New(env, Load));
  api.Set("saveToString", Napi::Function::New(env, SaveToString));
  api.Set("SaveToString", Napi::Function::New(env, SaveToString));
  api.Set("loadFromString", Napi::Function::New(env, LoadFromString));
  api.Set("LoadFromString", Napi::Function::New(env, LoadFromString));
  exports.Set("FeaturePersistence", api);
}

}  // namespace feature
}  // namespace node_pylon
