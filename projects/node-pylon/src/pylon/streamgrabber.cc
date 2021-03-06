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

#include "streamgrabber.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> StreamGrabberWrap::prototype;
Nan::Persistent<Function> StreamGrabberWrap::constructor;

// Supported implementations
// IStreamGrabber()
// IStreamGrabber(IStreamGrabber& const arg0)
StreamGrabberWrap::StreamGrabberWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_StreamGrabber(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IStreamGrabber()
        m_StreamGrabber = new IStreamGrabber();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IStreamGrabber")))
    {
        // Unwrap object
        StreamGrabberWrap* arg0_wrap = ObjectWrap::Unwrap<StreamGrabberWrap>(info[0]->ToObject());
        IStreamGrabber* arg0 = arg0_wrap->GetWrapped();

        // IStreamGrabber(IStreamGrabber& const arg0)
        m_StreamGrabber = new IStreamGrabber(*arg0);
    }
}

StreamGrabberWrap::~StreamGrabberWrap()
{
    delete m_StreamGrabber;
}

NAN_MODULE_INIT(StreamGrabberWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("StreamGrabberWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "open", Open);
    Nan::SetPrototypeMethod(tpl, "close", Close);
    Nan::SetPrototypeMethod(tpl, "isOpen", IsOpen);
    Nan::SetPrototypeMethod(tpl, "registerBuffer", RegisterBuffer);
    Nan::SetPrototypeMethod(tpl, "deregisterBuffer", DeregisterBuffer);
    Nan::SetPrototypeMethod(tpl, "prepareGrab", PrepareGrab);
    Nan::SetPrototypeMethod(tpl, "finishGrab", FinishGrab);
    Nan::SetPrototypeMethod(tpl, "queueBuffer", QueueBuffer);
    Nan::SetPrototypeMethod(tpl, "cancelGrab", CancelGrab);
    Nan::SetPrototypeMethod(tpl, "retrieveResult", RetrieveResult);
    Nan::SetPrototypeMethod(tpl, "getWaitObject", GetWaitObject);
    Nan::SetPrototypeMethod(tpl, "getNodeMap", GetNodeMap);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IStreamGrabber").ToLocalChecked(), function);
}

NAN_METHOD(StreamGrabberWrap::CancelGrab)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->CancelGrab();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::Close)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->Close();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::DeregisterBuffer)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsObject())
    {
        // TODO: Implement wrapper for void

        // Call wrapped method
        // TODO: Implement return value wrapper for streamGrabber->DeregisterBuffer(arg0)
    }
}

NAN_METHOD(StreamGrabberWrap::FinishGrab)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->FinishGrab();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::GetNodeMap)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->GetNodeMap();
    }
}

NAN_METHOD(StreamGrabberWrap::GetWaitObject)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        // TODO: Implement return value wrapper for streamGrabber->GetWaitObject()
    }
}

NAN_METHOD(StreamGrabberWrap::IsOpen)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = streamGrabber->IsOpen();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(StreamGrabberWrap::Open)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->Open();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::PrepareGrab)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        streamGrabber->PrepareGrab();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::QueueBuffer)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsObject() && info[1]->IsObject())
    {
        // TODO: Implement wrapper for void

        // TODO: Implement wrapper for void

        // Call wrapped method
        streamGrabber->QueueBuffer(arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(StreamGrabberWrap::RegisterBuffer)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsObject() && info[1]->IsNumber())
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Call wrapped method
        // TODO: Implement return value wrapper for streamGrabber->RegisterBuffer(arg0, arg1)
    }
}

NAN_METHOD(StreamGrabberWrap::RetrieveResult)
{
    StreamGrabberWrap* wrappedStreamGrabber = ObjectWrap::Unwrap<StreamGrabberWrap>(info.This());
    IStreamGrabber* streamGrabber = wrappedStreamGrabber->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GrabResult")))
    {
        // Unwrap object
        GrabResultWrap* arg0_wrap = ObjectWrap::Unwrap<GrabResultWrap>(info[0]->ToObject());
        GrabResult* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        bool result = streamGrabber->RetrieveResult(*arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}
