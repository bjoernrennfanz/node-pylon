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

#include <pylon/GrabResultData.h>

using namespace v8;
using namespace Pylon;

class GrabResultDataWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CGrabResultData* GetWrapped() const
    {
        return m_GrabResultData;
    };

    void SetWrapped(CGrabResultData* grabResultData)
    {
        m_GrabResultData = grabResultData;
    };

    static v8::Handle<v8::Value> NewInstance(CGrabResultData* grabResultData);

private:
    static Nan::Persistent<v8::Function> constructor;
    GrabResultDataWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~GrabResultDataWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GrabSucceeded);
    static NAN_METHOD(GetErrorDescription);
    static NAN_METHOD(GetErrorCode);
    static NAN_METHOD(GetPayloadType);
    static NAN_METHOD(GetPixelType);
    static NAN_METHOD(GetWidth);
    static NAN_METHOD(GetHeight);
    static NAN_METHOD(GetOffsetX);
    static NAN_METHOD(GetOffsetY);
    static NAN_METHOD(GetPaddingX);
    static NAN_METHOD(GetPaddingY);
    static NAN_METHOD(GetBuffer);
    static NAN_METHOD(GetPayloadSize);
    static NAN_METHOD(GetFrameNumber);
    static NAN_METHOD(GetBlockID);
    static NAN_METHOD(GetTimeStamp);
    static NAN_METHOD(GetStride);
    static NAN_METHOD(GetImageSize);
    static NAN_METHOD(GetCameraContext);
    static NAN_METHOD(GetID);
    static NAN_METHOD(GetImageNumber);
    static NAN_METHOD(GetNumberOfSkippedImages);
    static NAN_METHOD(IsChunkDataAvailable);
    static NAN_METHOD(GetChunkDataNodeMap);
    static NAN_METHOD(HasCRC);
    static NAN_METHOD(CheckCRC);
    static NAN_METHOD(GetBufferContext);
    static NAN_METHOD(GetGrabResultDataImpl);

    // Wrapped object
    CGrabResultData* m_GrabResultData;
};
