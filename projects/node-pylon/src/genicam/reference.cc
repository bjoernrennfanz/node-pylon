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

#include "reference.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> ReferenceWrap::prototype;
Nan::Persistent<Function> ReferenceWrap::constructor;

// Supported implementations
// IReference()
// IReference(IReference& const arg0)
ReferenceWrap::ReferenceWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_Reference(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // IReference()
        m_Reference = new IReference();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IReference")))
    {
        // Unwrap object
        ReferenceWrap* arg0_wrap = ObjectWrap::Unwrap<ReferenceWrap>(info[0]->ToObject());
        IReference* arg0 = arg0_wrap->GetWrapped();

        // IReference(IReference& const arg0)
        m_Reference = new IReference(*arg0);
    }
}

ReferenceWrap::~ReferenceWrap()
{
    delete m_Reference;
}

NAN_MODULE_INIT(ReferenceWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ReferenceWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "setReference", SetReference);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("IReference").ToLocalChecked(), function);
}

NAN_METHOD(ReferenceWrap::SetReference)
{
    ReferenceWrap* wrappedReference = ObjectWrap::Unwrap<ReferenceWrap>(info.This());
    IReference* reference = wrappedReference->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IBase")))
    {
        // Unwrap object
        BaseWrap* arg0_wrap = ObjectWrap::Unwrap<BaseWrap>(info[0]->ToObject());
        IBase* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        reference->SetReference(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
