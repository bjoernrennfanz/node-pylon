// DeviceInfo N-API wrapper: implementation (full CDeviceInfo/CInfoBase).
#include "device_info.h"

#include <string>
#include <vector>

#include "error_map.h"

namespace node_pylon {

Napi::FunctionReference DeviceInfoWrap::constructor;

namespace {

void RequireStringValue(Napi::Env env, const Napi::Value& value,
                        const char* what, std::string* out, bool* ok) {
  if (!value.IsString()) {
    Napi::TypeError::New(env, std::string(what) + " must be a string")
        .ThrowAsJavaScriptException();
    *ok = false;
    return;
  }
  *ok = true;
  *out = value.As<Napi::String>().Utf8Value();
}

std::string ToStd(const Pylon::String_t& s) {
  return std::string(s.c_str());
}

Napi::Array PropertyNameArray(Napi::Env env, const Pylon::CDeviceInfo& info) {
  Pylon::StringList_t names;
  info.GetPropertyNames(names);
  Napi::Array arr = Napi::Array::New(env, static_cast<uint32_t>(names.size()));
  for (size_t i = 0; i < names.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), ToStd(names[i]));
  }
  return arr;
}

}  // namespace

// --- Accessor/method generation -------------------------------------------
// Getters serve both the InstanceAccessor and the legacy Get<Name> method.
#define NODEPYLON_DEFINE_RW_ACCESSOR(Name)                                     \
  Napi::Value DeviceInfoWrap::Get##Name(const Napi::CallbackInfo& info) {      \
    try {                                                                      \
      return Napi::String::New(info.Env(), ToStd(info_.Get##Name()));          \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }                                                                            \
  void DeviceInfoWrap::Set##Name(const Napi::CallbackInfo& info,                \
                                 const Napi::Value& value) {                    \
    std::string v;                                                             \
    bool ok = false;                                                           \
    RequireStringValue(info.Env(), value, #Name, &v, &ok);                     \
    if (!ok) return;                                                           \
    try {                                                                      \
      info_.Set##Name(v.c_str());                                               \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
    }                                                                          \
  }                                                                            \
  Napi::Value DeviceInfoWrap::Set##Name##Method(                                \
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
  Napi::Value DeviceInfoWrap::Is##Name##Available(                              \
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

#define NODEPYLON_DEFINE_RO_ACCESSOR(Name)                                     \
  Napi::Value DeviceInfoWrap::Get##Name(const Napi::CallbackInfo& info) {      \
    try {                                                                      \
      return Napi::String::New(info.Env(), ToStd(info_.Get##Name()));          \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(info.Env(), ex);                               \
      return info.Env().Null();                                                \
    }                                                                          \
  }                                                                            \
  Napi::Value DeviceInfoWrap::Is##Name##Available(                              \
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

NODEPYLON_DEVICEINFO_RW_LIST(NODEPYLON_DEFINE_RW_ACCESSOR)
NODEPYLON_DEVICEINFO_RO_LIST(NODEPYLON_DEFINE_RO_ACCESSOR)

#undef NODEPYLON_DEFINE_RW_ACCESSOR
#undef NODEPYLON_DEFINE_RO_ACCESSOR

void DeviceInfoWrap::Init(Napi::Env env, Napi::Object exports) {
  std::vector<Napi::ClassPropertyDescriptor<DeviceInfoWrap>> props;
#define NODEPYLON_ADD_RW(Name)                                                 \
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(                      \
      #Name, &DeviceInfoWrap::Get##Name, &DeviceInfoWrap::Set##Name));         \
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(                         \
      "Get" #Name, &DeviceInfoWrap::Get##Name));                               \
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(                         \
      "Set" #Name, &DeviceInfoWrap::Set##Name##Method));                       \
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(                         \
      "Is" #Name "Available", &DeviceInfoWrap::Is##Name##Available));
#define NODEPYLON_ADD_RO(Name)                                                 \
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(                      \
      #Name, &DeviceInfoWrap::Get##Name, nullptr));                            \
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(                         \
      "Get" #Name, &DeviceInfoWrap::Get##Name));                               \
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(                         \
      "Is" #Name "Available", &DeviceInfoWrap::Is##Name##Available));
  NODEPYLON_DEVICEINFO_RW_LIST(NODEPYLON_ADD_RW)
  NODEPYLON_DEVICEINFO_RO_LIST(NODEPYLON_ADD_RO)
#undef NODEPYLON_ADD_RW
#undef NODEPYLON_ADD_RO
  // camelCase aliases for the discovery fields used by the facade/tests.
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "friendlyName", &DeviceInfoWrap::GetFriendlyName,
      &DeviceInfoWrap::SetFriendlyName));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "fullName", &DeviceInfoWrap::GetFullName, &DeviceInfoWrap::SetFullName));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "vendorName", &DeviceInfoWrap::GetVendorName,
      &DeviceInfoWrap::SetVendorName));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "deviceClass", &DeviceInfoWrap::GetDeviceClass,
      &DeviceInfoWrap::SetDeviceClass));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "serialNumber", &DeviceInfoWrap::GetSerialNumber,
      &DeviceInfoWrap::SetSerialNumber));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "modelName", &DeviceInfoWrap::GetModelName,
      &DeviceInfoWrap::SetModelName));
  props.push_back(Napi::InstanceAccessor<DeviceInfoWrap>(
      "tlType", &DeviceInfoWrap::GetTLType, &DeviceInfoWrap::SetTLType));
  // Generic IProperties + comparison + dict helpers + GigE predicates.
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "GetPropertyNames", &DeviceInfoWrap::GetPropertyNames));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "GetPropertyAvailable", &DeviceInfoWrap::GetPropertyAvailable));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "GetPropertyValue", &DeviceInfoWrap::GetPropertyValue));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "SetPropertyValue", &DeviceInfoWrap::SetPropertyValue));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("IsSubset", &DeviceInfoWrap::IsSubset));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsUserProvided", &DeviceInfoWrap::IsUserProvided));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "GetPropertyNotAvailable", &DeviceInfoWrap::GetPropertyNotAvailable));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("equals", &DeviceInfoWrap::Equals));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "lessThan", &DeviceInfoWrap::LessThan));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("keys", &DeviceInfoWrap::Keys));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("values", &DeviceInfoWrap::Values));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("items", &DeviceInfoWrap::Items));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("toDict", &DeviceInfoWrap::ToDict));
  props.push_back(
      Napi::InstanceMethod<DeviceInfoWrap>("update", &DeviceInfoWrap::Update));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsPersistentIpActive", &DeviceInfoWrap::IsPersistentIpActive));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsDhcpActive", &DeviceInfoWrap::IsDhcpActive));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsAutoIpActive", &DeviceInfoWrap::IsAutoIpActive));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsPersistentIpSupported", &DeviceInfoWrap::IsPersistentIpSupported));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsDhcpSupported", &DeviceInfoWrap::IsDhcpSupported));
  props.push_back(Napi::InstanceMethod<DeviceInfoWrap>(
      "IsAutoIpSupported", &DeviceInfoWrap::IsAutoIpSupported));
  Napi::Function ctor = DefineClass(env, "DeviceInfo", props);
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  // Static pypylon parity: DeviceInfo.GetPropertyNotAvailable().
  Napi::Function notAvail = Napi::Function::New(
      env,
      [](const Napi::CallbackInfo& info) -> Napi::Value {
        return Napi::String::New(info.Env(),
                                 Pylon::CDeviceInfo::GetPropertyNotAvailable());
      },
      "GetPropertyNotAvailable");
  ctor.Set("GetPropertyNotAvailable", notAvail);
  exports.Set("DeviceInfo", ctor);
}

