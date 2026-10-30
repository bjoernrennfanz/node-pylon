// GenICam/pylon exception mapping: implementation.
#include "error_map.h"

#include <string>

namespace node_pylon {
namespace errormap {

namespace {

// Persistent constructors, one per mapped exception. The same function
// object is exposed on `exports` and on `exports.genicam`.
Napi::FunctionReference gGeneric;
Napi::FunctionReference gBadAlloc;
Napi::FunctionReference gInvalidArgument;
Napi::FunctionReference gOutOfRange;
Napi::FunctionReference gProperty;
Napi::FunctionReference gRuntime;
Napi::FunctionReference gLogicalError;
Napi::FunctionReference gAccess;
Napi::FunctionReference gTimeout;
Napi::FunctionReference gDynamicCast;

// Shared constructor body: stores message, captures a stack trace.
void CtorBody(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  const std::string msg = info.Length() > 0 && info[0].IsString()
                              ? info[0].As<Napi::String>().Utf8Value()
                              : "";
  Napi::Object self = info.This().As<Napi::Object>();
  self.Set("message", msg);
  Napi::Object fresh =
      env.Global()
          .Get("Error")
          .As<Napi::Function>()
          .New({Napi::String::New(env, msg)});
  if (fresh.Has("stack")) self.Set("stack", fresh.Get("stack"));
}

// Makes `Sub extends Base` with the right prototype name, then publishes
// the constructor on both the top level and the genicam namespace object.
void DefineSubclass(Napi::Env env, Napi::Object on, Napi::Object genicam,
                    Napi::Function base, const char* name,
                    Napi::FunctionReference* slot) {
  Napi::Function ctor = Napi::Function::New(env, &CtorBody, name);
  Napi::Object global = env.Global();
  Napi::Function createFn =
      global.Get("Object").As<Napi::Object>().Get("create").As<Napi::Function>();
  Napi::Function setProto = global.Get("Reflect")
                                .As<Napi::Object>()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();
  Napi::Object proto =
      createFn.Call({base.Get("prototype")}).As<Napi::Object>();
  proto.Set("constructor", ctor);
  proto.Set("name", name);
  ctor.Set("prototype", proto);
  setProto.Call({ctor, base});
  setProto.Call({proto, base.Get("prototype")});
  on.Set(name, ctor);
  genicam.Set(name, ctor);
  *slot = Napi::Persistent(ctor);
  slot->SuppressDestruct();
}

void ThrowJs(Napi::Env env, Napi::FunctionReference& ctorRef,
             const std::string& msg) {
  Napi::Value argv[] = {Napi::String::New(env, msg)};
  Napi::Object err = ctorRef.Value().New(1, argv);
  err.Set("message", msg);
  Napi::Error(env, err).ThrowAsJavaScriptException();
}

}  // namespace

void Init(Napi::Env env, Napi::Object exports) {
  Napi::Object genicam = Napi::Object::New(env);
  Napi::Object global = env.Global();
  Napi::Function errorCtor = global.Get("Error").As<Napi::Function>();
  Napi::Function createFn =
      global.Get("Object").As<Napi::Object>().Get("create").As<Napi::Function>();
  Napi::Function setProto = global.Get("Reflect")
                                .As<Napi::Object>()
                                .Get("setPrototypeOf")
                                .As<Napi::Function>();

  Napi::Function generic = Napi::Function::New(env, &CtorBody, "GenericException");
  Napi::Object genericProto =
      createFn.Call({errorCtor.Get("prototype")}).As<Napi::Object>();
  genericProto.Set("constructor", generic);
  genericProto.Set("name", "GenericException");
  generic.Set("prototype", genericProto);
  setProto.Call({generic, errorCtor});
  setProto.Call({genericProto, errorCtor.Get("prototype")});
  exports.Set("GenericException", generic);
  genicam.Set("GenericException", generic);
  gGeneric = Napi::Persistent(generic);
  gGeneric.SuppressDestruct();

  DefineSubclass(env, exports, genicam, generic, "BadAllocException", &gBadAlloc);
  DefineSubclass(env, exports, genicam, generic, "InvalidArgumentException",
                 &gInvalidArgument);
  DefineSubclass(env, exports, genicam, generic, "OutOfRangeException",
                 &gOutOfRange);
  DefineSubclass(env, exports, genicam, generic, "PropertyException", &gProperty);
  DefineSubclass(env, exports, genicam, generic, "RuntimeException", &gRuntime);
  DefineSubclass(env, exports, genicam, generic, "LogicalErrorException",
                 &gLogicalError);
  DefineSubclass(env, exports, genicam, generic, "AccessException", &gAccess);
  DefineSubclass(env, exports, genicam, generic, "TimeoutException", &gTimeout);
  DefineSubclass(env, exports, genicam, generic, "DynamicCastException",
                 &gDynamicCast);

  exports.Set("genicam", genicam);
}

void ThrowTranslated(Napi::Env env, const Pylon::GenericException& ex) {
  const std::string msg = ex.what();
  // Concrete C++ type probe. The GenICam hierarchy is flat (every class
  // derives directly from GenericException), so order is irrelevant.
  // NOTE: must be called from inside a catch handler (uses `throw;`).
  try {
    throw;
  } catch (const GenICam::TimeoutException&) {
    ThrowJs(env, gTimeout, msg);
  } catch (const GenICam::AccessException&) {
    ThrowJs(env, gAccess, msg);
  } catch (const GenICam::LogicalErrorException&) {
    ThrowJs(env, gLogicalError, msg);
  } catch (const GenICam::RuntimeException&) {
    ThrowJs(env, gRuntime, msg);
  } catch (const GenICam::PropertyException&) {
    ThrowJs(env, gProperty, msg);
  } catch (const GenICam::OutOfRangeException&) {
    ThrowJs(env, gOutOfRange, msg);
  } catch (const GenICam::InvalidArgumentException&) {
    ThrowJs(env, gInvalidArgument, msg);
  } catch (const GenICam::BadAllocException&) {
    ThrowJs(env, gBadAlloc, msg);
  } catch (const GenICam::DynamicCastException&) {
    ThrowJs(env, gDynamicCast, msg);
  } catch (const Pylon::GenericException&) {
    ThrowJs(env, gGeneric, msg);
  }
}

void ThrowTranslated(Napi::Env env, const std::exception& ex) {
  // std-only mapping (GenICam exceptions never reach this overload: they
  // are not std::exceptions and are caught by the overload above first).
  // Pointer dynamic_casts: no active-exception requirement, no terminate risk.
  const std::string msg = ex.what();
  if (dynamic_cast<const std::bad_alloc*>(&ex) != nullptr) {
    ThrowJs(env, gBadAlloc, msg);
  } else if (dynamic_cast<const std::out_of_range*>(&ex) != nullptr) {
    ThrowJs(env, gOutOfRange, msg);
  } else if (dynamic_cast<const std::invalid_argument*>(&ex) != nullptr) {
    ThrowJs(env, gInvalidArgument, msg);
  } else {
    ThrowJs(env, gGeneric, msg);
  }
}

Napi::Function GenericCtor(Napi::Env env) { return gGeneric.Value(); }

}  // namespace errormap
}  // namespace node_pylon
