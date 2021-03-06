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

#include "imageformatconverter.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> ImageFormatConverterWrap::prototype;
Nan::Persistent<Function> ImageFormatConverterWrap::constructor;

// Supported implementations
// CImageFormatConverter()
// CImageFormatConverter(CImageFormatConverter& const src)
ImageFormatConverterWrap::ImageFormatConverterWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ImageFormatConverter(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CImageFormatConverter()
        m_ImageFormatConverter = new CImageFormatConverter();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CImageFormatConverter")))
    {
        // Unwrap object
        ImageFormatConverterWrap* arg0_wrap = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info[0]->ToObject());
        CImageFormatConverter* arg0 = arg0_wrap->GetWrapped();

        // CImageFormatConverter(CImageFormatConverter& const src)
        m_ImageFormatConverter = new CImageFormatConverter(*arg0);
    }
}

ImageFormatConverterWrap::~ImageFormatConverterWrap()
{
    delete m_ImageFormatConverter;
}

NAN_MODULE_INIT(ImageFormatConverterWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ImageFormatConverterWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "initialize", Initialize);
    Nan::SetPrototypeMethod(tpl, "isInitialized", IsInitialized);
    Nan::SetPrototypeMethod(tpl, "uninitialize", Uninitialize);
    Nan::SetPrototypeMethod(tpl, "imageHasDestinationFormat", ImageHasDestinationFormat);
    Nan::SetPrototypeMethod(tpl, "getBufferSizeForConversion", GetBufferSizeForConversion);
    Nan::SetPrototypeMethod(tpl, "convert", Convert);
    Nan::SetPrototypeMethod(tpl, "isSupportedInputFormat", IsSupportedInputFormat);
    Nan::SetPrototypeMethod(tpl, "isSupportedOutputFormat", IsSupportedOutputFormat);
    Nan::SetPrototypeMethod(tpl, "getNodeMap", GetNodeMap);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CImageFormatConverter").ToLocalChecked(), function);
}

NAN_METHOD(ImageFormatConverterWrap::Convert)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 9) && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsObject() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() && info[6]->IsNumber() && info[7]->IsNumber() && info[8]->IsNumber())
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg3 = static_cast<unsigned int>(info[3]->NumberValue());

        // Convert from number value
        EPixelType arg4 = static_cast<EPixelType>(info[4]->NumberValue());

        // Convert from number value
        unsigned int arg5 = static_cast<unsigned int>(info[5]->NumberValue());

        // Convert from number value
        unsigned int arg6 = static_cast<unsigned int>(info[6]->NumberValue());

        // Convert from number value
        unsigned int arg7 = static_cast<unsigned int>(info[7]->NumberValue());

        // Convert from number value
        EImageOrientation arg8 = static_cast<EImageOrientation>(info[8]->NumberValue());

        // Call wrapped method
        imageFormatConverter->Convert(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 8) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IReusableImage")) && info[1]->IsObject() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber() && info[5]->IsNumber() && info[6]->IsNumber() && info[7]->IsNumber())
    {
        // Unwrap object
        ReusableImageWrap* arg0_wrap = ObjectWrap::Unwrap<ReusableImageWrap>(info[0]->ToObject());
        IReusableImage* arg0 = arg0_wrap->GetWrapped();

        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Convert from number value
        EPixelType arg3 = static_cast<EPixelType>(info[3]->NumberValue());

        // Convert from number value
        unsigned int arg4 = static_cast<unsigned int>(info[4]->NumberValue());

        // Convert from number value
        unsigned int arg5 = static_cast<unsigned int>(info[5]->NumberValue());

        // Convert from number value
        unsigned int arg6 = static_cast<unsigned int>(info[6]->NumberValue());

        // Convert from number value
        EImageOrientation arg7 = static_cast<EImageOrientation>(info[7]->NumberValue());

        // Call wrapped method
        imageFormatConverter->Convert(*arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 3) && info[0]->IsObject() && info[1]->IsNumber() && (info[2]->IsObject() && (pylon_v8::ToGCString(info[2]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Unwrap object
        ImageWrap* arg2_wrap = ObjectWrap::Unwrap<ImageWrap>(info[2]->ToObject());
        IImage* arg2 = arg2_wrap->GetWrapped();

        // Call wrapped method
        imageFormatConverter->Convert(arg0, arg1, *arg2);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IReusableImage")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ReusableImageWrap* arg0_wrap = ObjectWrap::Unwrap<ReusableImageWrap>(info[0]->ToObject());
        IReusableImage* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        ImageWrap* arg1_wrap = ObjectWrap::Unwrap<ImageWrap>(info[1]->ToObject());
        IImage* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        imageFormatConverter->Convert(*arg0, *arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(ImageFormatConverterWrap::GetBufferSizeForConversion)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 3) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        unsigned int arg2 = static_cast<unsigned int>(info[2]->NumberValue());

        // Call wrapped method
        unsigned int result = imageFormatConverter->GetBufferSizeForConversion(arg0, arg1, arg2);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        unsigned int result = imageFormatConverter->GetBufferSizeForConversion(*arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(ImageFormatConverterWrap::GetNodeMap)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        imageFormatConverter->GetNodeMap();
    }
}

NAN_METHOD(ImageFormatConverterWrap::ImageHasDestinationFormat)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 3) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Convert from number value
        EImageOrientation arg2 = static_cast<EImageOrientation>(info[2]->NumberValue());

        // Call wrapped method
        bool result = imageFormatConverter->ImageHasDestinationFormat(arg0, arg1, arg2);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        bool result = imageFormatConverter->ImageHasDestinationFormat(*arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageFormatConverterWrap::Initialize)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Call wrapped method
        imageFormatConverter->Initialize(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(ImageFormatConverterWrap::IsInitialized)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Call wrapped method
        bool result = imageFormatConverter->IsInitialized(arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageFormatConverterWrap::IsSupportedInputFormat)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Call wrapped method
        bool result = imageFormatConverter->IsSupportedInputFormat(arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageFormatConverterWrap::IsSupportedOutputFormat)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EPixelType arg0 = static_cast<EPixelType>(info[0]->NumberValue());

        // Call wrapped method
        bool result = imageFormatConverter->IsSupportedOutputFormat(arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ImageFormatConverterWrap::Uninitialize)
{
    ImageFormatConverterWrap* wrappedImageFormatConverter = ObjectWrap::Unwrap<ImageFormatConverterWrap>(info.This());
    CImageFormatConverter* imageFormatConverter = wrappedImageFormatConverter->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        imageFormatConverter->Uninitialize();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
