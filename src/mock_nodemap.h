// Mock GenICam nodemap without the pylon SDK.
// Pure C++ logic without N-API/V8, so it can later be reused for the
// React Native JSI bridge.
// A real pylon wiring (GenApi::INodeMap) only replaces the
// MockNodeMap methods; the N-API layer above stays stable.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace node_pylon {

// Distinct error keys for translation into JS exceptions.
enum class NodeError {
  kOk = 0,
  kUnknownNode,
  kWrongType,
  kNotReadable,
  kNotWritable,
  kOutOfRange,
  kInvalidEnumEntry,
  kCommandFailed,
};

const char* NodeErrorMessage(NodeError e);

// Supported GenICam types (MVP mirrors pypylon, without DataProcessing).
enum class NodeType {
  Integer,
  Float,
  Boolean,
  String,
  Enumeration,
  Command,
};

const char* NodeTypeName(NodeType t);

struct IntegerNode {
  int64_t value = 0;
  int64_t min = 0;
  int64_t max = 0;
  bool readable = true;
  bool writable = true;
};

struct FloatNode {
  double value = 0.0;
  double min = 0.0;
  double max = 0.0;
  bool readable = true;
  bool writable = true;
};

struct BoolNode {
  bool value = false;
  bool readable = true;
  bool writable = true;
};

struct StringNode {
  std::string value;
  bool readable = true;
  bool writable = true;
  size_t maxLen = 64;
};

struct EnumNode {
  std::string value;
  std::vector<std::string> entries;
  bool readable = true;
  bool writable = true;
};

struct CommandNode {
  // Counts executions (observable for tests/samples).
  int executeCount = 0;
  bool available = true;
};

// Deterministic mock state of the camera parameters.
// Defaults: 640x480 Mono8, exposure 5000 us, gain 0 dB.
class MockNodeMap {
 public:
  MockNodeMap();

  NodeError GetType(const std::string& name, NodeType* out) const;
  NodeError IsReadable(const std::string& name, bool* out) const;
  NodeError IsWritable(const std::string& name, bool* out) const;

  NodeError GetInteger(const std::string& name, int64_t* out) const;
  NodeError SetInteger(const std::string& name, int64_t v);
  NodeError GetIntegerLimits(const std::string& name, int64_t* minOut,
                             int64_t* maxOut) const;
  // Sets Integer/Float to max; returns the value set.
  NodeError TryMax(const std::string& name, double* outValue);

  NodeError GetFloat(const std::string& name, double* out) const;
  NodeError SetFloat(const std::string& name, double v);
  NodeError GetFloatLimits(const std::string& name, double* minOut,
                           double* maxOut) const;

  NodeError GetBoolean(const std::string& name, bool* out) const;
  NodeError SetBoolean(const std::string& name, bool v);

  NodeError GetString(const std::string& name, std::string* out) const;
  NodeError SetString(const std::string& name, const std::string& v);

  NodeError GetEnum(const std::string& name, std::string* out) const;
  NodeError SetEnum(const std::string& name, const std::string& v);
  NodeError GetEnumEntries(const std::string& name,
                           std::vector<std::string>* out) const;

  NodeError Execute(const std::string& name);

  // Hard-wired device info (mock device).
  void GetDeviceInfo(std::string* friendlyName, std::string* modelName,
                     std::string* serialNumber, std::string* vendorName,
                     std::string* deviceClass) const;

  // Current image geometry from Width/Height/PixelFormat.
  void GetImageGeometry(int64_t* w, int64_t* h, std::string* pixelFormat) const;
  // Bytes per pixel for the mock formats (Mono8=1, Mono12=2, RGB8=3).
  static int BytesPerPixel(const std::string& pixelFormat);

 private:
  std::map<std::string, IntegerNode> integers_;
  std::map<std::string, FloatNode> floats_;
  std::map<std::string, BoolNode> bools_;
  std::map<std::string, StringNode> strings_;
  std::map<std::string, EnumNode> enums_;
  std::map<std::string, CommandNode> commands_;
};

}  // namespace node_pylon
