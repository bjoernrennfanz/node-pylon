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

#include "eendianessclass.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> EEndianessClassWrap::prototype;
Nan::Persistent<Function> EEndianessClassWrap::constructor;

// Supported implementations
// EEndianessClass()
// EEndianessClass(EEndianessClass& const arg0)
EEndianessClassWrap::EEndianessClassWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_EEndianessClass(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // EEndianessClass()
        m_EEndianessClass = new EEndianessClass();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "EEndianessClass")))
    {
        // Unwrap object
        EEndianessClassWrap* arg0_wrap = ObjectWrap::Unwrap<EEndianessClassWrap>(info[0]->ToObject());
        EEndianessClass* arg0 = arg0_wrap->GetWrapped();

        // EEndianessClass(EEndianessClass& const arg0)
        m_EEndianessClass = new EEndianessClass(*arg0);
    }
}

EEndianessClassWrap::~EEndianessClassWrap()
{
    delete m_EEndianessClass;
}

NAN_MODULE_INIT(EEndianessClassWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("EEndianessClassWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "fromString", FromString);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("EEndianessClass").ToLocalChecked(), function);
}

NAN_METHOD(EEndianessClassWrap::FromString)
{
    EEndianessClassWrap* wrappedEEndianessClass = ObjectWrap::Unwrap<EEndianessClassWrap>(info.This());
    EEndianessClass* eEndianessClass = wrappedEEndianessClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        EEndianess arg1_value = static_cast<EEndianess>(info[1]->NumberValue());
        EEndianess* arg1 = &arg1_value;

        // Call wrapped method
        eEndianessClass->FromString(*arg0, arg1);
    }
}

NAN_METHOD(EEndianessClassWrap::ToString)
{
    EEndianessClassWrap* wrappedEEndianessClass = ObjectWrap::Unwrap<EEndianessClassWrap>(info.This());
    EEndianessClass* eEndianessClass = wrappedEEndianessClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        EEndianess arg1_value = static_cast<EEndianess>(info[1]->NumberValue());
        EEndianess* arg1 = &arg1_value;

        // Call wrapped method
        eEndianessClass->ToString(*arg0, arg1);
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        EEndianess arg0 = static_cast<EEndianess>(info[0]->NumberValue());

        // Call wrapped method
        eEndianessClass->ToString(arg0);
    }
}
