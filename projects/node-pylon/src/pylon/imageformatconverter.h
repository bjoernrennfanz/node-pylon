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

#pragma once

#include <node.h>
#include <nan.h>

#include <pylon/ImageFormatConverter.h>
#include "image.h"

using namespace v8;
using namespace Pylon;

class ImageFormatConverterWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CImageFormatConverter* GetWrapped() const
    {
        return m_ImageFormatConverter;
    };

    void SetWrapped(CImageFormatConverter* imageFormatConverter)
    {
        m_ImageFormatConverter = imageFormatConverter;
    };

    static v8::Handle<v8::Value> NewInstance(CImageFormatConverter* imageFormatConverter);

private:
    static Nan::Persistent<v8::Function> constructor;
    ImageFormatConverterWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~ImageFormatConverterWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(Initialize);
    static NAN_METHOD(IsInitialized);
    static NAN_METHOD(Uninitialize);
    static NAN_METHOD(ImageHasDestinationFormat);
    static NAN_METHOD(GetBufferSizeForConversion);
    static NAN_METHOD(Convert);
    static NAN_METHOD(IsSupportedInputFormat);
    static NAN_METHOD(IsSupportedOutputFormat);
    static NAN_METHOD(GetNodeMap);

    // Wrapped object
    CImageFormatConverter* m_ImageFormatConverter;
};
