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

#include <GenApi/IBase.h>

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

class ReferenceTWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CReferenceT<GenApi_3_0_Basler_pylon_v5_0::IFloat, GenApi_3_0_Basler_pylon_v5_0::IFloat>* GetWrapped() const
    {
        return m_ReferenceT;
    };

    void SetWrapped(CReferenceT<GenApi_3_0_Basler_pylon_v5_0::IFloat, GenApi_3_0_Basler_pylon_v5_0::IFloat>* referenceT)
    {
        m_ReferenceT = referenceT;
    };

    static v8::Handle<v8::Value> NewInstance(CReferenceT<GenApi_3_0_Basler_pylon_v5_0::IFloat, GenApi_3_0_Basler_pylon_v5_0::IFloat>* referenceT);

private:
    static Nan::Persistent<v8::Function> constructor;
    ReferenceTWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~ReferenceTWrap();
    static NAN_METHOD(New);

    // Wrapped object
    CReferenceT<GenApi_3_0_Basler_pylon_v5_0::IFloat, GenApi_3_0_Basler_pylon_v5_0::IFloat>* m_ReferenceT;
};
