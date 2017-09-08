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

#include <GenApi/IEnumeration.h>
#include "container.h"
#include "ienumentry.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

class EnumerationWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    IEnumeration* GetWrapped() const
    {
        return m_Enumeration;
    };

    void SetWrapped(IEnumeration* enumeration)
    {
        m_Enumeration = enumeration;
    };

    static v8::Handle<v8::Value> NewInstance(IEnumeration* enumeration);

private:
    static Nan::Persistent<v8::Function> constructor;
    EnumerationWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~EnumerationWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetSymbolics);
    static NAN_METHOD(GetEntries);
    static NAN_METHOD(SetIntValue);
    static NAN_METHOD(GetIntValue);
    static NAN_METHOD(GetEntryByName);
    static NAN_METHOD(GetEntry);
    static NAN_METHOD(GetCurrentEntry);

    // Wrapped object
    IEnumeration* m_Enumeration;
};
