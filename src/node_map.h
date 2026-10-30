// GenICam node map access without pylon-DataProcessing (MVP).
// RealNodeMap translates the node facade (Integer/Float/Boolean/String/
// Enumeration/Command) 1:1 onto GenApi nodes of the open camera, the way
// pypylon exposes them. No synthetic values: every read/write goes to the
// device (or emulator). Errors map to NodeError for JS translation.
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <GenApi/INodeMap.h>

namespace node_pylon {

// Error keys for JS exception translation (message via NodeErrorMessage).
enum class NodeError {
  kOk = 0,
  kUnknownNode,
  kNotOpen,
  kWrongType,
  kNotReadable,
  kNotWritable,
  kOutOfRange,
  kInvalidEnumEntry,
  kCommandFailed,
};

const char* NodeErrorMessage(NodeError e);

// Supported GenICam types (MVP, no DataProcessing).
enum class NodeType {
  Integer,
  Float,
  Boolean,
  String,
  Enumeration,
  Command,
};

const char* NodeTypeName(NodeType t);

// Thin accessor over the camera's GenApi node map. The INodeMap reference
// must outlive this object (owned by the open CInstantCamera).
class RealNodeMap {
 public:
  explicit RealNodeMap(GenApi::INodeMap* node_map) : node_map_(node_map) {}
  void Rebind(GenApi::INodeMap* node_map) {
    if (node_map == nullptr && !callbacks_.empty()) ClearCallbacks();
    node_map_ = node_map;
  }
  ~RealNodeMap() {
    // Only safe when callbacks were cleared while nodes were alive
    // (CameraWrap clears before Close/delete); guard the empty case.
    if (!callbacks_.empty()) ClearCallbacks();
  }

  NodeError GetType(const std::string& name, NodeType* out) const;
  NodeError IsReadable(const std::string& name, bool* out) const;
  NodeError IsWritable(const std::string& name, bool* out) const;
  NodeError IsAvailable(const std::string& name, bool* out) const;

  // INode naming (mirrors pypylon node_test.py / properties checks).
  NodeError GetDisplayName(const std::string& name, std::string* out) const;
  NodeError GetDescription(const std::string& name, std::string* out) const;

  NodeError GetInteger(const std::string& name, int64_t* out) const;
  NodeError SetInteger(const std::string& name, int64_t v);
  NodeError GetIntegerLimits(const std::string& name, int64_t* min_out,
                             int64_t* max_out) const;
  NodeError GetIntegerInc(const std::string& name, int64_t* out) const;
  NodeError GetIntegerUnit(const std::string& name, std::string* out) const;
  NodeError GetIntegerRepresentation(const std::string& name,
                                     std::string* out) const;
  NodeError SetIntegerToMin(const std::string& name);
  NodeError SetIntegerToMax(const std::string& name);
  // Sets Integer/Float to Max; returns the value set.
  NodeError TryMax(const std::string& name, double* out_value);

  NodeError GetFloat(const std::string& name, double* out) const;
  NodeError SetFloat(const std::string& name, double v);
  NodeError GetFloatLimits(const std::string& name, double* min_out,
                           double* max_out) const;
  NodeError HasFloatInc(const std::string& name, bool* out) const;
  NodeError GetFloatInc(const std::string& name, double* out) const;
  NodeError GetFloatUnit(const std::string& name, std::string* out) const;
  NodeError GetFloatRepresentation(const std::string& name,
                                   std::string* out) const;
  NodeError SetFloatToMin(const std::string& name);
  NodeError SetFloatToMax(const std::string& name);

  // Generic IValue string conversion (SwissKnife/converter evaluation,
  // FromString/ToString in pypylon).
  NodeError GetValueAsString(const std::string& name, std::string* out) const;
  NodeError SetValueFromString(const std::string& name, const std::string& v);

  NodeError GetBoolean(const std::string& name, bool* out) const;
  NodeError SetBoolean(const std::string& name, bool v);

  NodeError GetString(const std::string& name, std::string* out) const;
  NodeError SetString(const std::string& name, const std::string& v);
  NodeError GetStringMaxLength(const std::string& name, int64_t* out) const;

  NodeError GetEnum(const std::string& name, std::string* out) const;
  NodeError SetEnum(const std::string& name, const std::string& v);
  NodeError GetEnumEntries(const std::string& name,
                           std::vector<std::string>* out) const;
  NodeError GetEnumIntValue(const std::string& name, int64_t* out) const;
  NodeError SetEnumIntValue(const std::string& name, int64_t v);

  NodeError Execute(const std::string& name);
  NodeError IsCommandDone(const std::string& name, bool* out) const;

  // Category nodes (ICategory::GetFeatures).
  NodeError GetCategoryFeatures(const std::string& name,
                                std::vector<std::string>* out) const;

  // Selector nodes (ISelector::IsSelector + selected/selecting features).
  NodeError IsSelector(const std::string& name, bool* out) const;
  NodeError GetSelectedFeatures(const std::string& name,
                                std::vector<std::string>* out) const;
  NodeError GetSelectingFeatures(const std::string& name,
                                 std::vector<std::string>* out) const;

  // Register nodes (IRegister::GetLength/GetAddress/Get/Set).
  NodeError GetRegisterLength(const std::string& name, int64_t* out) const;
  NodeError GetRegisterAddress(const std::string& name, int64_t* out) const;
  NodeError ReadRegister(const std::string& name,
                         std::vector<uint8_t>* out) const;
  NodeError WriteRegister(const std::string& name,
                          const std::vector<uint8_t>& data);

  // Node-change callbacks (GenApi INode::RegisterCallback). The C++
  // callback only bumps an atomic counter; JS polls the count. This keeps
  // callback semantics (fires on value change) without crossing threads
  // into V8. Returns a small integer handle for poll/deregister.
  NodeError RegisterCallback(const std::string& name, int64_t* out_handle);
  NodeError GetCallbackCount(int64_t handle, int* out) const;
  NodeError DeregisterCallback(int64_t handle);
  // Removes all callbacks while the underlying INodeMap is still alive.
  // Must run before Close/delete of the camera; Rebind also calls it but
  // only while nodes are valid.
  void ClearCallbacks();

  // Current image geometry from Width/Height/PixelFormat.
  NodeError GetImageGeometry(int64_t* w, int64_t* h,
                             std::string* pixel_format) const;
  // Bytes per pixel for the common formats (Mono8=1, Mono12=2, RGB8=3).
  static int BytesPerPixel(const std::string& pixel_format);

 private:
  struct CallbackRecord {
    GenApi::INode* node = nullptr;
    GenApi::CallbackHandleType gen_handle = 0;
    std::shared_ptr<std::atomic<int>> count;
  };
  GenApi::INode* Lookup(const std::string& name) const;

  GenApi::INodeMap* node_map_;
  mutable std::mutex callbacks_mutex_;
  std::unordered_map<int64_t, CallbackRecord> callbacks_;
  int64_t next_callback_id_ = 1;
};

}  // namespace node_pylon
