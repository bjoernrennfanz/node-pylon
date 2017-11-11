// MIT License
// 
// Copyright (c) 2017 Björn Rennfanz <bjoern@fam-rennfanz.de>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// ----------------------------------------------------------------------------
// This is auto generated code by pylon-node-gen.
// Do not edit this file or all your changes will be lost after re-generation.
// ----------------------------------------------------------------------------

#include "ideviceinfo.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> DeviceInfoWrap::prototype;
Nan::Persistent<Function> DeviceInfoWrap::constructor;

// Supported implementations
// IDeviceInfo()
// IDeviceInfo(IDeviceInfo& const arg0)
DeviceInfoWrap::DeviceInfoWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_DeviceInfo(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IDeviceInfo()
        m_DeviceInfo = new IDeviceInfo();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IDeviceInfo")))
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        IDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // IDeviceInfo(IDeviceInfo& const arg0)
        m_DeviceInfo = new IDeviceInfo(*arg0);
    }
}

DeviceInfoWrap::~DeviceInfoWrap()
{
    delete m_DeviceInfo;
}

NAN_MODULE_INIT(DeviceInfoWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("DeviceInfoWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getModelName", GetModelName);
    Nan::SetPrototypeMethod(tpl, "getVendorName", GetVendorName);
    Nan::SetPrototypeMethod(tpl, "getToolTip", GetToolTip);
    Nan::SetPrototypeMethod(tpl, "getStandardNameSpace", GetStandardNameSpace);
    Nan::SetPrototypeMethod(tpl, "getGenApiVersion", GetGenApiVersion);
    Nan::SetPrototypeMethod(tpl, "getSchemaVersion", GetSchemaVersion);
    Nan::SetPrototypeMethod(tpl, "getDeviceVersion", GetDeviceVersion);
    Nan::SetPrototypeMethod(tpl, "getProductGuid", GetProductGuid);
    Nan::SetPrototypeMethod(tpl, "getVersionGuid", GetVersionGuid);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IDeviceInfo").ToLocalChecked(), function);
}

NAN_METHOD(DeviceInfoWrap::GetDeviceVersion)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "Version_t")))
    {
        // Unwrap object
        Version_tWrap* arg0_wrap = ObjectWrap::Unwrap<Version_tWrap>(info[0]->ToObject());
        Version_t* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(DeviceInfoWrap::GetGenApiVersion)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "Version_t")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "unsigned short")))
    {
        // Unwrap object
        Version_tWrap* arg0_wrap = ObjectWrap::Unwrap<Version_tWrap>(info[0]->ToObject());
        Version_t* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        unsigned shortWrap* arg1_wrap = ObjectWrap::Unwrap<unsigned shortWrap>(info[1]->ToObject());
        unsigned short* arg1 = arg1_wrap->GetWrapped();
    }
}

NAN_METHOD(DeviceInfoWrap::GetModelName)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(DeviceInfoWrap::GetProductGuid)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(DeviceInfoWrap::GetSchemaVersion)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "Version_t")))
    {
        // Unwrap object
        Version_tWrap* arg0_wrap = ObjectWrap::Unwrap<Version_tWrap>(info[0]->ToObject());
        Version_t* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(DeviceInfoWrap::GetStandardNameSpace)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(DeviceInfoWrap::GetToolTip)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(DeviceInfoWrap::GetVendorName)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(DeviceInfoWrap::GetVersionGuid)
{
    DeviceInfoWrap* wrappedDeviceInfo = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
    IDeviceInfo* deviceInfo = wrappedDeviceInfo->GetWrapped();

    if (info.Length() == 0)
    {
    }
}
