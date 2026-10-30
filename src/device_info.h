// DeviceInfo N-API wrapper: full Pylon::CDeviceInfo + CInfoBase parity.
// Maps pypylon DeviceInfo.i / Info.i: typed accessors (PascalCase like
// pypylon, plus camelCase aliases for the discovery fields used by the
// existing facade), legacy Get/Set/IsAvailable methods, generic IProperties
// methods, comparison semantics (equals/lessThan), dict-style helpers
// (keys/values/items/toDict/update) and the GigE IP-config predicates.
// N-API only, no V8 internals; errors via ThrowTranslated.
#pragma once

#include <napi.h>

#include <string>

#include <pylon/PylonIncludes.h>

namespace node_pylon {

class DeviceInfoWrap : public Napi::ObjectWrap<DeviceInfoWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  // new DeviceInfo() or new DeviceInfo(otherDeviceInfo)
  explicit DeviceInfoWrap(const Napi::CallbackInfo& info);

  const Pylon::CDeviceInfo& Info() const { return info_; }
  void SetInfo(const Pylon::CDeviceInfo& info) { info_ = info; }

  // Non-throwing downcast for user-provided values: nullptr for anything
  // that is not a DeviceInfo instance (plain dicts included). Unlike
  // ObjectWrap::Unwrap it never leaves a pending exception behind.
  static DeviceInfoWrap* FromValue(const Napi::Value& value);

  // Filter helper shared with the discovery layer: accepts a DeviceInfo
  // instance or a plain object (dict). Plain-object values must be strings,
  // otherwise a TypeError is thrown (pypylon parity). Returns false with a
  // pending JS exception on type errors.
  static bool ToDeviceInfo(Napi::Env env, const Napi::Value& value,
                           Pylon::CDeviceInfo* out);

 private:
// Read-write CDeviceInfo/CInfoBase string properties: generates the
// accessor pair plus the legacy Set<Name>/Is<Name>Available methods
// (legacy getters reuse the accessor getters).
#define NODEPYLON_DEVICEINFO_RW_LIST(X)                                        \
  X(FriendlyName)                                                              \
  X(FullName)                                                                  \
  X(VendorName)                                                                \
  X(DeviceClass)                                                               \
  X(TLType)                                                                    \
  X(SerialNumber)                                                              \
  X(UserDefinedName)                                                           \
  X(ModelName)                                                                 \
  X(DeviceVersion)                                                             \
  X(DeviceFactory)                                                             \
  X(XMLSource)                                                                 \
  X(InterfaceID)                                                               \
  X(Address)                                                                   \
  X(IpAddress)                                                                 \
  X(SubnetAddress)                                                             \
  X(DefaultGateway)                                                            \
  X(SubnetMask)                                                                \
  X(PortNr)                                                                    \
  X(MacAddress)                                                                \
  X(Interface)                                                                 \
  X(IpConfigOptions)                                                           \
  X(IpConfigCurrent)                                                           \
  X(PortID)                                                                    \
  X(DeviceID)                                                                  \
  X(InitialBaudRate)                                                           \
  X(DeviceXMLFileOverride)                                                     \
  X(DeviceSpecificString)                                                      \
  X(PortSpecificString)
// Read-only properties (getter + IsAvailable only).
#define NODEPYLON_DEVICEINFO_RO_LIST(X)                                        \
  X(DeviceGUID)                                                                \
  X(ManufacturerInfo)                                                          \
  X(DeviceIdx)                                                                 \
  X(ProductId)                                                                 \
  X(VendorId)                                                                  \
  X(DriverKeyName)                                                             \
  X(UsbDriverType)                                                             \
  X(TransferMode)

#define NODEPYLON_DECLARE_RW_ACCESSOR(Name)                                    \
  Napi::Value Get##Name(const Napi::CallbackInfo& info);                       \
  void Set##Name(const Napi::CallbackInfo& info, const Napi::Value& value);     \
  Napi::Value Set##Name##Method(const Napi::CallbackInfo& info);               \
  Napi::Value Is##Name##Available(const Napi::CallbackInfo& info);
#define NODEPYLON_DECLARE_RO_ACCESSOR(Name)                                    \
  Napi::Value Get##Name(const Napi::CallbackInfo& info);                       \
  Napi::Value Is##Name##Available(const Napi::CallbackInfo& info);
  NODEPYLON_DEVICEINFO_RW_LIST(NODEPYLON_DECLARE_RW_ACCESSOR)
  NODEPYLON_DEVICEINFO_RO_LIST(NODEPYLON_DECLARE_RO_ACCESSOR)
#undef NODEPYLON_DECLARE_RW_ACCESSOR
#undef NODEPYLON_DECLARE_RO_ACCESSOR

  // Generic IProperties surface.
  Napi::Value GetPropertyNames(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyAvailable(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value SetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value IsSubset(const Napi::CallbackInfo& info);
  Napi::Value IsUserProvided(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyNotAvailable(const Napi::CallbackInfo& info);
  // Comparison semantics: operator== / CDeviceInfo::operator<.
  Napi::Value Equals(const Napi::CallbackInfo& info);
  Napi::Value LessThan(const Napi::CallbackInfo& info);
  // Dict-style helpers (pypylon %extend parity, method-based in JS).
  Napi::Value Keys(const Napi::CallbackInfo& info);
  Napi::Value Values(const Napi::CallbackInfo& info);
  Napi::Value Items(const Napi::CallbackInfo& info);
  Napi::Value ToDict(const Napi::CallbackInfo& info);
  Napi::Value Update(const Napi::CallbackInfo& info);
  // GigE IP-config predicates (throw GenericException without config).
  Napi::Value IsPersistentIpActive(const Napi::CallbackInfo& info);
  Napi::Value IsDhcpActive(const Napi::CallbackInfo& info);
  Napi::Value IsAutoIpActive(const Napi::CallbackInfo& info);
  Napi::Value IsPersistentIpSupported(const Napi::CallbackInfo& info);
  Napi::Value IsDhcpSupported(const Napi::CallbackInfo& info);
  Napi::Value IsAutoIpSupported(const Napi::CallbackInfo& info);

  Pylon::CDeviceInfo info_;
};

}  // namespace node_pylon
