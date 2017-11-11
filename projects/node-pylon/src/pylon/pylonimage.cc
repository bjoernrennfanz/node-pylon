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

#include "pylonimage.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> PylonImageWrap::prototype;
Nan::Persistent<Function> PylonImageWrap::constructor;

// Supported implementations
// CPylonImage()
// CPylonImage(CPylonImage& const source)
PylonImageWrap::PylonImageWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_PylonImage(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CPylonImage()
        m_PylonImage = new CPylonImage();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CPylonImage")))
    {
        // Unwrap object
        PylonImageWrap* arg0_wrap = ObjectWrap::Unwrap<PylonImageWrap>(info[0]->ToObject());
        CPylonImage* arg0 = arg0_wrap->GetWrapped();

        // CPylonImage(CPylonImage& const source)
        m_PylonImage = new CPylonImage(*arg0);
    }
}

PylonImageWrap::~PylonImageWrap()
{
    delete m_PylonImage;
}

NAN_MODULE_INIT(PylonImageWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("PylonImageWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "create", Create);
    Nan::SetPrototypeMethod(tpl, "copyImage", CopyImage);
    Nan::SetPrototypeMethod(tpl, "attachGrabResultBuffer", AttachGrabResultBuffer);
    Nan::SetPrototypeMethod(tpl, "attachUserBuffer", AttachUserBuffer);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "getPixelType", GetPixelType);
    Nan::SetPrototypeMethod(tpl, "getWidth", GetWidth);
    Nan::SetPrototypeMethod(tpl, "getHeight", GetHeight);
    Nan::SetPrototypeMethod(tpl, "getPaddingX", GetPaddingX);
    Nan::SetPrototypeMethod(tpl, "getOrientation", GetOrientation);
    Nan::SetPrototypeMethod(tpl, "getBuffer", GetBuffer);
    Nan::SetPrototypeMethod(tpl, "getImageSize", GetImageSize);
    Nan::SetPrototypeMethod(tpl, "isUnique", IsUnique);
    Nan::SetPrototypeMethod(tpl, "getStride", GetStride);
    Nan::SetPrototypeMethod(tpl, "isSupportedPixelType", IsSupportedPixelType);
    Nan::SetPrototypeMethod(tpl, "isAdditionalPaddingSupported", IsAdditionalPaddingSupported);
    Nan::SetPrototypeMethod(tpl, "reset", Reset);
    Nan::SetPrototypeMethod(tpl, "release", Release);
    Nan::SetPrototypeMethod(tpl, "isUserBufferAttached", IsUserBufferAttached);
    Nan::SetPrototypeMethod(tpl, "isGrabResultBufferAttached", IsGrabResultBufferAttached);
    Nan::SetPrototypeMethod(tpl, "getAllocatedBufferSize", GetAllocatedBufferSize);
    Nan::SetPrototypeMethod(tpl, "changePixelType", ChangePixelType);
    Nan::SetPrototypeMethod(tpl, "getPlane", GetPlane);
    Nan::SetPrototypeMethod(tpl, "getAoi", GetAoi);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CPylonImage").ToLocalChecked(), function);
}

NAN_METHOD(PylonImageWrap::AttachGrabResultBuffer)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CGrabResultPtr")))
    {
        // Unwrap object
        GrabResultPtrWrap* arg0_wrap = ObjectWrap::Unwrap<GrabResultPtrWrap>(info[0]->ToObject());
        CGrabResultPtr* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(PylonImageWrap::AttachUserBuffer)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 7) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "void")) && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() && info[6]->IsNumber())
    {
        // Unwrap object
        voidWrap* arg0_wrap = ObjectWrap::Unwrap<voidWrap>(info[0]->ToObject());
        void* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        EPixelType arg2 = static_cast<EPixelType>(info[2]->NumberValue());

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());

        // Convert from number value
        unsigned int arg4 = static_cast<unsigned int>(info[4]->NumberValue());

        // Convert from number value
        unsigned int arg5 = static_cast<unsigned int>(info[5]->NumberValue());

        // Convert from number value
        EImageOrientation arg6 = static_cast<EImageOrientation>(info[6]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::ChangePixelType)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::CopyImage)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 7) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "void")) && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() && info[6]->IsNumber())
    {
        // Unwrap object
        voidWrap* arg0_wrap = ObjectWrap::Unwrap<voidWrap>(info[0]->ToObject());
        void* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        EPixelType arg2 = static_cast<EPixelType>(info[2]->NumberValue());

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());

        // Convert from number value
        unsigned int arg4 = static_cast<unsigned int>(info[4]->NumberValue());

        // Convert from number value
        unsigned int arg5 = static_cast<unsigned int>(info[5]->NumberValue());

        // Convert from number value
        EImageOrientation arg6 = static_cast<EImageOrientation>(info[6]->NumberValue());
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")) && info[1]->IsNumber())
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(PylonImageWrap::Create)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 5) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());

        // Convert from number value
        EImageOrientation arg4 = static_cast<EImageOrientation>(info[4]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::GetAllocatedBufferSize)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetAoi)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 4) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::GetBuffer)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
    else if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetHeight)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetImageSize)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetOrientation)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetPaddingX)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetPixelType)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::GetPlane)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::GetStride)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "unsigned int")))
    {
        // Unwrap object
        unsigned intWrap* arg0_wrap = ObjectWrap::Unwrap<unsigned intWrap>(info[0]->ToObject());
        unsigned int* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(PylonImageWrap::GetWidth)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::IsAdditionalPaddingSupported)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::IsGrabResultBufferAttached)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::IsSupportedPixelType)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());
    }
}

NAN_METHOD(PylonImageWrap::IsUnique)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::IsUserBufferAttached)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::IsValid)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::Release)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(PylonImageWrap::Reset)
{
    PylonImageWrap* wrappedPylonImage = ObjectWrap::Unwrap<PylonImageWrap>(info.This());
    CPylonImage* pylonImage = wrappedPylonImage->GetWrapped();

    if ((info.Length() == 5) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());

        // Convert from number value
        EImageOrientation arg4 = static_cast<EImageOrientation>(info[4]->NumberValue());
    }
    else if ((info.Length() == 4) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Convert from number value
        EImageOrientation arg3 = static_cast<EImageOrientation>(info[3]->NumberValue());
    }
}
