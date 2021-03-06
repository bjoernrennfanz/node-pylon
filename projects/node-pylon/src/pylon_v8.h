﻿// MIT License
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

#pragma once

#include <node.h>
#include <nan.h>
#include <Base/GCString.h>

namespace pylon_v8 
{
    inline bool InstanceOf(v8::Local<v8::Value> value, Nan::Persistent<v8::FunctionTemplate>* prototype) 
    {
        return value->IsObject() && Nan::New(*prototype)->HasInstance(value);
    }

    inline GENICAM_NAMESPACE::gcstring ToGCString(v8::Local<v8::String> str) 
    {
        return GENICAM_NAMESPACE::gcstring(*v8::String::Utf8Value(str));
    }

    inline v8::Local<v8::String> FromGCString(GENICAM_NAMESPACE::gcstring str)
    {
        return Nan::New<v8::String>(str.c_str()).ToLocalChecked();
    }
}