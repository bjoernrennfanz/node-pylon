// InstantCamera N-API wrapper, mock state.
// N-API rules: ObjectWrap, no V8 internals, errors as JS exceptions.
// The grab is synchronous and fast (mock); the real RetrieveResult loop
// runs via AsyncWorker and never blocks the JS loop.
#pragma once

#include <napi.h>

#include <cstdint>

#include "mock_nodemap.h"

#if !NODE_PYLON_MOCK
// Real build: TlFactory/InstantCamera against the Basler pylon SDK.
// Mock paths (MockNodeMap) remain for geometry/parameter defaults
// until the nodemap wiring mirrors the GenApi nodes directly.
#include <pylon/PylonIncludes.h>
#endif

namespace node_pylon {

class CameraWrap : public Napi::ObjectWrap<CameraWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: first-found device (mirrors TlFactory.CreateFirstDevice).
  // Always available in the mock build; in the real build the constructor throws
  // a JS exception when no device is found.
  explicit CameraWrap(const Napi::CallbackInfo& info);
  ~CameraWrap() override;

 private:
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceInfo(const Napi::CallbackInfo& info);

  // Core grab (mirrors pypylon): explicit grab loop.
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

  // grab() -> { buffer, width, height, pixelFormat, payloadSize,
  //              timestamp, frameNumber, chunk }
  // Zero-copy: buffer with finalizer, no intermediate copy.
  Napi::Value Grab(const Napi::CallbackInfo& info);

  static void ThrowNodeError(Napi::Env env, const std::string& node,
                             NodeError err);

   bool open_ = false;
   bool grabbing_ = false;
   uint64_t frameNumber_ = 0;
   MockNodeMap nodemap_;
#if !NODE_PYLON_MOCK
  // Real camera; nullptr until successfully wired in the constructor.
  // initOk_ controls whether the destructor must call ReleaseRef().
   Pylon::CInstantCamera* camera_ = nullptr;
   bool initOk_ = false;
#endif
};

}  // namespace node_pylon
