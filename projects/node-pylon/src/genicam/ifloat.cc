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

#include "ifloat.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> FloatWrap::prototype;
Nan::Persistent<Function> FloatWrap::constructor;

// Supported implementations
// IFloat()
// IFloat(IFloat& const arg0)
FloatWrap::FloatWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_Float(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IFloat()
        m_Float = new IFloat();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IFloat")))
    {
        // Unwrap object
        FloatWrap* arg0_wrap = ObjectWrap::Unwrap<FloatWrap>(info[0]->ToObject());
        IFloat* arg0 = arg0_wrap->GetWrapped();

        // IFloat(IFloat& const arg0)
        m_Float = new IFloat(*arg0);
    }
}

FloatWrap::~FloatWrap()
{
    delete m_Float;
}

NAN_MODULE_INIT(FloatWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("FloatWrap").ToLocalChecked());
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
    Nan::SetPrototypeMethod(tpl, "imposeMin", ImposeMin);
    Nan::SetPrototypeMethod(tpl, "imposeMax", ImposeMax);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IFloat").ToLocalChecked(), function);
}

NAN_METHOD(FloatWrap::GetDisplayNotation)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        EDisplayNotation result = float->GetDisplayNotation();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetDisplayPrecision)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        __int128_t result = float->GetDisplayPrecision();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetInc)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        double result = float->GetInc();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetIncMode)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        EIncMode result = float->GetIncMode();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetListOfValidValues)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsBoolean())
    {
        // Convert from boolean value
        bool arg0 = info[0]->BooleanValue();

        // Call wrapped method
        // TODO: Implement return value wrapper for float->GetListOfValidValues(arg0)
    }
}

NAN_METHOD(FloatWrap::GetMax)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        double result = float->GetMax();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetMin)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        double result = float->GetMin();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetRepresentation)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        ERepresentation result = float->GetRepresentation();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::GetUnit)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        float->GetUnit();
    }
}

NAN_METHOD(FloatWrap::GetValue)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsBoolean() && info[1]->IsBoolean())
    {
        // Convert from boolean value
        bool arg0 = info[0]->BooleanValue();

        // Convert from boolean value
        bool arg1 = info[1]->BooleanValue();

        // Call wrapped method
        double result = float->GetValue(arg0, arg1);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(FloatWrap::HasInc)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = float->HasInc();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(FloatWrap::ImposeMax)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());

        // Call wrapped method
        float->ImposeMax(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(FloatWrap::ImposeMin)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());

        // Call wrapped method
        float->ImposeMin(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(FloatWrap::SetValue)
{
    FloatWrap* wrappedFloat = ObjectWrap::Unwrap<FloatWrap>(info.This());
    IFloat* float = wrappedFloat->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && info[1]->IsBoolean())
    {
        // Convert from number value
        double arg0 = static_cast<double>(info[0]->NumberValue());

        // Convert from boolean value
        bool arg1 = info[1]->BooleanValue();

        // Call wrapped method
        float->SetValue(arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
