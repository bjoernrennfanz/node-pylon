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

#include "ifloat.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> FloatRefTWrap::prototype;
Nan::Persistent<Function> FloatRefTWrap::constructor;

// Supported implementations
// CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>()
// CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>(CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>& const arg0)
FloatRefTWrap::FloatRefTWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_FloatRefT(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>()
        m_FloatRefT = new CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>")))
    {
        // Unwrap object
        FloatRefTWrap* arg0_wrap = ObjectWrap::Unwrap<FloatRefTWrap>(info[0]->ToObject());
        CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* arg0 = arg0_wrap->GetWrapped();

        // CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>(CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>& const arg0)
        m_FloatRefT = new CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>(*arg0);
    }
}

FloatRefTWrap::~FloatRefTWrap()
{
    delete m_FloatRefT;
}

NAN_MODULE_INIT(FloatRefTWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("FloatRefTWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "setValue", SetValue);
    Nan::SetPrototypeMethod(tpl, "getValue", GetValue);
    Nan::SetPrototypeMethod(tpl, "getMin", GetMin);
    Nan::SetPrototypeMethod(tpl, "getMax", GetMax);
    Nan::SetPrototypeMethod(tpl, "hasInc", HasInc);
    Nan::SetPrototypeMethod(tpl, "getIncMode", GetIncMode);
    Nan::SetPrototypeMethod(tpl, "getInc", GetInc);
    Nan::SetPrototypeMethod(tpl, "getListOfValidValues", GetListOfValidValues);
    Nan::SetPrototypeMethod(tpl, "getRepresentation", GetRepresentation);
    Nan::SetPrototypeMethod(tpl, "getUnit", GetUnit);
    Nan::SetPrototypeMethod(tpl, "getDisplayNotation", GetDisplayNotation);
    Nan::SetPrototypeMethod(tpl, "getDisplayPrecision", GetDisplayPrecision);
    Nan::SetPrototypeMethod(tpl, "getIntAlias", GetIntAlias);
    Nan::SetPrototypeMethod(tpl, "getEnumAlias", GetEnumAlias);
    Nan::SetPrototypeMethod(tpl, "imposeMin", ImposeMin);
    Nan::SetPrototypeMethod(tpl, "imposeMax", ImposeMax);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>").ToLocalChecked(), function);
}

NAN_METHOD(FloatRefTWrap::GetDisplayNotation)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetDisplayPrecision)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetEnumAlias)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetInc)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetIncMode)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetIntAlias)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetListOfValidValues)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsBoolean())
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetMax)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetMin)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetRepresentation)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetUnit)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::GetValue)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsBoolean() && info[1]->IsBoolean())
    {
    }
}

NAN_METHOD(FloatRefTWrap::HasInc)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(FloatRefTWrap::ImposeMax)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());
    }
}

NAN_METHOD(FloatRefTWrap::ImposeMin)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());
    }
}

NAN_METHOD(FloatRefTWrap::SetValue)
{
    FloatRefTWrap* wrappedFloatRefT = ObjectWrap::Unwrap<FloatRefTWrap>(info.This());
    CFloatRefT<GenApi_3_0_Basler_pylon_v5_0::IFloat>* floatRefT = wrappedFloatRefT->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && info[1]->IsBoolean())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());
    }
}
