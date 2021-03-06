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

#include "iportrecorder.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> PortRecorderWrap::prototype;
Nan::Persistent<Function> PortRecorderWrap::constructor;

// Supported implementations
// IPortRecorder()
// IPortRecorder(IPortRecorder& const arg0)
PortRecorderWrap::PortRecorderWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_PortRecorder(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IPortRecorder()
        m_PortRecorder = new IPortRecorder();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPortRecorder")))
    {
        // Unwrap object
        PortRecorderWrap* arg0_wrap = ObjectWrap::Unwrap<PortRecorderWrap>(info[0]->ToObject());
        IPortRecorder* arg0 = arg0_wrap->GetWrapped();

        // IPortRecorder(IPortRecorder& const arg0)
        m_PortRecorder = new IPortRecorder(*arg0);
    }
}

PortRecorderWrap::~PortRecorderWrap()
{
    delete m_PortRecorder;
}

NAN_MODULE_INIT(PortRecorderWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("PortRecorderWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "startRecording", StartRecording);
    Nan::SetPrototypeMethod(tpl, "stopRecording", StopRecording);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IPortRecorder").ToLocalChecked(), function);
}

NAN_METHOD(PortRecorderWrap::StartRecording)
{
    PortRecorderWrap* wrappedPortRecorder = ObjectWrap::Unwrap<PortRecorderWrap>(info.This());
    IPortRecorder* portRecorder = wrappedPortRecorder->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPortWriteList")))
    {
        // Unwrap object
        PortWriteListWrap* arg0_wrap = ObjectWrap::Unwrap<PortWriteListWrap>(info[0]->ToObject());
        IPortWriteList* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        portRecorder->StartRecording(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(PortRecorderWrap::StopRecording)
{
    PortRecorderWrap* wrappedPortRecorder = ObjectWrap::Unwrap<PortRecorderWrap>(info.This());
    IPortRecorder* portRecorder = wrappedPortRecorder->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        portRecorder->StopRecording();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
