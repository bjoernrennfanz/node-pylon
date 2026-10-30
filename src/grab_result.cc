// GrabResult N-API wrapper: implementation (real pylon SDK only).
#include "grab_result.h"

#include <pylon/PixelTypeMapper.h>
#include <pylon/PylonDataContainer.h>

namespace node_pylon {

Napi::FunctionReference GrabResultWrap::constructor;

namespace {

bool ValidGrab(Pylon::CGrabResultPtr& ptr) {
  try {
    return ptr.IsValid();
  } catch (...) {
    return false;
  }
}

// Zero-copy JS Buffer over a grab/component buffer. `owner` (a GrabPtr or
// any shared handle) is moved into a heap holder released by the finalizer.
template <typename Owner>
Napi::Value ZeroCopyBuffer(Napi::Env env, uint8_t* data, size_t size,
                           Owner owner) {
  using Holder = Owner;
  auto* holder = new Holder(std::move(owner));
  return Napi::Buffer<uint8_t>::New(
      env, data, size, [](Napi::Env /*env*/, uint8_t* /*p*/, Holder* h) {
        delete h;
      }, holder);
}

Napi::Value EmptyBuffer(Napi::Env env) {
  return Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(0));
}

}  // namespace

const char* PixelTypeName(Pylon::EPixelType t) {
  switch (t) {
    case Pylon::PixelType_Mono8:
      return "Mono8";
    case Pylon::PixelType_Mono8signed:
      return "Mono8s";
    case Pylon::PixelType_Mono10:
      return "Mono10";
    case Pylon::PixelType_Mono10packed:
      return "Mono10packed";
    case Pylon::PixelType_Mono10p:
      return "Mono10p";
    case Pylon::PixelType_Mono12:
      return "Mono12";
    case Pylon::PixelType_Mono12packed:
      return "Mono12packed";
    case Pylon::PixelType_Mono12p:
      return "Mono12p";
    case Pylon::PixelType_Mono16:
      return "Mono16";
    case Pylon::PixelType_RGB8packed:
      return "RGB8";
    case Pylon::PixelType_BGR8packed:
      return "BGR8";
    case Pylon::PixelType_RGBA8packed:
      return "RGBa8";
    case Pylon::PixelType_BGRA8packed:
      return "BGRa8";
    default:
      break;
  }
  // Fallback: canonical SFNC name from the SDK mapper (never throws across
  // the boundary here; guarded by callers).
  try {
    const char* n = Pylon::CPixelTypeMapper::GetNameByPixelType(
        t, Pylon::SFNCVersion_2_0);
    if (n != nullptr && *n != '\0') return n;
  } catch (...) {
  }
  return "Unknown";
}

const char* PayloadTypeName(Pylon::EPayloadType t) {
  switch (t) {
    case Pylon::PayloadType_Image:
      return "Image";
    case Pylon::PayloadType_RawData:
      return "RawData";
    case Pylon::PayloadType_File:
      return "File";
    case Pylon::PayloadType_ChunkData:
      return "ChunkData";
    case Pylon::PayloadType_GenDC:
      return "GenDC";
    case Pylon::PayloadType_DeviceSpecific:
      return "DeviceSpecific";
    default:
      return "Undefined";
  }
}

