// Image pipeline N-API wrappers: ImageFormatConverter, ImageDecompressor
// and the pixel-type / payload-type / SFNC helpers shared with the facade.
// Real pylon SDK only; DataProcessing recipes stay out of scope.
#pragma once

#include <napi.h>

#include <pylon/PylonIncludes.h>

namespace node_pylon {

class ImageFormatConverterWrap
    : public Napi::ObjectWrap<ImageFormatConverterWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit ImageFormatConverterWrap(const Napi::CallbackInfo& info);

 private:
  // Configuration (mirrors CImageFormatConverter node map properties).
  Napi::Value SetOutputPixelFormat(const Napi::CallbackInfo& info);
  Napi::Value GetOutputPixelFormat(const Napi::CallbackInfo& info);
  Napi::Value SetOutputBitAlignment(const Napi::CallbackInfo& info);
  Napi::Value GetOutputBitAlignment(const Napi::CallbackInfo& info);
  // convert(destination: PylonImage, source: PylonImage|GrabResult)
  Napi::Value Convert(const Napi::CallbackInfo& info);
  // getBufferSizeForConversion(source: PylonImage|GrabResult)
  Napi::Value GetBufferSizeForConversion(const Napi::CallbackInfo& info);
  // imageHasDestinationFormat(source: PylonImage|GrabResult)
  Napi::Value ImageHasDestinationFormat(const Napi::CallbackInfo& info);
  static Napi::Value IsSupportedInputFormat(const Napi::CallbackInfo& info);
  static Napi::Value IsSupportedOutputFormat(const Napi::CallbackInfo& info);

  Pylon::CImageFormatConverter converter_;
};

class ImageDecompressorWrap
    : public Napi::ObjectWrap<ImageDecompressorWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit ImageDecompressorWrap(const Napi::CallbackInfo& info);

 private:
  // describe(source: PylonImage|GrabResult) -> compression info object.
  Napi::Value Describe(const Napi::CallbackInfo& info);
  // decompress(destination: PylonImage, source: PylonImage|GrabResult)
  Napi::Value Decompress(const Napi::CallbackInfo& info);
  static Napi::Value GetCompressionMode(const Napi::CallbackInfo& info);

  Pylon::CImageDecompressor decompressor_;
};

// Module-level helpers registered on exports by InitImagePipeline().
void InitImagePipeline(Napi::Env env, Napi::Object exports);

}  // namespace node_pylon
