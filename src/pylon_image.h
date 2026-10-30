// PylonImage N-API wrapper over the real Basler pylon SDK.
// Mirrors pypylon PylonImage semantics: Create/IsValid/Release/Copy,
// geometry/pixel accessors, zero-copy buffer views, Save/Load persistence.
// DataProcessing recipes stay out of scope.
#pragma once

#include <memory>

#include <napi.h>

#include <pylon/PylonIncludes.h>

#include "grab_result.h"

namespace node_pylon {

using ImagePtr = std::shared_ptr<Pylon::CPylonImage>;

class PylonImageWrap : public Napi::ObjectWrap<PylonImageWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit PylonImageWrap(const Napi::CallbackInfo& info);
  ~PylonImageWrap() override = default;

  // Internal attach used by converter/decompressor/load paths.
  void Attach(Pylon::CPylonImage img);
  Pylon::CPylonImage& Image() { return *img_; }

  // Wrap an existing image into a new JS object.
  static Napi::Object FromImage(Napi::Env env, Pylon::CPylonImage img);

 private:
  // create(pixelType: string|number, width, height, [paddingX]) -> PylonImage
  static Napi::Value Create(const Napi::CallbackInfo& info);
  Napi::Value IsValid(const Napi::CallbackInfo& info);
  Napi::Value IsUnique(const Napi::CallbackInfo& info);
  Napi::Value Release(const Napi::CallbackInfo& info);
  // copyImage(source: PylonImage|GrabResult)
  Napi::Value CopyImage(const Napi::CallbackInfo& info);
  // attachGrabResult(grab: GrabResult)
  Napi::Value AttachGrabResult(const Napi::CallbackInfo& info);
  Napi::Value GetWidth(const Napi::CallbackInfo& info);
  Napi::Value GetHeight(const Napi::CallbackInfo& info);
  Napi::Value GetPaddingX(const Napi::CallbackInfo& info);
  Napi::Value GetOrientation(const Napi::CallbackInfo& info);
  Napi::Value GetPixelType(const Napi::CallbackInfo& info);
  Napi::Value GetPixelTypeValue(const Napi::CallbackInfo& info);
  Napi::Value GetImageSize(const Napi::CallbackInfo& info);
  Napi::Value GetAllocatedBufferSize(const Napi::CallbackInfo& info);
  Napi::Value GetStride(const Napi::CallbackInfo& info);
  Napi::Value IsSupportedPixelType(const Napi::CallbackInfo& info);
  // Zero-copy views over the image buffer.
  Napi::Value GetBuffer(const Napi::CallbackInfo& info);
  Napi::Value GetArray(const Napi::CallbackInfo& info);
  Napi::Value GetMemoryView(const Napi::CallbackInfo& info);
  // save(format, filename) / load(filename); formats: Bmp/Tiff/Jpeg/Png/Raw.
  Napi::Value Save(const Napi::CallbackInfo& info);
  Napi::Value Load(const Napi::CallbackInfo& info);
  Napi::Value CanSaveWithoutConversion(const Napi::CallbackInfo& info);
  Napi::Value ToObject(const Napi::CallbackInfo& info);

  ImagePtr img_;
};

// Parse "Mono8"/"RGB8"/... or a numeric EPixelType into EPixelType.
// Returns false (and throws a JS TypeError) for unknown names.
bool ParsePixelType(Napi::Env env, const Napi::Value& v,
                    Pylon::EPixelType* out);

}  // namespace node_pylon
