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

#include "imageformatconverterparamsparams.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Basler_ImageFormatConverterParams;

Nan::Persistent<FunctionTemplate> ImageFormatConverterParamsParamsWrap::prototype;
Nan::Persistent<Function> ImageFormatConverterParamsParamsWrap::constructor;

// Supported implementations
// CImageFormatConverterParams_Params()
// CImageFormatConverterParams_Params(CImageFormatConverterParams_Params& arg0)
ImageFormatConverterParamsParamsWrap::ImageFormatConverterParamsParamsWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ImageFormatConverterParamsParams(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CImageFormatConverterParams_Params()
        m_ImageFormatConverterParamsParams = new CImageFormatConverterParams_Params();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CImageFormatConverterParams_Params")))
    {
        // Unwrap object
        ImageFormatConverterParamsParamsWrap* arg0_wrap = ObjectWrap::Unwrap<ImageFormatConverterParamsParamsWrap>(info[0]->ToObject());
        CImageFormatConverterParams_Params* arg0 = arg0_wrap->GetWrapped();

        // CImageFormatConverterParams_Params(CImageFormatConverterParams_Params& arg0)
        m_ImageFormatConverterParamsParams = new CImageFormatConverterParams_Params(*arg0);
    }
}

ImageFormatConverterParamsParamsWrap::~ImageFormatConverterParamsParamsWrap()
{
    delete m_ImageFormatConverterParamsParams;
}

NAN_MODULE_INIT(ImageFormatConverterParamsParamsWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ImageFormatConverterParamsParamsWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CImageFormatConverterParams_Params").ToLocalChecked(), function);
}
