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
};

class StaticDefectPixelVectorWrap
    : public Napi::ObjectWrap<StaticDefectPixelVectorWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit StaticDefectPixelVectorWrap(const Napi::CallbackInfo& info);

  Pylon::StaticDefectPixelList_t list_;
};

void Init(Napi::Env env, Napi::Object exports);

}  // namespace defect
}  // namespace node_pylon
