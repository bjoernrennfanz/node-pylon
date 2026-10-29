// RealNodeMap: GenICam node access against the open camera, the way pypylon
// exposes it (TlFactory -> InstantCamera -> GetNodeMap -> typed access).
// All reads/writes reach GenApi; errors map to NodeError deterministically
// (pre-validated where possible instead of parsing exception texts).
#include "node_map.h"

#include <pylon/PylonIncludes.h>

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
