// Discovery N-API layer: implementation (TlFactory / TransportLayer /
// Interface / PylonDevice / TlInfo / InterfaceInfo).
#include "discovery.h"

#include <string>
#include <vector>

#include "device_info.h"
#include "error_map.h"
#include "pylon_lifecycle.h"

namespace node_pylon {

namespace {

std::string ToStd(const Pylon::String_t& s) { return std::string(s.c_str()); }

Napi::Array PropertyNameArray(Napi::Env env,
                              const Pylon::CInfoBase& info) {
  Pylon::StringList_t names;
  info.GetPropertyNames(names);
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(names.size()));
  for (size_t i = 0; i < names.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), ToStd(names[i]));
  }
  return arr;
}

void SetProtoChain(Napi::Env env, Napi::Function derived,
                   const Napi::Function& base) {
  Napi::Object global = env.Global();
  Napi::Function setProto = global.Get("Reflect")
                                .As<Napi::Object>()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();
  Napi::Object derivedProto = derived.Get("prototype").As<Napi::Object>();
  Napi::Object baseProto = base.Get("prototype").As<Napi::Object>();
  setProto.Call({derivedProto, baseProto});
}

Napi::Object WrapDeviceInfo(Napi::Env env, const Pylon::CDeviceInfo& di) {
  Napi::Object obj = DeviceInfoWrap::constructor.New({});
  if (!env.IsExceptionPending()) {
    DeviceInfoWrap::Unwrap(obj)->SetInfo(di);
  }
  return obj;
}

Napi::Object WrapTlInfo(Napi::Env env, const Pylon::CTlInfo& ti) {
  Napi::Object obj = TlInfoWrap::constructor.New({});
  if (!env.IsExceptionPending()) {
    TlInfoWrap::Unwrap(obj)->SetInfo(ti);
  }
  return obj;
}

Napi::Object WrapInterfaceInfo(Napi::Env env, const Pylon::CInterfaceInfo& ii) {
  Napi::Object obj = InterfaceInfoWrap::constructor.New({});
  if (!env.IsExceptionPending()) {
    InterfaceInfoWrap::Unwrap(obj)->SetInfo(ii);
  }
  return obj;
}

Pylon::CDeviceInfo RequireDeviceInfo(Napi::Env env, const Napi::Value& value,
                                     bool* ok) {
  Pylon::CDeviceInfo out;
  *ok = DeviceInfoWrap::ToDeviceInfo(env, value, &out);
  return out;
}

}  // namespace

// --- Shared filter/mode helpers -------------------------------------------

namespace discovery {

bool BuildFilter(Napi::Env env, const Napi::Value& value,
                 Pylon::DeviceInfoList_t* out, bool* useNoFilter) {
  *useNoFilter = false;
  out->clear();
  if (value.IsUndefined() || value.IsNull()) {
    *useNoFilter = true;
    return true;
  }
  auto append = [&](const Napi::Value& entry) -> bool {
    if (entry.IsArray()) {
      Napi::TypeError::New(env, "filter entries must be DeviceInfo or dict")
          .ThrowAsJavaScriptException();
      return false;
    }
    Pylon::CDeviceInfo di;
    if (!DeviceInfoWrap::ToDeviceInfo(env, entry, &di)) return false;
    try {
      out->push_back(di);
    } catch (const std::exception& ex) {
      errormap::ThrowTranslated(env, ex);
      return false;
    }
    return true;
  };
  if (value.IsArray()) {
    Napi::Array arr = value.As<Napi::Array>();
    for (uint32_t i = 0; i < arr.Length(); ++i) {
      if (!append(arr.Get(i))) return false;
    }
    return true;
  }
  if (value.IsObject()) {
    return append(value);
  }
  Napi::TypeError::New(env, "filter must be a DeviceInfo, dict or array")
      .ThrowAsJavaScriptException();
  return false;
}

bool ToAccessMode(Napi::Env env, const Napi::Value& value,
                  Pylon::AccessModeSet* out) {
  if (value.IsUndefined()) {
    *out = Pylon::AccessModeSet(Pylon::Control);
    return true;
  }
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "access mode must be a number")
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = Pylon::AccessModeSet(
      static_cast<unsigned long>(value.As<Napi::Number>().Uint32Value()));
  return true;
}

}  // namespace discovery

// --- IDeviceFactoryWrap ----------------------------------------------------

Napi::FunctionReference IDeviceFactoryWrap::constructor;

void IDeviceFactoryWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(env, "IDeviceFactory", {});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("IDeviceFactory", ctor);
}

IDeviceFactoryWrap::IDeviceFactoryWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<IDeviceFactoryWrap>(info) {
  Napi::TypeError::New(info.Env(), "IDeviceFactory cannot be constructed")
      .ThrowAsJavaScriptException();
}

// --- TlInfoWrap ------------------------------------------------------------

Napi::FunctionReference TlInfoWrap::constructor;

#define NODEPYLON_DEFINE_TLINFO_RW(Name)                                        \
  Napi::Value TlInfoWrap::Get##Name(const Napi::CallbackInfo& info) {           \
    try {                                                                      \
      return Napi::String::New(info.Env(), ToStd(info_.Get##Name()));           \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }                                                                            \
  void TlInfoWrap::Set##Name(const Napi::CallbackInfo& info,                    \
                             const Napi::Value& value) {                        \
    if (!value.IsString()) {                                                   \
      Napi::TypeError::New(info.Env(), #Name " must be a string")               \
          .ThrowAsJavaScriptException();                                       \
      return;                                                                  \
    }                                                                          \
    try {                                                                      \
      info_.Set##Name(value.As<Napi::String>().Utf8Value().c_str());            \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    }                                                                          \
  }                                                                            \
  Napi::Value TlInfoWrap::Set##Name##Method(const Napi::CallbackInfo& info) {   \
    Napi::Env env = info.Env();                                                \
    if (info.Length() < 1 || !info[0].IsString()) {                            \
      Napi::TypeError::New(env, "Set" #Name "(value: string) expected")        \
          .ThrowAsJavaScriptException();                                       \
      return env.Undefined();                                                  \
    }                                                                          \
    try {                                                                      \
      info_.Set##Name(info[0].As<Napi::String>().Utf8Value().c_str());          \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(env, ex);                                      \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(env, ex);                                      \
    }                                                                          \
    return env.Undefined();                                                    \
  }                                                                            \
  Napi::Value TlInfoWrap::Is##Name##Available(                                 \
      const Napi::CallbackInfo& info) {                                        \
    try {                                                                      \
      return Napi::Boolean::New(info.Env(), info_.Is##Name##Available());      \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }

NODEPYLON_TLINFO_RW_LIST(NODEPYLON_DEFINE_TLINFO_RW)

#undef NODEPYLON_DEFINE_TLINFO_RW

void TlInfoWrap::Init(Napi::Env env, Napi::Object exports,
                      const Napi::Function& base) {
  std::vector<Napi::ClassPropertyDescriptor<TlInfoWrap>> props;
#define NODEPYLON_ADD_TLINFO_RW(Name)                                          \
  props.push_back(Napi::InstanceAccessor<TlInfoWrap>(                          \
      #Name, &TlInfoWrap::Get##Name, &TlInfoWrap::Set##Name));                 \
  props.push_back(Napi::InstanceMethod<TlInfoWrap>("Get" #Name,                 \
                                                   &TlInfoWrap::Get##Name));   \
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(                             \
      "Set" #Name, &TlInfoWrap::Set##Name##Method));                           \
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(                             \
      "Is" #Name "Available", &TlInfoWrap::Is##Name##Available));
  NODEPYLON_TLINFO_RW_LIST(NODEPYLON_ADD_TLINFO_RW)
#undef NODEPYLON_ADD_TLINFO_RW
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "GetPropertyNames", &TlInfoWrap::GetPropertyNames));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "GetPropertyAvailable", &TlInfoWrap::GetPropertyAvailable));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "GetPropertyValue", &TlInfoWrap::GetPropertyValue));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "SetPropertyValue", &TlInfoWrap::SetPropertyValue));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("IsSubset", &TlInfoWrap::IsSubset));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "IsUserProvided", &TlInfoWrap::IsUserProvided));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>(
      "GetPropertyNotAvailable", &TlInfoWrap::GetPropertyNotAvailable));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("equals", &TlInfoWrap::Equals));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("lessThan", &TlInfoWrap::LessThan));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>("keys", &TlInfoWrap::Keys));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("values", &TlInfoWrap::Values));
  props.push_back(Napi::InstanceMethod<TlInfoWrap>("items", &TlInfoWrap::Items));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("toDict", &TlInfoWrap::ToDict));
  props.push_back(
      Napi::InstanceMethod<TlInfoWrap>("update", &TlInfoWrap::Update));
  Napi::Function ctor = DefineClass(env, "TlInfo", props);
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  Napi::Function notAvail = Napi::Function::New(
      env,
      [](const Napi::CallbackInfo& info) -> Napi::Value {
        return Napi::String::New(info.Env(),
                                 Pylon::CTlInfo::GetPropertyNotAvailable());
      },
      "GetPropertyNotAvailable");
  ctor.Set("GetPropertyNotAvailable", notAvail);
  SetProtoChain(env, ctor, base);
  exports.Set("TlInfo", ctor);
}

