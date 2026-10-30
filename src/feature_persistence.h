// FeaturePersistence N-API wrapper (pylon infra persistence API).
// Wraps Pylon::CFeaturePersistence Save/Load/SaveToString/LoadFromString
// against the open camera's GenApi nodemap (same map the typed accessors
// use). Registered as the `FeaturePersistence` object; the camera argument
// is the native InstantCamera object (the JS facade unwraps its handle).
#pragma once

#include <napi.h>

namespace node_pylon {
namespace feature {

void Init(Napi::Env env, Napi::Object exports);

}  // namespace feature
}  // namespace node_pylon
