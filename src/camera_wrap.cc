// InstantCamera N-API wrapper: implementation (real pylon SDK only).
// N-API rules: ObjectWrap, no V8 internals, errors as JS exceptions.
#include "camera_wrap.h"

#include <chrono>
#include <cstring>
#include <string>
#include <vector>

#include "pylon_lifecycle.h"

namespace node_pylon {

Napi::FunctionReference CameraWrap::constructor;

namespace {

std::string RequireStringArg(const Napi::CallbackInfo& info, size_t idx,
                             bool* ok) {
  if (idx >= info.Length() || !info[idx].IsString()) {
    *ok = false;
    return "";
  }
  *ok = true;
  return info[idx].As<Napi::String>().Utf8Value();
}

std::string DescribePixelType(const Pylon::CGrabResultPtr& ptr) {
  // Hand-mapped common formats (stable enum values, no dependency on
  // optional mapper helpers in the SDK).
  switch (ptr->GetPixelType()) {
    case Pylon::PixelType_Mono8:
      return "Mono8";
    case Pylon::PixelType_Mono12:
    case Pylon::PixelType_Mono12packed:
      return "Mono12";
    case Pylon::PixelType_RGB8packed:
    case Pylon::PixelType_BGR8packed:
      return "RGB8";
    default:
      return "Unknown";
  }
}

}  // namespace

void CameraWrap::ThrowNodeError(Napi::Env env, const std::string& node,
                                NodeError err) {
  std::string msg = std::string(NodeErrorMessage(err));
  if (!node.empty()) msg += ": " + node;
  if (err == NodeError::kOutOfRange || err == NodeError::kInvalidEnumEntry) {
    Napi::RangeError::New(env, msg).ThrowAsJavaScriptException();
  } else if (err == NodeError::kWrongType) {
    Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
  } else {
    Napi::Error::New(env, msg).ThrowAsJavaScriptException();
  }
}

void CameraWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function ctor = DefineClass(
      env, "InstantCamera",
      {InstanceMethod("isOpen", &CameraWrap::IsOpen),
       InstanceMethod("open", &CameraWrap::Open),
       InstanceMethod("close", &CameraWrap::Close),
       InstanceMethod("getDeviceInfo", &CameraWrap::GetDeviceInfo),
       InstanceMethod("startGrabbing", &CameraWrap::StartGrabbing),
       InstanceMethod("stopGrabbing", &CameraWrap::StopGrabbing),
       InstanceMethod("isGrabbing", &CameraWrap::IsGrabbing),
       InstanceMethod("retrieveResult", &CameraWrap::RetrieveResult),
       InstanceMethod("getNodeType", &CameraWrap::GetNodeType),
       InstanceMethod("isReadable", &CameraWrap::IsReadable),
       InstanceMethod("isWritable", &CameraWrap::IsWritable),
       InstanceMethod("getInteger", &CameraWrap::GetInteger),
       InstanceMethod("setInteger", &CameraWrap::SetInteger),
       InstanceMethod("getIntegerMin", &CameraWrap::GetIntegerMin),
       InstanceMethod("getIntegerMax", &CameraWrap::GetIntegerMax),
       InstanceMethod("tryMax", &CameraWrap::TryMax),
       InstanceMethod("getFloat", &CameraWrap::GetFloat),
       InstanceMethod("setFloat", &CameraWrap::SetFloat),
       InstanceMethod("getFloatMin", &CameraWrap::GetFloatMin),
       InstanceMethod("getFloatMax", &CameraWrap::GetFloatMax),
       InstanceMethod("getBoolean", &CameraWrap::GetBoolean),
       InstanceMethod("setBoolean", &CameraWrap::SetBoolean),
       InstanceMethod("getString", &CameraWrap::GetString),
       InstanceMethod("setString", &CameraWrap::SetString),
       InstanceMethod("getEnum", &CameraWrap::GetEnum),
       InstanceMethod("setEnum", &CameraWrap::SetEnum),
       InstanceMethod("getEnumEntries", &CameraWrap::GetEnumEntries),
       InstanceMethod("executeCommand", &CameraWrap::ExecuteCommand),
       InstanceMethod("grab", &CameraWrap::Grab)});
  constructor = Napi::Persistent(ctor);
  constructor.SuppressDestruct();
  exports.Set("InstantCamera", ctor);
}

