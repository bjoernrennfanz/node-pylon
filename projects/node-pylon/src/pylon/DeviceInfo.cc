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

#include "deviceinfo.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> DeviceInfoWrap::prototype;
Nan::Persistent<Function> DeviceInfoWrap::constructor;

// Supported implementations
// CDeviceInfo()
// CDeviceInfo(CDeviceInfo& const arg0)
DeviceInfoWrap::DeviceInfoWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_DeviceInfo(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CDeviceInfo()
        m_DeviceInfo = new CDeviceInfo();
    }
    else if (info[0]->IsObject())
    {
        gcstring info0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
        if (info0_constructor != "CDeviceInfo")
        {
            ThrowException(Exception::TypeError(String::New("CDeviceInfo::CDeviceInfo: bad argument")));
        }

        // Unwrap obj
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // CDeviceInfo(CDeviceInfo& const arg0)
        m_DeviceInfo = new CDeviceInfo(*arg0);
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
    Nan::SetPrototypeMethod(tpl, "getSerialNumber", GetSerialNumber);
    Nan::SetPrototypeMethod(tpl, "setSerialNumber", SetSerialNumber);
    Nan::SetPrototypeMethod(tpl, "isSerialNumberAvailable", IsSerialNumberAvailable);
    Nan::SetPrototypeMethod(tpl, "getUserDefinedName", GetUserDefinedName);
    Nan::SetPrototypeMethod(tpl, "setUserDefinedName", SetUserDefinedName);
    Nan::SetPrototypeMethod(tpl, "isUserDefinedNameAvailable", IsUserDefinedNameAvailable);
    Nan::SetPrototypeMethod(tpl, "getModelName", GetModelName);
    Nan::SetPrototypeMethod(tpl, "setModelName", SetModelName);
    Nan::SetPrototypeMethod(tpl, "isModelNameAvailable", IsModelNameAvailable);
    Nan::SetPrototypeMethod(tpl, "getDeviceVersion", GetDeviceVersion);
    Nan::SetPrototypeMethod(tpl, "setDeviceVersion", SetDeviceVersion);
    Nan::SetPrototypeMethod(tpl, "isDeviceVersionAvailable", IsDeviceVersionAvailable);
    Nan::SetPrototypeMethod(tpl, "getDeviceFactory", GetDeviceFactory);
    Nan::SetPrototypeMethod(tpl, "setDeviceFactory", SetDeviceFactory);
    Nan::SetPrototypeMethod(tpl, "isDeviceFactoryAvailable", IsDeviceFactoryAvailable);
    Nan::SetPrototypeMethod(tpl, "getXMLSource", GetXMLSource);
    Nan::SetPrototypeMethod(tpl, "setXMLSource", SetXMLSource);
    Nan::SetPrototypeMethod(tpl, "isXMLSourceAvailable", IsXMLSourceAvailable);
    Nan::SetPrototypeMethod(tpl, "setFriendlyName", SetFriendlyName);
    Nan::SetPrototypeMethod(tpl, "setFullName", SetFullName);
    Nan::SetPrototypeMethod(tpl, "setVendorName", SetVendorName);
    Nan::SetPrototypeMethod(tpl, "setDeviceClass", SetDeviceClass);
    Nan::SetPrototypeMethod(tpl, "setPropertyValue", SetPropertyValue);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CDeviceInfo").ToLocalChecked(), function);
}
