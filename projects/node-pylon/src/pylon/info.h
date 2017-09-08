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

#include <pylon/Info.h>
#include "deviceinfo.h"

using namespace v8;
using namespace Pylon;

class InfoBaseWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CInfoBase* GetWrapped() const
    {
        return m_InfoBase;
    };

    void SetWrapped(CInfoBase* infoBase)
    {
        m_InfoBase = infoBase;
    };

    static v8::Handle<v8::Value> NewInstance(CInfoBase* infoBase);

private:
    static Nan::Persistent<v8::Function> constructor;
    InfoBaseWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~InfoBaseWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetFriendlyName);
    static NAN_METHOD(SetFriendlyName);
    static NAN_METHOD(IsFriendlyNameAvailable);
    static NAN_METHOD(GetFullName);
    static NAN_METHOD(SetFullName);
    static NAN_METHOD(IsFullNameAvailable);
    static NAN_METHOD(GetVendorName);
    static NAN_METHOD(SetVendorName);
    static NAN_METHOD(IsVendorNameAvailable);
    static NAN_METHOD(GetDeviceClass);
    static NAN_METHOD(SetDeviceClass);
    static NAN_METHOD(IsDeviceClassAvailable);
    static NAN_METHOD(GetPropertyNotAvailable);
    static NAN_METHOD(GetPropertyNames);
    static NAN_METHOD(GetPropertyAvailable);
    static NAN_METHOD(GetPropertyValue);
    static NAN_METHOD(SetPropertyValue);
    static NAN_METHOD(IsUserProvided);
    static NAN_METHOD(IsSubset);

    // Wrapped object
    CInfoBase* m_InfoBase;
};
