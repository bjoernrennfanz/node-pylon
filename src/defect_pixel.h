// StaticDefectPixelCorrection N-API wrapper (pylon infra API for ace 2).
// Exposes Pylon::CStaticDefectPixelCorrection Get/Set/NormalizePixelList
// against the open camera's nodemap, plus the StaticDefectPixel struct,
// the StaticDefectPixelVector list container and the ListType constants.
// Pixel entries cross the boundary as [x, y] / [x, y, type] arrays or
// {x, y, type?} objects; getters return [success, vector] pairs like
// pypylon. SDK failures (e.g. camera without user-list support) surface as
// translated GenICam errors (usually RuntimeException).
#pragma once

#include <napi.h>

#include <pylon/PylonIncludes.h>
#include <pylon/StaticDefectPixel.h>
#include <pylon/StaticDefectPixelCorrection.h>

namespace node_pylon {
namespace defect {

// JS object ({x,y,type} or StaticDefectPixel instance) -> native pixel.
// Throws a pending JS TypeError/RangeError on invalid entries.
bool ParsePixel(Napi::Env env, const Napi::Value& value,
                Pylon::StaticDefectPixel* out);

// Native pixel -> plain {x, y, type} object.
Napi::Object PixelToJs(Napi::Env env, const Pylon::StaticDefectPixel& pixel);

class StaticDefectPixelWrap : public Napi::ObjectWrap<StaticDefectPixelWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // StaticDefectPixel() zeros; StaticDefectPixel(x, y, type = 0).
  explicit StaticDefectPixelWrap(const Napi::CallbackInfo& info);

  Pylon::StaticDefectPixel pixel_{0, 0,
                                  Pylon::StaticDefectPixelType_Reserved};

 private:
  Napi::Value GetX(const Napi::CallbackInfo& info);
  void SetX(const Napi::CallbackInfo& info, const Napi::Value& value);
  Napi::Value GetY(const Napi::CallbackInfo& info);
  void SetY(const Napi::CallbackInfo& info, const Napi::Value& value);
  Napi::Value GetPixelType(const Napi::CallbackInfo& info);
  void SetPixelType(const Napi::CallbackInfo& info, const Napi::Value& value);
};

class StaticDefectPixelVectorWrap
    : public Napi::ObjectWrap<StaticDefectPixelVectorWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit StaticDefectPixelVectorWrap(const Napi::CallbackInfo& info);

  Pylon::StaticDefectPixelList_t list_;

 private:
  Napi::Value GetLength(const Napi::CallbackInfo& info);
  void SetLength(const Napi::CallbackInfo& info, const Napi::Value& value);
  Napi::Value Size(const Napi::CallbackInfo& info);
  Napi::Value Capacity(const Napi::CallbackInfo& info);
  Napi::Value Empty(const Napi::CallbackInfo& info);
  Napi::Value Reserve(const Napi::CallbackInfo& info);
  Napi::Value Resize(const Napi::CallbackInfo& info);
  Napi::Value Clear(const Napi::CallbackInfo& info);
  Napi::Value Swap(const Napi::CallbackInfo& info);
  Napi::Value At(const Napi::CallbackInfo& info);
  Napi::Value Push(const Napi::CallbackInfo& info);
  Napi::Value ToArray(const Napi::CallbackInfo& info);
  Napi::Value Iterator(const Napi::CallbackInfo& info);
};

void Init(Napi::Env env, Napi::Object exports);

}  // namespace defect
}  // namespace node_pylon
