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

#pragma once

#include <node.h>
#include <nan.h>

#include <GenApi/ChunkPort.h>

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

class ChunkPortWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CChunkPort* GetWrapped() const
    {
        return m_ChunkPort;
    };

    void SetWrapped(CChunkPort* hunkPort)
    {
        m_ChunkPort = hunkPort;
    };

    static v8::Handle<v8::Value> NewInstance(CChunkPort* hunkPort);

private:
    static Nan::Persistent<v8::Function> constructor;
    ChunkPortWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~ChunkPortWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetAccessMode);
    static NAN_METHOD(GetPrincipalInterfaceType);
    static NAN_METHOD(Read);
    static NAN_METHOD(Write);
    static NAN_METHOD(SetPortImpl);
    static NAN_METHOD(GetSwapEndianess);
    static NAN_METHOD(InvalidateNode);
    static NAN_METHOD(AttachPort);
    static NAN_METHOD(DetachPort);
    static NAN_METHOD(AttachChunk);
    static NAN_METHOD(DetachChunk);
    static NAN_METHOD(GetChunkIDLength);
    static NAN_METHOD(CheckChunkID);
    static NAN_METHOD(UpdateBuffer);
    static NAN_METHOD(ClearCache);

    // Wrapped object
    CChunkPort* m_ChunkPort;
};
