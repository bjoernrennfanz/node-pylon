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

#include "image.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> ImageWrap::prototype;
Nan::Persistent<Function> ImageWrap::constructor;

// Supported implementations
// IImage()
// IImage(IImage& const arg0)
ImageWrap::ImageWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_Image(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IImage()
        m_Image = new IImage();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();

        // IImage(IImage& const arg0)
        m_Image = new IImage(*arg0);
    }
}

ImageWrap::~ImageWrap()
{
    delete m_Image;
}

NAN_MODULE_INIT(ImageWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ImageWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
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

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IImage").ToLocalChecked(), function);
}

NAN_METHOD(ImageWrap::GetBuffer)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        // TODO: Implement return value wrapper for image->GetBuffer()
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        // TODO: Implement return value wrapper for image->GetBuffer()
    }
}

NAN_METHOD(ImageWrap::GetHeight)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = image->GetHeight();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::GetImageSize)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = image->GetImageSize();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::GetOrientation)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        EImageOrientation result = image->GetOrientation();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::GetPaddingX)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = image->GetPaddingX();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::GetPixelType)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        EPixelType result = image->GetPixelType();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::GetStride)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        bool result = image->GetStride(*arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageWrap::GetWidth)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = image->GetWidth();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageWrap::IsUnique)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = image->IsUnique();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageWrap::IsValid)
{
    ImageWrap* wrappedImage = ObjectWrap::Unwrap<ImageWrap>(info.This());
    IImage* image = wrappedImage->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = image->IsValid();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}
