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

#include "booleanreft.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> BooleanRefTWrap::prototype;
Nan::Persistent<Function> BooleanRefTWrap::constructor;

// Supported implementations
// CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>()
// CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>(CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>& const arg0)
BooleanRefTWrap::BooleanRefTWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_BooleanRefT(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>()
        m_BooleanRefT = new CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>")))
    {
        // Unwrap object
        BooleanRefTWrap* arg0_wrap = ObjectWrap::Unwrap<BooleanRefTWrap>(info[0]->ToObject());
        CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>* arg0 = arg0_wrap->GetWrapped();

        // CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>(CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>& const arg0)
        m_BooleanRefT = new CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>(*arg0);
    }
}

BooleanRefTWrap::~BooleanRefTWrap()
{
    delete m_BooleanRefT;
}

NAN_MODULE_INIT(BooleanRefTWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("BooleanRefTWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "setValue", SetValue);
    Nan::SetPrototypeMethod(tpl, "getValue", GetValue);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>").ToLocalChecked(), function);
}

NAN_METHOD(BooleanRefTWrap::GetValue)
{
    BooleanRefTWrap* wrappedBooleanRefT = ObjectWrap::Unwrap<BooleanRefTWrap>(info.This());
    CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>* booleanRefT = wrappedBooleanRefT->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsBoolean() && info[1]->IsBoolean())
    {
        // Convert from boolean value
        bool arg0 = info[0]->BooleanValue();

        // Convert from boolean value
        bool arg1 = info[1]->BooleanValue();

        // Call wrapped method
        bool result = booleanRefT->GetValue(arg0, arg1);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(BooleanRefTWrap::SetValue)
{
    BooleanRefTWrap* wrappedBooleanRefT = ObjectWrap::Unwrap<BooleanRefTWrap>(info.This());
    CBooleanRefT<GenApi_3_0_Basler_pylon_v5_0::IBoolean>* booleanRefT = wrappedBooleanRefT->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsBoolean() && info[1]->IsBoolean())
    {
        // Convert from boolean value
        bool arg0 = info[0]->BooleanValue();

        // Convert from boolean value
        bool arg1 = info[1]->BooleanValue();

        // Call wrapped method
        booleanRefT->SetValue(arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
