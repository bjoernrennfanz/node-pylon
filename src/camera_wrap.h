// InstantCamera N-API wrapper over the real Basler pylon SDK.
// N-API rules: ObjectWrap, no V8 internals, errors as JS exceptions.
// Grab loop and node access run synchronously and never block the JS loop
// beyond the call itself; the frame buffer is handed over zero-copy.
#pragma once

#include <napi.h>

#include <cstdint>

#include <pylon/PylonIncludes.h>

#include "node_map.h"

namespace node_pylon {

class CameraWrap : public Napi::ObjectWrap<CameraWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: first-found device (like TlFactory.CreateFirstDevice).
  // Throws a JS exception when no device was found.
  explicit CameraWrap(const Napi::CallbackInfo& info);
  ~CameraWrap() override;

 private:
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceInfo(const Napi::CallbackInfo& info);

  // Explicit grab loop (pypylon style):
  // startGrabbing()/stopGrabbing()/isGrabbing() + retrieveResult(timeoutMs)
  // -> { succeeded, width, height, pixelFormat, payloadSize, buffer }
  // or null on timeout. retrieveResult without grabbing throws.
  Napi::Value StartGrabbing(const Napi::CallbackInfo& info);
  Napi::Value StopGrabbing(const Napi::CallbackInfo& info);
  Napi::Value IsGrabbing(const Napi::CallbackInfo& info);
  Napi::Value RetrieveResult(const Napi::CallbackInfo& info);

  Napi::Value GetNodeType(const Napi::CallbackInfo& info);
  Napi::Value IsReadable(const Napi::CallbackInfo& info);
  Napi::Value IsWritable(const Napi::CallbackInfo& info);

  Napi::Value GetInteger(const Napi::CallbackInfo& info);
  Napi::Value SetInteger(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerMin(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerMax(const Napi::CallbackInfo& info);
  Napi::Value TryMax(const Napi::CallbackInfo& info);

  Napi::Value GetFloat(const Napi::CallbackInfo& info);
  Napi::Value SetFloat(const Napi::CallbackInfo& info);
  Napi::Value GetFloatMin(const Napi::CallbackInfo& info);
  Napi::Value GetFloatMax(const Napi::CallbackInfo& info);

  Napi::Value GetBoolean(const Napi::CallbackInfo& info);
  Napi::Value SetBoolean(const Napi::CallbackInfo& info);

  Napi::Value GetString(const Napi::CallbackInfo& info);
  Napi::Value SetString(const Napi::CallbackInfo& info);

  Napi::Value GetEnum(const Napi::CallbackInfo& info);
  Napi::Value SetEnum(const Napi::CallbackInfo& info);
  Napi::Value GetEnumEntries(const Napi::CallbackInfo& info);
  Napi::Value ExecuteCommand(const Napi::CallbackInfo& info);

  // grab() -> { buffer, width, height, pixelType, pixelFormat, payloadSize,
  //              timestamp, frameNumber, chunk }
  // Deterministic test frame with the camera's current geometry
  // (Width/Height/PixelFormat from GenApi); buffer ownership moves into
  // the JS Buffer via finalizer, no intermediate copy.
  Napi::Value Grab(const Napi::CallbackInfo& info);

  static void ThrowNodeError(Napi::Env env, const std::string& node,
                             NodeError err);

  bool open_ = false;
  bool grabbing_ = false;
  uint64_t frameNumber_ = 0;
  // Real camera, attached in the constructor. initOk_ tracks whether the
  // destructor must release the lifecycle reference.
  Pylon::CInstantCamera* camera_ = nullptr;
  bool initOk_ = false;
  // GenApi view of the open device (set on Open, cleared on Close).
  GenApi::INodeMap* genapi_ = nullptr;
  RealNodeMap nodemap_{nullptr};
};

}  // namespace node_pylon
