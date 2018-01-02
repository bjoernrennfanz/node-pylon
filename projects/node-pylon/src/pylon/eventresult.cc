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

#include "eventresult.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> EventResultWrap::prototype;
Nan::Persistent<Function> EventResultWrap::constructor;

// Supported implementations
// EventResult()
// EventResult(EventResult& const arg0)
EventResultWrap::EventResultWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_EventResult(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // EventResult()
        m_EventResult = new EventResult();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "EventResult")))
    {
        // Unwrap object
        EventResultWrap* arg0_wrap = ObjectWrap::Unwrap<EventResultWrap>(info[0]->ToObject());
        EventResult* arg0 = arg0_wrap->GetWrapped();

        // EventResult(EventResult& const arg0)
        m_EventResult = new EventResult(*arg0);
    }
}

EventResultWrap::~EventResultWrap()
{
    delete m_EventResult;
}

NAN_MODULE_INIT(EventResultWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("EventResultWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "succeeded", Succeeded);
    Nan::SetPrototypeMethod(tpl, "errorDescription", ErrorDescription);
    Nan::SetPrototypeMethod(tpl, "errorCode", ErrorCode);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("EventResult").ToLocalChecked(), function);
}

NAN_METHOD(EventResultWrap::ErrorCode)
{
    EventResultWrap* wrappedEventResult = ObjectWrap::Unwrap<EventResultWrap>(info.This());
    EventResult* eventResult = wrappedEventResult->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned long result = eventResult->ErrorCode();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(EventResultWrap::ErrorDescription)
{
    EventResultWrap* wrappedEventResult = ObjectWrap::Unwrap<EventResultWrap>(info.This());
    EventResult* eventResult = wrappedEventResult->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        eventResult->ErrorDescription();
    }
}

NAN_METHOD(EventResultWrap::Succeeded)
{
    EventResultWrap* wrappedEventResult = ObjectWrap::Unwrap<EventResultWrap>(info.This());
    EventResult* eventResult = wrappedEventResult->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = eventResult->Succeeded();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}
