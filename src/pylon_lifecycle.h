// Pylon lifecycle: PylonInitialize/PylonTerminate with refcounting.
// Header-only (inline) so addon.cc and mock_camera.cc share the same
// counter instance. Thread-safe via mutex. No SDK calls in the mock build,
// only the counter. Never let exceptions cross the N-API boundary:
// callers translate them into JS exceptions.
#pragma once

#include <atomic>
#include <mutex>

#if !NODE_PYLON_MOCK
#include <pylon/PylonIncludes.h>
#endif

namespace node_pylon {
namespace lifecycle {

inline std::mutex& Mutex() {
  static std::mutex m;
  return m;
}

inline std::atomic<int>& Count() {
  static std::atomic<int> c(0);
  return c;
}

// Increments the refcount; initializes pylon on first use.
// Throws std::exception / Pylon::GenericException on SDK errors.
inline void EnsureInitialized() {
#if NODE_PYLON_MOCK
  Count().fetch_add(1);
#else
  std::lock_guard<std::mutex> lock(Mutex());
  if (Count().load() == 0) {
    Pylon::PylonInitialize();
  }
  Count().fetch_add(1);
#endif
}

// Decrements the refcount; terminates pylon on last use.
inline void ReleaseRef() {
#if NODE_PYLON_MOCK
  int prev = Count().fetch_sub(1);
  if (prev <= 0) Count().store(0);
#else
  std::lock_guard<std::mutex> lock(Mutex());
  int prev = Count().fetch_sub(1);
  if (prev <= 1) {
    Count().store(0);
    Pylon::PylonTerminate();
  }
#endif
}

}  // namespace lifecycle
}  // namespace node_pylon
