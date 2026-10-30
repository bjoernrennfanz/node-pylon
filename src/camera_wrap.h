// InstantCamera N-API wrapper over the real Basler pylon SDK.
// N-API rules: ObjectWrap, no V8 internals, errors as JS exceptions.
// Grab loop and node access run synchronously and never block the JS loop
// beyond the call itself; the frame buffer is handed over zero-copy.
#pragma once

#include <napi.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <pylon/PylonIncludes.h>

#include "node_map.h"

namespace node_pylon {

// Grab-strategy / timeout-handling / grab-loop values mirror the pylon C++
// enums (GrabStrategy_OneByOne=0, LatestImageOnly=1, LatestImages=2,
// UpcomingImage=3; TimeoutHandling_Return=0, ThrowException=1;
// GrabLoop_ProvidedByInstantCamera=0, ProvidedByUser=1;
// RegistrationMode_Append=0, ReplaceAll=1; Cleanup_None=0, Delete=1).
// The JS facade also accepts the same names as strings.
constexpr int kGrabStrategyOneByOne = 0;
constexpr int kGrabStrategyLatestImageOnly = 1;
constexpr int kGrabStrategyLatestImages = 2;
constexpr int kGrabStrategyUpcomingImage = 3;
constexpr int kTimeoutHandlingReturn = 0;
constexpr int kTimeoutHandlingThrow = 1;
constexpr int kGrabLoopByCamera = 0;
constexpr int kGrabLoopByUser = 1;
constexpr int kRegistrationAppend = 0;
constexpr int kRegistrationReplaceAll = 1;

class CameraWrap : public Napi::ObjectWrap<CameraWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: first-found device (like TlFactory.CreateFirstDevice).
  // Throws a JS exception when no device was found.
  explicit CameraWrap(const Napi::CallbackInfo& info);
  ~CameraWrap() override;

  // GenApi node map of the open device (nullptr unless open). Shared with
  // the infra wrappers (feature persistence, defect pixel correction,
  // buffer factory) so they operate on the same nodemap as the accessors.
  GenApi::INodeMap* InfraNodeMap() const { return open_ ? genapi_ : nullptr; }
  Pylon::CInstantCamera* PylonCamera() const { return camera_; }

  // Attach a device enumerated by the transport layer (used by the
  // device-index constructor overload and attachDevice()).
  void AttachEnumeratedDevice(size_t device_index);

 private:
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceInfo(const Napi::CallbackInfo& info);

  // Device attach/detach/destroy semantics (like CInstantCamera).
  Napi::Value IsPylonDeviceAttached(const Napi::CallbackInfo& info);
  Napi::Value HasOwnership(const Napi::CallbackInfo& info);
  Napi::Value AttachDevice(const Napi::CallbackInfo& info);
  Napi::Value DetachDevice(const Napi::CallbackInfo& info);
  Napi::Value DestroyDevice(const Napi::CallbackInfo& info);
  Napi::Value IsCameraDeviceRemoved(const Napi::CallbackInfo& info);
  Napi::Value GetCameraContext(const Napi::CallbackInfo& info);
  Napi::Value SetCameraContext(const Napi::CallbackInfo& info);
  Napi::Value GetQueuedBufferCount(const Napi::CallbackInfo& info);

  // Explicit grab loop (pypylon style):
  // startGrabbing()/stopGrabbing()/isGrabbing() + retrieveResult(timeoutMs)
  // -> { succeeded, width, height, pixelFormat, payloadSize, buffer }
  // or null on timeout. retrieveResult without grabbing throws.
  // retrieveGrabResult(timeoutMs) returns the GrabResult ObjectWrap with
  // the same fields plus IsValid/IsUnique/Release, buffer views and GenDC
  // component access.
  Napi::Value StartGrabbing(const Napi::CallbackInfo& info);
  Napi::Value StartGrabbingMax(const Napi::CallbackInfo& info);
  Napi::Value StopGrabbing(const Napi::CallbackInfo& info);
  Napi::Value IsGrabbing(const Napi::CallbackInfo& info);
  Napi::Value RetrieveResult(const Napi::CallbackInfo& info);
  Napi::Value RetrieveGrabResult(const Napi::CallbackInfo& info);
  Napi::Value GrabOne(const Napi::CallbackInfo& info);
  Napi::Value WaitForFrameTriggerReady(const Napi::CallbackInfo& info);
  Napi::Value ExecuteSoftwareTrigger(const Napi::CallbackInfo& info);

  // Event-handler registration (JS callbacks; invoked synchronously on the
  // calling thread for lifecycle/image events so the JS loop never blocks).
  Napi::Value RegisterConfiguration(const Napi::CallbackInfo& info);
  Napi::Value DeregisterConfiguration(const Napi::CallbackInfo& info);
  Napi::Value RegisterImageEventHandler(const Napi::CallbackInfo& info);
  Napi::Value DeregisterImageEventHandler(const Napi::CallbackInfo& info);
  Napi::Value RegisterCameraEventHandler(const Napi::CallbackInfo& info);
  Napi::Value DeregisterCameraEventHandler(const Napi::CallbackInfo& info);

  Napi::Value GetNodeType(const Napi::CallbackInfo& info);
  Napi::Value IsReadable(const Napi::CallbackInfo& info);
  Napi::Value IsWritable(const Napi::CallbackInfo& info);
  Napi::Value IsAvailable(const Napi::CallbackInfo& info);
  Napi::Value GetDisplayName(const Napi::CallbackInfo& info);
  Napi::Value GetDescription(const Napi::CallbackInfo& info);