DeviceInfoWrap::DeviceInfoWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<DeviceInfoWrap>(info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0) return;
  if (info.Length() == 1) {
    DeviceInfoWrap* other = nullptr;
    if (info[0].IsObject()) {
      other = Napi::ObjectWrap<DeviceInfoWrap>::Unwrap(
          info[0].As<Napi::Object>());
    }
    if (other != nullptr) {
      try {
        info_ = other->info_;
      } catch (const std::exception& ex) {
        errormap::ThrowTranslated(env, ex);
      }
      return;
    }
  }
  Napi::TypeError::New(env, "new DeviceInfo() or new DeviceInfo(other) expected")
      .ThrowAsJavaScriptException();
}

bool DeviceInfoWrap::ToDeviceInfo(Napi::Env env, const Napi::Value& value,
                                  Pylon::CDeviceInfo* out) {
  if (value.IsArray()) {
    Napi::TypeError::New(env, "filter entries must be DeviceInfo or dict")
        .ThrowAsJavaScriptException();
    return false;
  }
  if (value.IsObject()) {
    Napi::Object obj = value.As<Napi::Object>();
    DeviceInfoWrap* wrapped = Napi::ObjectWrap<DeviceInfoWrap>::Unwrap(obj);
    if (wrapped != nullptr) {
      *out = wrapped->Info();
      return true;
    }
    // Plain-object dict: every value must be a string (pypylon parity).
    Napi::Array keys;
    try {
      keys = obj.GetPropertyNames();
    } catch (...) {
      Napi::TypeError::New(env, "filter entries must be DeviceInfo or dict")
          .ThrowAsJavaScriptException();
      return false;
    }
    Pylon::CDeviceInfo info;
    for (uint32_t i = 0; i < keys.Length(); ++i) {
      Napi::Value key = keys.Get(i);
      std::string name = key.As<Napi::String>().Utf8Value();
      Napi::Value v = obj.Get(key);
      if (!v.IsString()) {
        Napi::TypeError::New(env,
                             "filter dict values must be strings (" + name + ")")
            .ThrowAsJavaScriptException();
        return false;
      }
      std::string s = v.As<Napi::String>().Utf8Value();
      try {
        info.SetPropertyValue(name.c_str(), s.c_str());
      } catch (const Pylon::GenericException& ex) {
        errormap::ThrowTranslated(env, ex);
        return false;
      } catch (const std::exception& ex) {
        errormap::ThrowTranslated(env, ex);
        return false;
      }
    }
    *out = info;
    return true;
  }
  Napi::TypeError::New(env, "filter entries must be DeviceInfo or dict")
      .ThrowAsJavaScriptException();
  return false;
}

