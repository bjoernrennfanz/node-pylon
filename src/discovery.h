// Discovery N-API layer: TlFactory / TransportLayer / Interface /
// PylonDevice / TlInfo / InterfaceInfo (pypylon factory/transport parity).
// N-API only (ObjectWrap, no V8 internals); Pylon/GenApi/std exceptions are
// translated via errormap and never cross the boundary. Device lists are JS
// arrays; pypylon tuples map to arrays, (bool, value) tuples to 2-arrays.
#pragma once

#include <napi.h>

#include <functional>

#include <pylon/PylonIncludes.h>

namespace node_pylon {

// Shared helpers for the discovery layer.
namespace discovery {

// Builds a DeviceInfoList_t filter from a JS value: undefined/null means
// "no filter" (use the no-filter overload); a DeviceInfo instance or plain
// object means a single-entry filter; an array means one entry per element
// (each DeviceInfo or plain object with string values). Anything else, or
// dict values that are not strings, throws TypeError (pypylon parity).
// Returns true when the no-filter overload should be used.
bool BuildFilter(Napi::Env env, const Napi::Value& value,
                 Pylon::DeviceInfoList_t* out, bool* useNoFilter);

// Converts an access-mode JS value (undefined or number) to AccessModeSet.
// Default is Control. Non-numbers throw TypeError.
bool ToAccessMode(Napi::Env env, const Napi::Value& value,
                  Pylon::AccessModeSet* out);

}  // namespace discovery

// Empty interface base so ported tests can check the IDeviceFactory
// inheritance (TlFactory/TransportLayer/Interface derive from it).
class IDeviceFactoryWrap : public Napi::ObjectWrap<IDeviceFactoryWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit IDeviceFactoryWrap(const Napi::CallbackInfo& info);
};

// Value wrapper over Pylon::CTlInfo (copyable).
class TlInfoWrap : public Napi::ObjectWrap<TlInfoWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports,
                   const Napi::Function& baseProto);
  explicit TlInfoWrap(const Napi::CallbackInfo& info);

  // Non-throwing downcast (nullptr for non-instances, no pending exception).
  static TlInfoWrap* FromValue(const Napi::Value& value);

  const Pylon::CTlInfo& Info() const { return info_; }
  void SetInfo(const Pylon::CTlInfo& info) { info_ = info; }

 private:
#define NODEPYLON_TLINFO_RW_LIST(X) \
  X(FriendlyName)                   \
  X(FullName)                       \
  X(VendorName)                     \
  X(DeviceClass)                    \
  X(TLType)                         \
  X(FileName)                       \
  X(InfoID)                         \
  X(ModelName)                      \
  X(Version)
#define NODEPYLON_TLINFO_DECLARE_RW(Name)                                      \
  Napi::Value Get##Name(const Napi::CallbackInfo& info);                       \
  void Set##Name(const Napi::CallbackInfo& info, const Napi::Value& value);     \
  Napi::Value Set##Name##Method(const Napi::CallbackInfo& info);               \
  Napi::Value Is##Name##Available(const Napi::CallbackInfo& info);
  NODEPYLON_TLINFO_RW_LIST(NODEPYLON_TLINFO_DECLARE_RW)
