// RealNodeMap: GenICam node access against the open camera, the way pypylon
// exposes it (TlFactory -> InstantCamera -> GetNodeMap -> typed access).
// All reads/writes reach GenApi; errors map to NodeError deterministically
// (pre-validated where possible instead of parsing exception texts).
#include "node_map.h"

#include <GenApi/NodeCallback.h>
#include <pylon/PylonIncludes.h>

#include <atomic>
#include <memory>
#include <mutex>

namespace node_pylon {

const char* NodeErrorMessage(NodeError e) {
  switch (e) {
    case NodeError::kOk:
      return "ok";
    case NodeError::kUnknownNode:
      return "Unknown node";
    case NodeError::kNotOpen:
      return "Camera is not open. Call open() first.";
    case NodeError::kWrongType:
      return "Node has an unexpected type";
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
  return "Unknown node error";
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

namespace {

const char* RepresentationName(GenApi::ERepresentation r) {
  switch (r) {
    case GenApi::Linear:
      return "Linear";
    case GenApi::Logarithmic:
      return "Logarithmic";
    case GenApi::Boolean:
      return "Boolean";
    case GenApi::PureNumber:
      return "PureNumber";
    case GenApi::HexNumber:
      return "HexNumber";
    case GenApi::IPV4Address:
      return "IPV4Address";
    case GenApi::MACAddress:
      return "MACAddress";
    default:
      return "_UndefinedRepresentation";
  }
}

// Counter-only GenApi callback: bumps an atomic, never touches V8/JS.
// GenApi takes ownership via RegisterCallback/DeregisterCallback.
class CounterCallback : public GenApi::CNodeCallback {
 public:
  CounterCallback(GenApi::INode* node,
                  std::shared_ptr<std::atomic<int>> count)
      : GenApi::CNodeCallback(node, GenApi::cbPostOutsideLock),
        count_(std::move(count)) {}
  void operator()(GenApi::ECallbackType type) const override {
    if (type == GenApi::cbPostOutsideLock && count_) {
      count_->fetch_add(1);
    }
  }
  void Destroy() override { delete this; }

 private:
  std::shared_ptr<std::atomic<int>> count_;
};

bool ToNodeType(GenApi::EInterfaceType ifType, NodeType* out) {
  switch (ifType) {
    case GenApi::intfIInteger:
      *out = NodeType::Integer;
      return true;
    case GenApi::intfIFloat:
      *out = NodeType::Float;
      return true;
    case GenApi::intfIBoolean:
      *out = NodeType::Boolean;
      return true;
    case GenApi::intfIString:
      *out = NodeType::String;
      return true;
    case GenApi::intfIEnumeration:
      *out = NodeType::Enumeration;
      return true;
    case GenApi::intfICommand:
      *out = NodeType::Command;
      return true;
    default:
      return false;
  }
}

}  // namespace

GenApi::INode* RealNodeMap::Lookup(const std::string& name) const {
  if (node_map_ == nullptr) return nullptr;
  try {
    return node_map_->GetNode(name.c_str());
  } catch (const Pylon::GenericException&) {
    return nullptr;
  } catch (const std::exception&) {
    return nullptr;
  }
}

NodeError RealNodeMap::GetType(const std::string& name, NodeType* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  if (!ToNodeType(node->GetPrincipalInterfaceType(), out)) {
    return NodeError::kWrongType;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::IsReadable(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  *out = GenApi::IsReadable(node);
  return NodeError::kOk;
}

NodeError RealNodeMap::IsWritable(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  *out = GenApi::IsWritable(node);
  return NodeError::kOk;
}

NodeError RealNodeMap::IsAvailable(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  *out = GenApi::IsAvailable(node);
  return NodeError::kOk;
}

NodeError RealNodeMap::GetDisplayName(const std::string& name,
                                      std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  try {
    *out = std::string(node->GetDisplayName().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetDescription(const std::string& name,
                                     std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  try {
    *out = std::string(node->GetDescription().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetInteger(const std::string& name,
                                  int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = ptr->GetValue();
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetInteger(const std::string& name, int64_t v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    if (v < ptr->GetMin() || v > ptr->GetMax()) {
      return NodeError::kOutOfRange;
    }
    ptr->SetValue(v);
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetIntegerLimits(const std::string& name, int64_t* min_out,
                                        int64_t* max_out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *min_out = ptr->GetMin();
    *max_out = ptr->GetMax();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetIntegerInc(const std::string& name,
                                     int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->GetInc();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetIntegerUnit(const std::string& name,
                                      std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = std::string(ptr->GetUnit().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetIntegerRepresentation(const std::string& name,
                                               std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = RepresentationName(ptr->GetRepresentation());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetIntegerToMin(const std::string& name) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetValue(ptr->GetMin());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetIntegerToMax(const std::string& name) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CIntegerPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetValue(ptr->GetMax());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::TryMax(const std::string& name, double* out_value) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  try {
    if (node->GetPrincipalInterfaceType() == GenApi::intfIInteger) {
      GenApi::CIntegerPtr ptr(node);
      if (!ptr.IsValid() || !GenApi::IsWritable(node)) {
        return NodeError::kNotWritable;
      }
      const int64_t max = ptr->GetMax();
      ptr->SetValue(max);
      *out_value = static_cast<double>(max);
      return NodeError::kOk;
    }
    if (node->GetPrincipalInterfaceType() == GenApi::intfIFloat) {
      GenApi::CFloatPtr ptr(node);
      if (!ptr.IsValid() || !GenApi::IsWritable(node)) {
        return NodeError::kNotWritable;
      }
      const double max = ptr->GetMax();
      ptr->SetValue(max);
      *out_value = max;
      return NodeError::kOk;
    }
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kWrongType;
}

NodeError RealNodeMap::GetFloat(const std::string& name, double* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = ptr->GetValue();
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetFloat(const std::string& name, double v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    if (v < ptr->GetMin() || v > ptr->GetMax()) {
      return NodeError::kOutOfRange;
    }
    ptr->SetValue(v);
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetFloatLimits(const std::string& name, double* min_out,
                                      double* max_out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *min_out = ptr->GetMin();
    *max_out = ptr->GetMax();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::HasFloatInc(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->HasInc();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetFloatInc(const std::string& name, double* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->GetInc();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetFloatUnit(const std::string& name,
                                    std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = std::string(ptr->GetUnit().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetFloatRepresentation(const std::string& name,
                                             std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = RepresentationName(ptr->GetRepresentation());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetFloatToMin(const std::string& name) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetValue(ptr->GetMin());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetFloatToMax(const std::string& name) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CFloatPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetValue(ptr->GetMax());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetValueAsString(const std::string& name,
                                        std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CValuePtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = std::string(ptr->ToString().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetValueFromString(const std::string& name,
                                          const std::string& v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CValuePtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->FromString(v.c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kOutOfRange;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetBoolean(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CBooleanPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = ptr->GetValue();
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetBoolean(const std::string& name, bool v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CBooleanPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetValue(v);
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetString(const std::string& name,
                                 std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CStringPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = std::string(ptr->GetValue().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetString(const std::string& name,
                                 const std::string& v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CStringPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    if (static_cast<int64_t>(v.size()) > ptr->GetMaxLength()) {
      return NodeError::kOutOfRange;
    }
    ptr->SetValue(v.c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetStringMaxLength(const std::string& name,
                                          int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CStringPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->GetMaxLength();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetEnum(const std::string& name,
                               std::string* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = std::string(ptr->ToString().c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetEnum(const std::string& name,
                               const std::string& v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    GenApi::StringList_t symbolics;
    ptr->GetSymbolics(symbolics);
    bool known = false;
    for (const auto& entry : symbolics) {
      if (std::string(entry.c_str()) == v) {
        known = true;
        break;
      }
    }
    if (!known) return NodeError::kInvalidEnumEntry;
    ptr->FromString(v.c_str());
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetEnumEntries(
    const std::string& name, std::vector<std::string>* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    GenApi::StringList_t symbolics;
    ptr->GetSymbolics(symbolics);
    out->clear();
    for (const auto& entry : symbolics) {
      out->push_back(std::string(entry.c_str()));
    }
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::Execute(const std::string& name) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CCommandPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    ptr->Execute();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetEnumIntValue(const std::string& name,
                                       int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    *out = ptr->GetIntValue();
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::SetEnumIntValue(const std::string& name, int64_t v) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CEnumerationPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    ptr->SetIntValue(v);
  } catch (const Pylon::GenericException&) {
    return NodeError::kInvalidEnumEntry;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::IsCommandDone(const std::string& name,
                                     bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CCommandPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->IsDone();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetCategoryFeatures(
    const std::string& name, std::vector<std::string>* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CCategoryPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    GenApi::FeatureList_t features;
    ptr->GetFeatures(features);
    out->clear();
    for (auto* f : features) {
      if (f != nullptr) out->push_back(std::string(f->GetName().c_str()));
    }
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::IsSelector(const std::string& name, bool* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CSelectorPtr ptr(node);
  if (!ptr.IsValid()) {
    // Non-selector values still answer false when the node exists and is
    // value-like; unknown interface kinds stay WrongType.
    GenApi::CValuePtr value(node);
    if (value.IsValid()) {
      *out = false;
      return NodeError::kOk;
    }
    return NodeError::kWrongType;
  }
  try {
    *out = ptr->IsSelector();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetSelectedFeatures(
    const std::string& name, std::vector<std::string>* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CSelectorPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    GenApi::FeatureList_t features;
    ptr->GetSelectedFeatures(features);
    out->clear();
    for (auto* f : features) {
      if (f != nullptr) out->push_back(std::string(f->GetName().c_str()));
    }
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetSelectingFeatures(
    const std::string& name, std::vector<std::string>* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CSelectorPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    GenApi::FeatureList_t features;
    ptr->GetSelectingFeatures(features);
    out->clear();
    for (auto* f : features) {
      if (f != nullptr) out->push_back(std::string(f->GetName().c_str()));
    }
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetRegisterLength(const std::string& name,
                                         int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CRegisterPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->GetLength();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetRegisterAddress(const std::string& name,
                                          int64_t* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CRegisterPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    *out = ptr->GetAddress();
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::ReadRegister(const std::string& name,
                                    std::vector<uint8_t>* out) const {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CRegisterPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsReadable(node)) return NodeError::kNotReadable;
    const int64_t len = ptr->GetLength();
    out->resize(static_cast<size_t>(len));
    if (len > 0) ptr->Get(out->data(), len);
  } catch (const Pylon::GenericException&) {
    return NodeError::kNotReadable;
  } catch (const std::exception&) {
    return NodeError::kNotReadable;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::WriteRegister(const std::string& name,
                                     const std::vector<uint8_t>& data) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  GenApi::CRegisterPtr ptr(node);
  if (!ptr.IsValid()) return NodeError::kWrongType;
  try {
    if (!GenApi::IsWritable(node)) return NodeError::kNotWritable;
    if (!data.empty()) ptr->Set(data.data(), static_cast<int64_t>(data.size()));
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

void RealNodeMap::ClearCallbacks() {
  std::lock_guard<std::mutex> lock(callbacks_mutex_);
  for (auto& kv : callbacks_) {
    try {
      if (kv.second.node != nullptr) {
        kv.second.node->DeregisterCallback(kv.second.gen_handle);
      }
    } catch (...) {
      // Best effort during rebind/destruction.
    }
  }
  callbacks_.clear();
}

NodeError RealNodeMap::RegisterCallback(const std::string& name,
                                        int64_t* out_handle) {
  GenApi::INode* node = Lookup(name);
  if (node == nullptr) return NodeError::kUnknownNode;
  auto count = std::make_shared<std::atomic<int>>(0);
  GenApi::CNodeCallback* cb = nullptr;
  try {
    cb = new CounterCallback(node, count);
  } catch (...) {
    return NodeError::kCommandFailed;
  }
  GenApi::CallbackHandleType gen_handle = 0;
  try {
    gen_handle = node->RegisterCallback(cb);
  } catch (const Pylon::GenericException&) {
    delete cb;
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    delete cb;
    return NodeError::kCommandFailed;
  }
  // GenApi took ownership of cb; only the counter stays with us.
  std::lock_guard<std::mutex> lock(callbacks_mutex_);
  const int64_t id = next_callback_id_++;
  callbacks_[id] = CallbackRecord{node, gen_handle, count};
  *out_handle = id;
  return NodeError::kOk;
}

NodeError RealNodeMap::GetCallbackCount(int64_t handle, int* out) const {
  std::lock_guard<std::mutex> lock(callbacks_mutex_);
  auto it = callbacks_.find(handle);
  if (it == callbacks_.end()) return NodeError::kUnknownNode;
  *out = it->second.count ? it->second.count->load() : 0;
  return NodeError::kOk;
}

NodeError RealNodeMap::DeregisterCallback(int64_t handle) {
  CallbackRecord rec;
  {
    std::lock_guard<std::mutex> lock(callbacks_mutex_);
    auto it = callbacks_.find(handle);
    if (it == callbacks_.end()) return NodeError::kUnknownNode;
    rec = it->second;
    callbacks_.erase(it);
  }
  try {
    if (rec.node != nullptr) rec.node->DeregisterCallback(rec.gen_handle);
  } catch (const Pylon::GenericException&) {
    return NodeError::kCommandFailed;
  } catch (const std::exception&) {
    return NodeError::kCommandFailed;
  }
  return NodeError::kOk;
}

NodeError RealNodeMap::GetImageGeometry(int64_t* w, int64_t* h,
                                        std::string* pixel_format) const {
  NodeError e = GetInteger("Width", w);
  if (e != NodeError::kOk) return e;
  e = GetInteger("Height", h);
  if (e != NodeError::kOk) return e;
  return GetEnum("PixelFormat", pixel_format);
}

int RealNodeMap::BytesPerPixel(const std::string& pixel_format) {
  if (pixel_format == "RGB8") return 3;
  if (pixel_format == "Mono12") return 2;
  return 1;
}

}  // namespace node_pylon
