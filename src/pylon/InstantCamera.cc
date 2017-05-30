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

#include "InstantCamera.h"

#include "DeviceInfo.h"
#include "PylonDevice.h"

#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> InstantCameraWrap::prototype;
Nan::Persistent<Function> InstantCameraWrap::constructor;

// Supported implementations:
//   InstantCamera()
InstantCameraWrap::InstantCameraWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_InstantCamera(NULL)
{
    // InstantCamera(PylonDevice)
    if (info.Length() >= 1)
    {
        IPylonDevice* pylonDevice = NULL;
        ECleanup cleanupProcedure = Cleanup_Delete;

        if (pylon_v8::InstanceOf(info[0], &PylonDeviceWrap::prototype))
        {
            PylonDeviceWrap* pylonDeviceWrapper = ObjectWrap::Unwrap<PylonDeviceWrap>(info[0]->ToObject());
            pylonDevice = pylonDeviceWrapper->GetWrapped();
        }
        else
        {
            // Throw bad argument exception
            Nan::ThrowError(Exception::TypeError(Nan::New("InstantCamera::InstantCamera: bad argument").ToLocalChecked()));
        }

        // InstantCamera(PylonDevice, Cleanup)
        if (info.Length() == 2)
        {
            if (info[1]->IsNumber())
            {
                // Convert int to enum
                cleanupProcedure = static_cast<ECleanup>(info[1]->IntegerValue());
            }
            else
            {
                // Throw bad argument exception
                Nan::ThrowError(Exception::TypeError(Nan::New("InstantCamera::InstantCamera: bad argument").ToLocalChecked()));
            }
        }

        m_InstantCamera = new CInstantCamera(pylonDevice, cleanupProcedure);
    }
    else
    {
        m_InstantCamera = new CInstantCamera();
    }
}

InstantCameraWrap::~InstantCameraWrap() 
{
    delete m_InstantCamera;
}

NAN_MODULE_INIT(InstantCameraWrap::Initialize)
{
    // Prepare constructor template
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("InstantCamera").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);  

    // Prototype
    Nan::SetPrototypeMethod(tpl, "getDeviceInfo", GetDeviceInfo);

    // Property
    Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("maxNumBuffer").ToLocalChecked(), MaxNumBufferGet, MaxNumBufferSet);

    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("InstantCamera").ToLocalChecked(), function);
}

NAN_METHOD(InstantCameraWrap::New) 
{
    InstantCameraWrap* wrappedInstantCamera = new InstantCameraWrap(info);
    wrappedInstantCamera->Wrap(info.This());
}

Handle<Value> InstantCameraWrap::NewInstance(CInstantCamera* instantCamera)
{
    Nan::EscapableHandleScope scope;
  
    Local<Object> instance = Nan::NewInstance(Nan::New(constructor), 0, NULL).ToLocalChecked();
    InstantCameraWrap* wrappedInstantCamera = node::ObjectWrap::Unwrap<InstantCameraWrap>(instance);
    wrappedInstantCamera->SetWrapped(instantCamera);

    return scope.Escape(instance);
}

NAN_METHOD(InstantCameraWrap::GetDeviceInfo) 
{
    InstantCameraWrap* wrappedInstantCamera = ObjectWrap::Unwrap<InstantCameraWrap>(info.This());
    CInstantCamera* instantCamera = wrappedInstantCamera->GetWrapped();

    info.GetReturnValue().Set(DeviceInfoWrap::NewInstance(instantCamera->GetDeviceInfo()));
}

NAN_GETTER(InstantCameraWrap::MaxNumBufferGet)
{
    InstantCameraWrap* wrappedInstantCamera = ObjectWrap::Unwrap<InstantCameraWrap>(info.This());
    CInstantCamera* instantCamera = wrappedInstantCamera->GetWrapped();

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_SETTER(InstantCameraWrap::MaxNumBufferSet)
{
    InstantCameraWrap* wrappedInstantCamera = ObjectWrap::Unwrap<InstantCameraWrap>(info.This());
    CInstantCamera* instantCamera = wrappedInstantCamera->GetWrapped();
}