Napi::Object BuildGrabFields(Napi::Env env, Pylon::CGrabResultPtr& ptr) {
  Napi::Object out = Napi::Object::New(env);
  if (!ValidGrab(ptr)) {
    out.Set("succeeded", false);
    out.Set("width", Napi::Number::New(env, 0));
    out.Set("height", Napi::Number::New(env, 0));
    out.Set("pixelType", "Unknown");
    out.Set("pixelFormat", "Unknown");
    out.Set("payloadSize", Napi::Number::New(env, 0));
    out.Set("buffer", EmptyBuffer(env));
    out.Set("valid", false);
    return out;
  }
  bool ok = false;
  try {
    ok = ptr->GrabSucceeded();
  } catch (...) {
    ok = false;
  }
  out.Set("succeeded", ok);
  out.Set("valid", true);
  size_t w = 0, h = 0, payload = 0;
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  Pylon::EPayloadType plt = Pylon::PayloadType_Undefined;
  uint64_t ts = 0;
  try {
    if (ok) {
      w = ptr->GetWidth();
      h = ptr->GetHeight();
    }
    pt = ptr->GetPixelType();
    plt = ptr->GetPayloadType();
    payload = ptr->GetPayloadSize();
    ts = ptr->GetTimeStamp();
  } catch (...) {
  }
  const char* pname = PixelTypeName(pt);
  out.Set("width", Napi::Number::New(env, static_cast<double>(w)));
  out.Set("height", Napi::Number::New(env, static_cast<double>(h)));
  out.Set("pixelType", std::string(pname));
  out.Set("pixelFormat", std::string(pname));
  out.Set("pixelTypeValue",
          Napi::Number::New(env, static_cast<double>(pt)));
  out.Set("payloadType", std::string(PayloadTypeName(plt)));
  out.Set("payloadSize", Napi::Number::New(env, static_cast<double>(payload)));
  out.Set("timestamp", Napi::Number::New(env, static_cast<double>(ts)));
  if (ok && payload > 0) {
    uint8_t* data = nullptr;
    size_t bufSize = 0;
    try {
      data = static_cast<uint8_t*>(ptr->GetBuffer());
      bufSize = ptr->GetBufferSize();
      if (bufSize < payload) bufSize = payload;
    } catch (...) {
      data = nullptr;
    }
    if (data != nullptr) {
      auto owner = std::make_shared<Pylon::CGrabResultPtr>(ptr);
      out.Set("buffer", ZeroCopyBuffer(env, data, bufSize, std::move(owner)));
    } else {
      out.Set("buffer", EmptyBuffer(env));
    }
  } else {
    out.Set("buffer", EmptyBuffer(env));
    if (!ok) {
      uint32_t code = 0;
      std::string desc;
      try {
        code = ptr->GetErrorCode();
        desc = std::string(ptr->GetErrorDescription().c_str());
      } catch (...) {
      }
      out.Set("errorCode", Napi::Number::New(env, code));
      out.Set("errorDescription", desc);
    }
  }
  return out;
}

void GrabResultWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "GrabResult",
      {InstanceMethod("isValid", &GrabResultWrap::IsValid),
       InstanceMethod("isUnique", &GrabResultWrap::IsUnique),
       InstanceMethod("release", &GrabResultWrap::Release),
       InstanceMethod("succeeded", &GrabResultWrap::Succeeded),
       InstanceMethod("getWidth", &GrabResultWrap::GetWidth),
       InstanceMethod("getHeight", &GrabResultWrap::GetHeight),
       InstanceMethod("getOffsetX", &GrabResultWrap::GetOffsetX),
       InstanceMethod("getOffsetY", &GrabResultWrap::GetOffsetY),
       InstanceMethod("getPaddingX", &GrabResultWrap::GetPaddingX),
       InstanceMethod("getPaddingY", &GrabResultWrap::GetPaddingY),
       InstanceMethod("getPixelType", &GrabResultWrap::GetPixelType),
       InstanceMethod("getPixelTypeValue",
                      &GrabResultWrap::GetPixelTypeValue),
       InstanceMethod("getPayloadType", &GrabResultWrap::GetPayloadType),
       InstanceMethod("getPayloadSize", &GrabResultWrap::GetPayloadSize),
       InstanceMethod("getBufferSize", &GrabResultWrap::GetBufferSize),
       InstanceMethod("getImageSize", &GrabResultWrap::GetImageSize),
       InstanceMethod("getTimestamp", &GrabResultWrap::GetTimestamp),
       InstanceMethod("getBlockId", &GrabResultWrap::GetBlockId),
       InstanceMethod("getId", &GrabResultWrap::GetId),
       InstanceMethod("getImageNumber", &GrabResultWrap::GetImageNumber),
       InstanceMethod("getSkippedImages",
                      &GrabResultWrap::GetSkippedImages),
       InstanceMethod("getErrorCode", &GrabResultWrap::GetErrorCode),
       InstanceMethod("getErrorDescription",
                      &GrabResultWrap::GetErrorDescription),
       InstanceMethod("getBufferContext", &GrabResultWrap::GetBufferContext),
       InstanceMethod("GetBufferContext", &GrabResultWrap::GetBufferContext),
       InstanceMethod("getBufferFactory", &GrabResultWrap::GetBufferFactory),
       InstanceMethod("GetBufferFactory", &GrabResultWrap::GetBufferFactory),
       InstanceMethod("isChunkDataAvailable",
                      &GrabResultWrap::IsChunkDataAvailable),
       InstanceMethod("hasCrc", &GrabResultWrap::HasCrc),
       InstanceMethod("checkCrc", &GrabResultWrap::CheckCrc),
       InstanceMethod("getStride", &GrabResultWrap::GetStride),
       InstanceMethod("getBuffer", &GrabResultWrap::GetBuffer),
       InstanceMethod("getArray", &GrabResultWrap::GetArray),
       InstanceMethod("getMemoryView", &GrabResultWrap::GetMemoryView),
       InstanceMethod("getDataComponentCount",
                      &GrabResultWrap::GetDataComponentCount),
       InstanceMethod("getDataComponent",
                      &GrabResultWrap::GetDataComponent),
       InstanceMethod("getDataComponentBuffer",
                      &GrabResultWrap::GetDataComponentBuffer),
       InstanceMethod("toObject", &GrabResultWrap::ToObject)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("GrabResult", ctor);
}

