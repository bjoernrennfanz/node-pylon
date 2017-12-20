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

#include "iport.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> PortWrap::prototype;
Nan::Persistent<Function> PortWrap::constructor;

// Supported implementations
// IPort()
// IPort(IPort& const arg0)
PortWrap::PortWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_Port(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IPort()
        m_Port = new IPort();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();

        // IPort(IPort& const arg0)
        m_Port = new IPort(*arg0);
    }
}

PortWrap::~PortWrap()
{
    delete m_Port;
}

NAN_MODULE_INIT(PortWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("PortWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "read", Read);
    Nan::SetPrototypeMethod(tpl, "write", Write);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IPort").ToLocalChecked(), function);
}

NAN_METHOD(PortWrap::Read)
{
    PortWrap* wrappedPort = ObjectWrap::Unwrap<PortWrap>(info.This());
    IPort* port = wrappedPort->GetWrapped();

    if ((info.Length() == 3) && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());

        // Call wrapped method
        port->Read(arg0, arg1, arg2);
    }
}

NAN_METHOD(PortWrap::Write)
{
    PortWrap* wrappedPort = ObjectWrap::Unwrap<PortWrap>(info.This());
    IPort* port = wrappedPort->GetWrapped();

    if ((info.Length() == 3) && info[0]->IsObject() && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());

        // Call wrapped method
        port->Write(arg0, arg1, arg2);
    }
}