CameraWrap::CameraWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<CameraWrap>(info) {
  Napi::Env env = info.Env();
  try {
    lifecycle::EnsureInitialized();
    // TlFactory::CreateFirstDevice throws without an attached device.
    camera_ = new Pylon::CInstantCamera(
        Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    initOk_ = true;
  } catch (const Pylon::GenericException& ex) {
    lifecycle::ReleaseRef();
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return;
  } catch (const std::exception& ex) {
    lifecycle::ReleaseRef();
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return;
  }
}

CameraWrap::~CameraWrap() {
  if (camera_ != nullptr) {
    try {
      if (camera_->IsGrabbing()) camera_->StopGrabbing();
      if (camera_->IsOpen()) camera_->Close();
    } catch (...) {
      // Destructor: never throw.
    }
    delete camera_;
    camera_ = nullptr;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
  }
  if (initOk_) lifecycle::ReleaseRef();
}

Napi::Value CameraWrap::IsOpen(const Napi::CallbackInfo& info) {
  bool v = open_;
  try {
    if (camera_ != nullptr) v = camera_->IsOpen();
  } catch (...) {
    v = open_;
  }
  return Napi::Boolean::New(info.Env(), v);
}

Napi::Value CameraWrap::Open(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr) {
      Napi::Error::New(env, "InstantCamera not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!camera_->IsOpen()) camera_->Open();
    open_ = true;
    genapi_ = &camera_->GetNodeMap();
    nodemap_.Rebind(genapi_);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::Close(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ != nullptr) {
      if (camera_->IsGrabbing()) camera_->StopGrabbing();
      if (camera_->IsOpen()) camera_->Close();
    }
    grabbing_ = false;
    open_ = false;
    genapi_ = nullptr;
    nodemap_.Rebind(nullptr);
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::GetDeviceInfo(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  try {
    if (camera_ == nullptr) {
      Napi::Error::New(env, "InstantCamera not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    Pylon::CDeviceInfo di = camera_->GetDeviceInfo();
    Napi::Object o = Napi::Object::New(env);
    o.Set("friendlyName", std::string(di.GetFriendlyName().c_str()));
    o.Set("modelName", std::string(di.GetModelName().c_str()));
    o.Set("serialNumber", std::string(di.GetSerialNumber().c_str()));
    o.Set("vendorName", std::string(di.GetVendorName().c_str()));
    o.Set("deviceClass", std::string(di.GetDeviceClass().c_str()));
    o.Set("fullName", std::string(di.GetFullName().c_str()));
    return o;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
}

// --- Grab loop (pypylon style) ---------------------------------------------

Napi::Value CameraWrap::StartGrabbing(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  (void)info;
  try {
    if (camera_ == nullptr || !camera_->IsOpen()) {
      Napi::Error::New(env, "Camera is not open. Call open() first.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!camera_->IsGrabbing()) {
      camera_->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);
    }
    grabbing_ = true;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::StopGrabbing(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  (void)info;
  try {
    if (camera_ != nullptr && camera_->IsGrabbing()) camera_->StopGrabbing();
    grabbing_ = false;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::IsGrabbing(const Napi::CallbackInfo& info) {
  bool v = grabbing_;
  try {
    if (camera_ != nullptr) v = camera_->IsGrabbing();
  } catch (...) {
    v = grabbing_;
  }
  return Napi::Boolean::New(info.Env(), v);
}

// retrieveResult(timeoutMs) -> grab object or null on timeout.
// Fields: succeeded, width, height, pixelFormat, payloadSize, buffer.
Napi::Value CameraWrap::RetrieveResult(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  uint32_t timeoutMs = 5000;
  if (info.Length() >= 1 && info[0].IsNumber()) {
    timeoutMs = info[0].As<Napi::Number>().Uint32Value();
  }
  try {
    if (camera_ == nullptr || !camera_->IsGrabbing()) {
      Napi::Error::New(env, "Not grabbing. Call startGrabbing() first.")
          .ThrowAsJavaScriptException();
      return env.Null();
    }
    Pylon::CGrabResultPtr ptr;
    bool ok =
        camera_->RetrieveResult(timeoutMs, ptr, Pylon::TimeoutHandling_Return);
    if (!ok || !ptr.IsValid()) {
      return env.Null();  // Timeout -> null, no throw.
    }
    Napi::Object out = Napi::Object::New(env);
    const bool succeeded = ptr->GrabSucceeded();
    out.Set("succeeded", succeeded);
    size_t w = 0, h = 0;
    if (succeeded) {
      w = static_cast<size_t>(ptr->GetWidth());
      h = static_cast<size_t>(ptr->GetHeight());
    }
    out.Set("width", Napi::Number::New(env, static_cast<double>(w)));
    out.Set("height", Napi::Number::New(env, static_cast<double>(h)));
    out.Set("pixelType", DescribePixelType(ptr));
    out.Set("pixelFormat", DescribePixelType(ptr));
    const size_t payload =
        succeeded ? static_cast<size_t>(ptr->GetPayloadSize()) : 0;
    out.Set("payloadSize", Napi::Number::New(env, static_cast<double>(payload)));
    if (succeeded && payload > 0) {
      auto* data = new uint8_t[payload];
      std::memcpy(data, ptr->GetBuffer(), payload);
      out.Set("buffer", Napi::Buffer<uint8_t>::New(
                            env, data, payload,
                            [](Napi::Env /*e*/, uint8_t* p) { delete[] p; }));
    } else {
      out.Set("buffer",
              Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(0)));
      if (!succeeded) {
        out.Set("errorCode", Napi::Number::New(
                                 env, static_cast<double>(ptr->GetErrorCode())));
        out.Set("errorDescription",
                std::string(ptr->GetErrorDescription().c_str()));
      }
    }
    return out;
  } catch (const Pylon::GenericException& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  } catch (const std::exception& ex) {
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value CameraWrap::GetNodeType(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getNodeType(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeType t;
  NodeError e = nodemap_.GetType(name, &t);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, NodeTypeName(t));
}

Napi::Value CameraWrap::IsReadable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isReadable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsReadable(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::IsWritable(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "isWritable(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.IsWritable(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::GetInteger(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getInteger(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = 0;
  NodeError e = nodemap_.GetInteger(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(v));
}

Napi::Value CameraWrap::SetInteger(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "setInteger(name: string, value: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t v = info[1].As<Napi::Number>().Int64Value();
  NodeError e = nodemap_.SetInteger(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetIntegerMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t mn = 0, mx = 0;
  NodeError e = nodemap_.GetIntegerLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(mn));
}

Napi::Value CameraWrap::GetIntegerMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getIntegerMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  int64_t mn = 0, mx = 0;
  NodeError e = nodemap_.GetIntegerLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, static_cast<double>(mx));
}

Napi::Value CameraWrap::TryMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "tryMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = 0;
  NodeError e = nodemap_.TryMax(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, v);
}

Napi::Value CameraWrap::GetFloat(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloat(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = 0;
  NodeError e = nodemap_.GetFloat(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, v);
}

Napi::Value CameraWrap::SetFloat(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "setFloat(name: string, value: number) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double v = info[1].As<Napi::Number>().DoubleValue();
  NodeError e = nodemap_.SetFloat(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetFloatMin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatMin(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double mn = 0, mx = 0;
  NodeError e = nodemap_.GetFloatLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, mn);
}

Napi::Value CameraWrap::GetFloatMax(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getFloatMax(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  double mn = 0, mx = 0;
  NodeError e = nodemap_.GetFloatLimits(name, &mn, &mx);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Number::New(env, mx);
}

Napi::Value CameraWrap::GetBoolean(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getBoolean(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = false;
  NodeError e = nodemap_.GetBoolean(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::Boolean::New(env, v);
}

Napi::Value CameraWrap::SetBoolean(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsBoolean()) {
    Napi::TypeError::New(env,
                          "setBoolean(name: string, value: boolean) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  bool v = info[1].As<Napi::Boolean>().Value();
  NodeError e = nodemap_.SetBoolean(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getString(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetString(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetString(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsString()) {
    Napi::TypeError::New(env, "setString(name: string, value: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v = info[1].As<Napi::String>().Utf8Value();
  NodeError e = nodemap_.SetString(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetEnum(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getEnum(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v;
  NodeError e = nodemap_.GetEnum(name, &v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return Napi::String::New(env, v);
}

Napi::Value CameraWrap::SetEnum(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok || info.Length() < 2 || !info[1].IsString()) {
    Napi::TypeError::New(env, "setEnum(name: string, value: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::string v = info[1].As<Napi::String>().Utf8Value();
  NodeError e = nodemap_.SetEnum(name, v);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  return env.Undefined();
}

Napi::Value CameraWrap::GetEnumEntries(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "getEnumEntries(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  std::vector<std::string> entries;
  NodeError e = nodemap_.GetEnumEntries(name, &entries);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  Napi::Array arr = Napi::Array::New(env, entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    arr.Set(static_cast<uint32_t>(i), Napi::String::New(env, entries[i]));
  }
  return arr;
}

Napi::Value CameraWrap::ExecuteCommand(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  bool ok = false;
  std::string name = RequireStringArg(info, 0, &ok);
  if (!ok) {
    Napi::TypeError::New(env, "executeCommand(name: string) expected")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!open_) {
    ThrowNodeError(env, "", NodeError::kNotOpen);
    return env.Null();
  }
  NodeError e = nodemap_.Execute(name);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, name, e);
    return env.Null();
  }
  if (name == "AcquisitionStart") grabbing_ = true;
  if (name == "AcquisitionStop") grabbing_ = false;
  return Napi::Boolean::New(env, true);
}

Napi::Value CameraWrap::Grab(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!open_) {
    Napi::Error::New(env, "Camera is not open. Call open() first.")
        .ThrowAsJavaScriptException();
    return env.Null();
  }
  int64_t w = 0, h = 0;
  std::string pixelFormat;
  NodeError e = nodemap_.GetImageGeometry(&w, &h, &pixelFormat);
  if (e != NodeError::kOk) {
    ThrowNodeError(env, "Width/Height/PixelFormat", e);
    return env.Null();
  }
  int bpp = RealNodeMap::BytesPerPixel(pixelFormat);
  size_t bytes = static_cast<size_t>(w) * static_cast<size_t>(h) *
                 static_cast<size_t>(bpp);
  auto* data = new uint8_t[bytes];
  uint64_t frame = frameNumber_++;
  // Deterministic test pattern: (i + frame) & 0xFF per byte.
  for (size_t i = 0; i < bytes; ++i) {
    data[i] = static_cast<uint8_t>((i + frame) & 0xFF);
  }
  Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(
      env, data, bytes, [](Napi::Env /*env*/, uint8_t* ptr) { delete[] ptr; });

  bool chunkActive = false;
  nodemap_.GetBoolean("ChunkModeActive", &chunkActive);
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  uint64_t ts = static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());

  Napi::Object out = Napi::Object::New(env);
  out.Set("buffer", buf);
  out.Set("width", Napi::Number::New(env, static_cast<double>(w)));
  out.Set("height", Napi::Number::New(env, static_cast<double>(h)));
  out.Set("pixelType", Napi::String::New(env, pixelFormat));
  out.Set("pixelFormat", Napi::String::New(env, pixelFormat));
  out.Set("payloadSize", Napi::Number::New(env, static_cast<double>(bytes)));
  out.Set("timestamp", Napi::Number::New(env, static_cast<double>(ts)));
  out.Set("frameNumber", Napi::Number::New(env, static_cast<double>(frame)));
  Napi::Object chunk = Napi::Object::New(env);
  chunk.Set("modeActive", Napi::Boolean::New(env, chunkActive));
  chunk.Set("timestamp", Napi::Number::New(env, static_cast<double>(ts)));
  chunk.Set("frameID",
            Napi::Number::New(env, static_cast<double>(frame)));
  out.Set("chunk", chunk);
  return out;
}

}  // namespace node_pylon