TlInfoWrap::TlInfoWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TlInfoWrap>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) return;
  if (info.Length() == 1 && info[0].IsObject()) {
    TlInfoWrap* other =
        Napi::ObjectWrap<TlInfoWrap>::Unwrap(info[0].As<Napi::Object>());
    if (other != nullptr) {
      info_ = other->info_;
      return;
    }
  }
  Napi::TypeError::New(env, "new TlInfo() or new TlInfo(other) expected")
      .ThrowAsJavaScriptException();
}

Napi::Value TlInfoWrap::GetPropertyNames(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0),
            Napi::Number::New(env, static_cast<double>(names.Length())));
    out.Set(uint32_t(1), names);
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::GetPropertyAvailable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "GetPropertyAvailable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    std::string name = info[0].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(env, info_.GetPropertyAvailable(name.c_str()));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::GetPropertyValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "GetPropertyValue(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Pylon::String_t value;
    bool ok = info_.GetPropertyValue(name.c_str(), value);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0), Napi::Boolean::New(env, ok));
    out.Set(uint32_t(1), Napi::String::New(env, ToStd(value)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::SetPropertyValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
    Napi::TypeError::New(env, "SetPropertyValue(name: string, value: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    info_.SetPropertyValue(info[0].As<Napi::String>().Utf8Value().c_str(),
                           info[1].As<Napi::String>().Utf8Value().c_str());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value TlInfoWrap::IsSubset(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "IsSubset(other: TlInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  TlInfoWrap* other =
      Napi::ObjectWrap<TlInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "IsSubset(other: TlInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return Napi::Boolean::New(env, info_.IsSubset(other->info_));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::IsUserProvided(const Napi::CallbackInfo& info) {
  try {
    return Napi::Boolean::New(info.Env(), info_.IsUserProvided());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  }
}

Napi::Value TlInfoWrap::GetPropertyNotAvailable(
    const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(),
                           Pylon::CTlInfo::GetPropertyNotAvailable());
}

Napi::Value TlInfoWrap::Equals(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject())
    return Napi::Boolean::New(env, false);
  TlInfoWrap* other =
      Napi::ObjectWrap<TlInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) return Napi::Boolean::New(env, false);
  try {
    const Pylon::CInfoBase& lhs = info_;
    const Pylon::CInfoBase& rhs = other->info_;
    return Napi::Boolean::New(env, lhs == rhs);
  } catch (...) {
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value TlInfoWrap::LessThan(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "lessThan(other: TlInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  TlInfoWrap* other =
      Napi::ObjectWrap<TlInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "lessThan(other: TlInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    const Pylon::CInfoBase& lhs = info_;
    const Pylon::CInfoBase& rhs = other->info_;
    return Napi::Boolean::New(env, lhs < rhs);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::Keys(const Napi::CallbackInfo& info) {
  try {
    return PropertyNameArray(info.Env(), info_);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  }
}

Napi::Value TlInfoWrap::Values(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, names.Length());
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      out.Set(i, Napi::String::New(env, ToStd(value)));
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::Items(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, names.Length());
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      Napi::Array pair = Napi::Array::New(env, 2);
      pair.Set(uint32_t(0), Napi::String::New(env, name));
      pair.Set(uint32_t(1), Napi::String::New(env, ToStd(value)));
      out.Set(i, pair);
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::ToDict(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Object out = Napi::Object::New(env);
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      out.Set(name, Napi::String::New(env, ToStd(value)));
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TlInfoWrap::Update(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "update(dict: object) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::Object dict = info[0].As<Napi::Object>();
  Napi::Array keys = dict.GetPropertyNames();
  for (uint32_t i = 0; i < keys.Length(); ++i) {
    Napi::Value key = keys.Get(i);
    std::string name = key.As<Napi::String>().Utf8Value();
    Napi::Value v = dict.Get(key);
    if (!v.IsString()) {
      Napi::TypeError::New(env, "update values must be strings (" + name + ")")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    try {
      info_.SetPropertyValue(name.c_str(),
                             v.As<Napi::String>().Utf8Value().c_str());
    } catch (const Pylon::GenericException& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    } catch (const std::exception& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    }
  }
  return env.Undefined();
}

// --- InterfaceInfoWrap -----------------------------------------------------

Napi::FunctionReference InterfaceInfoWrap::constructor;

#define NODEPYLON_DEFINE_IFINFO_RW(Name)                                       \
  Napi::Value InterfaceInfoWrap::Get##Name(const Napi::CallbackInfo& info) {    \
    try {                                                                      \
      return Napi::String::New(info.Env(), ToStd(info_.Get##Name()));           \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }                                                                            \
  void InterfaceInfoWrap::Set##Name(const Napi::CallbackInfo& info,             \
                                    const Napi::Value& value) {                \
    if (!value.IsString()) {                                                   \
      Napi::TypeError::New(info.Env(), #Name " must be a string")               \
          .ThrowAsJavaScriptException();                                       \
      return;                                                                  \
    }                                                                          \
    try {                                                                      \
      info_.Set##Name(value.As<Napi::String>().Utf8Value().c_str());            \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    }                                                                          \
  }                                                                            \
  Napi::Value InterfaceInfoWrap::Set##Name##Method(                             \
      const Napi::CallbackInfo& info) {                                         \
    Napi::Env env = info.Env();                                                \
    if (info.Length() < 1 || !info[0].IsString()) {                            \
      Napi::TypeError::New(env, "Set" #Name "(value: string) expected")        \
          .ThrowAsJavaScriptException();                                       \
      return env.Undefined();                                                  \
    }                                                                          \
    try {                                                                      \
      info_.Set##Name(info[0].As<Napi::String>().Utf8Value().c_str());          \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(env, ex);                                      \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(env, ex);                                      \
    }                                                                          \
    return env.Undefined();                                                    \
  }                                                                            \
  Napi::Value InterfaceInfoWrap::Is##Name##Available(                           \
      const Napi::CallbackInfo& info) {                                        \
    try {                                                                      \
      return Napi::Boolean::New(info.Env(), info_.Is##Name##Available());      \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }

NODEPYLON_IFINFO_RW_LIST(NODEPYLON_DEFINE_IFINFO_RW)

#undef NODEPYLON_DEFINE_IFINFO_RW

void InterfaceInfoWrap::Init(Napi::Env env, Napi::Object exports,
                             const Napi::Function& base) {
  std::vector<Napi::ClassPropertyDescriptor<InterfaceInfoWrap>> props;
#define NODEPYLON_ADD_IFINFO_RW(Name)                                          \
  props.push_back(Napi::InstanceAccessor<InterfaceInfoWrap>(                    \
      #Name, &InterfaceInfoWrap::Get##Name, &InterfaceInfoWrap::Set##Name));   \
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(                      \
      "Get" #Name, &InterfaceInfoWrap::Get##Name));                            \
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(                      \
      "Set" #Name, &InterfaceInfoWrap::Set##Name##Method));                    \
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(                      \
      "Is" #Name "Available", &InterfaceInfoWrap::Is##Name##Available));
  NODEPYLON_IFINFO_RW_LIST(NODEPYLON_ADD_IFINFO_RW)
#undef NODEPYLON_ADD_IFINFO_RW
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "GetPropertyNames", &InterfaceInfoWrap::GetPropertyNames));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "GetPropertyAvailable", &InterfaceInfoWrap::GetPropertyAvailable));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "GetPropertyValue", &InterfaceInfoWrap::GetPropertyValue));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "SetPropertyValue", &InterfaceInfoWrap::SetPropertyValue));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "IsSubset", &InterfaceInfoWrap::IsSubset));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "IsUserProvided", &InterfaceInfoWrap::IsUserProvided));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "GetPropertyNotAvailable", &InterfaceInfoWrap::GetPropertyNotAvailable));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "equals", &InterfaceInfoWrap::Equals));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "lessThan", &InterfaceInfoWrap::LessThan));
  props.push_back(
      Napi::InstanceMethod<InterfaceInfoWrap>("keys", &InterfaceInfoWrap::Keys));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "values", &InterfaceInfoWrap::Values));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "items", &InterfaceInfoWrap::Items));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "toDict", &InterfaceInfoWrap::ToDict));
  props.push_back(Napi::InstanceMethod<InterfaceInfoWrap>(
      "update", &InterfaceInfoWrap::Update));
  Napi::Function ctor = DefineClass(env, "InterfaceInfo", props);
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  Napi::Function notAvail = Napi::Function::New(
      env,
      [](const Napi::CallbackInfo& info) -> Napi::Value {
        return Napi::String::New(
            info.Env(), Pylon::CInterfaceInfo::GetPropertyNotAvailable());
      },
      "GetPropertyNotAvailable");
  ctor.Set("GetPropertyNotAvailable", notAvail);
  SetProtoChain(env, ctor, base);
  exports.Set("InterfaceInfo", ctor);
}

