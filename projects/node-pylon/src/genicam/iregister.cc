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

#include "iregister.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> RegisterWrap::prototype;
Nan::Persistent<Function> RegisterWrap::constructor;

// Supported implementations
// IRegister()
// IRegister(IRegister& const arg0)
RegisterWrap::RegisterWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_Register(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IRegister()
        m_Register = new IRegister();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IRegister")))
    {
        // Unwrap object
        RegisterWrap* arg0_wrap = ObjectWrap::Unwrap<RegisterWrap>(info[0]->ToObject());
        IRegister* arg0 = arg0_wrap->GetWrapped();

        // IRegister(IRegister& const arg0)
        m_Register = new IRegister(*arg0);
    }
}

RegisterWrap::~RegisterWrap()
{
    delete m_Register;
}

NAN_MODULE_INIT(RegisterWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("RegisterWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "set", Set);
    Nan::SetPrototypeMethod(tpl, "get", Get);
    Nan::SetPrototypeMethod(tpl, "getLength", GetLength);
    Nan::SetPrototypeMethod(tpl, "getAddress", GetAddress);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IRegister").ToLocalChecked(), function);
}

NAN_METHOD(RegisterWrap::Get)
{
    RegisterWrap* wrappedRegister = ObjectWrap::Unwrap<RegisterWrap>(info.This());
    IRegister* register = wrappedRegister->GetWrapped();

    if ((info.Length() == 4) && info[0]->IsString() && info[1]->IsNumber() && info[2]->IsBoolean() && info[3]->IsBoolean())
    {
        // Convert from string value
        unsigned char* arg0 = static_cast<unsigned char*>(pylon_v8::ToGCString(info[0]->ToString()).c_str());

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from boolean value
        bool arg2 = info[2]->BooleanValue();

        // Convert from boolean value
        bool arg3 = info[3]->BooleanValue();

        // Call wrapped method
        register->Get(arg0, arg1, arg2, arg3);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(RegisterWrap::GetAddress)
{
    RegisterWrap* wrappedRegister = ObjectWrap::Unwrap<RegisterWrap>(info.This());
    IRegister* register = wrappedRegister->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        __int128_t result = register->GetAddress();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(RegisterWrap::GetLength)
{
    RegisterWrap* wrappedRegister = ObjectWrap::Unwrap<RegisterWrap>(info.This());
    IRegister* register = wrappedRegister->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        __int128_t result = register->GetLength();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(RegisterWrap::Set)
{
    RegisterWrap* wrappedRegister = ObjectWrap::Unwrap<RegisterWrap>(info.This());
    IRegister* register = wrappedRegister->GetWrapped();

    if ((info.Length() == 3) && info[0]->IsString() && info[1]->IsNumber() && info[2]->IsBoolean())
    {
        // Convert from string value
        unsigned char* arg0 = static_cast<unsigned char*>(pylon_v8::ToGCString(info[0]->ToString()).c_str());

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from boolean value
        bool arg2 = info[2]->BooleanValue();

        // Call wrapped method
        register->Set(arg0, arg1, arg2);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
