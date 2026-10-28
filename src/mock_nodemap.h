// Mock-GenICam-Nodemap ohne pylon-SDK (ZAU-151).
// Reine C++-Logik ohne N-API/V8, damit sie spaeter fuer die
// React-Native-JSI-Bruecke (ZAU-153) wiederverwendet werden kann.
// Echte pylon-Anbindung (GenApi::INodeMap) ersetzt nur die
// MockNodeMap-Methoden; die N-API-Schicht darueber bleibt stabil.
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace node_pylon {

// Eindeutige Fehlerschluessel fuer die JS-Exception-Uebersetzung.
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

// Unterstuetzte GenICam-Typen (MVP analog pypylon, ohne DataProcessing).
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
  // Zaehlt Ausfuehrungen (fuer Tests/Samples beobachtbar).
  int executeCount = 0;
  bool available = true;
};

// Deterministischer Mock-Stand der Kamera-Parameter.
// Defaults: 640x480 Mono8, Exposure 5000 us, Gain 0 dB.
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
  // Setzt Integer/Float auf Max; gibt den gesetzten Wert zurueck.
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

  // Fest verdrahtete DeviceInfo (Mock-Geraet).
  void GetDeviceInfo(std::string* friendlyName, std::string* modelName,
                     std::string* serialNumber, std::string* vendorName,
                     std::string* deviceClass) const;

  // Aktuelle Bildgeometrie aus Width/Height/PixelFormat.
  void GetImageGeometry(int64_t* w, int64_t* h, std::string* pixelFormat) const;
  // Bytes pro Pixel fuer die Mock-Formate (Mono8=1, Mono12=2, RGB8=3).
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
