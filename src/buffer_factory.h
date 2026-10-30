// BufferFactory N-API wrapper (pylon infra allocator hooks) plus the
// camera/result integration points.
//
// BufferFactoryWrap owns a malloc-backed Pylon::IBufferFactory with shared
// allocation statistics (allocation count, free count, last allocation
// size, released flag). Attaching it to a camera hands ownership to pylon
// (Cleanup_Delete): pylon calls DestroyBufferFactory when the factory is
// replaced, the camera is destroyed, or the last outstanding grab buffer is
// freed — the deferred-release semantics the pypylon bufferfactory tests
// assert. The `released` flag lives in shared state so isReleased() stays
// readable after the native object is gone.
//
// Integration is exposed as free functions (not CameraWrap methods) so this
// translation unit stays independent of CameraWrap::Init:
//   attachBufferFactory(camera, factory|null), getAttachedBufferFactory,
//   setGrabResultFactory(result, factory|null).
// The attached factory JS object is kept alive as a hidden property on the
// camera JS object. One attach per BufferFactory instance: after pylon
// takes ownership the wrapper is spent (re-attach throws).
#pragma once

#include <napi.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <pylon/PylonIncludes.h>

namespace node_pylon {
namespace bufferfactory {

struct FactoryState {
  std::atomic<size_t> allocations{0};
  std::atomic<size_t> frees{0};
  std::atomic<size_t> lastSize{0};
  std::atomic<bool> released{false};
};

// malloc-backed IBufferFactory with context ids (0 = none) and stats.
class NativeBufFactory : public Pylon::IBufferFactory {
 public:
  explicit NativeBufFactory(std::shared_ptr<FactoryState> state)
      : state_(state) {}
  void AllocateBuffer(size_t bufferSize, void** pCreatedBuffer,
                      intptr_t& bufferContext) override;
  void FreeBuffer(void* pCreatedBuffer, intptr_t bufferContext) override;
  void DestroyBufferFactory() override;

 private:
  std::shared_ptr<FactoryState> state_;
  std::atomic<intptr_t> nextId_{0};
};

class BufferFactoryWrap : public Napi::ObjectWrap<BufferFactoryWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit BufferFactoryWrap(const Napi::CallbackInfo& info);
  ~BufferFactoryWrap() override;

  Pylon::IBufferFactory* native() const { return native_; }
  std::shared_ptr<FactoryState> state() const { return state_; }
  bool attached() const { return attached_; }
  void markAttached();
  void markDetached();

 private:
  Napi::Value AllocationCount(const Napi::CallbackInfo& info);
  Napi::Value FreeCount(const Napi::CallbackInfo& info);
  Napi::Value LastAllocationSize(const Napi::CallbackInfo& info);
  Napi::Value IsReleased(const Napi::CallbackInfo& info);

  std::shared_ptr<FactoryState> state_;
  NativeBufFactory* native_ = nullptr;
  bool attached_ = false;
};

void Init(Napi::Env env, Napi::Object exports);

}  // namespace bufferfactory
}  // namespace node_pylon
