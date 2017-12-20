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

#include "eslopeclass.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> ESlopeClassWrap::prototype;
Nan::Persistent<Function> ESlopeClassWrap::constructor;

// Supported implementations
// ESlopeClass()
// ESlopeClass(ESlopeClass& const arg0)
ESlopeClassWrap::ESlopeClassWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ESlopeClass(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // ESlopeClass()
        m_ESlopeClass = new ESlopeClass();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "ESlopeClass")))
    {
        // Unwrap object
        ESlopeClassWrap* arg0_wrap = ObjectWrap::Unwrap<ESlopeClassWrap>(info[0]->ToObject());
        ESlopeClass* arg0 = arg0_wrap->GetWrapped();

        // ESlopeClass(ESlopeClass& const arg0)
        m_ESlopeClass = new ESlopeClass(*arg0);
    }
}

ESlopeClassWrap::~ESlopeClassWrap()
{
    delete m_ESlopeClass;
}

NAN_MODULE_INIT(ESlopeClassWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ESlopeClassWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "fromString", FromString);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("ESlopeClass").ToLocalChecked(), function);
}

NAN_METHOD(ESlopeClassWrap::FromString)
{
    ESlopeClassWrap* wrappedESlopeClass = ObjectWrap::Unwrap<ESlopeClassWrap>(info.This());
    ESlopeClass* eSlopeClass = wrappedESlopeClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        ESlope arg1_value = static_cast<ESlope>(info[1]->NumberValue());
        ESlope* arg1 = &arg1_value;

        // Call wrapped method
        eSlopeClass->FromString(*arg0, arg1);
    }
}

NAN_METHOD(ESlopeClassWrap::ToString)
{
    ESlopeClassWrap* wrappedESlopeClass = ObjectWrap::Unwrap<ESlopeClassWrap>(info.This());
    ESlopeClass* eSlopeClass = wrappedESlopeClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        ESlope arg1_value = static_cast<ESlope>(info[1]->NumberValue());
        ESlope* arg1 = &arg1_value;

        // Call wrapped method
        eSlopeClass->ToString(*arg0, arg1);
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        ESlope arg0 = static_cast<ESlope>(info[0]->NumberValue());

        // Call wrapped method
        eSlopeClass->ToString(arg0);
    }
}
