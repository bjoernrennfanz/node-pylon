// StaticDefectPixelCorrection N-API wrapper: implementation.
#include "defect_pixel.h"

#include <cstdint>
#include <string>
#include <vector>

#include "camera_wrap.h"
#include "error_map.h"

namespace node_pylon {
namespace defect {

Napi::FunctionReference StaticDefectPixelWrap::constructor;
Napi::FunctionReference StaticDefectPixelVectorWrap::constructor;

namespace {

constexpr uint32_t kMaxUint16 = 65535;

GenApi::INodeMap* RequireNodeMap(Napi::Env env, const Napi::Value& camera) {
  if (!camera.IsObject() ||
      !camera.As<Napi::Object>().InstanceOf(CameraWrap::constructor.Value())) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  GenApi::INodeMap* map =
      CameraWrap::Unwrap(camera.As<Napi::Object>())->InfraNodeMap();
  if (map == nullptr) {
    Napi::Error::New(env, "Camera is not open. Call open() first.")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  return map;
}

bool ParseUint16(Napi::Env env, const Napi::Value& value, const char* field,
                 uint16_t* out) {
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, std::string("defect pixel ") + field +
                                  " must be an integer")
        .ThrowAsJavaScriptException();
    return false;
  }
  const double v = value.As<Napi::Number>().DoubleValue();
  if (v != static_cast<double>(static_cast<int64_t>(v))) {
    Napi::TypeError::New(env, std::string("defect pixel ") + field +
                                  " must be an integer")
        .ThrowAsJavaScriptException();
    return false;
  }
  if (v < 0 || v > kMaxUint16) {
    Napi::RangeError::New(env, std::string("defect pixel ") + field +
                                   " out of range [0, 65535]")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = static_cast<uint16_t>(v);
  return true;
}

bool ParseListType(Napi::Env env, const Napi::Value& value,
                   Pylon::CStaticDefectPixelCorrection::EListType* out) {
  if (value.IsUndefined()) {
    *out = Pylon::CStaticDefectPixelCorrection::ListType_User;
    return true;
  }
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "listType must be 0 (Factory) or 1 (User)")
        .ThrowAsJavaScriptException();
    return false;
  }
  const double v = value.As<Napi::Number>().DoubleValue();
  if (v == 0) {
    *out = Pylon::CStaticDefectPixelCorrection::ListType_Factory;
    return true;
  }
  if (v == 1) {
    *out = Pylon::CStaticDefectPixelCorrection::ListType_User;
    return true;
  }
  Napi::RangeError::New(env, "listType must be 0 (Factory) or 1 (User)")
      .ThrowAsJavaScriptException();
  return false;
}

// Reads one [x, y] / [x, y, type] entry from an array or {x,y,type} object.
bool ParsePixelFromArray(Napi::Env env, const Napi::Array& arr,
                         Pylon::StaticDefectPixel* out) {
  const uint32_t len = arr.Length();
  if (len != 2 && len != 3) {
    Napi::RangeError::New(
        env, "defect pixel entry must have 2 or 3 values [x, y, type?]")
        .ThrowAsJavaScriptException();
    return false;
  }
  if (!ParseUint16(env, arr.Get(uint32_t(0)), "x", &out->X)) return false;
  if (!ParseUint16(env, arr.Get(uint32_t(1)), "y", &out->Y)) return false;
  out->Type = Pylon::StaticDefectPixelType_Reserved;
  if (len == 3) {
    const Napi::Value t = arr.Get(uint32_t(2));
    if (!t.IsNumber()) {
      Napi::TypeError::New(env, "defect pixel type must be an integer")
          .ThrowAsJavaScriptException();
      return false;
    }
    const double v = t.As<Napi::Number>().DoubleValue();
    if (v != static_cast<double>(static_cast<int64_t>(v)) || v < 0) {
      Napi::RangeError::New(env, "defect pixel type out of range")
          .ThrowAsJavaScriptException();
      return false;
    }
    out->Type = static_cast<Pylon::EStaticDefectPixelType>(
        static_cast<int>(v));
  }
  return true;
}

bool ParsePixelList(Napi::Env env, const Napi::Value& value,
                    Pylon::StaticDefectPixelList_t* out) {
  out->clear();
  if (value.IsObject() &&
      value.As<Napi::Object>().InstanceOf(
          StaticDefectPixelVectorWrap::constructor.Value())) {
    out->operator=(
        StaticDefectPixelVectorWrap::Unwrap(value.As<Napi::Object>())->list_);
    return true;
  }
  if (!value.IsArray()) {
    Napi::TypeError::New(env, "pixel list must be an array")
        .ThrowAsJavaScriptException();
    return false;
  }
  Napi::Array arr = value.As<Napi::Array>();
  for (uint32_t i = 0; i < arr.Length(); ++i) {
    Pylon::StaticDefectPixel pixel = {
        0, 0, Pylon::StaticDefectPixelType_Reserved};
    if (!ParsePixel(env, arr.Get(i), &pixel)) return false;
    out->push_back(pixel);
  }
  return true;
}

// Wraps a native list into a fresh StaticDefectPixelVector JS object.
Napi::Object VectorToJs(Napi::Env env,
                        const Pylon::StaticDefectPixelList_t& list) {
  Napi::Object js = StaticDefectPixelVectorWrap::constructor.New({});
  StaticDefectPixelVectorWrap::Unwrap(js)->list_ = list;
  return js;
}

// [success, vector] pair, like pypylon's [bool, list] return.
Napi::Array PairToJs(Napi::Env env, bool ok,
                     const Pylon::StaticDefectPixelList_t& list) {
  Napi::Array out = Napi::Array::New(env, 2);
  out.Set(uint32_t(0), Napi::Boolean::New(env, ok));
  out.Set(uint32_t(1), VectorToJs(env, list));
  return out;
}

Napi::Value GetDefectPixelList(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "getDefectPixelList(camera, listType?) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  GenApi::INodeMap* map = RequireNodeMap(env, info[0]);
  if (map == nullptr) return env.Null();
  Pylon::CStaticDefectPixelCorrection::EListType type;
  if (!ParseListType(env, info.Length() > 1 ? info[1] : env.Undefined(),
                     &type)) {
    return env.Null();
  }
  try {
    Pylon::StaticDefectPixelList_t list;
    const bool ok =
        Pylon::CStaticDefectPixelCorrection::GetDefectPixelList(map, list, type);
    return PairToJs(env, ok, list);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value SetDefectPixelList(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2) {
    Napi::TypeError::New(
        env, "setDefectPixelList(camera, pixels, listType?) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  GenApi::INodeMap* map = RequireNodeMap(env, info[0]);
  if (map == nullptr) return env.Null();
  Pylon::StaticDefectPixelList_t list;
  if (!ParsePixelList(env, info[1], &list)) return env.Null();
  Pylon::CStaticDefectPixelCorrection::EListType type;
  if (!ParseListType(env, info.Length() > 2 ? info[2] : env.Undefined(),
                     &type)) {
    return env.Null();
  }
  try {
    const bool ok =
        Pylon::CStaticDefectPixelCorrection::SetDefectPixelList(map, list, type);
    return PairToJs(env, ok, list);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value NormalizePixelList(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2) {
    Napi::TypeError::New(env, "normalizePixelList(camera, pixels) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  GenApi::INodeMap* map = RequireNodeMap(env, info[0]);
  if (map == nullptr) return env.Null();
  Pylon::StaticDefectPixelList_t list;
  if (!ParsePixelList(env, info[1], &list)) return env.Null();
  try {
    const bool ok =
        Pylon::CStaticDefectPixelCorrection::NormalizePixelList(map, list);
    return PairToJs(env, ok, list);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

}  // namespace

// --- StaticDefectPixel struct -------------------------------------------
// ObjectWrap accessors/methods (defined at node_pylon::defect scope).

Napi::Value StaticDefectPixelWrap::GetX(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), static_cast<double>(pixel_.X));
}
void StaticDefectPixelWrap::SetX(const Napi::CallbackInfo& info,
                                 const Napi::Value& value) {
  uint16_t v = 0;
  if (ParseUint16(info.Env(), value, "X", &v)) pixel_.X = v;
}
Napi::Value StaticDefectPixelWrap::GetY(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), static_cast<double>(pixel_.Y));
}
void StaticDefectPixelWrap::SetY(const Napi::CallbackInfo& info,
                                 const Napi::Value& value) {
  uint16_t v = 0;
  if (ParseUint16(info.Env(), value, "Y", &v)) pixel_.Y = v;
}
Napi::Value StaticDefectPixelWrap::GetPixelType(
    const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(),
                           static_cast<double>(static_cast<int>(pixel_.Type)));
}
void StaticDefectPixelWrap::SetPixelType(const Napi::CallbackInfo& info,
                                         const Napi::Value& value) {
  Napi::Env env = info.Env();
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "defect pixel Type must be an integer")
        .ThrowAsJavaScriptException();
    return;
  }
  pixel_.Type = static_cast<Pylon::EStaticDefectPixelType>(
      value.As<Napi::Number>().Int32Value());
}

// --- StaticDefectPixelVector container -----------------------------------

bool ParseSizeArg(Napi::Env env, const Napi::Value& value, size_t* out,
                  const char* what) {
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, std::string(what) + " (non-negative integer) expected")
        .ThrowAsJavaScriptException();
    return false;
  }
  const double v = value.As<Napi::Number>().DoubleValue();
  if (v != static_cast<double>(static_cast<int64_t>(v)) || v < 0) {
    Napi::RangeError::New(env, std::string(what) + " out of range")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = static_cast<size_t>(v);
  return true;
}

