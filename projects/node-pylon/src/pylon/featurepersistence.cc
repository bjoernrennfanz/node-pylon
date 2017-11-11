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

#include "featurepersistence.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> FeaturePersistenceWrap::prototype;
Nan::Persistent<Function> FeaturePersistenceWrap::constructor;

// Supported implementations
// CFeaturePersistence()
// CFeaturePersistence(CFeaturePersistence& const arg0)
FeaturePersistenceWrap::FeaturePersistenceWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_FeaturePersistence(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CFeaturePersistence()
        m_FeaturePersistence = new CFeaturePersistence();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CFeaturePersistence")))
    {
        // Unwrap object
        FeaturePersistenceWrap* arg0_wrap = ObjectWrap::Unwrap<FeaturePersistenceWrap>(info[0]->ToObject());
        CFeaturePersistence* arg0 = arg0_wrap->GetWrapped();

        // CFeaturePersistence(CFeaturePersistence& const arg0)
        m_FeaturePersistence = new CFeaturePersistence(*arg0);
    }
}

FeaturePersistenceWrap::~FeaturePersistenceWrap()
{
    delete m_FeaturePersistence;
}

NAN_MODULE_INIT(FeaturePersistenceWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("FeaturePersistenceWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "load", Load);
    Nan::SetPrototypeMethod(tpl, "save", Save);
    Nan::SetPrototypeMethod(tpl, "loadFromString", LoadFromString);
    Nan::SetPrototypeMethod(tpl, "saveToString", SaveToString);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CFeaturePersistence").ToLocalChecked(), function);
}

NAN_METHOD(FeaturePersistenceWrap::Load)
{
    FeaturePersistenceWrap* wrappedFeaturePersistence = ObjectWrap::Unwrap<FeaturePersistenceWrap>(info.This());
    CFeaturePersistence* featurePersistence = wrappedFeaturePersistence->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INodeMap")) && info[2]->IsBoolean())
    {
        // Unwrap object
        gcstringWrap* arg0_wrap = ObjectWrap::Unwrap<gcstringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        NodeMapWrap* arg1_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[1]->ToObject());
        INodeMap* arg1 = arg1_wrap->GetWrapped();
    }
}

NAN_METHOD(FeaturePersistenceWrap::LoadFromString)
{
    FeaturePersistenceWrap* wrappedFeaturePersistence = ObjectWrap::Unwrap<FeaturePersistenceWrap>(info.This());
    CFeaturePersistence* featurePersistence = wrappedFeaturePersistence->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INodeMap")) && info[2]->IsBoolean())
    {
        // Unwrap object
        gcstringWrap* arg0_wrap = ObjectWrap::Unwrap<gcstringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        NodeMapWrap* arg1_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[1]->ToObject());
        INodeMap* arg1 = arg1_wrap->GetWrapped();
    }
}

NAN_METHOD(FeaturePersistenceWrap::Save)
{
    FeaturePersistenceWrap* wrappedFeaturePersistence = ObjectWrap::Unwrap<FeaturePersistenceWrap>(info.This());
    CFeaturePersistence* featurePersistence = wrappedFeaturePersistence->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        gcstringWrap* arg0_wrap = ObjectWrap::Unwrap<gcstringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        NodeMapWrap* arg1_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[1]->ToObject());
        INodeMap* arg1 = arg1_wrap->GetWrapped();
    }
}

NAN_METHOD(FeaturePersistenceWrap::SaveToString)
{
    FeaturePersistenceWrap* wrappedFeaturePersistence = ObjectWrap::Unwrap<FeaturePersistenceWrap>(info.This());
    CFeaturePersistence* featurePersistence = wrappedFeaturePersistence->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        gcstringWrap* arg0_wrap = ObjectWrap::Unwrap<gcstringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        NodeMapWrap* arg1_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[1]->ToObject());
        INodeMap* arg1 = arg1_wrap->GetWrapped();
    }
}
