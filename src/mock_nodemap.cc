// Mock-GenICam-Nodemap: Implementierung (ZAU-151).
// Keine pylon-Abhaengigkeit, keine Exceptions ueber Modulgrenzen.
#include "mock_nodemap.h"

namespace node_pylon {

const char* NodeErrorMessage(NodeError e) {
  switch (e) {
    case NodeError::kOk:
      return "ok";
    case NodeError::kUnknownNode:
      return "Unknown node";
    case NodeError::kWrongType:
      return "Wrong node type for this access";
    case NodeError::kNotReadable:
      return "Node is not readable";
    case NodeError::kNotWritable:
      return "Node is not writable";
    case NodeError::kOutOfRange:
      return "Value out of range";
    case NodeError::kInvalidEnumEntry:
      return "Invalid enumeration entry";
    case NodeError::kCommandFailed:
      return "Command execution failed";
  }
  return "Unknown nodemap error";
}

const char* NodeTypeName(NodeType t) {
  switch (t) {
    case NodeType::Integer:
      return "Integer";
    case NodeType::Float:
      return "Float";
    case NodeType::Boolean:
      return "Boolean";
    case NodeType::String:
      return "String";
    case NodeType::Enumeration:
      return "Enumeration";
    case NodeType::Command:
      return "Command";
  }
  return "Unknown";
}

MockNodeMap::MockNodeMap() {
  integers_["Width"] = IntegerNode{640, 64, 1920, true, true};
  integers_["Height"] = IntegerNode{480, 48, 1080, true, true};
  // Readonly, wird aus Width*Height*Bpp berechnet (GetInteger-Sonderfall).
  integers_["PayloadSize"] = IntegerNode{640 * 480, 0, 1920 * 1080 * 3, true, false};

  floats_["ExposureTime"] = FloatNode{5000.0, 10.0, 100000.0, true, true};
  floats_["Gain"] = FloatNode{0.0, 0.0, 24.0, true, true};

  bools_["ChunkModeActive"] = BoolNode{false, true, true};
  bools_["AcquisitionFrameRateEnable"] = BoolNode{false, true, true};

  strings_["DeviceUserID"] = StringNode{"", true, true, 64};
  strings_["DeviceModelName"] = StringNode{"MockModel Mono", true, false, 64};
  strings_["DeviceSerialNumber"] = StringNode{"MOCK0001", true, false, 64};

  enums_["PixelFormat"] =
      EnumNode{"Mono8", {"Mono8", "Mono12", "RGB8"}, true, true};
  enums_["TestPattern"] =
      EnumNode{"Off", {"Off", "SensorTestPattern"}, true, true};

  commands_["AcquisitionStart"] = CommandNode{};
  commands_["AcquisitionStop"] = CommandNode{};
  commands_["UserSetSave"] = CommandNode{};
}

NodeError MockNodeMap::GetType(const std::string& name, NodeType* out) const {
  if (integers_.count(name)) {
    *out = NodeType::Integer;
    return NodeError::kOk;
  }
  if (floats_.count(name)) {
    *out = NodeType::Float;
    return NodeError::kOk;
  }
  if (bools_.count(name)) {
    *out = NodeType::Boolean;
    return NodeError::kOk;
  }
  if (strings_.count(name)) {
    *out = NodeType::String;
    return NodeError::kOk;
  }
  if (enums_.count(name)) {
    *out = NodeType::Enumeration;
    return NodeError::kOk;
  }
  if (commands_.count(name)) {
    *out = NodeType::Command;
    return NodeError::kOk;
  }
  return NodeError::kUnknownNode;
}

NodeError MockNodeMap::IsReadable(const std::string& name, bool* out) const {
  NodeType t;
  NodeError e = GetType(name, &t);
  if (e != NodeError::kOk) return e;
  switch (t) {
    case NodeType::Integer:
      *out = integers_.at(name).readable;
      return NodeError::kOk;
    case NodeType::Float:
      *out = floats_.at(name).readable;
      return NodeError::kOk;
    case NodeType::Boolean:
      *out = bools_.at(name).readable;
      return NodeError::kOk;
    case NodeType::String:
      *out = strings_.at(name).readable;
      return NodeError::kOk;
    case NodeType::Enumeration:
      *out = enums_.at(name).readable;
      return NodeError::kOk;
    case NodeType::Command:
      // Commands haben keinen Lesezugriff auf einen Wert.
      *out = false;
      return NodeError::kOk;
  }
  return NodeError::kUnknownNode;
}

NodeError MockNodeMap::IsWritable(const std::string& name, bool* out) const {
  NodeType t;
  NodeError e = GetType(name, &t);
  if (e != NodeError::kOk) return e;
  switch (t) {
    case NodeType::Integer:
      *out = integers_.at(name).writable;
      return NodeError::kOk;
    case NodeType::Float:
      *out = floats_.at(name).writable;
      return NodeError::kOk;
    case NodeType::Boolean:
      *out = bools_.at(name).writable;
      return NodeError::kOk;
    case NodeType::String:
      *out = strings_.at(name).writable;
      return NodeError::kOk;
    case NodeType::Enumeration:
      *out = enums_.at(name).writable;
      return NodeError::kOk;
    case NodeType::Command:
      // Commands sind ausfuehrbar, wenn verfuegbar.
      *out = commands_.at(name).available;
      return NodeError::kOk;
  }
  return NodeError::kUnknownNode;
}

NodeError MockNodeMap::GetInteger(const std::string& name, int64_t* out) const {
  auto it = integers_.find(name);
  if (it == integers_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.readable) return NodeError::kNotReadable;
  if (name == "PayloadSize") {
    int64_t w = integers_.at("Width").value;
    int64_t h = integers_.at("Height").value;
    *out = w * h * BytesPerPixel(enums_.at("PixelFormat").value);
    return NodeError::kOk;
  }
  *out = it->second.value;
  return NodeError::kOk;
}

NodeError MockNodeMap::SetInteger(const std::string& name, int64_t v) {
  auto it = integers_.find(name);
  if (it == integers_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.writable) return NodeError::kNotWritable;
  if (v < it->second.min || v > it->second.max) return NodeError::kOutOfRange;
  it->second.value = v;
  return NodeError::kOk;
}

NodeError MockNodeMap::GetIntegerLimits(const std::string& name, int64_t* minOut,
                                        int64_t* maxOut) const {
  auto it = integers_.find(name);
  if (it == integers_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  *minOut = it->second.min;
  *maxOut = it->second.max;
  return NodeError::kOk;
}

NodeError MockNodeMap::TryMax(const std::string& name, double* outValue) {
  NodeType t;
  NodeError e = GetType(name, &t);
  if (e != NodeError::kOk) return e;
  if (t == NodeType::Integer) {
    auto it = integers_.find(name);
    if (!it->second.writable) return NodeError::kNotWritable;
    it->second.value = it->second.max;
    *outValue = static_cast<double>(it->second.max);
    return NodeError::kOk;
  }
  if (t == NodeType::Float) {
    auto it = floats_.find(name);
    if (!it->second.writable) return NodeError::kNotWritable;
    it->second.value = it->second.max;
    *outValue = it->second.max;
    return NodeError::kOk;
  }
  return NodeError::kWrongType;
}

NodeError MockNodeMap::GetFloat(const std::string& name, double* out) const {
  auto it = floats_.find(name);
  if (it == floats_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.readable) return NodeError::kNotReadable;
  *out = it->second.value;
  return NodeError::kOk;
}

NodeError MockNodeMap::SetFloat(const std::string& name, double v) {
  auto it = floats_.find(name);
  if (it == floats_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.writable) return NodeError::kNotWritable;
  if (v < it->second.min || v > it->second.max) return NodeError::kOutOfRange;
  it->second.value = v;
  return NodeError::kOk;
}

NodeError MockNodeMap::GetFloatLimits(const std::string& name, double* minOut,
                                      double* maxOut) const {
  auto it = floats_.find(name);
  if (it == floats_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  *minOut = it->second.min;
  *maxOut = it->second.max;
  return NodeError::kOk;
}

NodeError MockNodeMap::GetBoolean(const std::string& name, bool* out) const {
  auto it = bools_.find(name);
  if (it == bools_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.readable) return NodeError::kNotReadable;
  *out = it->second.value;
  return NodeError::kOk;
}

NodeError MockNodeMap::SetBoolean(const std::string& name, bool v) {
  auto it = bools_.find(name);
  if (it == bools_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.writable) return NodeError::kNotWritable;
  it->second.value = v;
  return NodeError::kOk;
}

NodeError MockNodeMap::GetString(const std::string& name, std::string* out) const {
  auto it = strings_.find(name);
  if (it == strings_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.readable) return NodeError::kNotReadable;
  *out = it->second.value;
  return NodeError::kOk;
}

NodeError MockNodeMap::SetString(const std::string& name, const std::string& v) {
  auto it = strings_.find(name);
  if (it == strings_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.writable) return NodeError::kNotWritable;
  if (v.size() > it->second.maxLen) return NodeError::kOutOfRange;
  it->second.value = v;
  return NodeError::kOk;
}

NodeError MockNodeMap::GetEnum(const std::string& name, std::string* out) const {
  auto it = enums_.find(name);
  if (it == enums_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.readable) return NodeError::kNotReadable;
  *out = it->second.value;
  return NodeError::kOk;
}

NodeError MockNodeMap::SetEnum(const std::string& name, const std::string& v) {
  auto it = enums_.find(name);
  if (it == enums_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.writable) return NodeError::kNotWritable;
  for (const auto& e : it->second.entries) {
    if (e == v) {
      it->second.value = v;
      return NodeError::kOk;
    }
  }
  return NodeError::kInvalidEnumEntry;
}

NodeError MockNodeMap::GetEnumEntries(const std::string& name,
                                      std::vector<std::string>* out) const {
  auto it = enums_.find(name);
  if (it == enums_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  *out = it->second.entries;
  return NodeError::kOk;
}

NodeError MockNodeMap::Execute(const std::string& name) {
  auto it = commands_.find(name);
  if (it == commands_.end()) {
    NodeType t;
    if (GetType(name, &t) == NodeError::kOk) return NodeError::kWrongType;
    return NodeError::kUnknownNode;
  }
  if (!it->second.available) return NodeError::kCommandFailed;
  it->second.executeCount += 1;
  return NodeError::kOk;
}

void MockNodeMap::GetDeviceInfo(std::string* friendlyName, std::string* modelName,
                                std::string* serialNumber, std::string* vendorName,
                                std::string* deviceClass) const {
  *friendlyName = "Mock Camera (node-pylon)";
  *modelName = strings_.at("DeviceModelName").value;
  *serialNumber = strings_.at("DeviceSerialNumber").value;
  *vendorName = "Basler (mock)";
  *deviceClass = "MockDevice";
}

void MockNodeMap::GetImageGeometry(int64_t* w, int64_t* h,
                                   std::string* pixelFormat) const {
  *w = integers_.at("Width").value;
  *h = integers_.at("Height").value;
  *pixelFormat = enums_.at("PixelFormat").value;
}

int MockNodeMap::BytesPerPixel(const std::string& pixelFormat) {
  if (pixelFormat == "RGB8") return 3;
  if (pixelFormat == "Mono12") return 2;
  return 1;  // Mono8 + Default
}

}  // namespace node_pylon
