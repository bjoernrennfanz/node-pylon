// GenICam/pylon exception mapping to JS Error subclasses.
// pypylon re-exports the GenICam exception hierarchy on both `pylon` and
// `genicam` (same class objects, catchable across the alias boundary).
// This helper owns those classes: Init() creates them once per module load
// (GenericException extends Error, the rest extend GenericException) and
// ThrowTranslated() converts a caught C++ exception into the matching JS
// class by dynamic type. All GenICam subclasses derive directly from
// GenericException (flat hierarchy), so probe order does not matter.
#pragma once

#include <napi.h>

#include <pylon/PylonIncludes.h>

namespace node_pylon {
namespace errormap {

// Creates the hierarchy on `exports` and on `exports.genicam` (identical
// class objects on both). Must run once during module init, before any
// ThrowTranslated() call.
void Init(Napi::Env env, Napi::Object exports);

// Throws the matching JS exception for a caught C++ exception. Falls back
// to GenericException for unknown std/pylon failures. Always leaves a
// pending JS exception; callers must return immediately afterwards.
void ThrowTranslated(Napi::Env env, const Pylon::GenericException& ex);
void ThrowTranslated(Napi::Env env, const std::exception& ex);

// Accessors for tests/facade wiring (valid after Init).
Napi::Function GenericCtor(Napi::Env env);

}  // namespace errormap
}  // namespace node_pylon
