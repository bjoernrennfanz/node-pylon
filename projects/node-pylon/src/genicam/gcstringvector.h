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

#include <Base/GCStringVector.h>
#include "../pylon/container.h"
#include "gcstring.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

class GCStringVectorWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    gcstring_vector* GetWrapped() const
    {
        return m_GCStringVector;
    };

    void SetWrapped(gcstring_vector* gCStringVector)
    {
        m_GCStringVector = gCStringVector;
    };

    static v8::Handle<v8::Value> NewInstance(gcstring_vector* gCStringVector);

private:
    static Nan::Persistent<v8::Function> constructor;
    GCStringVectorWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~GCStringVectorWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(assign);
    static NAN_METHOD(clear);
    static NAN_METHOD(erase);
    static NAN_METHOD(at);
    static NAN_METHOD(back);
    static NAN_METHOD(begin);
    static NAN_METHOD(capacity);
    static NAN_METHOD(end);
    static NAN_METHOD(front);
    static NAN_METHOD(max_size);
    static NAN_METHOD(size);
    static NAN_METHOD(insert);
    static NAN_METHOD(empty);
    static NAN_METHOD(pop_back);
    static NAN_METHOD(push_back);
    static NAN_METHOD(resize);
    static NAN_METHOD(reserve);

    // Wrapped object
    gcstring_vector* m_GCStringVector;
};
