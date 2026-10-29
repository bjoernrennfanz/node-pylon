// Pylon lifecycle: PylonInitialize/PylonTerminate with reference counting.
// Header-only (inline) so addon.cc and camera_wrap.cc share the same
// counter instance. Thread-safe via mutex. Never throws across the
// N-API boundary: callers translate into JS exceptions.
#pragma once

#include <atomic>
#include <mutex>

#include <pylon/PylonIncludes.h>

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

// Raises the refcount; initializes Pylon on first use.
// Throws std::exception / Pylon::GenericException on SDK errors.
inline void EnsureInitialized() {
  std::lock_guard<std::mutex> lock(Mutex());
  if (Count().load() == 0) {
    Pylon::PylonInitialize();
  }
  Count().fetch_add(1);
}

// Lowers the refcount; terminates Pylon on last use.
inline void ReleaseRef() {
  std::lock_guard<std::mutex> lock(Mutex());
  int prev = Count().fetch_sub(1);
  if (prev <= 1) {
    Count().store(0);
    Pylon::PylonTerminate();
  }
}

}  // namespace lifecycle
}  // namespace node_pylon