#undef NODEPYLON_TLINFO_DECLARE_RW
  Napi::Value GetPropertyNames(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyAvailable(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value SetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value IsSubset(const Napi::CallbackInfo& info);
  Napi::Value IsUserProvided(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyNotAvailable(const Napi::CallbackInfo& info);
  Napi::Value Equals(const Napi::CallbackInfo& info);
  Napi::Value LessThan(const Napi::CallbackInfo& info);
  Napi::Value Keys(const Napi::CallbackInfo& info);
  Napi::Value Values(const Napi::CallbackInfo& info);
  Napi::Value Items(const Napi::CallbackInfo& info);
  Napi::Value ToDict(const Napi::CallbackInfo& info);
  Napi::Value Update(const Napi::CallbackInfo& info);

  Pylon::CTlInfo info_;
};

// Value wrapper over Pylon::CInterfaceInfo (copyable).
class InterfaceInfoWrap : public Napi::ObjectWrap<InterfaceInfoWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports,
                   const Napi::Function& baseProto);
  explicit InterfaceInfoWrap(const Napi::CallbackInfo& info);

  // Non-throwing downcast (nullptr for non-instances, no pending exception).
  static InterfaceInfoWrap* FromValue(const Napi::Value& value);

  const Pylon::CInterfaceInfo& Info() const { return info_; }
  void SetInfo(const Pylon::CInterfaceInfo& info) { info_ = info; }

 private:
#define NODEPYLON_IFINFO_RW_LIST(X) \
  X(FriendlyName)                  \
  X(FullName)                      \
  X(VendorName)                    \
  X(DeviceClass)                   \
  X(TLType)                        \
  X(InterfaceID)
#define NODEPYLON_IFINFO_DECLARE_RW(Name)                                      \
  Napi::Value Get##Name(const Napi::CallbackInfo& info);                       \
  void Set##Name(const Napi::CallbackInfo& info, const Napi::Value& value);     \
  Napi::Value Set##Name##Method(const Napi::CallbackInfo& info);               \
  Napi::Value Is##Name##Available(const Napi::CallbackInfo& info);
  NODEPYLON_IFINFO_RW_LIST(NODEPYLON_IFINFO_DECLARE_RW)
#undef NODEPYLON_IFINFO_DECLARE_RW
  Napi::Value GetPropertyNames(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyAvailable(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value SetPropertyValue(const Napi::CallbackInfo& info);
  Napi::Value IsSubset(const Napi::CallbackInfo& info);
  Napi::Value IsUserProvided(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyNotAvailable(const Napi::CallbackInfo& info);
  Napi::Value Equals(const Napi::CallbackInfo& info);
  Napi::Value LessThan(const Napi::CallbackInfo& info);
  Napi::Value Keys(const Napi::CallbackInfo& info);
  Napi::Value Values(const Napi::CallbackInfo& info);
  Napi::Value Items(const Napi::CallbackInfo& info);
  Napi::Value ToDict(const Napi::CallbackInfo& info);
  Napi::Value Update(const Napi::CallbackInfo& info);

  Pylon::CInterfaceInfo info_;
};

// Handle wrapper over Pylon::IPylonDevice (explicit DestroyDevice parity).
class PylonDeviceWrap : public Napi::ObjectWrap<PylonDeviceWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports);
  explicit PylonDeviceWrap(const Napi::CallbackInfo& info);
  ~PylonDeviceWrap() override;

  enum class CreatorKind { kFactory, kTransportLayer, kInterface };

  // Attaches a created device. The owner JS object (TransportLayer or
  // Interface wrapper, empty for factory devices) is referenced to keep the
  // creator alive until this device is destroyed. Returns false with a
  // pending JS exception when PylonInitialize fails.
  bool Attach(Napi::Env env, Pylon::IPylonDevice* device, CreatorKind kind,
              void* creator, const Napi::Value& owner);
  bool IsDestroyed() const { return destroyed_; }
  void DestroyNative();
  static PylonDeviceWrap* FromValue(const Napi::Value& value);

 private:
  Napi::Value GetDeviceInfo(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value Destroy(const Napi::CallbackInfo& info);

  Pylon::IPylonDevice* device_ = nullptr;
  CreatorKind kind_ = CreatorKind::kFactory;
  void* creator_ = nullptr;
  Napi::ObjectReference owner_;
  bool destroyed_ = true;
  bool lifecycleHeld_ = false;
};

// Handle wrapper over Pylon::ITransportLayer (CreateTl/ReleaseTl parity).
class TransportLayerWrap : public Napi::ObjectWrap<TransportLayerWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports,
                   const Napi::Function& baseProto);
  explicit TransportLayerWrap(const Napi::CallbackInfo& info);
  ~TransportLayerWrap() override;

  // Creates a JS wrapper around a Created TL (takes ownership).
  static Napi::Object Create(Napi::Env env, Pylon::ITransportLayer* tl);
  bool IsReleased() const { return released_; }
  Pylon::ITransportLayer* Get() const { return tl_; }
  void ReleaseNative();
  static TransportLayerWrap* FromValue(const Napi::Value& value);

 private:
  Napi::Value GetTlInfo(const Napi::CallbackInfo& info);
  Napi::Value EnumerateDevices(const Napi::CallbackInfo& info);
  Napi::Value EnumerateInterfaces(const Napi::CallbackInfo& info);
  Napi::Value CreateInterface(const Napi::CallbackInfo& info);
  Napi::Value DestroyInterface(const Napi::CallbackInfo& info);
  Napi::Value CreateDevice(const Napi::CallbackInfo& info);
  Napi::Value CreateFirstDevice(const Napi::CallbackInfo& info);
  Napi::Value DestroyDevice(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessible(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessibleInfo(const Napi::CallbackInfo& info);
  Napi::Value GetNodeMapNumNodes(const Napi::CallbackInfo& info);
  Napi::Value Release(const Napi::CallbackInfo& info);

  Pylon::ITransportLayer* tl_ = nullptr;
  bool released_ = true;
  bool lifecycleHeld_ = false;
};

// Handle wrapper over Pylon::IInterface (CreateInterface parity).
class InterfaceWrap : public Napi::ObjectWrap<InterfaceWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports,
                   const Napi::Function& baseProto);
  explicit InterfaceWrap(const Napi::CallbackInfo& info);
  ~InterfaceWrap() override;

  static Napi::Object Create(Napi::Env env, Pylon::IInterface* iface,
                             Pylon::ITransportLayer* parent,
                             const Napi::Value& parentObj);
  bool IsDestroyed() const { return destroyed_; }
  void DestroyNative();
  static InterfaceWrap* FromValue(const Napi::Value& value);

 private:
  Napi::Value GetInterfaceInfo(const Napi::CallbackInfo& info);
  Napi::Value EnumerateDevices(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  Napi::Value GetNodeMapNumNodes(const Napi::CallbackInfo& info);
  Napi::Value CreateDevice(const Napi::CallbackInfo& info);
  Napi::Value CreateFirstDevice(const Napi::CallbackInfo& info);
  Napi::Value DestroyDevice(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessible(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessibleInfo(const Napi::CallbackInfo& info);
  Napi::Value Destroy(const Napi::CallbackInfo& info);

  Pylon::IInterface* iface_ = nullptr;
  Pylon::ITransportLayer* parent_ = nullptr;
  Napi::ObjectReference parentObj_;
  bool destroyed_ = true;
  bool lifecycleHeld_ = false;
};

// Stateless TlFactory facade (all methods delegate to the singleton).
class TlFactoryWrap : public Napi::ObjectWrap<TlFactoryWrap> {
 public:
  static Napi::FunctionReference constructor;
  static void Init(Napi::Env env, Napi::Object exports,
                   const Napi::Function& baseProto);
  explicit TlFactoryWrap(const Napi::CallbackInfo& info);

  // Returns the cached singleton JS object (identity-stable GetInstance).
  static Napi::Value GetInstance(const Napi::CallbackInfo& info);

 private:
  Napi::Value EnumerateTls(const Napi::CallbackInfo& info);
  Napi::Value CreateTl(const Napi::CallbackInfo& info);
  Napi::Value ReleaseTl(const Napi::CallbackInfo& info);
  Napi::Value EnumerateDevices(const Napi::CallbackInfo& info);
  Napi::Value CreateDevice(const Napi::CallbackInfo& info);
  Napi::Value CreateFirstDevice(const Napi::CallbackInfo& info);
  Napi::Value DestroyDevice(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessible(const Napi::CallbackInfo& info);
  Napi::Value IsDeviceAccessibleInfo(const Napi::CallbackInfo& info);

  static Napi::ObjectReference singleton_;
};

// Registers the discovery classes, TlFactory singleton accessor and all
// discovery constants (device classes, TL types, property keys, access
// modes, accessibility states) on exports.
void InitDiscovery(Napi::Env env, Napi::Object exports);

}  // namespace node_pylon
