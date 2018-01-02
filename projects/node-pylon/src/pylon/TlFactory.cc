// MIT License
// 
// Copyright (c) 2017 - 2018 Björn Rennfanz <bjoern@fam-rennfanz.de>
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

#include "tlfactory.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> TlFactoryWrap::prototype;
Nan::Persistent<Function> TlFactoryWrap::constructor;

// Supported implementations
// CTlFactory()
// CTlFactory(CTlFactory& const arg0)
TlFactoryWrap::TlFactoryWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_TlFactory(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CTlFactory()
        m_TlFactory = new CTlFactory();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CTlFactory")))
    {
        // Unwrap object
        TlFactoryWrap* arg0_wrap = ObjectWrap::Unwrap<TlFactoryWrap>(info[0]->ToObject());
        CTlFactory* arg0 = arg0_wrap->GetWrapped();

        // CTlFactory(CTlFactory& const arg0)
        m_TlFactory = new CTlFactory(*arg0);
    }
}

TlFactoryWrap::~TlFactoryWrap()
{
    delete m_TlFactory;
}

NAN_MODULE_INIT(TlFactoryWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("TlFactoryWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getInstance", GetInstance);
    Nan::SetPrototypeMethod(tpl, "enumerateTls", EnumerateTls);
    Nan::SetPrototypeMethod(tpl, "createTl", CreateTl);
    Nan::SetPrototypeMethod(tpl, "releaseTl", ReleaseTl);
    Nan::SetPrototypeMethod(tpl, "enumerateDevices", EnumerateDevices);
    Nan::SetPrototypeMethod(tpl, "createDevice", CreateDevice);
    Nan::SetPrototypeMethod(tpl, "createFirstDevice", CreateFirstDevice);
    Nan::SetPrototypeMethod(tpl, "destroyDevice", DestroyDevice);
    Nan::SetPrototypeMethod(tpl, "isDeviceAccessible", IsDeviceAccessible);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CTlFactory").ToLocalChecked(), function);
}

NAN_METHOD(TlFactoryWrap::CreateDevice)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CDeviceInfo")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring_vector")))
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringVectorWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringVectorWrap>(info[1]->ToObject());
        gcstring_vector* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateDevice(*arg0, *arg1);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateDevice(*arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CDeviceInfo")))
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateDevice(*arg0);
    }
}

NAN_METHOD(TlFactoryWrap::CreateFirstDevice)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CDeviceInfo")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring_vector")))
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringVectorWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringVectorWrap>(info[1]->ToObject());
        gcstring_vector* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateFirstDevice(*arg0, *arg1);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CDeviceInfo")))
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateFirstDevice(*arg0);
    }
}

NAN_METHOD(TlFactoryWrap::CreateTl)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->CreateTl(*arg0);
    }
    else if ((info.Length() == 1) && info[0]->IsObject())
    {
        // TODO: Implement wrapper for CTlInfo

        // Call wrapped method
        tlFactory->CreateTl(*arg0);
    }
}

NAN_METHOD(TlFactoryWrap::DestroyDevice)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPylonDevice")))
    {
        // Unwrap object
        PylonDeviceWrap* arg0_wrap = ObjectWrap::Unwrap<PylonDeviceWrap>(info[0]->ToObject());
        IPylonDevice* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->DestroyDevice(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(TlFactoryWrap::EnumerateDevices)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "DeviceInfoList")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "DeviceInfoList")) && info[2]->IsBoolean())
    {
        // Unwrap object
        DeviceInfoListWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoListWrap>(info[0]->ToObject());
        DeviceInfoList* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        DeviceInfoListWrap* arg1_wrap = ObjectWrap::Unwrap<DeviceInfoListWrap>(info[1]->ToObject());
        DeviceInfoList* arg1 = arg1_wrap->GetWrapped();

        // Convert from boolean value
        bool arg2 = info[2]->BooleanValue();

        // Call wrapped method
        int result = tlFactory->EnumerateDevices(*arg0, *arg1, arg2);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "DeviceInfoList")) && info[1]->IsBoolean())
    {
        // Unwrap object
        DeviceInfoListWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoListWrap>(info[0]->ToObject());
        DeviceInfoList* arg0 = arg0_wrap->GetWrapped();

        // Convert from boolean value
        bool arg1 = info[1]->BooleanValue();

        // Call wrapped method
        int result = tlFactory->EnumerateDevices(*arg0, arg1);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(TlFactoryWrap::EnumerateTls)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "TlInfoList")))
    {
        // Unwrap object
        TlInfoListWrap* arg0_wrap = ObjectWrap::Unwrap<TlInfoListWrap>(info[0]->ToObject());
        TlInfoList* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        int result = tlFactory->EnumerateTls(*arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(TlFactoryWrap::GetInstance)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        tlFactory->GetInstance();
    }
}

NAN_METHOD(TlFactoryWrap::IsDeviceAccessible)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CDeviceInfo")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "AccessModeSet")) && info[2]->IsNumber())
    {
        // Unwrap object
        DeviceInfoWrap* arg0_wrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
        CDeviceInfo* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        AccessModeSetWrap* arg1_wrap = ObjectWrap::Unwrap<AccessModeSetWrap>(info[1]->ToObject());
        AccessModeSet* arg1 = arg1_wrap->GetWrapped();

        // Convert from number value to pointer
        EDeviceAccessiblityInfo arg2_value = static_cast<EDeviceAccessiblityInfo>(info[2]->NumberValue());
        EDeviceAccessiblityInfo* arg2 = &arg2_value;

        // Call wrapped method
        bool result = tlFactory->IsDeviceAccessible(*arg0, arg1, arg2);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(TlFactoryWrap::ReleaseTl)
{
    TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
    CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "ITransportLayer")))
    {
        // Unwrap object
        TransportLayerWrap* arg0_wrap = ObjectWrap::Unwrap<TransportLayerWrap>(info[0]->ToObject());
        ITransportLayer* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        tlFactory->ReleaseTl(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