GrabResultWrap::GrabResultWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<GrabResultWrap>(info),
      ptr_(std::make_shared<Pylon::CGrabResultPtr>()) {}

void GrabResultWrap::Attach(Pylon::CGrabResultPtr ptr) { *ptr_ = ptr; }

void GrabResultWrap::SetFactory(Napi::Object factory) {
  factoryRef_ = Napi::Persistent(factory);
  hasFactory_ = true;
}

void GrabResultWrap::ClearFactory() {
  factoryRef_.Reset();
  hasFactory_ = false;
}

Napi::Value GrabResultWrap::GetBufferContext(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return env.Null();
    const intptr_t ctx = (*ptr_)->GetBufferContext();
    if (ctx == 0) return env.Null();
    return Napi::Number::New(env, static_cast<double>(ctx));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value GrabResultWrap::GetBufferFactory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!hasFactory_) return env.Null();
  return factoryRef_.Value();
}

Napi::Object GrabResultWrap::FromPtr(Napi::Env env,
                                     Pylon::CGrabResultPtr ptr) {
  Napi::Object obj = constructor.New({});
  GrabResultWrap* w = Napi::ObjectWrap<GrabResultWrap>::Unwrap(obj);
  w->Attach(ptr);
  // Backward-compatible plain fields (existing tests read res.succeeded,
  // res.width, res.buffer, ... directly).
  Napi::Object fields = BuildGrabFields(env, ptr);
  Napi::Array names = fields.GetPropertyNames();
  for (uint32_t i = 0; i < names.Length(); ++i) {
    Napi::Value key = names[i];
    obj.Set(key, fields.Get(key));
  }
  return obj;
}

Napi::Value GrabResultWrap::IsValid(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), ValidGrab(*ptr_));
}
Napi::Value GrabResultWrap::IsUnique(const Napi::CallbackInfo& info) {
  bool u = false;
  try {
    if (ValidGrab(*ptr_)) u = ptr_->IsUnique();
  } catch (...) {
    u = false;
  }
  return Napi::Boolean::New(info.Env(), u);
}
Napi::Value GrabResultWrap::Release(const Napi::CallbackInfo& info) {
  try {
    ptr_->Release();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
  } catch (const std::exception& ex) {
    Napi::Error::New(info.Env(), ex.what()).ThrowAsJavaScriptException();
  }
  // Refresh plain fields so property access matches the released state.
  Napi::Object self = info.This().As<Napi::Object>();
  Napi::Object fields = BuildGrabFields(info.Env(), *ptr_);
  Napi::Array names = fields.GetPropertyNames();
  for (uint32_t i = 0; i < names.Length(); ++i) {
    Napi::Value key = names[i];
    self.Set(key, fields.Get(key));
  }
  return info.Env().Undefined();
}
Napi::Value GrabResultWrap::Succeeded(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (ValidGrab(*ptr_)) v = (*ptr_)->GrabSucceeded();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}

#define GRAB_UINT(getter)                                               \
  Napi::Value GrabResultWrap::getter(const Napi::CallbackInfo& info) {  \
    Napi::Env env = info.Env();                                         \
    double v = 0;                                                       \
    try {                                                               \
      if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);          \
    } catch (...) {                                                     \
      return Napi::Number::New(env, 0);                                 \
    }                                                                   \
    try {                                                               \
      v = static_cast<double>((*ptr_)->getter());                       \
    } catch (const Pylon::GenericException& ex) {                      \
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();   \
      return env.Undefined();                                           \
    } catch (const std::exception& ex) {                                \
      Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();   \
      return env.Undefined();                                           \
    }                                                                   \
    return Napi::Number::New(env, v);                                   \
  }

