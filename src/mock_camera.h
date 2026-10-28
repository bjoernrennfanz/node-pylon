// InstantCamera-N-API-Wrapper, Mock-Stand (ZAU-151).
// N-API-Regeln: ObjectWrap, keine V8-Internals, Fehler als JS-Exceptions.
// Der Grab ist synchron und schnell (Mock); der echte RetrieveResult-Loop
// (ZAU-150/152) laeuft ueber AsyncWorker und blockiert nie den JS-Loop.
#pragma once

#include <napi.h>

#include <cstdint>

#include "mock_nodemap.h"

#if !NODE_PYLON_MOCK
// Echter Build: TlFactory/InstantCamera gegen Basler-pylon-SDK (ZAU-150).
// Mock-Pfade (MockNodeMap) bleiben fuer Geometrie-/Parameter-Defaults bestehen,
// bis die Nodemap-Anbindung (ZAU-151) die GenApi-Knoten direkt spiegelt.
#include <pylon/PylonIncludes.h>
#endif

namespace node_pylon {

class CameraWrap : public Napi::ObjectWrap<CameraWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // Default: FirstFound-Geraet (analog TlFactory.CreateFirstDevice).
  // Im Mock-Build immer verfuegbar; im echten Build wirft der Konstruktor
  // eine JS-Exception, wenn kein Geraet gefunden wurde.
  explicit CameraWrap(const Napi::CallbackInfo& info);
  ~CameraWrap() override;

 private:
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceInfo(const Napi::CallbackInfo& info);

  // ZAU-150 Core-Grab (pypylon-nah): expliziter Grab-Loop.
  // startGrabbing()/stopGrabbing()/isGrabbing() + retrieveResult(timeoutMs)
  // -> { succeeded, width, height, pixelFormat, payloadSize, buffer }
  // oder null bei Timeout. retrieveResult ohne Grabbing wirft.
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
  // Zero-copy: Buffer mit Finalizer, keine Zwischenkopie.
  Napi::Value Grab(const Napi::CallbackInfo& info);

  static void ThrowNodeError(Napi::Env env, const std::string& node,
                             NodeError err);

   bool open_ = false;
   bool grabbing_ = false;
   uint64_t frameNumber_ = 0;
   MockNodeMap nodemap_;
#if !NODE_PYLON_MOCK
   // Echte Kamera; nullptr bis zur erfolgreichen Anbindung im Konstruktor.
   // initOk_ steuert, ob der Destruktor ReleaseRef() rufen muss.
   Pylon::CInstantCamera* camera_ = nullptr;
   bool initOk_ = false;
#endif
};

}  // namespace node_pylon
