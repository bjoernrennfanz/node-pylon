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

#include <Base/GCString.h>

using namespace v8;
using namespace GenICam_3_0_Basler_pylon_v5_0;

class GCStringWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    gcstring* GetWrapped() const
    {
        return m_GCString;
    };

    void SetWrapped(gcstring* gCString)
    {
        m_GCString = gCString;
    };

    static v8::Handle<v8::Value> NewInstance(gcstring* gCString);

private:
    static Nan::Persistent<v8::Function> constructor;
    GCStringWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~GCStringWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(append);
    static NAN_METHOD(assign);
    static NAN_METHOD(compare);
    static NAN_METHOD(w_str);
    static NAN_METHOD(c_str);
    static NAN_METHOD(empty);
    static NAN_METHOD(find);
    static NAN_METHOD(length);
    static NAN_METHOD(size);
    static NAN_METHOD(resize);
    static NAN_METHOD(max_size);
    static NAN_METHOD(substr);
    static NAN_METHOD(find_first_of);
    static NAN_METHOD(find_first_not_of);
    static NAN_METHOD(_npos);
    static NAN_METHOD(swap);

private:
    // Wrapped functions
    static NAN_METHOD(ThrowBadAlloc);
    static NAN_METHOD(getline);

    // Wrapped object
    gcstring* m_GCString;
};