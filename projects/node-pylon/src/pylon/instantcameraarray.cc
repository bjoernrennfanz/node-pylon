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

#include "instantcameraarray.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> InstantCameraArrayWrap::prototype;
Nan::Persistent<Function> InstantCameraArrayWrap::constructor;

// Supported implementations
// CInstantCameraArray()
// CInstantCameraArray(unsigned int numberOfCameras)
// CInstantCameraArray(CInstantCameraArray& const arg0)
InstantCameraArrayWrap::InstantCameraArrayWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_InstantCameraArray(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CInstantCameraArray()
        m_InstantCameraArray = new CInstantCameraArray();
    }
    else if (info[0]->IsNumber())
    {
        // Convert number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // CInstantCameraArray(unsigned int numberOfCameras)
        m_InstantCameraArray = new CInstantCameraArray(arg0);
    }
    else if (info[0]->IsObject())
    {
        gcstring info0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
        if (info0_constructor != "CInstantCameraArray")
        {
            ThrowException(Exception::TypeError(String::New("CInstantCameraArray::CInstantCameraArray: bad argument")));
        }

        // Unwrap obj
        InstantCameraArrayWrap* arg0_wrap = ObjectWrap::Unwrap<InstantCameraArrayWrap>(info[0]->ToObject());
        CInstantCameraArray* arg0 = arg0_wrap->GetWrapped();

        // CInstantCameraArray(CInstantCameraArray& const arg0)
        m_InstantCameraArray = new CInstantCameraArray(*arg0);
    }
}

InstantCameraArrayWrap::~InstantCameraArrayWrap()
{
    delete m_InstantCameraArray;
}

NAN_MODULE_INIT(InstantCameraArrayWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("InstantCameraArrayWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "initialize", Initialize);
    Nan::SetPrototypeMethod(tpl, "isPylonDeviceAttached", IsPylonDeviceAttached);
    Nan::SetPrototypeMethod(tpl, "isCameraDeviceRemoved", IsCameraDeviceRemoved);
    Nan::SetPrototypeMethod(tpl, "destroyDevice", DestroyDevice);
    Nan::SetPrototypeMethod(tpl, "detachDevice", DetachDevice);
    Nan::SetPrototypeMethod(tpl, "open", Open);
    Nan::SetPrototypeMethod(tpl, "isOpen", IsOpen);
    Nan::SetPrototypeMethod(tpl, "close", Close);
    Nan::SetPrototypeMethod(tpl, "getSize", GetSize);
    Nan::SetPrototypeMethod(tpl, "startGrabbing", StartGrabbing);
    Nan::SetPrototypeMethod(tpl, "retrieveResult", RetrieveResult);
    Nan::SetPrototypeMethod(tpl, "stopGrabbing", StopGrabbing);
    Nan::SetPrototypeMethod(tpl, "isGrabbing", IsGrabbing);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CInstantCameraArray").ToLocalChecked(), function);
}
