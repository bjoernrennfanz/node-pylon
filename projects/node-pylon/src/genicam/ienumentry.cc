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

#include "ienumentry.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> EnumEntryWrap::prototype;
Nan::Persistent<Function> EnumEntryWrap::constructor;

// Supported implementations
// IEnumEntry()
// IEnumEntry(IEnumEntry& const arg0)
EnumEntryWrap::EnumEntryWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_EnumEntry(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IEnumEntry()
        m_EnumEntry = new IEnumEntry();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IEnumEntry")))
    {
        // Unwrap object
        EnumEntryWrap* arg0_wrap = ObjectWrap::Unwrap<EnumEntryWrap>(info[0]->ToObject());
        IEnumEntry* arg0 = arg0_wrap->GetWrapped();

        // IEnumEntry(IEnumEntry& const arg0)
        m_EnumEntry = new IEnumEntry(*arg0);
    }
}

EnumEntryWrap::~EnumEntryWrap()
{
    delete m_EnumEntry;
}

NAN_MODULE_INIT(EnumEntryWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("EnumEntryWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getValue", GetValue);
    Nan::SetPrototypeMethod(tpl, "getSymbolic", GetSymbolic);
    Nan::SetPrototypeMethod(tpl, "getNumericValue", GetNumericValue);
    Nan::SetPrototypeMethod(tpl, "isSelfClearing", IsSelfClearing);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IEnumEntry").ToLocalChecked(), function);
}

NAN_METHOD(EnumEntryWrap::GetNumericValue)
{
    EnumEntryWrap* wrappedEnumEntry = ObjectWrap::Unwrap<EnumEntryWrap>(info.This());
    IEnumEntry* enumEntry = wrappedEnumEntry->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        enumEntry->GetNumericValue();
    }
}

NAN_METHOD(EnumEntryWrap::GetSymbolic)
{
    EnumEntryWrap* wrappedEnumEntry = ObjectWrap::Unwrap<EnumEntryWrap>(info.This());
    IEnumEntry* enumEntry = wrappedEnumEntry->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        enumEntry->GetSymbolic();
    }
}

NAN_METHOD(EnumEntryWrap::GetValue)
{
    EnumEntryWrap* wrappedEnumEntry = ObjectWrap::Unwrap<EnumEntryWrap>(info.This());
    IEnumEntry* enumEntry = wrappedEnumEntry->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        enumEntry->GetValue();
    }
}

NAN_METHOD(EnumEntryWrap::IsSelfClearing)
{
    EnumEntryWrap* wrappedEnumEntry = ObjectWrap::Unwrap<EnumEntryWrap>(info.This());
    IEnumEntry* enumEntry = wrappedEnumEntry->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        enumEntry->IsSelfClearing();
    }
}
