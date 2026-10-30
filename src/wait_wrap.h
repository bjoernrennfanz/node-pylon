// WaitObject/WaitObjects N-API wrappers (pylon infra wait API).
// WaitObjectWrap holds a Pylon::WaitObject copy (the copy ctor duplicates
// the OS handle/fd, so the JS object stays usable while the camera lives).
// WaitObjectsWrap holds a Pylon::WaitObjects container and keeps persistent
// references to added JS wrappers so the underlying handles cannot vanish
// mid-wait. All waits are synchronous with millisecond timeouts, like
// pypylon; EWaitExResult codes surface as plain numbers (see the waitex_*
// constants registered in addon.cc).
#pragma once

#include <napi.h>

#include <vector>

#include <pylon/PylonIncludes.h>

namespace node_pylon {

class WaitObjectWrap : public Napi::ObjectWrap<WaitObjectWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: invalid object (like pylon.WaitObject()).
  explicit WaitObjectWrap(const Napi::CallbackInfo& info);

  // Adopts a copy of a native wait object into a fresh JS wrapper.
  static Napi::Object Adopt(Napi::Env env, const Pylon::WaitObject& obj);
  const Pylon::WaitObject& native() const { return obj_; }

 private:
  Napi::Value IsValid(const Napi::CallbackInfo& info);
  Napi::Value Wait(const Napi::CallbackInfo& info);
  Napi::Value WaitEx(const Napi::CallbackInfo& info);
  static Napi::Value Sleep(const Napi::CallbackInfo& info);

  Pylon::WaitObject obj_;
};

class WaitObjectsWrap : public Napi::ObjectWrap<WaitObjectsWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: empty container (like pylon.WaitObjects()).
  explicit WaitObjectsWrap(const Napi::CallbackInfo& info);

 private:
  Napi::Value Add(const Napi::CallbackInfo& info);
  Napi::Value RemoveAll(const Napi::CallbackInfo& info);
  Napi::Value WaitForAll(const Napi::CallbackInfo& info);
  Napi::Value WaitForAny(const Napi::CallbackInfo& info);
  Napi::Value WaitForAllEx(const Napi::CallbackInfo& info);
  Napi::Value WaitForAnyEx(const Napi::CallbackInfo& info);

  Pylon::WaitObjects container_;
  // Keeps added wrappers alive (handles must outlive the wait calls).
  std::vector<Napi::ObjectReference> refs_;
};

// Registers WaitObject/WaitObjects plus the camera wait-object accessors
// (__waitOps.getGrabResultWaitObject/getGrabStopWaitObject, taking the
// native camera object). Free functions so CameraWrap::Init stays untouched.
void InitWaitApi(Napi::Env env, Napi::Object exports);

}  // namespace node_pylon
