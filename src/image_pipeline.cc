// Image pipeline N-API wrappers: implementation (real pylon SDK only).
#include "image_pipeline.h"

#include <pylon/ImageDecompressor.h>
#include <pylon/ImagePersistence.h>
#include <pylon/PixelTypeMapper.h>

#include "grab_result.h"
#include "pylon_image.h"

namespace node_pylon {

Napi::FunctionReference ImageFormatConverterWrap::constructor;
Napi::FunctionReference ImageDecompressorWrap::constructor;

namespace {

// Resolve a converter source argument into an IImage pointer.
// Grab results are not IImages in the SDK; they are shared through `adapted`
// (AttachGrabResultBuffer references the grab buffer, no pixel copy), which
// must outlive the call. Returns nullptr for anything else.
const Pylon::IImage* ResolveImage(Napi::Env env, const Napi::Value& v,
                                  Pylon::CPylonImage* adapted) {
  if (!v.IsObject()) return nullptr;
  Napi::Object o = v.As<Napi::Object>();
  if (IsNativeInstance(o, PylonImageWrap::constructor)) {
    return &Napi::ObjectWrap<PylonImageWrap>::Unwrap(o)->Image();
  }
  if (IsNativeInstance(o, GrabResultWrap::constructor)) {
    auto* g = Napi::ObjectWrap<GrabResultWrap>::Unwrap(o);
    if (!g->Ptr().IsValid()) return nullptr;
    adapted->AttachGrabResultBuffer(g->Ptr());
    return adapted;
  }
  return nullptr;
}

}  // namespace

void ImageFormatConverterWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "ImageFormatConverter",
      {InstanceMethod("setOutputPixelFormat",
                      &ImageFormatConverterWrap::SetOutputPixelFormat),
       InstanceMethod("getOutputPixelFormat",
                      &ImageFormatConverterWrap::GetOutputPixelFormat),
       InstanceMethod("setOutputBitAlignment",
                      &ImageFormatConverterWrap::SetOutputBitAlignment),
       InstanceMethod("getOutputBitAlignment",
                      &ImageFormatConverterWrap::GetOutputBitAlignment),
       InstanceMethod("convert", &ImageFormatConverterWrap::Convert),
       InstanceMethod("getBufferSizeForConversion",
                      &ImageFormatConverterWrap::GetBufferSizeForConversion),
       InstanceMethod("imageHasDestinationFormat",
                      &ImageFormatConverterWrap::ImageHasDestinationFormat),
       StaticMethod("isSupportedInputFormat",
                    &ImageFormatConverterWrap::IsSupportedInputFormat),
       StaticMethod("isSupportedOutputFormat",
                    &ImageFormatConverterWrap::IsSupportedOutputFormat)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("ImageFormatConverter", ctor);
}

ImageFormatConverterWrap::ImageFormatConverterWrap(
    const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ImageFormatConverterWrap>(info) {}

Napi::Value ImageFormatConverterWrap::SetOutputPixelFormat(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  if (info.Length() < 1 || !ParsePixelType(env, info[0], &pt) ||
      env.IsExceptionPending()) {
    if (!env.IsExceptionPending()) {
      Napi::TypeError::New(env, "setOutputPixelFormat(pixelType)").ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }
  try {
    converter_.OutputPixelFormat.SetValue(pt);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  }
  return env.Undefined();
}

Napi::Value ImageFormatConverterWrap::GetOutputPixelFormat(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    return Napi::String::New(
        env, PixelTypeName(converter_.OutputPixelFormat.GetValue()));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageFormatConverterWrap::SetOutputBitAlignment(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "setOutputBitAlignment('LsbAligned'|'MsbAligned')")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  const std::string v = info[0].As<Napi::String>().Utf8Value();
  try {
    if (v == "LsbAligned") {
      converter_.OutputBitAlignment.SetValue(
          Basler_ImageFormatConverterParams::OutputBitAlignment_LsbAligned);
    } else if (v == "MsbAligned") {
      converter_.OutputBitAlignment.SetValue(
          Basler_ImageFormatConverterParams::OutputBitAlignment_MsbAligned);
    } else {
      Napi::TypeError::New(env, "Unknown bit alignment: " + v)
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
  }
  return env.Undefined();
}

Napi::Value ImageFormatConverterWrap::GetOutputBitAlignment(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    auto v = converter_.OutputBitAlignment.GetValue();
    return Napi::String::New(
        env, v == Basler_ImageFormatConverterParams::
                      OutputBitAlignment_MsbAligned
                 ? "MsbAligned"
                 : "LsbAligned");
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageFormatConverterWrap::Convert(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
    Napi::TypeError::New(env, "convert(destination: PylonImage, source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Napi::Object dstArg = info[0].As<Napi::Object>();
    if (!IsNativeInstance(dstArg, PylonImageWrap::constructor)) {
      Napi::TypeError::New(env, "convert(destination: PylonImage, source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto* dst = Napi::ObjectWrap<PylonImageWrap>::Unwrap(dstArg);
    Pylon::CPylonImage adapted;
    const Pylon::IImage* src = ResolveImage(env, info[1], &adapted);
    if (src == nullptr) {
      Napi::TypeError::New(env, "convert(destination: PylonImage, source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    converter_.Convert(dst->Image(), *src);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value ImageFormatConverterWrap::GetBufferSizeForConversion(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "getBufferSizeForConversion(source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Pylon::CPylonImage adapted;
    const Pylon::IImage* src = ResolveImage(env, info[0], &adapted);
    if (src == nullptr) {
      Napi::TypeError::New(env, "getBufferSizeForConversion(source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Napi::Number::New(
        env, static_cast<double>(converter_.GetBufferSizeForConversion(*src)));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageFormatConverterWrap::ImageHasDestinationFormat(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "imageHasDestinationFormat(source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Pylon::CPylonImage adapted;
    const Pylon::IImage* src = ResolveImage(env, info[0], &adapted);
    if (src == nullptr) {
      Napi::TypeError::New(env, "imageHasDestinationFormat(source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Napi::Boolean::New(
        env, converter_.ImageHasDestinationFormat(*src));
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageFormatConverterWrap::IsSupportedInputFormat(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  if (info.Length() < 1 || !ParsePixelType(env, info[0], &pt) ||
      env.IsExceptionPending()) {
    if (!env.IsExceptionPending()) {
      Napi::TypeError::New(env, "pixelType expected").ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }
  bool v = false;
  try {
    v = Pylon::CImageFormatConverter::IsSupportedInputFormat(pt);
  } catch (...) {
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value ImageFormatConverterWrap::IsSupportedOutputFormat(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Pylon::EPixelType pt = Pylon::PixelType_Undefined;
  if (info.Length() < 1 || !ParsePixelType(env, info[0], &pt) ||
      env.IsExceptionPending()) {
    if (!env.IsExceptionPending()) {
      Napi::TypeError::New(env, "pixelType expected").ThrowAsJavaScriptException();
    }
    return env.Undefined();
  }
  bool v = false;
  try {
    v = Pylon::CImageFormatConverter::IsSupportedOutputFormat(pt);
  } catch (...) {
  }
  return Napi::Boolean::New(env, v);
}

void ImageDecompressorWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "ImageDecompressor",
      {InstanceMethod("describe", &ImageDecompressorWrap::Describe),
       InstanceMethod("decompress", &ImageDecompressorWrap::Decompress),
       StaticMethod("getCompressionMode",
                    &ImageDecompressorWrap::GetCompressionMode)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("ImageDecompressor", ctor);
}

ImageDecompressorWrap::ImageDecompressorWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ImageDecompressorWrap>(info) {}

Napi::Value ImageDecompressorWrap::Describe(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "describe(source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Napi::Object arg = info[0].As<Napi::Object>();
    const bool isGrab = IsNativeInstance(arg, GrabResultWrap::constructor);
    const bool isImage = IsNativeInstance(arg, PylonImageWrap::constructor);
    if (!isGrab && !isImage) {
      Napi::TypeError::New(env, "describe(source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Pylon::CompressionInfo_t ci;
    bool has = false;
    if (isGrab) {
      auto* g = Napi::ObjectWrap<GrabResultWrap>::Unwrap(arg);
      if (!g->Ptr().IsValid()) {
        Napi::Error::New(env, "GrabResult is not valid")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      has = Pylon::CImageDecompressor::GetCompressionInfo(ci, g->Ptr());
    } else {
      auto* w = Napi::ObjectWrap<PylonImageWrap>::Unwrap(arg);
      if (!w->Image().IsValid()) {
        Napi::Error::New(env, "PylonImage is not valid")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      has = Pylon::CImageDecompressor::GetCompressionInfo(
          ci, w->Image().GetBuffer(), w->Image().GetImageSize());
    }
    Napi::Object o = Napi::Object::New(env);
    o.Set("hasCompressedImage", has && ci.hasCompressedImage);
    o.Set("lossy", ci.lossy);
    o.Set("width", Napi::Number::New(env, static_cast<double>(ci.width)));
    o.Set("height", Napi::Number::New(env, static_cast<double>(ci.height)));
    o.Set("offsetX", Napi::Number::New(env, static_cast<double>(ci.offsetX)));
    o.Set("offsetY", Napi::Number::New(env, static_cast<double>(ci.offsetY)));
    o.Set("decompressedImageSize",
          Napi::Number::New(env, static_cast<double>(ci.decompressedImageSize)));
    o.Set("decompressedPayloadSize",
          Napi::Number::New(env, static_cast<double>(ci.decompressedPayloadSize)));
    o.Set("compressionStatus",
          Napi::Number::New(env, static_cast<double>(ci.compressionStatus)));
    return o;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageDecompressorWrap::Decompress(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject()) {
    Napi::TypeError::New(env, "decompress(destination: PylonImage, source: PylonImage|GrabResult)")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    Napi::Object dstArg = info[0].As<Napi::Object>();
    if (!IsNativeInstance(dstArg, PylonImageWrap::constructor)) {
      Napi::TypeError::New(env, "decompress(destination: PylonImage, source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto* dst = Napi::ObjectWrap<PylonImageWrap>::Unwrap(dstArg);
    Napi::Object arg = info[1].As<Napi::Object>();
    const bool isGrab = IsNativeInstance(arg, GrabResultWrap::constructor);
    const bool isImage = IsNativeInstance(arg, PylonImageWrap::constructor);
    if (!isGrab && !isImage) {
      Napi::TypeError::New(env, "decompress(destination: PylonImage, source: PylonImage|GrabResult)")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Pylon::CompressionInfo_t ci;
    if (isGrab) {
      auto* g = Napi::ObjectWrap<GrabResultWrap>::Unwrap(arg);
      if (!g->Ptr().IsValid()) {
        Napi::Error::New(env, "GrabResult is not valid")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      ci = decompressor_.DecompressImage(dst->Image(), g->Ptr());
    } else {
      auto* w = Napi::ObjectWrap<PylonImageWrap>::Unwrap(arg);
      if (!w->Image().IsValid()) {
        Napi::Error::New(env, "PylonImage is not valid")
            .ThrowAsJavaScriptException();
        return env.Undefined();
      }
      ci = decompressor_.DecompressImage(dst->Image(), w->Image().GetBuffer(),
                                         w->Image().GetImageSize());
    }
    Napi::Object o = Napi::Object::New(env);
    o.Set("hasCompressedImage", ci.hasCompressedImage);
    o.Set("width", Napi::Number::New(env, static_cast<double>(ci.width)));
    o.Set("height", Napi::Number::New(env, static_cast<double>(ci.height)));
    return o;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

Napi::Value ImageDecompressorWrap::GetCompressionMode(
    const Napi::CallbackInfo& info) {
  // Static probe without a camera nodemap: the emulator exposes no
  // compression, so report Off (matches CImageDecompressor::GetCompressionMode
  // on a nodemap with compression disabled).
  (void)info;
  return Napi::String::New(info.Env(), "Off");
}

void InitImagePipeline(Napi::Env env, Napi::Object exports) {
  ImageFormatConverterWrap::Init(env, exports);
  ImageDecompressorWrap::Init(env, exports);

  // PixelType enum values (numeric EPixelType, stable ABI).
  Napi::Object pixelType = Napi::Object::New(env);
  pixelType.Set("Undefined",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Undefined)));
  pixelType.Set("Mono8",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono8)));
  pixelType.Set("Mono8s", Napi::Number::New(
                              env, static_cast<double>(Pylon::PixelType_Mono8signed)));
  pixelType.Set("Mono10",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono10)));
  pixelType.Set("Mono10packed", Napi::Number::New(
                                    env, static_cast<double>(Pylon::PixelType_Mono10packed)));
  pixelType.Set("Mono10p",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono10p)));
  pixelType.Set("Mono12",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono12)));
  pixelType.Set("Mono12packed", Napi::Number::New(
                                    env, static_cast<double>(Pylon::PixelType_Mono12packed)));
  pixelType.Set("Mono12p",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono12p)));
  pixelType.Set("Mono16",
                Napi::Number::New(env, static_cast<double>(Pylon::PixelType_Mono16)));
  pixelType.Set("RGB8", Napi::Number::New(
                            env, static_cast<double>(Pylon::PixelType_RGB8packed)));
  pixelType.Set("BGR8", Napi::Number::New(
                            env, static_cast<double>(Pylon::PixelType_BGR8packed)));
  pixelType.Set("RGBa8", Napi::Number::New(
                             env, static_cast<double>(Pylon::PixelType_RGBA8packed)));
  pixelType.Set("BGRa8", Napi::Number::New(
                             env, static_cast<double>(Pylon::PixelType_BGRA8packed)));
  exports.Set("PixelType", pixelType);

  // PayloadType enum values.
  Napi::Object payloadType = Napi::Object::New(env);
  payloadType.Set("Undefined", Napi::Number::New(
                                   env, static_cast<double>(Pylon::PayloadType_Undefined)));
  payloadType.Set("Image",
                  Napi::Number::New(env, static_cast<double>(Pylon::PayloadType_Image)));
  payloadType.Set("RawData", Napi::Number::New(
                                 env, static_cast<double>(Pylon::PayloadType_RawData)));
  payloadType.Set("File",
                  Napi::Number::New(env, static_cast<double>(Pylon::PayloadType_File)));
  payloadType.Set("ChunkData", Napi::Number::New(
                                   env, static_cast<double>(Pylon::PayloadType_ChunkData)));
  payloadType.Set("GenDC",
                  Napi::Number::New(env, static_cast<double>(Pylon::PayloadType_GenDC)));
  exports.Set("PayloadType", payloadType);

  // Image file formats for save/load.
  Napi::Object fileFormat = Napi::Object::New(env);
  fileFormat.Set("Bmp", Napi::String::New(env, "Bmp"));
  fileFormat.Set("Tiff", Napi::String::New(env, "Tiff"));
  fileFormat.Set("Jpeg", Napi::String::New(env, "Jpeg"));
  fileFormat.Set("Png", Napi::String::New(env, "Png"));
  fileFormat.Set("Raw", Napi::String::New(env, "Raw"));
  fileFormat.Set("Dng", Napi::String::New(env, "Dng"));
  exports.Set("ImageFileFormat", fileFormat);

  // Pixel helpers: getPixelTypeByName(name) -> number, getNameByPixelType(n).
  exports.Set("getPixelTypeByName",
              Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() < 1 || !info[0].IsString()) {
                  Napi::TypeError::New(env, "getPixelTypeByName(name: string)")
                      .ThrowAsJavaScriptException();
                  return env.Undefined();
                }
                Pylon::EPixelType t = Pylon::PixelType_Undefined;
                if (!ParsePixelType(env, info[0], &t) || env.IsExceptionPending()) {
                  return env.Undefined();
                }
                return Napi::Number::New(env, static_cast<double>(t));
              }));
  exports.Set("getNameByPixelType",
              Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                if (info.Length() < 1 || !info[0].IsNumber()) {
                  Napi::TypeError::New(env, "getNameByPixelType(value: number)")
                      .ThrowAsJavaScriptException();
                  return env.Undefined();
                }
                auto t = static_cast<Pylon::EPixelType>(
                    info[0].As<Napi::Number>().Int32Value());
                return Napi::String::New(env, PixelTypeName(t));
              }));
  exports.Set("isMono",
              Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Pylon::EPixelType t = Pylon::PixelType_Undefined;
                if (info.Length() < 1 || !ParsePixelType(env, info[0], &t) ||
                    env.IsExceptionPending()) {
                  if (!env.IsExceptionPending()) {
                    Napi::TypeError::New(env, "pixelType expected")
                        .ThrowAsJavaScriptException();
                  }
                  return env.Undefined();
                }
                return Napi::Boolean::New(env, Pylon::IsMono(t));
              }));
  exports.Set("isColor",
              Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Pylon::EPixelType t = Pylon::PixelType_Undefined;
                if (info.Length() < 1 || !ParsePixelType(env, info[0], &t) ||
                    env.IsExceptionPending()) {
                  if (!env.IsExceptionPending()) {
                    Napi::TypeError::New(env, "pixelType expected")
                        .ThrowAsJavaScriptException();
                  }
                  return env.Undefined();
                }
                return Napi::Boolean::New(env, Pylon::IsColorImage(t));
              }));
  exports.Set("isPacked",
              Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                Pylon::EPixelType t = Pylon::PixelType_Undefined;
                if (info.Length() < 1 || !ParsePixelType(env, info[0], &t) ||
                    env.IsExceptionPending()) {
                  if (!env.IsExceptionPending()) {
                    Napi::TypeError::New(env, "pixelType expected")
                        .ThrowAsJavaScriptException();
                  }
                  return env.Undefined();
                }
                return Napi::Boolean::New(env, Pylon::IsPacked(t));
              }));
}

}  // namespace node_pylon