InterfaceInfoWrap::InterfaceInfoWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<InterfaceInfoWrap>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) return;
  if (info.Length() == 1 && info[0].IsObject()) {
    InterfaceInfoWrap* other =
        Napi::ObjectWrap<InterfaceInfoWrap>::Unwrap(info[0].As<Napi::Object>());
    if (other != nullptr) {
      info_ = other->info_;
      return;
    }
  }
  Napi::TypeError::New(env, "new InterfaceInfo() or new InterfaceInfo(other)")
      .ThrowAsJavaScriptException();
}

Napi::Value InterfaceInfoWrap::GetPropertyNames(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0),
            Napi::Number::New(env, static_cast<double>(names.Length())));
    out.Set(uint32_t(1), names);
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::GetPropertyAvailable(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "GetPropertyAvailable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    std::string name = info[0].As<Napi::String>().Utf8Value();
    return Napi::Boolean::New(env, info_.GetPropertyAvailable(name.c_str()));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::GetPropertyValue(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "GetPropertyValue(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    std::string name = info[0].As<Napi::String>().Utf8Value();
    Pylon::String_t value;
    bool ok = info_.GetPropertyValue(name.c_str(), value);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0), Napi::Boolean::New(env, ok));
    out.Set(uint32_t(1), Napi::String::New(env, ToStd(value)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::SetPropertyValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
    Napi::TypeError::New(env, "SetPropertyValue(name: string, value: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    info_.SetPropertyValue(info[0].As<Napi::String>().Utf8Value().c_str(),
                           info[1].As<Napi::String>().Utf8Value().c_str());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value InterfaceInfoWrap::IsSubset(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "IsSubset(other: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  InterfaceInfoWrap* other =
      Napi::ObjectWrap<InterfaceInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "IsSubset(other: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return Napi::Boolean::New(env, info_.IsSubset(other->info_));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::IsUserProvided(const Napi::CallbackInfo& info) {
  try {
    return Napi::Boolean::New(info.Env(), info_.IsUserProvided());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  }
}

Napi::Value InterfaceInfoWrap::GetPropertyNotAvailable(
    const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(),
                           Pylon::CInterfaceInfo::GetPropertyNotAvailable());
}

Napi::Value InterfaceInfoWrap::Equals(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject())
    return Napi::Boolean::New(env, false);
  InterfaceInfoWrap* other =
      Napi::ObjectWrap<InterfaceInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) return Napi::Boolean::New(env, false);
  try {
    const Pylon::CInfoBase& lhs = info_;
    const Pylon::CInfoBase& rhs = other->info_;
    return Napi::Boolean::New(env, lhs == rhs);
  } catch (...) {
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value InterfaceInfoWrap::LessThan(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "lessThan(other: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  InterfaceInfoWrap* other =
      Napi::ObjectWrap<InterfaceInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "lessThan(other: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return Napi::Boolean::New(env, info_ < other->info_);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::Keys(const Napi::CallbackInfo& info) {
  try {
    return PropertyNameArray(info.Env(), info_);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(info.Env(), ex);
    return info.Env().Null();
  }
}

Napi::Value InterfaceInfoWrap::Values(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, names.Length());
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      out.Set(i, Napi::String::New(env, ToStd(value)));
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::Items(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Array out = Napi::Array::New(env, names.Length());
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      Napi::Array pair = Napi::Array::New(env, 2);
      pair.Set(uint32_t(0), Napi::String::New(env, name));
      pair.Set(uint32_t(1), Napi::String::New(env, ToStd(value)));
      out.Set(i, pair);
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::ToDict(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    Napi::Array names = PropertyNameArray(env, info_);
    Napi::Object out = Napi::Object::New(env);
    for (uint32_t i = 0; i < names.Length(); ++i) {
      std::string name = names.Get(i).As<Napi::String>().Utf8Value();
      Pylon::String_t value;
      info_.GetPropertyValue(name.c_str(), value);
      out.Set(name, Napi::String::New(env, ToStd(value)));
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceInfoWrap::Update(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "update(dict: object) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::Object dict = info[0].As<Napi::Object>();
  Napi::Array keys = dict.GetPropertyNames();
  for (uint32_t i = 0; i < keys.Length(); ++i) {
    Napi::Value key = keys.Get(i);
    std::string name = key.As<Napi::String>().Utf8Value();
    Napi::Value v = dict.Get(key);
    if (!v.IsString()) {
      Napi::TypeError::New(env, "update values must be strings (" + name + ")")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    try {
      info_.SetPropertyValue(name.c_str(),
                             v.As<Napi::String>().Utf8Value().c_str());
    } catch (const Pylon::GenericException& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    } catch (const std::exception& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    }
  }
  return env.Undefined();
}

// --- PylonDeviceWrap -------------------------------------------------------

Napi::FunctionReference PylonDeviceWrap::constructor;

void PylonDeviceWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "PylonDevice",
      {InstanceMethod("getDeviceInfo", &PylonDeviceWrap::GetDeviceInfo),
       InstanceMethod("open", &PylonDeviceWrap::Open),
       InstanceMethod("close", &PylonDeviceWrap::Close),
       InstanceMethod("isOpen", &PylonDeviceWrap::IsOpen),
       InstanceMethod("destroy", &PylonDeviceWrap::Destroy)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("PylonDevice", ctor);
}

PylonDeviceWrap::PylonDeviceWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PylonDeviceWrap>(info) {}

PylonDeviceWrap::~PylonDeviceWrap() { DestroyNative(); }

void PylonDeviceWrap::Attach(Pylon::IPylonDevice* device, CreatorKind kind,
                             void* creator, const Napi::Value& owner) {
  device_ = device;
  kind_ = kind;
  creator_ = creator;
  destroyed_ = false;
  if (owner.IsObject() && !owner.IsArray()) {
    owner_ = Napi::Persistent(owner.As<Napi::Object>());
  }
  try {
    lifecycle::EnsureInitialized();
    lifecycleHeld_ = true;
  } catch (...) {
    device_ = nullptr;
    destroyed_ = true;
    throw;
  }
}

void PylonDeviceWrap::DestroyNative() {
  if (destroyed_ || device_ == nullptr) {
    if (lifecycleHeld_) {
      lifecycle::ReleaseRef();
      lifecycleHeld_ = false;
    }
    destroyed_ = true;
    device_ = nullptr;
    return;
  }
  Pylon::IPylonDevice* dev = device_;
  device_ = nullptr;
  destroyed_ = true;
  // Never throws: called from the destructor as well.
  try {
    switch (kind_) {
      case CreatorKind::kFactory:
        Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
        break;
      case CreatorKind::kTransportLayer:
        static_cast<Pylon::ITransportLayer*>(creator_)->DestroyDevice(dev);
        break;
      case CreatorKind::kInterface:
        static_cast<Pylon::IInterface*>(creator_)->DestroyDevice(dev);
        break;
    }
  } catch (...) {
  }
  owner_.Reset();
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
}

Napi::Value PylonDeviceWrap::GetDeviceInfo(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || device_ == nullptr) {
    Napi::Error::New(env, "PylonDevice destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return WrapDeviceInfo(env, device_->GetDeviceInfo());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value PylonDeviceWrap::Open(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || device_ == nullptr) {
    Napi::Error::New(env, "PylonDevice destroyed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::Value modeVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Undefined();
  try {
    device_->Open(mode);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value PylonDeviceWrap::Close(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || device_ == nullptr) {
    Napi::Error::New(env, "PylonDevice destroyed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    device_->Close();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value PylonDeviceWrap::IsOpen(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || device_ == nullptr) return Napi::Boolean::New(env, false);
  try {
    return Napi::Boolean::New(env, device_->IsOpen());
  } catch (...) {
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value PylonDeviceWrap::Destroy(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || device_ == nullptr) return env.Undefined();
  Pylon::IPylonDevice* dev = device_;
  void* creator = creator_;
  CreatorKind kind = kind_;
  device_ = nullptr;
  destroyed_ = true;
  try {
    switch (kind) {
      case CreatorKind::kFactory:
        Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
        break;
      case CreatorKind::kTransportLayer:
        static_cast<Pylon::ITransportLayer*>(creator)->DestroyDevice(dev);
        break;
      case CreatorKind::kInterface:
        static_cast<Pylon::IInterface*>(creator)->DestroyDevice(dev);
        break;
    }
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  owner_.Reset();
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
  return env.Undefined();
}

// --- TransportLayerWrap ----------------------------------------------------

Napi::FunctionReference TransportLayerWrap::constructor;

void TransportLayerWrap::Init(Napi::Env env, Napi::Object exports,
                              const Napi::Function& base) {
  Napi::Function ctor = DefineClass(
      env, "TransportLayer",
      {InstanceMethod("getTlInfo", &TransportLayerWrap::GetTlInfo),
       InstanceMethod("enumerateDevices", &TransportLayerWrap::EnumerateDevices),
       InstanceMethod("enumerateInterfaces",
                      &TransportLayerWrap::EnumerateInterfaces),
       InstanceMethod("createInterface", &TransportLayerWrap::CreateInterface),
       InstanceMethod("destroyInterface", &TransportLayerWrap::DestroyInterface),
       InstanceMethod("createDevice", &TransportLayerWrap::CreateDevice),
       InstanceMethod("createFirstDevice",
                      &TransportLayerWrap::CreateFirstDevice),
       InstanceMethod("destroyDevice", &TransportLayerWrap::DestroyDevice),
       InstanceMethod("isDeviceAccessible",
                      &TransportLayerWrap::IsDeviceAccessible),
       InstanceMethod("isDeviceAccessibleInfo",
                      &TransportLayerWrap::IsDeviceAccessibleInfo),
       InstanceMethod("getNodeMapNumNodes",
                      &TransportLayerWrap::GetNodeMapNumNodes),
       InstanceMethod("release", &TransportLayerWrap::Release)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  SetProtoChain(env, ctor, base);
  exports.Set("TransportLayer", ctor);
}

TransportLayerWrap::TransportLayerWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TransportLayerWrap>(info) {}

TransportLayerWrap::~TransportLayerWrap() { ReleaseNative(); }

Napi::Object TransportLayerWrap::Create(Napi::Env env,
                                        Pylon::ITransportLayer* tl) {
  Napi::Object obj = constructor.New({});
  TransportLayerWrap* self = Unwrap(obj);
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return obj;
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return obj;
  }
  self->lifecycleHeld_ = true;
  self->tl_ = tl;
  self->released_ = false;
  return obj;
}

void TransportLayerWrap::ReleaseNative() {
  if (released_ || tl_ == nullptr) {
    if (lifecycleHeld_) {
      lifecycle::ReleaseRef();
      lifecycleHeld_ = false;
    }
    released_ = true;
    tl_ = nullptr;
    return;
  }
  Pylon::ITransportLayer* tl = tl_;
  tl_ = nullptr;
  released_ = true;
  try {
    Pylon::CTlFactory::GetInstance().ReleaseTl(tl);
  } catch (...) {
  }
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
}

Napi::Value TransportLayerWrap::GetTlInfo(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return WrapTlInfo(env, tl_->GetTlInfo());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TransportLayerWrap::EnumerateDevices(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  Pylon::DeviceInfoList_t filter;
  bool useNoFilter = false;
  if (!discovery::BuildFilter(env, filterVal, &filter, &useNoFilter)) {
    return env.Null();
  }
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::DeviceInfoList_t list;
  int count = 0;
  try {
    if (useNoFilter) {
      count = tl_->EnumerateDevices(list);
    } else {
      count = tl_->EnumerateDevices(list, filter);
    }
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    arr.Set(static_cast<uint32_t>(i), WrapDeviceInfo(env, list[i]));
    if (env.IsExceptionPending()) return env.Null();
  }
  return arr;
}

Napi::Value TransportLayerWrap::EnumerateInterfaces(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::InterfaceInfoList_t list;
  int count = 0;
  try {
    count = tl_->EnumerateInterfaces(list);
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    arr.Set(static_cast<uint32_t>(i), WrapInterfaceInfo(env, list[i]));
    if (env.IsExceptionPending()) return env.Null();
  }
  return arr;
}

Napi::Value TransportLayerWrap::CreateInterface(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "createInterface(info: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  InterfaceInfoWrap* iiWrap =
      Napi::ObjectWrap<InterfaceInfoWrap>::Unwrap(info[0].As<Napi::Object>());
  if (iiWrap == nullptr) {
    Napi::TypeError::New(env, "createInterface(info: InterfaceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Pylon::IInterface* iface = nullptr;
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  try {
    iface = tl_->CreateInterface(iiWrap->Info());
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  return InterfaceWrap::Create(env, iface, tl_, info.This());
}

Napi::Value TransportLayerWrap::DestroyInterface(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "destroyInterface(iface: Interface) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  InterfaceWrap* ifaceWrap =
      Napi::ObjectWrap<InterfaceWrap>::Unwrap(info[0].As<Napi::Object>());
  if (ifaceWrap == nullptr) {
    Napi::TypeError::New(env, "destroyInterface(iface: Interface) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    ifaceWrap->DestroyNative();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value TransportLayerWrap::CreateDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "createDevice(info: DeviceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Pylon::IPylonDevice* dev = nullptr;
  try {
    dev = tl_->CreateDevice(di);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      tl_->DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kTransportLayer, tl_, info.This());
  } catch (const Pylon::GenericException& ex) {
    try {
      tl_->DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    try {
      tl_->DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value TransportLayerWrap::CreateFirstDevice(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  bool hasFilter = !(filterVal.IsUndefined() || filterVal.IsNull());
  bool ok = false;
  Pylon::CDeviceInfo di;
  if (hasFilter) {
    di = RequireDeviceInfo(env, filterVal, &ok);
    if (!ok) return env.Null();
  }
  Pylon::IPylonDevice* dev = nullptr;
  try {
    if (hasFilter) {
      dev = tl_->CreateFirstDevice(di);
    } else {
      dev = tl_->CreateFirstDevice();
    }
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      tl_->DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kTransportLayer, tl_, info.This());
  } catch (const std::exception& ex) {
    try {
      tl_->DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value TransportLayerWrap::DestroyDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "destroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  PylonDeviceWrap* devWrap =
      Napi::ObjectWrap<PylonDeviceWrap>::Unwrap(info[0].As<Napi::Object>());
  if (devWrap == nullptr) {
    Napi::TypeError::New(env, "destroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    devWrap->DestroyNative();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value TransportLayerWrap::IsDeviceAccessible(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "isDeviceAccessible(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    return Napi::Boolean::New(env, tl_->IsDeviceAccessible(di, mode));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TransportLayerWrap::IsDeviceAccessibleInfo(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "isDeviceAccessibleInfo(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    Pylon::EDeviceAccessiblityInfo accessInfo =
        Pylon::Accessibility_Unknown;
    bool accessible = tl_->IsDeviceAccessible(di, mode, &accessInfo);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0), Napi::Boolean::New(env, accessible));
    out.Set(uint32_t(1),
            Napi::Number::New(env, static_cast<double>(accessInfo)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TransportLayerWrap::GetNodeMapNumNodes(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) {
    Napi::Error::New(env, "TransportLayer released")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    GenApi::INodeMap* map = tl_->GetNodeMap();
    uint64_t count = (map != nullptr) ? map->GetNumNodes() : 0;
    return Napi::Number::New(env, static_cast<double>(count));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value TransportLayerWrap::Release(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (released_ || tl_ == nullptr) return env.Undefined();
  Pylon::ITransportLayer* tl = tl_;
  tl_ = nullptr;
  released_ = true;
  try {
    Pylon::CTlFactory::GetInstance().ReleaseTl(tl);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
  return env.Undefined();
}

// --- InterfaceWrap ---------------------------------------------------------

Napi::FunctionReference InterfaceWrap::constructor;

void InterfaceWrap::Init(Napi::Env env, Napi::Object exports,
                         const Napi::Function& base) {
  Napi::Function ctor = DefineClass(
      env, "Interface",
      {InstanceMethod("getInterfaceInfo", &InterfaceWrap::GetInterfaceInfo),
       InstanceMethod("enumerateDevices", &InterfaceWrap::EnumerateDevices),
       InstanceMethod("open", &InterfaceWrap::Open),
       InstanceMethod("close", &InterfaceWrap::Close),
       InstanceMethod("isOpen", &InterfaceWrap::IsOpen),
       InstanceMethod("getNodeMapNumNodes",
                      &InterfaceWrap::GetNodeMapNumNodes),
       InstanceMethod("createDevice", &InterfaceWrap::CreateDevice),
       InstanceMethod("createFirstDevice", &InterfaceWrap::CreateFirstDevice),
       InstanceMethod("destroyDevice", &InterfaceWrap::DestroyDevice),
       InstanceMethod("isDeviceAccessible", &InterfaceWrap::IsDeviceAccessible),
       InstanceMethod("isDeviceAccessibleInfo",
                      &InterfaceWrap::IsDeviceAccessibleInfo),
       InstanceMethod("destroy", &InterfaceWrap::Destroy)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  SetProtoChain(env, ctor, base);
  exports.Set("Interface", ctor);
}

InterfaceWrap::InterfaceWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<InterfaceWrap>(info) {}

InterfaceWrap::~InterfaceWrap() { DestroyNative(); }

Napi::Object InterfaceWrap::Create(Napi::Env env, Pylon::IInterface* iface,
                                  Pylon::ITransportLayer* parent,
                                  const Napi::Value& parentObj) {
  Napi::Object obj = constructor.New({});
  InterfaceWrap* self = Unwrap(obj);
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return obj;
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return obj;
  }
  self->lifecycleHeld_ = true;
  self->iface_ = iface;
  self->parent_ = parent;
  self->destroyed_ = false;
  if (parentObj.IsObject() && !parentObj.IsArray()) {
    self->parentObj_ = Napi::Persistent(parentObj.As<Napi::Object>());
  }
  return obj;
}

void InterfaceWrap::DestroyNative() {
  if (destroyed_ || iface_ == nullptr || parent_ == nullptr) {
    if (lifecycleHeld_) {
      lifecycle::ReleaseRef();
      lifecycleHeld_ = false;
    }
    destroyed_ = true;
    iface_ = nullptr;
    return;
  }
  Pylon::IInterface* iface = iface_;
  Pylon::ITransportLayer* parent = parent_;
  iface_ = nullptr;
  destroyed_ = true;
  try {
    parent->DestroyInterface(iface);
  } catch (...) {
  }
  parentObj_.Reset();
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
}

Napi::Value InterfaceWrap::GetInterfaceInfo(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    return WrapInterfaceInfo(env, iface_->GetInterfaceInfo());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceWrap::EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  Pylon::DeviceInfoList_t filter;
  bool useNoFilter = false;
  if (!discovery::BuildFilter(env, filterVal, &filter, &useNoFilter)) {
    return env.Null();
  }
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::DeviceInfoList_t list;
  int count = 0;
  try {
    if (useNoFilter) {
      count = iface_->EnumerateDevices(list);
    } else {
      count = iface_->EnumerateDevices(list, filter);
    }
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    arr.Set(static_cast<uint32_t>(i), WrapDeviceInfo(env, list[i]));
    if (env.IsExceptionPending()) return env.Null();
  }
  return arr;
}

Napi::Value InterfaceWrap::Open(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    iface_->Open();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value InterfaceWrap::Close(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    iface_->Close();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value InterfaceWrap::IsOpen(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) return Napi::Boolean::New(env, false);
  try {
    return Napi::Boolean::New(env, iface_->IsOpen());
  } catch (...) {
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value InterfaceWrap::GetNodeMapNumNodes(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    GenApi::INodeMap* map = iface_->GetNodeMap();
    uint64_t count = (map != nullptr) ? map->GetNumNodes() : 0;
    return Napi::Number::New(env, static_cast<double>(count));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceWrap::CreateDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "createDevice(info: DeviceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Pylon::IPylonDevice* dev = nullptr;
  try {
    dev = iface_->CreateDevice(di);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      iface_->DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kInterface, iface_, info.This());
  } catch (const std::exception& ex) {
    try {
      iface_->DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value InterfaceWrap::CreateFirstDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  bool hasFilter = !(filterVal.IsUndefined() || filterVal.IsNull());
  bool ok = false;
  Pylon::CDeviceInfo di;
  if (hasFilter) {
    di = RequireDeviceInfo(env, filterVal, &ok);
    if (!ok) return env.Null();
  }
  Pylon::IPylonDevice* dev = nullptr;
  try {
    if (hasFilter) {
      dev = iface_->CreateFirstDevice(di);
    } else {
      dev = iface_->CreateFirstDevice();
    }
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      iface_->DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kInterface, iface_, info.This());
  } catch (const std::exception& ex) {
    try {
      iface_->DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value InterfaceWrap::DestroyDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "destroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  PylonDeviceWrap* devWrap =
      Napi::ObjectWrap<PylonDeviceWrap>::Unwrap(info[0].As<Napi::Object>());
  if (devWrap == nullptr) {
    Napi::TypeError::New(env, "destroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    devWrap->DestroyNative();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value InterfaceWrap::IsDeviceAccessible(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "isDeviceAccessible(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    return Napi::Boolean::New(env, iface_->IsDeviceAccessible(di, mode));
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceWrap::IsDeviceAccessibleInfo(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) {
    Napi::Error::New(env, "Interface destroyed").ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "isDeviceAccessibleInfo(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    Pylon::EDeviceAccessiblityInfo accessInfo = Pylon::Accessibility_Unknown;
    bool accessible = iface_->IsDeviceAccessible(di, mode, &accessInfo);
    Napi::Array out = Napi::Array::New(env, 2);
    out.Set(uint32_t(0), Napi::Boolean::New(env, accessible));
    out.Set(uint32_t(1),
            Napi::Number::New(env, static_cast<double>(accessInfo)));
    return out;
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value InterfaceWrap::Destroy(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (destroyed_ || iface_ == nullptr) return env.Undefined();
  Pylon::IInterface* iface = iface_;
  Pylon::ITransportLayer* parent = parent_;
  iface_ = nullptr;
  destroyed_ = true;
  try {
    parent->DestroyInterface(iface);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  parentObj_.Reset();
  if (lifecycleHeld_) {
    lifecycle::ReleaseRef();
    lifecycleHeld_ = false;
  }
  return env.Undefined();
}

// --- TlFactoryWrap ---------------------------------------------------------

Napi::FunctionReference TlFactoryWrap::constructor;
Napi::ObjectReference TlFactoryWrap::singleton_;

void TlFactoryWrap::Init(Napi::Env env, Napi::Object exports,
                         const Napi::Function& base) {
  Napi::Function ctor = DefineClass(
      env, "TlFactory",
      {StaticMethod("GetInstance", &TlFactoryWrap::GetInstance),
       InstanceMethod("EnumerateTls", &TlFactoryWrap::EnumerateTls),
       InstanceMethod("CreateTl", &TlFactoryWrap::CreateTl),
       InstanceMethod("ReleaseTl", &TlFactoryWrap::ReleaseTl),
       InstanceMethod("EnumerateDevices", &TlFactoryWrap::EnumerateDevices),
       InstanceMethod("CreateDevice", &TlFactoryWrap::CreateDevice),
       InstanceMethod("CreateFirstDevice", &TlFactoryWrap::CreateFirstDevice),
       InstanceMethod("DestroyDevice", &TlFactoryWrap::DestroyDevice),
       InstanceMethod("IsDeviceAccessible", &TlFactoryWrap::IsDeviceAccessible),
       InstanceMethod("IsDeviceAccessibleInfo",
                      &TlFactoryWrap::IsDeviceAccessibleInfo)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  SetProtoChain(env, ctor, base);
  exports.Set("TlFactory", ctor);
}

TlFactoryWrap::TlFactoryWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TlFactoryWrap>(info) {}

Napi::Value TlFactoryWrap::GetInstance(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (singleton_.IsEmpty()) {
    Napi::Object obj = constructor.New({});
    if (env.IsExceptionPending()) return env.Null();
    singleton_ = Napi::Persistent(obj);
  }
  return singleton_.Value();
}

Napi::Value TlFactoryWrap::EnumerateTls(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::TlInfoList_t list;
  int count = 0;
  try {
    count = Pylon::CTlFactory::GetInstance().EnumerateTls(list);
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    arr.Set(static_cast<uint32_t>(i), WrapTlInfo(env, list[i]));
    if (env.IsExceptionPending()) return env.Null();
  }
  return arr;
}

Napi::Value TlFactoryWrap::CreateTl(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "CreateTl(deviceClass: string | TlInfo)")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool byString = info[0].IsString();
  std::string deviceClass;
  Pylon::CTlInfo tlInfo;
  TlInfoWrap* tiWrap = nullptr;
  if (byString) {
    deviceClass = info[0].As<Napi::String>().Utf8Value();
  } else if (info[0].IsObject()) {
    tiWrap =
        Napi::ObjectWrap<TlInfoWrap>::Unwrap(info[0].As<Napi::Object>());
    if (tiWrap == nullptr) {
      Napi::TypeError::New(env, "CreateTl(deviceClass: string | TlInfo)")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    tlInfo = tiWrap->Info();
  } else {
    Napi::TypeError::New(env, "CreateTl(deviceClass: string | TlInfo)")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::ITransportLayer* tl = nullptr;
  try {
    if (byString) {
      tl = Pylon::CTlFactory::GetInstance().CreateTl(
          Pylon::String_t(deviceClass.c_str()));
    } else {
      tl = Pylon::CTlFactory::GetInstance().CreateTl(tlInfo);
    }
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  // No matching transport layer: return null (pypylon returns None).
  if (tl == nullptr) return env.Null();
  return TransportLayerWrap::Create(env, tl);
}

Napi::Value TlFactoryWrap::ReleaseTl(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "ReleaseTl(tl: TransportLayer) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  TransportLayerWrap* tlWrap =
      Napi::ObjectWrap<TransportLayerWrap>::Unwrap(info[0].As<Napi::Object>());
  if (tlWrap == nullptr) {
    Napi::TypeError::New(env, "ReleaseTl(tl: TransportLayer) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    tlWrap->ReleaseNative();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value TlFactoryWrap::EnumerateDevices(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  Pylon::DeviceInfoList_t filter;
  bool useNoFilter = false;
  if (!discovery::BuildFilter(env, filterVal, &filter, &useNoFilter)) {
    return env.Null();
  }
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  Pylon::DeviceInfoList_t list;
  int count = 0;
  try {
    if (useNoFilter) {
      count = Pylon::CTlFactory::GetInstance().EnumerateDevices(list);
    } else {
      count = Pylon::CTlFactory::GetInstance().EnumerateDevices(list, filter);
    }
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(count));
  for (int i = 0; i < count; ++i) {
    arr.Set(static_cast<uint32_t>(i), WrapDeviceInfo(env, list[i]));
    if (env.IsExceptionPending()) return env.Null();
  }
  return arr;
}

Napi::Value TlFactoryWrap::CreateDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "CreateDevice(info: DeviceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Pylon::IPylonDevice* dev = nullptr;
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  try {
    dev = Pylon::CTlFactory::GetInstance().CreateDevice(di);
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kFactory, nullptr, env.Null());
  } catch (const std::exception& ex) {
    try {
      Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value TlFactoryWrap::CreateFirstDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value filterVal =
      info.Length() >= 1 ? info[0] : env.Undefined().As<Napi::Value>();
  bool hasFilter = !(filterVal.IsUndefined() || filterVal.IsNull());
  bool ok = false;
  Pylon::CDeviceInfo di;
  if (hasFilter) {
    di = RequireDeviceInfo(env, filterVal, &ok);
    if (!ok) return env.Null();
  }
  Pylon::IPylonDevice* dev = nullptr;
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  try {
    if (hasFilter) {
      dev = Pylon::CTlFactory::GetInstance().CreateFirstDevice(di);
    } else {
      dev = Pylon::CTlFactory::GetInstance().CreateFirstDevice();
    }
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Object obj = PylonDeviceWrap::constructor.New({});
  if (env.IsExceptionPending()) {
    try {
      Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
    } catch (...) {
    }
    return env.Null();
  }
  try {
    PylonDeviceWrap::Unwrap(obj)->Attach(
        dev, PylonDeviceWrap::CreatorKind::kFactory, nullptr, env.Null());
  } catch (const std::exception& ex) {
    try {
      Pylon::CTlFactory::GetInstance().DestroyDevice(dev);
    } catch (...) {
    }
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  return obj;
}

Napi::Value TlFactoryWrap::DestroyDevice(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "DestroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  PylonDeviceWrap* devWrap =
      Napi::ObjectWrap<PylonDeviceWrap>::Unwrap(info[0].As<Napi::Object>());
  if (devWrap == nullptr) {
    Napi::TypeError::New(env, "DestroyDevice(device: PylonDevice) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    devWrap->DestroyNative();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value TlFactoryWrap::IsDeviceAccessible(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "IsDeviceAccessible(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  bool accessible = false;
  try {
    accessible =
        Pylon::CTlFactory::GetInstance().IsDeviceAccessible(di, mode);
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  return Napi::Boolean::New(env, accessible);
}

Napi::Value TlFactoryWrap::IsDeviceAccessibleInfo(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1) {
    Napi::TypeError::New(env, "IsDeviceAccessibleInfo(info[, mode]) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  bool ok = false;
  Pylon::CDeviceInfo di = RequireDeviceInfo(env, info[0], &ok);
  if (!ok) return env.Null();
  Napi::Value modeVal =
      info.Length() >= 2 ? info[1] : env.Undefined().As<Napi::Value>();
  Pylon::AccessModeSet mode(Pylon::Control);
  if (!discovery::ToAccessMode(env, modeVal, &mode)) return env.Null();
  try {
    lifecycle::EnsureInitialized();
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  bool accessible = false;
  Pylon::EDeviceAccessiblityInfo accessInfo = Pylon::Accessibility_Unknown;
  try {
    accessible = Pylon::CTlFactory::GetInstance().IsDeviceAccessible(
        di, mode, &accessInfo);
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
  lifecycle::ReleaseRef();
  Napi::Array out = Napi::Array::New(env, 2);
  out.Set(uint32_t(0), Napi::Boolean::New(env, accessible));
  out.Set(uint32_t(1), Napi::Number::New(env, static_cast<double>(accessInfo)));
  return out;
}

// --- Module init -----------------------------------------------------------

void InitDiscovery(Napi::Env env, Napi::Object exports) {
  // DeviceInfoWrap::Init runs separately in addon.cc Init; the info classes
  // below only need the IDeviceFactory base for the prototype chain.
  IDeviceFactoryWrap::Init(env, exports);
  Napi::Function base = IDeviceFactoryWrap::constructor.Value();
  TlInfoWrap::Init(env, exports, base);
  InterfaceInfoWrap::Init(env, exports, base);
  TransportLayerWrap::Init(env, exports, base);
  InterfaceWrap::Init(env, exports, base);
  PylonDeviceWrap::Init(env, exports);
  TlFactoryWrap::Init(env, exports, base);

  // Device classes (pylon/DeviceClass.h).
  exports.Set("BaslerCamEmuDeviceClass", Pylon::BaslerCamEmuDeviceClass);
  exports.Set("BaslerIpCamDeviceClass", Pylon::BaslerIpCamDeviceClass);
  exports.Set("BaslerCameraLinkDeviceClass", Pylon::BaslerCameraLinkDeviceClass);
  exports.Set("BaslerGigEDeviceClass", Pylon::BaslerGigEDeviceClass);
  exports.Set("BaslerGenTlDeviceClassPrefix", Pylon::BaslerGenTlDeviceClassPrefix);
  exports.Set("BaslerGenTlGevDeviceClass", Pylon::BaslerGenTlGevDeviceClass);
  exports.Set("BaslerGenTlU3vDeviceClass", Pylon::BaslerGenTlU3vDeviceClass);
  exports.Set("BaslerGenTlCxpDeviceClass", Pylon::BaslerGenTlCxpDeviceClass);
  exports.Set("BaslerGenTlBlazeDeviceClass", Pylon::BaslerGenTlBlazeDeviceClass);
  exports.Set("BaslerGenTlStaDeviceClass", Pylon::BaslerGenTlStaDeviceClass);
  exports.Set("BaslerUsbDeviceClass", Pylon::BaslerUsbDeviceClass);
  // Transport-layer types (pylon/Info.h, TLType namespace).
  exports.Set("TLTypeGigE", Pylon::TLType::TLTypeGigE);
  exports.Set("TLTypeUSB", Pylon::TLType::TLTypeUSB);
  exports.Set("TLTypeCL", Pylon::TLType::TLTypeCL);
  exports.Set("TLTypeCamEmu", Pylon::TLType::TLTypeCamEmu);
  exports.Set("TLTypeCXP", Pylon::TLType::TLTypeCXP);
  exports.Set("TLTypeCustom", Pylon::TLType::TLTypeCustom);
  // CInfoBase property keys.
  exports.Set("FriendlyNameKey", Pylon::Key::FriendlyNameKey);
  exports.Set("FullNameKey", Pylon::Key::FullNameKey);
  exports.Set("VendorNameKey", Pylon::Key::VendorNameKey);
  exports.Set("DeviceClassKey", Pylon::Key::DeviceClassKey);
  exports.Set("TLTypeKey", Pylon::Key::TLTypeKey);
  exports.Set("InterfaceIDKey", Pylon::Key::InterfaceIDKey);
  // CDeviceInfo property keys.
  exports.Set("SerialNumberKey", Pylon::Key::SerialNumberKey);
  exports.Set("UserDefinedNameKey", Pylon::Key::UserDefinedNameKey);
  exports.Set("ModelNameKey", Pylon::Key::ModelNameKey);
  exports.Set("DeviceVersionKey", Pylon::Key::DeviceVersionKey);
  exports.Set("DeviceFactoryKey", Pylon::Key::DeviceFactoryKey);
  exports.Set("XMLSourceKey", Pylon::Key::XMLSourceKey);
  exports.Set("ManufacturerInfoKey", Pylon::Key::ManufacturerInfoKey);
  exports.Set("DeviceIDKey", Pylon::Key::DeviceIDKey);
  exports.Set("AddressKey", Pylon::Key::AddressKey);
  exports.Set("IpAddressKey", Pylon::Key::IpAddressKey);
  exports.Set("DefaultGatewayKey", Pylon::Key::DefaultGatewayKey);
  exports.Set("SubnetMaskKey", Pylon::Key::SubnetMaskKey);
  exports.Set("SubnetAddressKey", Pylon::Key::SubnetAddressKey);
  exports.Set("PortNrKey", Pylon::Key::PortNrKey);
  exports.Set("MacAddressKey", Pylon::Key::MacAddressKey);
  exports.Set("InterfaceKey", Pylon::Key::InterfaceKey);
  exports.Set("IpConfigOptionsKey", Pylon::Key::IpConfigOptionsKey);
  exports.Set("IpConfigCurrentKey", Pylon::Key::IpConfigCurrentKey);
  exports.Set("DeviceGUIDKey", Pylon::Key::DeviceGUIDKey);
  exports.Set("VendorIdKey", Pylon::Key::VendorIdKey);
  exports.Set("ProductIdKey", Pylon::Key::ProductIdKey);
  exports.Set("DriverKeyNameKey", Pylon::Key::DriverKeyNameKey);
  exports.Set("UsbDriverTypeKey", Pylon::Key::UsbDriverTypeKey);
  exports.Set("UsbPortVersionBcdKey", Pylon::Key::UsbPortVersionBcdKey);
  exports.Set("SpeedSupportBitmaskKey", Pylon::Key::SpeedSupportBitmaskKey);
  exports.Set("TransferModeKey", Pylon::Key::TransferModeKey);
  exports.Set("PortIDKey", Pylon::Key::PortIDKey);
  exports.Set("InitialBaudRateKey", Pylon::Key::InitialBaudRateKey);
  exports.Set("DeviceXMLFileOverrideKey", Pylon::Key::DeviceXMLFileOverrideKey);
  exports.Set("DeviceSpecificStringKey", Pylon::Key::DeviceSpecificStringKey);
  exports.Set("PortSpecificStringKey", Pylon::Key::PortSpecificStringKey);
  exports.Set("DeviceIdxKey", Pylon::Key::DeviceIdxKey);
  // CTlInfo property keys.
  exports.Set("FileNameKey", Pylon::Key::FileNameKey);
  exports.Set("InfoIDKey", Pylon::Key::InfoIDKey);
  exports.Set("VersionKey", Pylon::Key::VersionKey);
  // Device access modes (pylon/DeviceAccessMode.h).
  exports.Set("DeviceAccessMode_Control",
              Napi::Number::New(env, static_cast<double>(Pylon::Control)));
  exports.Set("DeviceAccessMode_Stream",
              Napi::Number::New(env, static_cast<double>(Pylon::Stream)));
  exports.Set("DeviceAccessMode_Event",
              Napi::Number::New(env, static_cast<double>(Pylon::Event)));
  exports.Set("DeviceAccessMode_Exclusive",
              Napi::Number::New(env, static_cast<double>(Pylon::Exclusive)));
  // Device accessibility states (pylon/DeviceFactory.h).
  exports.Set("Accessibility_Unknown",
              Napi::Number::New(env, static_cast<double>(Pylon::Accessibility_Unknown)));
  exports.Set("Accessibility_Ok",
              Napi::Number::New(env, static_cast<double>(Pylon::Accessibility_Ok)));
  exports.Set("Accessibility_Opened",
              Napi::Number::New(env, static_cast<double>(Pylon::Accessibility_Opened)));
  exports.Set("Accessibility_OpenedExclusively",
              Napi::Number::New(
                  env, static_cast<double>(Pylon::Accessibility_OpenedExclusively)));
  exports.Set("Accessibility_NotReachable",
              Napi::Number::New(
                  env, static_cast<double>(Pylon::Accessibility_NotReachable)));
}

}  // namespace node_pylon