GRAB_UINT(GetWidth)
GRAB_UINT(GetHeight)
GRAB_UINT(GetOffsetX)
GRAB_UINT(GetOffsetY)
GRAB_UINT(GetPaddingX)
GRAB_UINT(GetPaddingY)
GRAB_UINT(GetPayloadSize)
GRAB_UINT(GetBufferSize)
GRAB_UINT(GetImageSize)
GRAB_UINT(GetErrorCode)

Napi::Value GrabResultWrap::GetTimestamp(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);
    return Napi::Number::New(env,
                             static_cast<double>((*ptr_)->GetTimeStamp()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
Napi::Value GrabResultWrap::GetBlockId(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);
    return Napi::Number::New(env, static_cast<double>((*ptr_)->GetBlockID()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
Napi::Value GrabResultWrap::GetImageNumber(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);
    return Napi::Number::New(
        env, static_cast<double>((*ptr_)->GetImageNumber()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
Napi::Value GrabResultWrap::GetSkippedImages(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);
    return Napi::Number::New(
        env, static_cast<double>((*ptr_)->GetNumberOfSkippedImages()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
Napi::Value GrabResultWrap::GetId(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!ValidGrab(*ptr_)) return Napi::Number::New(env, 0);
    return Napi::Number::New(env, static_cast<double>((*ptr_)->GetID()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value GrabResultWrap::GetPixelType(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType t = Pylon::PixelType_Undefined;
  try {
    if (ValidGrab(*ptr_)) t = (*ptr_)->GetPixelType();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::String::New(env, PixelTypeName(t));
}
Napi::Value GrabResultWrap::GetPixelTypeValue(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  double v = static_cast<double>(Pylon::PixelType_Undefined);
  try {
    if (ValidGrab(*ptr_))
      v = static_cast<double>((*ptr_)->GetPixelType());
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Number::New(env, v);
}
Napi::Value GrabResultWrap::GetPayloadType(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPayloadType t = Pylon::PayloadType_Undefined;
  try {
    if (ValidGrab(*ptr_)) t = (*ptr_)->GetPayloadType();
  } catch (...) {
  }
  return Napi::String::New(env, PayloadTypeName(t));
}
Napi::Value GrabResultWrap::GetErrorDescription(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  std::string s;
  try {
    if (ValidGrab(*ptr_)) s = std::string((*ptr_)->GetErrorDescription().c_str());
  } catch (...) {
  }
  return Napi::String::New(env, s);
}
Napi::Value GrabResultWrap::IsChunkDataAvailable(
    const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (ValidGrab(*ptr_)) v = (*ptr_)->IsChunkDataAvailable();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}
Napi::Value GrabResultWrap::HasCrc(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (ValidGrab(*ptr_)) v = (*ptr_)->HasCRC();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}
Napi::Value GrabResultWrap::CheckCrc(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    if (ValidGrab(*ptr_)) v = (*ptr_)->CheckCRC();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}
Napi::Value GrabResultWrap::GetStride(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t stride = 0;
  try {
    if (!ValidGrab(*ptr_) || !(*ptr_)->GetStride(stride)) {
      return env.Null();
    }
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(stride));
}

Napi::Value GrabResultWrap::GetBuffer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!ValidGrab(*ptr_)) return EmptyBuffer(env);
  uint8_t* data = nullptr;
  size_t size = 0;
  try {
    data = static_cast<uint8_t*>((*ptr_)->GetBuffer());
    size = (*ptr_)->GetBufferSize();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  if (data == nullptr || size == 0) return EmptyBuffer(env);
  return ZeroCopyBuffer(env, data, size, GrabPtr(ptr_));
}

Napi::Value GrabResultWrap::GetArray(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value buf = GetBuffer(info);
  if (env.IsExceptionPending() || !buf.IsObject()) return buf;
  Napi::Object o = buf.As<Napi::Object>();
  Napi::ArrayBuffer ab = o.Get("buffer").As<Napi::ArrayBuffer>();
  size_t off = o.Get("byteOffset").As<Napi::Number>().Uint32Value();
  size_t len = o.Get("length").As<Napi::Number>().Uint32Value();
  return Napi::Uint8Array::New(env, len, ab, off);
}

Napi::Value GrabResultWrap::GetMemoryView(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value buf = GetBuffer(info);
  if (env.IsExceptionPending() || !buf.IsObject()) return buf;
  Napi::Object o = buf.As<Napi::Object>();
  Napi::ArrayBuffer ab = o.Get("buffer").As<Napi::ArrayBuffer>();
  size_t off = o.Get("byteOffset").As<Napi::Number>().Uint32Value();
  size_t len = o.Get("length").As<Napi::Number>().Uint32Value();
  return Napi::DataView::New(env, ab, off, len);
}

Napi::Value GrabResultWrap::GetDataComponentCount(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t n = 0;
  try {
    if (ValidGrab(*ptr_)) n = (*ptr_)->GetDataComponentCount();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Number::New(env, static_cast<double>(n));
}

Napi::Value GrabResultWrap::GetDataComponent(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "getDataComponent(index: number) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  const size_t idx = info[0].As<Napi::Number>().Uint32Value();
  try {
    if (!ValidGrab(*ptr_)) {
      Napi::Error::New(env, "GrabResult is not valid")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Pylon::CPylonDataContainer container(*ptr_);
    if (idx >= container.GetDataComponentCount()) {
      Napi::RangeError::New(env, "Data component index out of range")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    const Pylon::CPylonDataComponent comp = container.GetDataComponent(idx);
    Napi::Object o = Napi::Object::New(env);
    o.Set("index", Napi::Number::New(env, static_cast<double>(idx)));
    o.Set("componentIndex",
          Napi::Number::New(
              env, static_cast<double>(comp.GetComponentIndex())));
    const auto ct = comp.GetComponentType();
    o.Set("componentType", Napi::Number::New(env, static_cast<double>(ct)));
    const char* ctName = "Unknown";
    switch (ct) {
      case Pylon::ComponentType_Intensity:
        ctName = "Intensity";
        break;
      case Pylon::ComponentType_Range:
        ctName = "Range";
        break;
      case Pylon::ComponentType_Confidence:
        ctName = "Confidence";
        break;
      case Pylon::ComponentType_Metadata:
        ctName = "Metadata";
        break;
      case Pylon::ComponentType_Infrared:
        ctName = "Infrared";
        break;
      case Pylon::ComponentType_Reflectance:
        ctName = "Reflectance";
        break;
      case Pylon::ComponentType_Disparity:
        ctName = "Disparity";
        break;
      default:
        break;
    }
    o.Set("componentTypeName", ctName);
    o.Set("width",
          Napi::Number::New(env, static_cast<double>(comp.GetWidth())));
    o.Set("height",
          Napi::Number::New(env, static_cast<double>(comp.GetHeight())));
    o.Set("offsetX",
          Napi::Number::New(env, static_cast<double>(comp.GetOffsetX())));
    o.Set("offsetY",
          Napi::Number::New(env, static_cast<double>(comp.GetOffsetY())));
    o.Set("pixelType", std::string(PixelTypeName(comp.GetPixelType())));
    o.Set("pixelTypeValue",
          Napi::Number::New(env, static_cast<double>(comp.GetPixelType())));
    o.Set("dataSize",
          Napi::Number::New(env, static_cast<double>(comp.GetDataSize())));
    o.Set("timestamp",
          Napi::Number::New(env, static_cast<double>(comp.GetTimeStamp())));
    o.Set("sourceId",
          Napi::Number::New(env, static_cast<double>(comp.GetSourceId())));
    return o;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value GrabResultWrap::GetDataComponentBuffer(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env,
                          "getDataComponentBuffer(index: number) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  const size_t idx = info[0].As<Napi::Number>().Uint32Value();
  try {
    if (!ValidGrab(*ptr_)) return EmptyBuffer(env);
    // Keep the container alive with the view: it references the grab.
    auto container =
        std::make_shared<Pylon::CPylonDataContainer>(*ptr_);
    if (idx >= container->GetDataComponentCount()) {
      Napi::RangeError::New(env, "Data component index out of range")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    const Pylon::CPylonDataComponent comp = container->GetDataComponent(idx);
    uint8_t* data = static_cast<uint8_t*>(
        const_cast<void*>(comp.GetData()));
    const size_t size = comp.GetDataSize();
    if (data == nullptr || size == 0) return EmptyBuffer(env);
    return ZeroCopyBuffer(env, data, size, container);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value GrabResultWrap::ToObject(const Napi::CallbackInfo& info) {
  return BuildGrabFields(info.Env(), *ptr_);
}

}  // namespace node_pylon
