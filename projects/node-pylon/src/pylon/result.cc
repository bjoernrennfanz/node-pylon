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

#include "result.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> GrabResultWrap::prototype;
Nan::Persistent<Function> GrabResultWrap::constructor;

// Supported implementations
// GrabResult()
// GrabResult(GrabResult& const arg0)
GrabResultWrap::GrabResultWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_GrabResult(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // GrabResult()
        m_GrabResult = new GrabResult();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GrabResult")))
    {
        // Unwrap object
        GrabResultWrap* arg0_wrap = ObjectWrap::Unwrap<GrabResultWrap>(info[0]->ToObject());
        GrabResult* arg0 = arg0_wrap->GetWrapped();

        // GrabResult(GrabResult& const arg0)
        m_GrabResult = new GrabResult(*arg0);
    }
}

GrabResultWrap::~GrabResultWrap()
{
    delete m_GrabResult;
}

NAN_MODULE_INIT(GrabResultWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("GrabResultWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "succeeded", Succeeded);
    Nan::SetPrototypeMethod(tpl, "handle", Handle);
    Nan::SetPrototypeMethod(tpl, "buffer", Buffer);
    Nan::SetPrototypeMethod(tpl, "status", Status);
    Nan::SetPrototypeMethod(tpl, "context", Context);
    Nan::SetPrototypeMethod(tpl, "frameNr", FrameNr);
    Nan::SetPrototypeMethod(tpl, "getPayloadType", GetPayloadType);
    Nan::SetPrototypeMethod(tpl, "getPixelType", GetPixelType);
    Nan::SetPrototypeMethod(tpl, "getTimeStamp", GetTimeStamp);
    Nan::SetPrototypeMethod(tpl, "getSizeX", GetSizeX);
    Nan::SetPrototypeMethod(tpl, "getSizeY", GetSizeY);
    Nan::SetPrototypeMethod(tpl, "getOffsetX", GetOffsetX);
    Nan::SetPrototypeMethod(tpl, "getOffsetY", GetOffsetY);
    Nan::SetPrototypeMethod(tpl, "getPaddingX", GetPaddingX);
    Nan::SetPrototypeMethod(tpl, "getPaddingY", GetPaddingY);
    Nan::SetPrototypeMethod(tpl, "getPayloadSize", GetPayloadSize);
    Nan::SetPrototypeMethod(tpl, "getPayloadSize_t", GetPayloadSize_t);
    Nan::SetPrototypeMethod(tpl, "getErrorDescription", GetErrorDescription);
    Nan::SetPrototypeMethod(tpl, "getErrorCode", GetErrorCode);
    Nan::SetPrototypeMethod(tpl, "getImage", GetImage);
    Nan::SetPrototypeMethod(tpl, "getBlockID", GetBlockID);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("GrabResult").ToLocalChecked(), function);
}

NAN_METHOD(GrabResultWrap::Buffer)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::Context)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::FrameNr)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetBlockID)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetErrorCode)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetErrorDescription)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetImage)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetOffsetX)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetOffsetY)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPaddingX)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPaddingY)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPayloadSize)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPayloadSize_t)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPayloadType)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetPixelType)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetSizeX)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetSizeY)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::GetTimeStamp)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::Handle)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::Status)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(GrabResultWrap::Succeeded)
{
    GrabResultWrap* wrappedGrabResult = ObjectWrap::Unwrap<GrabResultWrap>(info.This());
    GrabResult* grabResult = wrappedGrabResult->GetWrapped();

    if (info.Length() == 0)
    {
    }
}
