// Pylon-Lifecycle: PylonInitialize/PylonTerminate mit Referenzzaehlung (ZAU-150).
// Header-only (inline), damit addon.cc und mock_camera.cc dieselbe
// Zaehler-Instanz teilen. Thread-safe via Mutex. Im Mock-Build ohne SDK
// nur Zaehler, keine SDK-Calls. Niemals Exceptions ueber die N-API-Grenze:
// Aufrufer uebersetzen in JS-Exceptions.
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

// Erhoeht den Refcount; initialisiert Pylon beim ersten Nutzer.
// Wirft std::exception / Pylon::GenericException bei SDK-Fehlern.
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

// Senkt den Refcount; terminiert Pylon beim letzten Nutzer.
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
