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

#include <GenApi/EventPort.h>

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

class EventPortWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CEventPort* GetWrapped() const
    {
        return m_EventPort;
    };

    void SetWrapped(CEventPort* eventPort)
    {
        m_EventPort = eventPort;
    };

    static v8::Handle<v8::Value> NewInstance(CEventPort* eventPort);

private:
    static Nan::Persistent<v8::Function> constructor;
    EventPortWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~EventPortWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetAccessMode);
    static NAN_METHOD(GetPrincipalInterfaceType);
    static NAN_METHOD(Read);
    static NAN_METHOD(Write);
    static NAN_METHOD(SetPortImpl);
    static NAN_METHOD(GetSwapEndianess);
    static NAN_METHOD(InvalidateNode);
    static NAN_METHOD(AttachNode);
    static NAN_METHOD(DetachNode);
    static NAN_METHOD(GetEventIDLength);
    static NAN_METHOD(CheckEventID);
    static NAN_METHOD(AttachEvent);
    static NAN_METHOD(DetachEvent);

    // Wrapped object
    CEventPort* m_EventPort;
};