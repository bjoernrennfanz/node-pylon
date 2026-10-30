// WaitObject/WaitObjects N-API wrappers: implementation.
#include "wait_wrap.h"

#include "camera_wrap.h"
#include "error_map.h"

namespace node_pylon {

Napi::FunctionReference WaitObjectWrap::constructor;
Napi::FunctionReference WaitObjectsWrap::constructor;

namespace {

// Reads a millisecond timeout argument (unsigned 32-bit).
bool ParseTimeout(const Napi::CallbackInfo& info, size_t idx, uint32_t* out) {
  Napi::Env env = info.Env();
  if (idx >= info.Length() || !info[idx].IsNumber()) {
    Napi::TypeError::New(env, "timeout in milliseconds (number) expected")
        .ThrowAsJavaScriptException();
    return false;
  }
  const double v = info[idx].As<Napi::Number>().DoubleValue();
  if (!(v >= 0) || v > 4294967295.0) {
    Napi::TypeError::New(env, "timeout must be a number in [0, 4294967295]")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = static_cast<uint32_t>(v);
  return true;
}

// Reads an alertable flag (optional boolean, defaults to false).
bool ParseAlertable(const Napi::CallbackInfo& info, size_t idx, bool* out) {
  Napi::Env env = info.Env();
  if (idx >= info.Length() || info[idx].IsUndefined()) {
    *out = false;
    return true;
  }
  if (!info[idx].IsBoolean()) {
    Napi::TypeError::New(env, "alertable flag (boolean) expected")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = info[idx].As<Napi::Boolean>().Value();
  return true;
}

// Reads the withIndex flag: omitted/undefined/null -> plain result,
// true -> [result, index] tuple, anything else -> TypeError.
bool ParseWithIndex(const Napi::CallbackInfo& info, size_t idx, bool* out) {
  Napi::Env env = info.Env();
  if (idx >= info.Length() || info[idx].IsUndefined() ||
      info[idx].IsNull()) {
    *out = false;
    return true;
  }
  if (!info[idx].IsBoolean()) {
    Napi::TypeError::New(env, "withIndex must be null/undefined or true")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = info[idx].As<Napi::Boolean>().Value();
  return true;
}

}  // namespace

void WaitObjectWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "WaitObject",
      {InstanceMethod("isValid", &WaitObjectWrap::IsValid),
       InstanceMethod("IsValid", &WaitObjectWrap::IsValid),
       InstanceMethod("wait", &WaitObjectWrap::Wait),
       InstanceMethod("Wait", &WaitObjectWrap::Wait),
       InstanceMethod("waitEx", &WaitObjectWrap::WaitEx),
       InstanceMethod("WaitEx", &WaitObjectWrap::WaitEx),
       StaticMethod("sleep", &WaitObjectWrap::Sleep),
       StaticMethod("Sleep", &WaitObjectWrap::Sleep)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("WaitObject", ctor);
}

WaitObjectWrap::WaitObjectWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<WaitObjectWrap>(info) {}

Napi::Object WaitObjectWrap::Adopt(Napi::Env env,
                                   const Pylon::WaitObject& obj) {
  Napi::Object js = constructor.New({});
  WaitObjectWrap* self = WaitObjectWrap::Unwrap(js);
  self->obj_ = obj;
  return js;
}

Napi::Value WaitObjectWrap::IsValid(const Napi::CallbackInfo& info) {
  try {
    return Napi::Boolean::New(info.Env(), obj_.IsValid());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  }
}

Napi::Value WaitObjectWrap::Wait(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  try {
    return Napi::Boolean::New(env, obj_.Wait(timeout));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectWrap::WaitEx(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  bool alertable = false;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  if (!ParseAlertable(info, 1, &alertable)) return env.Null();
  try {
    const Pylon::EWaitExResult r = obj_.WaitEx(timeout, alertable);
    return Napi::Number::New(env, static_cast<double>(static_cast<int>(r)));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectWrap::Sleep(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t ms = 0;
  if (!ParseTimeout(info, 0, &ms)) return env.Null();
  try {
    Pylon::WaitObject::Sleep(ms);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return env.Undefined();
}

void WaitObjectsWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "WaitObjects",
      {InstanceMethod("add", &WaitObjectsWrap::Add),
       InstanceMethod("Add", &WaitObjectsWrap::Add),
       InstanceMethod("removeAll", &WaitObjectsWrap::RemoveAll),
       InstanceMethod("RemoveAll", &WaitObjectsWrap::RemoveAll),
       InstanceMethod("waitForAll", &WaitObjectsWrap::WaitForAll),
       InstanceMethod("WaitForAll", &WaitObjectsWrap::WaitForAll),
       InstanceMethod("waitForAny", &WaitObjectsWrap::WaitForAny),
       InstanceMethod("WaitForAny", &WaitObjectsWrap::WaitForAny),
       InstanceMethod("waitForAllEx", &WaitObjectsWrap::WaitForAllEx),
       InstanceMethod("WaitForAllEx", &WaitObjectsWrap::WaitForAllEx),
       InstanceMethod("waitForAnyEx", &WaitObjectsWrap::WaitForAnyEx),
       InstanceMethod("WaitForAnyEx", &WaitObjectsWrap::WaitForAnyEx)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("WaitObjects", ctor);
}

WaitObjectsWrap::WaitObjectsWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<WaitObjectsWrap>(info) {}

Napi::Value WaitObjectsWrap::Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject() ||
      !info[0].As<Napi::Object>().InstanceOf(
          WaitObjectWrap::constructor.Value())) {
    Napi::TypeError::New(env, "add(waitObject: WaitObject) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    Napi::Object js = info[0].As<Napi::Object>();
    WaitObjectWrap* w = WaitObjectWrap::Unwrap(js);
    container_.Add(w->native());
    refs_.push_back(Napi::Persistent(js));
    Napi::Number idx =
        Napi::Number::New(env, static_cast<double>(refs_.size() - 1));
    return idx;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectsWrap::RemoveAll(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    container_.RemoveAll();
    refs_.clear();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value WaitObjectsWrap::WaitForAll(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  try {
    return Napi::Boolean::New(env, container_.WaitForAll(timeout));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectsWrap::WaitForAny(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  bool withIndex = false;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  if (!ParseWithIndex(info, 1, &withIndex)) return env.Null();
  try {
    if (!withIndex) {
      return Napi::Boolean::New(env, container_.WaitForAny(timeout));
    }
    unsigned index = 0;
    const bool ok = container_.WaitForAny(timeout, &index);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0), Napi::Boolean::New(env, ok));
    out.Set(uint32_t(1), Napi::Number::New(env, static_cast<double>(index)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectsWrap::WaitForAllEx(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  bool alertable = false;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  if (!ParseAlertable(info, 1, &alertable)) return env.Null();
  try {
    const Pylon::EWaitExResult r = container_.WaitForAllEx(timeout, alertable);
    return Napi::Number::New(env, static_cast<double>(static_cast<int>(r)));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value WaitObjectsWrap::WaitForAnyEx(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeout = 0;
  bool alertable = false;
  bool withIndex = false;
  if (!ParseTimeout(info, 0, &timeout)) return env.Null();
  if (!ParseAlertable(info, 1, &alertable)) return env.Null();
  if (!ParseWithIndex(info, 2, &withIndex)) return env.Null();
  try {
    if (!withIndex) {
      const Pylon::EWaitExResult r =
          container_.WaitForAnyEx(timeout, alertable);
      return Napi::Number::New(env, static_cast<double>(static_cast<int>(r)));
    }
    unsigned index = 0;
    const Pylon::EWaitExResult r =
        container_.WaitForAnyEx(timeout, alertable, &index);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0),
            Napi::Number::New(env, static_cast<double>(static_cast<int>(r))));
    out.Set(uint32_t(1), Napi::Number::New(env, static_cast<double>(index)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

// --- Camera wait-object accessors (free functions) -------------------------

namespace {

CameraWrap* RequireCamera(Napi::Env env, const Napi::Value& camera) {
  if (!camera.IsObject() ||
      !camera.As<Napi::Object>().InstanceOf(CameraWrap::constructor.Value())) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  CameraWrap* wrap = CameraWrap::Unwrap(camera.As<Napi::Object>());
  if (wrap->PylonCamera() == nullptr) {
    Napi::Error::New(env, "InstantCamera not initialized")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  return wrap;
}

Napi::Value GetGrabResultWaitObject(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  CameraWrap* cam = RequireCamera(env, info[0]);
  if (cam == nullptr) return env.Null();
  try {
    return WaitObjectWrap::Adopt(
        env, cam->PylonCamera()->GetGrabResultWaitObject());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value GetGrabStopWaitObject(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  CameraWrap* cam = RequireCamera(env, info[0]);
  if (cam == nullptr) return env.Null();
  try {
    return WaitObjectWrap::Adopt(
        env, cam->PylonCamera()->GetGrabStopWaitObject());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

}  // namespace

void InitWaitApi(Napi::Env env, Napi::Object exports) {
  WaitObjectWrap::Init(env, exports);
  WaitObjectsWrap::Init(env, exports);
  exports.Set("waitex_timeout", Napi::Number::New(
                                    env, static_cast<double>(
                                             Pylon::waitex_timeout)));
  exports.Set("waitex_signaled", Napi::Number::New(
                                     env, static_cast<double>(
                                              Pylon::waitex_signaled)));
  exports.Set("waitex_abandoned", Napi::Number::New(
                                      env, static_cast<double>(
                                               Pylon::waitex_abandoned)));
  exports.Set("waitex_alerted", Napi::Number::New(
                                    env, static_cast<double>(
                                             Pylon::waitex_alerted)));
  exports.Set("waitForever",
              Napi::Number::New(env, static_cast<double>(Pylon::waitForever)));
  Napi::Object ops = Napi::Object::New(env);
  ops.Set("getGrabResultWaitObject",
          Napi::Function::New(env, GetGrabResultWaitObject));
  ops.Set("getGrabStopWaitObject",
          Napi::Function::New(env, GetGrabStopWaitObject));
  exports.Set("__waitOps", ops);
}

}  // namespace node_pylon