Napi::Value StaticDefectPixelVectorWrap::GetLength(
    const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), static_cast<double>(list_.size()));
}
void StaticDefectPixelVectorWrap::SetLength(const Napi::CallbackInfo& info,
                                            const Napi::Value&) {
  Napi::TypeError::New(info.Env(), "length is read-only")
      .ThrowAsJavaScriptException();
}
Napi::Value StaticDefectPixelVectorWrap::Size(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), static_cast<double>(list_.size()));
}
Napi::Value StaticDefectPixelVectorWrap::Capacity(
    const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), static_cast<double>(list_.capacity()));
}
Napi::Value StaticDefectPixelVectorWrap::Empty(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), list_.empty());
}
Napi::Value StaticDefectPixelVectorWrap::Reserve(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t n = 0;
  if (!ParseSizeArg(env, info.Length() > 0 ? info[0] : env.Undefined(), &n,
                    "reserve(n)")) {
    return env.Undefined();
  }
  try {
    list_.reserve(n);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}
Napi::Value StaticDefectPixelVectorWrap::Resize(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t n = 0;
  if (!ParseSizeArg(env, info.Length() > 0 ? info[0] : env.Undefined(), &n,
                    "resize(n)")) {
    return env.Undefined();
  }
  try {
    list_.resize(n);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}
Napi::Value StaticDefectPixelVectorWrap::Clear(const Napi::CallbackInfo& info) {
  list_.clear();
  return info.Env().Undefined();
}
Napi::Value StaticDefectPixelVectorWrap::Swap(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject() ||
      !info[0].As<Napi::Object>().InstanceOf(
          StaticDefectPixelVectorWrap::constructor.Value())) {
    Napi::TypeError::New(env, "swap(other: StaticDefectPixelVector) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  auto* other =
      StaticDefectPixelVectorWrap::Unwrap(info[0].As<Napi::Object>());
  if (other == this) return env.Undefined();
  Pylon::StaticDefectPixelList_t tmp = list_;
  list_ = other->list_;
  other->list_ = tmp;
  return env.Undefined();
}
Napi::Value StaticDefectPixelVectorWrap::At(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t i = 0;
  if (!ParseSizeArg(env, info.Length() > 0 ? info[0] : env.Undefined(), &i,
                    "at(index)")) {
    return env.Null();
  }
  if (i >= list_.size()) {
    Napi::RangeError::New(env, "index out of range")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Object js = StaticDefectPixelWrap::constructor.New({});
  StaticDefectPixelWrap::Unwrap(js)->pixel_ = list_.at(i);
  return js;
}
Napi::Value StaticDefectPixelVectorWrap::Push(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "push(pixel) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Pylon::StaticDefectPixel pixel = {0, 0,
                                    Pylon::StaticDefectPixelType_Reserved};
  if (!ParsePixel(env, info[0], &pixel)) return env.Undefined();
  list_.push_back(pixel);
  return env.Undefined();
}
Napi::Value StaticDefectPixelVectorWrap::ToArray(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Array out = Napi::Array::New(env, list_.size());
  for (size_t i = 0; i < list_.size(); ++i) {
    out.Set(static_cast<uint32_t>(i), PixelToJs(env, list_.at(i)));
  }
  return out;
}
// Symbol.iterator -> yields {x, y, type} objects in order.
Napi::Value StaticDefectPixelVectorWrap::Iterator(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Array snapshot = Napi::Array::New(env, list_.size());
  for (size_t i = 0; i < list_.size(); ++i) {
    snapshot.Set(static_cast<uint32_t>(i), PixelToJs(env, list_.at(i)));
  }
  Napi::Object it = Napi::Object::New(env);
  it.Set("snapshot", snapshot);
  it.Set("index", Napi::Number::New(env, 0));
  it.Set("next", Napi::Function::New(env, [](const Napi::CallbackInfo& cb) -> Napi::Value {
             Napi::Env env = cb.Env();
             Napi::Object self = cb.This().As<Napi::Object>();
             Napi::Array snap = self.Get("snapshot").As<Napi::Array>();
             const uint32_t i = self.Get("index").As<Napi::Number>().Uint32Value();
             Napi::Object out = Napi::Object::New(env);
             if (i < snap.Length()) {
               out.Set("value", snap.Get(i));
               out.Set("done", false);
               self.Set("index", Napi::Number::New(env, i + 1));
             } else {
               out.Set("value", env.Undefined());
               out.Set("done", true);
             }
             return out;
           }));
  return it;
}

bool ParsePixel(Napi::Env env, const Napi::Value& value,
                Pylon::StaticDefectPixel* out) {
  if (value.IsArray()) {
    return ParsePixelFromArray(env, value.As<Napi::Array>(), out);
  }
  if (value.IsObject() &&
      value.As<Napi::Object>().InstanceOf(
          StaticDefectPixelWrap::constructor.Value())) {
    *out = StaticDefectPixelWrap::Unwrap(value.As<Napi::Object>())->pixel_;
    return true;
  }
  if (value.IsObject()) {
    Napi::Object o = value.As<Napi::Object>();
    if (!o.Has("x") || !o.Has("y")) {
      Napi::TypeError::New(env, "defect pixel object needs x and y")
          .ThrowAsJavaScriptException();
      return false;
    }
    if (!ParseUint16(env, o.Get("x"), "x", &out->X)) return false;
    if (!ParseUint16(env, o.Get("y"), "y", &out->Y)) return false;
    out->Type = Pylon::StaticDefectPixelType_Reserved;
    if (o.Has("type")) {
      const Napi::Value t = o.Get("type");
      if (!t.IsNumber()) {
        Napi::TypeError::New(env, "defect pixel type must be an integer")
            .ThrowAsJavaScriptException();
        return false;
      }
      out->Type = static_cast<Pylon::EStaticDefectPixelType>(
          t.As<Napi::Number>().Int32Value());
    }
    return true;
  }
  Napi::TypeError::New(
      env, "defect pixel must be an [x, y, type?] array or {x, y, type?}")
      .ThrowAsJavaScriptException();
  return false;
}

Napi::Object PixelToJs(Napi::Env env, const Pylon::StaticDefectPixel& pixel) {
  Napi::Object o = Napi::Object::New(env);
  o.Set("x", Napi::Number::New(env, static_cast<double>(pixel.X)));
  o.Set("y", Napi::Number::New(env, static_cast<double>(pixel.Y)));
  o.Set("type",
        Napi::Number::New(env, static_cast<double>(static_cast<int>(pixel.Type))));
  return o;
}

StaticDefectPixelWrap::StaticDefectPixelWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<StaticDefectPixelWrap>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) return;
  if (info.Length() >= 2) {
    if (!ParseUint16(env, info[0], "X", &pixel_.X)) return;
    if (!ParseUint16(env, info[1], "Y", &pixel_.Y)) return;
    if (info.Length() >= 3) {
      if (!info[2].IsNumber()) {
        Napi::TypeError::New(env, "defect pixel Type must be an integer")
            .ThrowAsJavaScriptException();
        return;
      }
      pixel_.Type = static_cast<Pylon::EStaticDefectPixelType>(
          info[2].As<Napi::Number>().Int32Value());
    }
    return;
  }
  Napi::TypeError::New(env, "StaticDefectPixel() or StaticDefectPixel(x, y, type?)")
      .ThrowAsJavaScriptException();
}

StaticDefectPixelVectorWrap::StaticDefectPixelVectorWrap(
    const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<StaticDefectPixelVectorWrap>(info) {}

void Init(Napi::Env env, Napi::Object exports) {
  Napi::Function pixelCtor = StaticDefectPixelWrap::DefineClass(
      env, "StaticDefectPixel",
      {StaticDefectPixelWrap::InstanceAccessor("X", &StaticDefectPixelWrap::GetX,
                                               &StaticDefectPixelWrap::SetX),
       StaticDefectPixelWrap::InstanceAccessor("x", &StaticDefectPixelWrap::GetX,
                                               &StaticDefectPixelWrap::SetX),
       StaticDefectPixelWrap::InstanceAccessor("Y", &StaticDefectPixelWrap::GetY,
                                               &StaticDefectPixelWrap::SetY),
       StaticDefectPixelWrap::InstanceAccessor("y", &StaticDefectPixelWrap::GetY,
                                               &StaticDefectPixelWrap::SetY),
       StaticDefectPixelWrap::InstanceAccessor(
           "Type", &StaticDefectPixelWrap::GetPixelType,
           &StaticDefectPixelWrap::SetPixelType),
       StaticDefectPixelWrap::InstanceAccessor(
           "type", &StaticDefectPixelWrap::GetPixelType,
           &StaticDefectPixelWrap::SetPixelType)});
  StaticDefectPixelWrap::constructor = Napi::Persistent(pixelCtor);
  StaticDefectPixelWrap::constructor.SuppressDestruct();
  exports.Set("StaticDefectPixel",
              StaticDefectPixelWrap::constructor.Value());

  Napi::Function vecCtor = StaticDefectPixelVectorWrap::DefineClass(
      env, "StaticDefectPixelVector",
      {StaticDefectPixelVectorWrap::InstanceAccessor(
           "length", &StaticDefectPixelVectorWrap::GetLength,
           &StaticDefectPixelVectorWrap::SetLength),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "size", &StaticDefectPixelVectorWrap::Size),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "capacity", &StaticDefectPixelVectorWrap::Capacity),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "empty", &StaticDefectPixelVectorWrap::Empty),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "reserve", &StaticDefectPixelVectorWrap::Reserve),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "resize", &StaticDefectPixelVectorWrap::Resize),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "clear", &StaticDefectPixelVectorWrap::Clear),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "swap", &StaticDefectPixelVectorWrap::Swap),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "at", &StaticDefectPixelVectorWrap::At),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "push", &StaticDefectPixelVectorWrap::Push),
       StaticDefectPixelVectorWrap::InstanceMethod(
           "toArray", &StaticDefectPixelVectorWrap::ToArray)});
  // Symbol.iterator for for..of / spread / Array.from (plain property
  // assignment with the well-known symbol key).
  Napi::Symbol iteratorSym = Napi::Symbol::WellKnown(env, "iterator");
  vecCtor.Get("prototype")
      .As<Napi::Object>()
      .Set(iteratorSym,
           Napi::Function::New(
               env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                 auto* self = StaticDefectPixelVectorWrap::Unwrap(
                     info.This().As<Napi::Object>());
                 return self->Iterator(info);
               }));
  StaticDefectPixelVectorWrap::constructor =
      Napi::Persistent(vecCtor);
  StaticDefectPixelVectorWrap::constructor.SuppressDestruct();
  exports.Set("StaticDefectPixelVector",
              StaticDefectPixelVectorWrap::constructor.Value());

  Napi::Object api = Napi::Object::New(env);
  api.Set("ListType_Factory",
          Napi::Number::New(
              env, static_cast<double>(Pylon::CStaticDefectPixelCorrection::
                                           ListType_Factory)));
  api.Set("ListType_User",
          Napi::Number::New(env, static_cast<double>(
                                       Pylon::CStaticDefectPixelCorrection::
                                           ListType_User)));
  api.Set("getDefectPixelList", Napi::Function::New(env, GetDefectPixelList));
  api.Set("GetDefectPixelList", Napi::Function::New(env, GetDefectPixelList));
  api.Set("setDefectPixelList", Napi::Function::New(env, SetDefectPixelList));
  api.Set("SetDefectPixelList", Napi::Function::New(env, SetDefectPixelList));
  api.Set("normalizePixelList", Napi::Function::New(env, NormalizePixelList));
  api.Set("NormalizePixelList", Napi::Function::New(env, NormalizePixelList));
  exports.Set("StaticDefectPixelCorrection", api);
  exports.Set("StaticDefectPixelType_Reserved",
              Napi::Number::New(
                  env, static_cast<double>(
                           Pylon::StaticDefectPixelType_Reserved)));
}

}  // namespace defect
}  // namespace node_pylon
