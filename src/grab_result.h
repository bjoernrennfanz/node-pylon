// GrabResult N-API wrapper over the real Basler pylon SDK.
// Wraps CGrabResultPtr 1:1 (pypylon grabresult semantics): IsValid/IsUnique/
// Release, geometry, pixel/payload types, timestamps, chunk data, error
// codes, GenDC component awareness. Buffers are exposed zero-copy: the JS
// Buffer/ArrayBuffer views the grab buffer directly and holds a shared
// reference to the CGrabResultPtr via finalizer, so no memcpy happens.
#pragma once

#include <memory>

#include <napi.h>

#include <pylon/PylonIncludes.h>

namespace node_pylon {

// Shared ownership of a grab result so zero-copy JS buffers stay valid.
// The CGrabResultPtr is copyable/refcounted; the shared_ptr keeps one copy
// alive until the last JS view's finalizer runs.
using GrabPtr = std::shared_ptr<Pylon::CGrabResultPtr>;

class GrabResultWrap : public Napi::ObjectWrap<GrabResultWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // JS: new GrabResult() is not public API; instances come from
  // InstantCamera.retrieveGrabResult(). The default ctor creates an invalid
  // (empty) result so the class can still be constructed safely.
  explicit GrabResultWrap(const Napi::CallbackInfo& info);
  ~GrabResultWrap() override = default;

  // Attach a live grab result (used by InstantCamera.retrieveGrabResult).
  void Attach(Pylon::CGrabResultPtr ptr);

  // Buffer-factory provenance (infra allocator hooks): remembers which JS
  // factory allocated this result's buffer; exposed as getBufferContext /
  // getBufferFactory (like pypylon GrabResult.BufferContext/BufferFactory).
  // The factory reference keeps the JS factory object alive with the result.
  void SetFactory(Napi::Object factory);
  void ClearFactory();

  // Exposed for converter/decompressor/PylonImage attach paths.
  Pylon::CGrabResultPtr& Ptr() { return *ptr_; }

  // Build a JS GrabResult object from a native ptr (sets both methods via
  // prototype and plain fields for backward-compatible property access).
  static Napi::Object FromPtr(Napi::Env env, Pylon::CGrabResultPtr ptr);

 private:
  Napi::Value IsValid(const Napi::CallbackInfo& info);
  Napi::Value IsUnique(const Napi::CallbackInfo& info);
  Napi::Value Release(const Napi::CallbackInfo& info);
  Napi::Value Succeeded(const Napi::CallbackInfo& info);
  Napi::Value GetWidth(const Napi::CallbackInfo& info);
  Napi::Value GetHeight(const Napi::CallbackInfo& info);
  Napi::Value GetOffsetX(const Napi::CallbackInfo& info);
  Napi::Value GetOffsetY(const Napi::CallbackInfo& info);
  Napi::Value GetPaddingX(const Napi::CallbackInfo& info);
  Napi::Value GetPaddingY(const Napi::CallbackInfo& info);
  Napi::Value GetPixelType(const Napi::CallbackInfo& info);
  Napi::Value GetPixelTypeValue(const Napi::CallbackInfo& info);
  Napi::Value GetPayloadType(const Napi::CallbackInfo& info);
  Napi::Value GetPayloadSize(const Napi::CallbackInfo& info);
  Napi::Value GetBufferSize(const Napi::CallbackInfo& info);
  Napi::Value GetImageSize(const Napi::CallbackInfo& info);
  Napi::Value GetTimestamp(const Napi::CallbackInfo& info);
  Napi::Value GetBlockId(const Napi::CallbackInfo& info);
  Napi::Value GetId(const Napi::CallbackInfo& info);
  Napi::Value GetImageNumber(const Napi::CallbackInfo& info);
  Napi::Value GetSkippedImages(const Napi::CallbackInfo& info);
  Napi::Value GetErrorCode(const Napi::CallbackInfo& info);
  Napi::Value GetErrorDescription(const Napi::CallbackInfo& info);
  Napi::Value GetBufferContext(const Napi::CallbackInfo& info);
  Napi::Value GetBufferFactory(const Napi::CallbackInfo& info);
  Napi::Value IsChunkDataAvailable(const Napi::CallbackInfo& info);
  Napi::Value HasCrc(const Napi::CallbackInfo& info);
  Napi::Value CheckCrc(const Napi::CallbackInfo& info);
  Napi::Value GetStride(const Napi::CallbackInfo& info);
  // Zero-copy views over the grab buffer. getBuffer() returns a Node Buffer,
  // getArray() a Uint8Array, getMemoryView() a DataView over the same
  // memory; all share the grab lifetime (no memcpy). The first view is
  // created lazily and cached, so a fresh result stays the unique owner
  // (IsUnique() true, like pypylon) until a view is taken.
  Napi::Value GetBuffer(const Napi::CallbackInfo& info);
  Napi::Value GetArray(const Napi::CallbackInfo& info);
  Napi::Value GetMemoryView(const Napi::CallbackInfo& info);
  // GenDC component awareness: dataComponentCount + getDataComponent(i)
  // -> { type, typeName, width, height, pixelType, dataSize, timestamp }.
  // getDataComponentBuffer(i) returns a zero-copy Buffer over the component.
  Napi::Value GetDataComponentCount(const Napi::CallbackInfo& info);
  Napi::Value GetDataComponent(const Napi::CallbackInfo& info);
  Napi::Value GetDataComponentBuffer(const Napi::CallbackInfo& info);
  // Plain-object snapshot for property-style access (width, height,
  // pixelType, payloadSize, timestamp, ... plus buffer).
  Napi::Value ToObject(const Napi::CallbackInfo& info);
  // Lazy `buffer` accessor (installed by FromPtr/Release instead of an
  // eager data property) plus the shared getBuffer() implementation.
  static Napi::Value BufferAccessor(const Napi::CallbackInfo& info);
  Napi::Value GetOrCreateBuffer(Napi::Env env);
  static void InstallBufferAccessor(Napi::Env env, Napi::Object obj);

  GrabPtr ptr_;
  // Cached zero-copy view (created on first `buffer`/getBuffer() access;
  // empty until then so fresh results stay uniquely owned).
  Napi::Reference<Napi::Value> bufferCache_;
  bool hasBufferCache_ = false;
  // Buffer-factory provenance (see SetFactory): exact JS factory object
  // that allocated this result's buffer, if any.
  Napi::ObjectReference factoryRef_;
  bool hasFactory_ = false;
};

// True when obj is an instance of the given native constructor
// (JS instanceof semantics).
inline bool IsNativeInstance(Napi::Object obj, const Napi::Function& ctor) {
  return obj.InstanceOf(ctor);
}

inline bool IsNativeInstance(Napi::Object obj,
                             Napi::FunctionReference& ctorRef) {
  return IsNativeInstance(obj, ctorRef.Value());
}
// Pixel/payload/enum helpers shared by grab result, images and the facade.
const char* PixelTypeName(Pylon::EPixelType t);
const char* PayloadTypeName(Pylon::EPayloadType t);
Napi::Object BuildGrabFields(Napi::Env env, Pylon::CGrabResultPtr& ptr,
                            bool includeBuffer = true);

}  // namespace node_pylon
