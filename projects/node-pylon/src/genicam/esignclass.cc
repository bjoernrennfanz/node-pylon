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

#include "esignclass.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> ESignClassWrap::prototype;
Nan::Persistent<Function> ESignClassWrap::constructor;

// Supported implementations
// ESignClass()
// ESignClass(ESignClass& const arg0)
ESignClassWrap::ESignClassWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ESignClass(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // ESignClass()
        m_ESignClass = new ESignClass();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "ESignClass")))
    {
        // Unwrap object
        ESignClassWrap* arg0_wrap = ObjectWrap::Unwrap<ESignClassWrap>(info[0]->ToObject());
        ESignClass* arg0 = arg0_wrap->GetWrapped();

        // ESignClass(ESignClass& const arg0)
        m_ESignClass = new ESignClass(*arg0);
    }
}

ESignClassWrap::~ESignClassWrap()
{
    delete m_ESignClass;
}

NAN_MODULE_INIT(ESignClassWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ESignClassWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "fromString", FromString);
    Nan::SetPrototypeMethod(tpl, "toString", ToString);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("ESignClass").ToLocalChecked(), function);
}

NAN_METHOD(ESignClassWrap::FromString)
{
    ESignClassWrap* wrappedESignClass = ObjectWrap::Unwrap<ESignClassWrap>(info.This());
    ESignClass* eSignClass = wrappedESignClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        ESign arg1_value = static_cast<ESign>(info[1]->NumberValue());
        ESign* arg1 = &arg1_value;

        // Call wrapped method
        bool result = eSignClass->FromString(*arg0, arg1);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(ESignClassWrap::ToString)
{
    ESignClassWrap* wrappedESignClass = ObjectWrap::Unwrap<ESignClassWrap>(info.This());
    ESignClass* eSignClass = wrappedESignClass->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && info[1]->IsNumber())
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value to pointer
        ESign arg1_value = static_cast<ESign>(info[1]->NumberValue());
        ESign* arg1 = &arg1_value;

        // Call wrapped method
        eSignClass->ToString(*arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        ESign arg0 = static_cast<ESign>(info[0]->NumberValue());

        // Call wrapped method
        eSignClass->ToString(arg0);
    }
}
