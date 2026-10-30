// BufferFactory N-API wrapper: implementation.
#include "buffer_factory.h"

#include <cstdlib>
#include <new>

#include "camera_wrap.h"
#include "error_map.h"
#include "grab_result.h"

namespace node_pylon {
namespace bufferfactory {

Napi::FunctionReference BufferFactoryWrap::constructor;

namespace {

// Hidden property keeping the attached factory JS object alive on the
// camera JS object (released on detach / with the camera object).
constexpr const char* kFactoryProp = "__nodePylonBufferFactory";

CameraWrap* RequireCamera(Napi::Env env, const Napi::Value& camera) {
  if (!camera.IsObject() ||
      !camera.As<Napi::Object>().InstanceOf(CameraWrap::constructor.Value())) {
    Napi::TypeError::New(env, "camera (InstantCamera) expected")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  CameraWrap* wrap = CameraWrap::Unwrap(camera.As<Napi::Object>());
  if (wrap->PylonCamera() == nullptr) {
    Napi::Error::New(env, "InstantCamera not initialized")
        .ThrowAsJavaScriptException();
    return nullptr;
  }
  return wrap;
}

Napi::Value Attach(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 2) {
    Napi::TypeError::New(env, "setBufferFactory(camera, factory|null) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  CameraWrap* cam = RequireCamera(env, info[0]);
  if (cam == nullptr) return env.Undefined();
  Napi::Object camObj = info[0].As<Napi::Object>();

  // Detach: null/undefined restores the default factory.
  if (info[1].IsNull() || info[1].IsUndefined()) {
    try {
      cam->PylonCamera()->SetBufferFactory(nullptr);
    } catch (const Pylon::GenericException& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    } catch (const std::exception& ex) {
      errormap::ThrowTranslated(env, ex);
      return env.Undefined();
    }
    if (camObj.Has(kFactoryProp)) {
      Napi::Value prev = camObj.Get(kFactoryProp);
      if (prev.IsObject() &&
          prev.As<Napi::Object>().InstanceOf(
              BufferFactoryWrap::constructor.Value())) {
        BufferFactoryWrap::Unwrap(prev.As<Napi::Object>())->markDetached();
      }
      camObj.Delete(kFactoryProp);
    }
    return env.Undefined();
  }

  // Attach: must be a fresh BufferFactory instance.
  if (!info[1].IsObject() ||
      !info[1].As<Napi::Object>().InstanceOf(
          BufferFactoryWrap::constructor.Value())) {
    Napi::TypeError::New(env, "factory (BufferFactory|null) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  Napi::Object factObj = info[1].As<Napi::Object>();
  BufferFactoryWrap* fact = BufferFactoryWrap::Unwrap(factObj);
  if (fact->native() == nullptr) {
    Napi::Error::New(env, "BufferFactory is already attached/consumed; "
                           "create a new one")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  try {
    // Replacing an existing factory: pylon destroys the old one (deferred
    // while its buffers are outstanding).
    cam->PylonCamera()->SetBufferFactory(fact->native(),
                                         Pylon::Cleanup_Delete);
  } catch (const Pylon::GenericException& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Undefined();
  } catch (const std::exception& ex) {
    errormap::ThrowTranslated(env, ex);
    return env.Undefined();
  }
  fact->markAttached();
  if (camObj.Has(kFactoryProp)) {
    Napi::Value prev = camObj.Get(kFactoryProp);
    if (prev.IsObject() &&
        prev.As<Napi::Object>().InstanceOf(
            BufferFactoryWrap::constructor.Value())) {
      BufferFactoryWrap::Unwrap(prev.As<Napi::Object>())->markDetached();
    }
  }
  camObj.Set(kFactoryProp, factObj);
  return env.Undefined();
}

Napi::Value GetAttached(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !RequireCamera(env, info[0])) {
    if (!env.IsExceptionPending()) {
      Napi::TypeError::New(env, "getBufferFactory(camera) expected")
          .ThrowAsJavaScriptException();
    }
    return env.Null();
  }
  Napi::Object camObj = info[0].As<Napi::Object>();
  if (camObj.Has(kFactoryProp)) return camObj.Get(kFactoryProp);
  return env.Null();
}

// Tags a grab result with its factory (provenance for getBufferFactory).
Napi::Value SetResultFactory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject() ||
      !info[0].As<Napi::Object>().InstanceOf(
          GrabResultWrap::constructor.Value())) {
    Napi::TypeError::New(env, "result (GrabResult) expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
  GrabResultWrap* res =
      GrabResultWrap::Unwrap(info[0].As<Napi::Object>());
  if (info.Length() >= 2 && info[1].IsObject() &&
      info[1].As<Napi::Object>().InstanceOf(
          BufferFactoryWrap::constructor.Value())) {
    res->SetFactory(info[1].As<Napi::Object>());
  } else {
    res->ClearFactory();
  }
  return env.Undefined();
}

}  // namespace

// --- NativeBufFactory ------------------------------------------------------

void NativeBufFactory::AllocateBuffer(size_t bufferSize, void** pCreatedBuffer,
                                      intptr_t& bufferContext) {
  void* mem = std::malloc(bufferSize > 0 ? bufferSize : 1);
  if (mem == nullptr) throw std::bad_alloc();
  *pCreatedBuffer = mem;
  bufferContext = ++nextId_;
  state_->allocations.fetch_add(1);
  state_->lastSize.store(bufferSize);
}

void NativeBufFactory::FreeBuffer(void* pCreatedBuffer,
                                  intptr_t /*bufferContext*/) {
  // Must not throw (pylon contract).
  std::free(pCreatedBuffer);
  state_->frees.fetch_add(1);
}

void NativeBufFactory::DestroyBufferFactory() {
  // Called by pylon when the factory is no longer needed (replaced,
  // camera destroyed, or last outstanding buffer freed). Infallible.
  state_->released.store(true);
  delete this;
}

// --- BufferFactoryWrap -----------------------------------------------------

void BufferFactoryWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "BufferFactory",
      {InstanceMethod("allocationCount", &BufferFactoryWrap::AllocationCount),
       InstanceMethod("freeCount", &BufferFactoryWrap::FreeCount),
       InstanceMethod("lastAllocationSize",
                      &BufferFactoryWrap::LastAllocationSize),
       InstanceMethod("isReleased", &BufferFactoryWrap::IsReleased)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("BufferFactory", ctor);
}

BufferFactoryWrap::BufferFactoryWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<BufferFactoryWrap>(info),
      state_(std::make_shared<FactoryState>()),
      native_(new NativeBufFactory(state_)) {}

BufferFactoryWrap::~BufferFactoryWrap() {
  // Still ours (never attached): mark released, like OnReleased when the
  // factory is no longer needed.
  if (native_ != nullptr) {
    state_->released.store(true);
    delete native_;
    native_ = nullptr;
  }
}

void BufferFactoryWrap::markAttached() {
  native_ = nullptr;  // ownership moved to pylon (Cleanup_Delete)
  attached_ = true;
}

void BufferFactoryWrap::markDetached() { attached_ = false; }

Napi::Value BufferFactoryWrap::AllocationCount(const Napi::CallbackInfo& info) {
  return Napi::Number::New(
      info.Env(), static_cast<double>(state_->allocations.load()));
}

Napi::Value BufferFactoryWrap::FreeCount(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(),
                           static_cast<double>(state_->frees.load()));
}

Napi::Value BufferFactoryWrap::LastAllocationSize(
    const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(),
                           static_cast<double>(state_->lastSize.load()));
}

Napi::Value BufferFactoryWrap::IsReleased(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), state_->released.load());
}

void Init(Napi::Env env, Napi::Object exports) {
  BufferFactoryWrap::Init(env, exports);
  Napi::Object api = Napi::Object::New(env);
  api.Set("attach", Napi::Function::New(env, Attach));
  api.Set("getAttached", Napi::Function::New(env, GetAttached));
  api.Set("setResultFactory", Napi::Function::New(env, SetResultFactory));
  exports.Set("__bufferFactoryOps", api);
}

}  // namespace bufferfactory
}  // namespace node_pylon
