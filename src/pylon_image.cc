// PylonImage N-API wrapper: implementation (real pylon SDK only).
#include "pylon_image.h"

#include <pylon/ImagePersistence.h>
#include <pylon/PixelTypeMapper.h>

namespace node_pylon {

Napi::FunctionReference PylonImageWrap::constructor;

namespace {

template <typename Owner>
Napi::Value ZeroCopyImageBuffer(Napi::Env env, uint8_t* data, size_t size,
                                Owner owner) {
  using Holder = Owner;
  auto* holder = new Holder(std::move(owner));
  return Napi::Buffer<uint8_t>::New(
      env, data, size, [](Napi::Env /*env*/, uint8_t* /*p*/, Holder* h) {
        delete h;
      }, holder);
}

Pylon::EImageFileFormat ParseFileFormat(const std::string& name, bool* ok) {
  *ok = true;
  if (name == "Bmp" || name == "bmp") return Pylon::ImageFileFormat_Bmp;
  if (name == "Tiff" || name == "tiff") return Pylon::ImageFileFormat_Tiff;
  if (name == "Jpeg" || name == "jpeg" || name == "Jpg" || name == "jpg") {
    return Pylon::ImageFileFormat_Jpeg;
  }
  if (name == "Png" || name == "png") return Pylon::ImageFileFormat_Png;
  if (name == "Raw" || name == "raw") return Pylon::ImageFileFormat_Raw;
  if (name == "Dng" || name == "dng") return Pylon::ImageFileFormat_Dng;
  *ok = false;
  return Pylon::ImageFileFormat_Bmp;
}

}  // namespace

bool ParsePixelType(Napi::Env env, const Napi::Value& v,
                    Pylon::EPixelType* out) {
  if (v.IsNumber()) {
    *out = static_cast<Pylon::EPixelType>(
        v.As<Napi::Number>().Int32Value());
    return true;
  }
  if (!v.IsString()) {
    Napi::TypeError::New(env, "pixelType must be a name string or a number")
        .ThrowAsJavaScriptException();
    return false;
  }
  const std::string name = v.As<Napi::String>().Utf8Value();
  // Canonical SFNC names first (covers every enum value), then the short
  // aliases used across the facade and node-pylon tests.
  Pylon::EPixelType t = Pylon::PixelType_Undefined;
  try {
    t = Pylon::CPixelTypeMapper::GetPylonPixelTypeByName(name.c_str());
  } catch (...) {
    t = Pylon::PixelType_Undefined;
  }
  if (t != Pylon::PixelType_Undefined) {
    *out = t;
    return true;
  }
  if (name == "Mono8") t = Pylon::PixelType_Mono8;
  else if (name == "Mono8s") t = Pylon::PixelType_Mono8signed;
  else if (name == "Mono10") t = Pylon::PixelType_Mono10;
  else if (name == "Mono10packed") t = Pylon::PixelType_Mono10packed;
  else if (name == "Mono10p") t = Pylon::PixelType_Mono10p;
  else if (name == "Mono12") t = Pylon::PixelType_Mono12;
  else if (name == "Mono12packed") t = Pylon::PixelType_Mono12packed;
  else if (name == "Mono12p") t = Pylon::PixelType_Mono12p;
  else if (name == "Mono16") t = Pylon::PixelType_Mono16;
  else if (name == "RGB8") t = Pylon::PixelType_RGB8packed;
  else if (name == "BGR8") t = Pylon::PixelType_BGR8packed;
  else if (name == "RGBa8") t = Pylon::PixelType_RGBA8packed;
  else if (name == "BGRa8") t = Pylon::PixelType_BGRA8packed;
  else {
    Napi::TypeError::New(env, "Unknown pixel type: " + name)
        .ThrowAsJavaScriptException();
    return false;
  }
  *out = t;
  return true;
}

void PylonImageWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "PylonImage",
      {StaticMethod("create", &PylonImageWrap::Create),
       InstanceMethod("isValid", &PylonImageWrap::IsValid),
       InstanceMethod("isUnique", &PylonImageWrap::IsUnique),
       InstanceMethod("release", &PylonImageWrap::Release),
       InstanceMethod("copyImage", &PylonImageWrap::CopyImage),
       InstanceMethod("attachGrabResult",
                      &PylonImageWrap::AttachGrabResult),
       InstanceMethod("getWidth", &PylonImageWrap::GetWidth),
       InstanceMethod("getHeight", &PylonImageWrap::GetHeight),
       InstanceMethod("getPaddingX", &PylonImageWrap::GetPaddingX),
       InstanceMethod("getOrientation", &PylonImageWrap::GetOrientation),
       InstanceMethod("getPixelType", &PylonImageWrap::GetPixelType),
       InstanceMethod("getPixelTypeValue",
                      &PylonImageWrap::GetPixelTypeValue),
       InstanceMethod("getImageSize", &PylonImageWrap::GetImageSize),
       InstanceMethod("getAllocatedBufferSize",
                      &PylonImageWrap::GetAllocatedBufferSize),
       InstanceMethod("getStride", &PylonImageWrap::GetStride),
       InstanceMethod("isSupportedPixelType",
                      &PylonImageWrap::IsSupportedPixelType),
       InstanceMethod("getBuffer", &PylonImageWrap::GetBuffer),
       InstanceMethod("getArray", &PylonImageWrap::GetArray),
       InstanceMethod("getMemoryView", &PylonImageWrap::GetMemoryView),
       InstanceMethod("save", &PylonImageWrap::Save),
       InstanceMethod("load", &PylonImageWrap::Load),
       InstanceMethod("canSaveWithoutConversion",
                      &PylonImageWrap::CanSaveWithoutConversion),
       InstanceMethod("toObject", &PylonImageWrap::ToObject)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("PylonImage", ctor);
}

PylonImageWrap::PylonImageWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PylonImageWrap>(info),
      img_(std::make_shared<Pylon::CPylonImage>()) {}

void PylonImageWrap::Attach(Pylon::CPylonImage img) { *img_ = img; }

Napi::Object PylonImageWrap::FromImage(Napi::Env env,
                                       Pylon::CPylonImage img) {
  Napi::Object obj = constructor.New({});
  PylonImageWrap* w = Napi::ObjectWrap<PylonImageWrap>::Unwrap(obj);
  w->Attach(img);
  return obj;
}

Napi::Value PylonImageWrap::Create(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 3 || info[1].IsEmpty() || info[2].IsEmpty() ||
      !info[1].IsNumber() || !info[2].IsNumber()) {
    Napi::TypeError::New(
        env, "create(pixelType, width, height, [paddingX]) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  if (!ParsePixelType(env, info[0], &pt) || env.IsExceptionPending()) {
    return env.Undefined();
  }
  const uint32_t w = info[1].As<Napi::Number>().Uint32Value();
  const uint32_t h = info[2].As<Napi::Number>().Uint32Value();
  size_t padX = 0;
  if (info.Length() >= 4 && !info[3].IsUndefined()) {
    if (!info[3].IsNumber()) {
      Napi::TypeError::New(env, "paddingX must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    padX = info[3].As<Napi::Number>().Uint32Value();
  }
  try {
    Pylon::CPylonImage img =
        Pylon::CPylonImage::Create(pt, w, h, padX);
    return FromImage(env, img);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value PylonImageWrap::IsValid(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    v = img_->IsValid();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}
Napi::Value PylonImageWrap::IsUnique(const Napi::CallbackInfo& info) {
  bool v = false;
  try {
    v = img_->IsUnique();
  } catch (...) {
  }
  return Napi::Boolean::New(info.Env(), v);
}
Napi::Value PylonImageWrap::Release(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    img_->Release();
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  }
  return env.Undefined();
}

Napi::Value PylonImageWrap::CopyImage(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || info[0].IsEmpty() || !info[0].IsObject()) {
    Napi::TypeError::New(env, "copyImage(source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Napi::Object src = info[0].As<Napi::Object>();
    if (IsNativeInstance(src, PylonImageWrap::constructor)) {
      auto* w = Napi::ObjectWrap<PylonImageWrap>::Unwrap(src);
      img_->CopyImage(w->Image());
    } else if (IsNativeInstance(src, GrabResultWrap::constructor)) {
      auto* g = Napi::ObjectWrap<GrabResultWrap>::Unwrap(src);
      if (!g->Ptr().IsValid()) {
        Napi::Error::New(env, "GrabResult is not valid")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      // CGrabResultData is not an IImage; share its buffer through a
      // temporary PylonImage and copy from there (no extra pixel copy:
      // AttachGrabResultBuffer references the grab buffer).
      Pylon::CPylonImage adapted;
      adapted.AttachGrabResultBuffer(g->Ptr());
      img_->CopyImage(adapted);
    } else {
      Napi::TypeError::New(env, "copyImage(source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value PylonImageWrap::AttachGrabResult(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "attachGrabResult(grab: GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::Object arg = info[0].As<Napi::Object>();
  if (!IsNativeInstance(arg, GrabResultWrap::constructor)) {
    Napi::TypeError::New(env, "attachGrabResult(grab: GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    auto* g = Napi::ObjectWrap<GrabResultWrap>::Unwrap(arg);
    if (!g->Ptr().IsValid()) {
      Napi::Error::New(env, "GrabResult is not valid")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    img_->AttachGrabResultBuffer(g->Ptr());
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value PylonImageWrap::GetWidth(const Napi::CallbackInfo& info) {
  double v = 0;
  try {
    v = static_cast<double>(img_->GetWidth());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetHeight(const Napi::CallbackInfo& info) {
  double v = 0;
  try {
    v = static_cast<double>(img_->GetHeight());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetPaddingX(const Napi::CallbackInfo& info) {
  double v = 0;
  try {
    v = static_cast<double>(img_->GetPaddingX());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetOrientation(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    return Napi::String::New(
        env, img_->GetOrientation() == Pylon::ImageOrientation_TopDown
                 ? "TopDown"
                 : "BottomUp");
  } catch (...) {
    return Napi::String::New(env, "TopDown");
  }
}
Napi::Value PylonImageWrap::GetPixelType(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType t = Pylon::PixelType_Undefined;
  try {
    t = img_->GetPixelType();
  } catch (...) {
  }
  return Napi::String::New(env, PixelTypeName(t));
}
Napi::Value PylonImageWrap::GetPixelTypeValue(
    const Napi::CallbackInfo& info) {
  double v = static_cast<double>(Pylon::PixelType_Undefined);
  try {
    v = static_cast<double>(img_->GetPixelType());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetImageSize(const Napi::CallbackInfo& info) {
  double v = 0;
  try {
    v = static_cast<double>(img_->GetImageSize());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetAllocatedBufferSize(
    const Napi::CallbackInfo& info) {
  double v = 0;
  try {
    v = static_cast<double>(img_->GetAllocatedBufferSize());
  } catch (...) {
  }
  return Napi::Number::New(info.Env(), v);
}
Napi::Value PylonImageWrap::GetStride(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  size_t stride = 0;
  try {
    if (!img_->GetStride(stride)) return env.Null();
  } catch (...) {
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(stride));
}
Napi::Value PylonImageWrap::IsSupportedPixelType(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  if (!ParsePixelType(env, info[0], &pt) || env.IsExceptionPending()) {
    return env.Undefined();
  }
  bool v = false;
  try {
    v = img_->IsSupportedPixelType(pt);
  } catch (...) {
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value PylonImageWrap::GetBuffer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (!img_->IsValid()) {
      return Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(0));
    }
    uint8_t* data = static_cast<uint8_t*>(img_->GetBuffer());
    const size_t size = img_->GetImageSize();
    if (data == nullptr || size == 0) {
      return Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(0));
    }
    return ZeroCopyImageBuffer(env, data, size, ImagePtr(img_));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
Napi::Value PylonImageWrap::GetArray(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value buf = GetBuffer(info);
  if (env.IsExceptionPending() || !buf.IsObject()) return buf;
  Napi::Object o = buf.As<Napi::Object>();
  Napi::ArrayBuffer ab = o.Get("buffer").As<Napi::ArrayBuffer>();
  size_t off = o.Get("byteOffset").As<Napi::Number>().Uint32Value();
  size_t len = o.Get("length").As<Napi::Number>().Uint32Value();
  return Napi::Uint8Array::New(env, len, ab, off);
}
Napi::Value PylonImageWrap::GetMemoryView(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Value buf = GetBuffer(info);
  if (env.IsExceptionPending() || !buf.IsObject()) return buf;
  Napi::Object o = buf.As<Napi::Object>();
  Napi::ArrayBuffer ab = o.Get("buffer").As<Napi::ArrayBuffer>();
  size_t off = o.Get("byteOffset").As<Napi::Number>().Uint32Value();
  size_t len = o.Get("length").As<Napi::Number>().Uint32Value();
  return Napi::DataView::New(env, ab, off, len);
}

Napi::Value PylonImageWrap::Save(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
    Napi::TypeError::New(env, "save(format: string, filename: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  bool ok = false;
  Pylon::EImageFileFormat fmt =
      ParseFileFormat(info[0].As<Napi::String>().Utf8Value(), &ok);
  if (!ok) {
    Napi::TypeError::New(env, "Unknown image format (Bmp/Tiff/Jpeg/Png/Raw/Dng)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  const std::string filename = info[1].As<Napi::String>().Utf8Value();
  try {
    Pylon::CImagePersistence::Save(fmt, Pylon::String_t(filename.c_str()),
                                  *img_);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value PylonImageWrap::Load(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "load(filename: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  const std::string filename = info[0].As<Napi::String>().Utf8Value();
  try {
    Pylon::CImagePersistence::Load(Pylon::String_t(filename.c_str()), *img_);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value PylonImageWrap::CanSaveWithoutConversion(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "canSaveWithoutConversion(format: string)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  bool ok = false;
  Pylon::EImageFileFormat fmt =
      ParseFileFormat(info[0].As<Napi::String>().Utf8Value(), &ok);
  if (!ok) {
    Napi::TypeError::New(env, "Unknown image format (Bmp/Tiff/Jpeg/Png/Raw/Dng)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  bool v = false;
  try {
    v = Pylon::CImagePersistence::CanSaveWithoutConversion(fmt, *img_);
  } catch (...) {
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value PylonImageWrap::ToObject(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Object o = Napi::Object::New(env);
  o.Set("width", GetWidth(info));
  o.Set("height", GetHeight(info));
  o.Set("pixelType", GetPixelType(info));
  o.Set("pixelTypeValue", GetPixelTypeValue(info));
  o.Set("imageSize", GetImageSize(info));
  o.Set("paddingX", GetPaddingX(info));
  o.Set("orientation", GetOrientation(info));
  o.Set("valid", IsValid(info));
  o.Set("buffer", GetBuffer(info));
  return o;
}

}  // namespace node_pylon