Napi::Value DeviceInfoWrap::GetPropertyNames(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::GetPropertyAvailable(
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

Napi::Value DeviceInfoWrap::GetPropertyValue(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::SetPropertyValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
    Napi::TypeError::New(env, "SetPropertyValue(name: string, value: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    std::string name = info[0].As<Napi::String>().Utf8Value();
    std::string value = info[1].As<Napi::String>().Utf8Value();
    info_.SetPropertyValue(name.c_str(), value.c_str());
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
  }
  return env.Undefined();
}

Napi::Value DeviceInfoWrap::IsSubset(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "IsSubset(other: DeviceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  DeviceInfoWrap* other = Napi::ObjectWrap<DeviceInfoWrap>::Unwrap(
      info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "IsSubset(other: DeviceInfo) expected")
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

Napi::Value DeviceInfoWrap::IsUserProvided(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::GetPropertyNotAvailable(
    const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(),
                           Pylon::CDeviceInfo::GetPropertyNotAvailable());
}

Napi::Value DeviceInfoWrap::Equals(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) return Napi::Boolean::New(env, false);
  DeviceInfoWrap* other = Napi::ObjectWrap<DeviceInfoWrap>::Unwrap(
      info[0].As<Napi::Object>());
  if (other == nullptr) return Napi::Boolean::New(env, false);
  try {
    return Napi::Boolean::New(env, info_ == other->info_);
  } catch (...) {
    return Napi::Boolean::New(env, false);
  }
}

Napi::Value DeviceInfoWrap::LessThan(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "lessThan(other: DeviceInfo) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  DeviceInfoWrap* other = Napi::ObjectWrap<DeviceInfoWrap>::Unwrap(
      info[0].As<Napi::Object>());
  if (other == nullptr) {
    Napi::TypeError::New(env, "lessThan(other: DeviceInfo) expected")
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

Napi::Value DeviceInfoWrap::Keys(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    return PropertyNameArray(env, info_);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Null();
  }
}

Napi::Value DeviceInfoWrap::Values(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::Items(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::ToDict(const Napi::CallbackInfo& info) {
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

Napi::Value DeviceInfoWrap::Update(const Napi::CallbackInfo& info) {
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

#define NODEPYLON_DEFINE_IP_PREDICATE(Method, Call)                            \
  Napi::Value DeviceInfoWrap::Method(const Napi::CallbackInfo& info) {          \
    Napi::Env env = info.Env();                                                \
    try {                                                                      \
      return Napi::Boolean::New(env, info_.Call());                            \
    } catch (const Pylon::GenericException& ex) {                              \
      errormap::ThrowTranslated(env, ex);                                      \
      return env.Null();                                                       \
    } catch (const std::exception& ex) {                                       \
      errormap::ThrowTranslated(env, ex);                                      \
      return env.Null();                                                       \
    }                                                                          \
  }

NODEPYLON_DEFINE_IP_PREDICATE(IsPersistentIpActive, IsPersistentIpActive)
NODEPYLON_DEFINE_IP_PREDICATE(IsDhcpActive, IsDhcpActive)
NODEPYLON_DEFINE_IP_PREDICATE(IsAutoIpActive, IsAutoIpActive)
NODEPYLON_DEFINE_IP_PREDICATE(IsPersistentIpSupported, IsPersistentIpSupported)
NODEPYLON_DEFINE_IP_PREDICATE(IsDhcpSupported, IsDhcpSupported)
NODEPYLON_DEFINE_IP_PREDICATE(IsAutoIpSupported, IsAutoIpSupported)

#undef NODEPYLON_DEFINE_IP_PREDICATE

}  // namespace node_pylon