  Napi::Value GetInteger(const Napi::CallbackInfo& info);
  Napi::Value SetInteger(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerMin(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerMax(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerInc(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerUnit(const Napi::CallbackInfo& info);
  Napi::Value GetIntegerRepresentation(const Napi::CallbackInfo& info);
  Napi::Value SetIntegerToMin(const Napi::CallbackInfo& info);
  Napi::Value SetIntegerToMax(const Napi::CallbackInfo& info);
  Napi::Value TryMax(const Napi::CallbackInfo& info);

  Napi::Value GetFloat(const Napi::CallbackInfo& info);
  Napi::Value SetFloat(const Napi::CallbackInfo& info);
  Napi::Value GetFloatMin(const Napi::CallbackInfo& info);
  Napi::Value GetFloatMax(const Napi::CallbackInfo& info);
  Napi::Value HasFloatInc(const Napi::CallbackInfo& info);
  Napi::Value GetFloatInc(const Napi::CallbackInfo& info);
  Napi::Value GetFloatUnit(const Napi::CallbackInfo& info);
  Napi::Value GetFloatRepresentation(const Napi::CallbackInfo& info);
  Napi::Value SetFloatToMin(const Napi::CallbackInfo& info);
  Napi::Value SetFloatToMax(const Napi::CallbackInfo& info);

  Napi::Value GetValueAsString(const Napi::CallbackInfo& info);
  Napi::Value SetValueFromString(const Napi::CallbackInfo& info);

  Napi::Value GetBoolean(const Napi::CallbackInfo& info);
  Napi::Value SetBoolean(const Napi::CallbackInfo& info);

  Napi::Value GetString(const Napi::CallbackInfo& info);
  Napi::Value SetString(const Napi::CallbackInfo& info);
  Napi::Value GetStringMaxLength(const Napi::CallbackInfo& info);

  Napi::Value GetEnum(const Napi::CallbackInfo& info);
  Napi::Value SetEnum(const Napi::CallbackInfo& info);
  Napi::Value GetEnumEntries(const Napi::CallbackInfo& info);
  Napi::Value GetEnumIntValue(const Napi::CallbackInfo& info);
  Napi::Value SetEnumIntValue(const Napi::CallbackInfo& info);
  Napi::Value ExecuteCommand(const Napi::CallbackInfo& info);
  Napi::Value IsCommandDone(const Napi::CallbackInfo& info);

  Napi::Value GetCategoryFeatures(const Napi::CallbackInfo& info);
  Napi::Value IsSelector(const Napi::CallbackInfo& info);
  Napi::Value GetSelectedFeatures(const Napi::CallbackInfo& info);
  Napi::Value GetSelectingFeatures(const Napi::CallbackInfo& info);

  Napi::Value GetRegisterLength(const Napi::CallbackInfo& info);
  Napi::Value GetRegisterAddress(const Napi::CallbackInfo& info);
  Napi::Value ReadRegister(const Napi::CallbackInfo& info);
  Napi::Value WriteRegister(const Napi::CallbackInfo& info);

  Napi::Value RegisterCallback(const Napi::CallbackInfo& info);
  Napi::Value GetCallbackCount(const Napi::CallbackInfo& info);
  Napi::Value DeregisterCallback(const Napi::CallbackInfo& info);

  // GrabResultWaitObject / GrabStopWaitObject, custom buffer factory and
  // related infra accessors live in camera_infra.cc (prototype patching, so
  // CameraWrap::Init stays untouched); shared state via InfraNodeMap() and
  // PylonCamera() above.
  // Deterministic test frame with the camera's current geometry
  // (Width/Height/PixelFormat from GenApi); buffer ownership moves into
  // the JS Buffer via finalizer, no intermediate copy.
  Napi::Value Grab(const Napi::CallbackInfo& info);

  static void ThrowNodeError(Napi::Env env, const std::string& node,
                             NodeError err);
  // Shared grab-result object builder (retrieveResult/grabOne). The result
  // carries validity semantics: isValid mirrors CGrabResultPtr::IsValid(),
  // succeeded mirrors GrabSucceeded(), blockId/frameNumber mirror
  // GetBlockID(). Buffer ownership moves into the JS Buffer via finalizer.
  Napi::Object BuildGrabResultObject(Napi::Env env,
                                     const Pylon::CGrabResultPtr& ptr);
  void FireConfigurationEvent(const char* event_name);
  void FireImageEventHandlers(Napi::Env env, Napi::Object result);

  bool open_ = false;
  bool grabbing_ = false;
  uint64_t frameNumber_ = 0;
  intptr_t cameraContext_ = 0;
  // Real camera, attached in the constructor. initOk_ tracks whether the
  // destructor must release the lifecycle reference.
  Pylon::CInstantCamera* camera_ = nullptr;
  bool initOk_ = false;
  // GenApi view of the open device (set on Open, cleared on Close).
  GenApi::INodeMap* genapi_ = nullptr;
  RealNodeMap nodemap_{nullptr};
  // Registered JS event handlers (id, callback). Configuration handlers run
  // on lifecycle transitions, image handlers after each retrieved result.
  // Camera-event (device-node) handlers are stored for register/deregister
  // lifecycle parity; device-event dispatch needs an event-grab thread and
  // is a follow-up (see docs).
  uint32_t nextHandlerId_ = 1;
  std::vector<std::pair<uint32_t, Napi::FunctionReference>> configHandlers_;
  std::vector<std::pair<uint32_t, Napi::FunctionReference>> imageHandlers_;
  struct CameraEventRegistration {
    uint32_t id;
    Napi::FunctionReference fn;
    std::string nodeName;
  };
  std::vector<CameraEventRegistration> cameraEventHandlers_;
};

}  // namespace node_pylon
