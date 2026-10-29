// GenICam node map access without pylon-DataProcessing (MVP).
// RealNodeMap translates the node facade (Integer/Float/Boolean/String/
// Enumeration/Command) 1:1 onto GenApi nodes of the open camera, the way
// pypylon exposes them. No synthetic values: every read/write goes to the
// device (or emulator). Errors map to NodeError for JS translation.
#pragma once

#include <cstdint>
#include <string>
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
  void Rebind(GenApi::INodeMap* node_map) { node_map_ = node_map; }

  NodeError GetType(const std::string& name, NodeType* out) const;
  NodeError IsReadable(const std::string& name, bool* out) const;
  NodeError IsWritable(const std::string& name, bool* out) const;

  NodeError GetInteger(const std::string& name, int64_t* out) const;
  NodeError SetInteger(const std::string& name, int64_t v);
  NodeError GetIntegerLimits(const std::string& name, int64_t* min_out,
                             int64_t* max_out) const;
  // Sets Integer/Float to Max; returns the value set.
  NodeError TryMax(const std::string& name, double* out_value);

  NodeError GetFloat(const std::string& name, double* out) const;
  NodeError SetFloat(const std::string& name, double v);
  NodeError GetFloatLimits(const std::string& name, double* min_out,
                           double* max_out) const;

  NodeError GetBoolean(const std::string& name, bool* out) const;
  NodeError SetBoolean(const std::string& name, bool v);

  NodeError GetString(const std::string& name, std::string* out) const;
  NodeError SetString(const std::string& name, const std::string& v);

  NodeError GetEnum(const std::string& name, std::string* out) const;
  NodeError SetEnum(const std::string& name, const std::string& v);
  NodeError GetEnumEntries(const std::string& name,
                           std::vector<std::string>* out) const;

  NodeError Execute(const std::string& name);

  // Current image geometry from Width/Height/PixelFormat.
  NodeError GetImageGeometry(int64_t* w, int64_t* h,
                             std::string* pixel_format) const;
  // Bytes per pixel for the common formats (Mono8=1, Mono12=2, RGB8=3).
  static int BytesPerPixel(const std::string& pixel_format);

 private:
  GenApi::INode* Lookup(const std::string& name) const;

  GenApi::INodeMap* node_map_;
};

}  // namespace node_pylon
