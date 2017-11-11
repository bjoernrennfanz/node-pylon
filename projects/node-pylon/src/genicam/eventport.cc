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

#include "eventport.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> EventPortWrap::prototype;
Nan::Persistent<Function> EventPortWrap::constructor;

// Supported implementations
// CEventPort(INode* pNode)
// CEventPort(CEventPort& const arg0)
EventPortWrap::EventPortWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_EventPort(NULL)
{
    // Check constructor arguments
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Unwrap object
        NodeWrap* arg0_wrap = ObjectWrap::Unwrap<NodeWrap>(info[0]->ToObject());
        INode* arg0 = arg0_wrap->GetWrapped();

        // CEventPort(INode* pNode)
        m_EventPort = new CEventPort(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CEventPort")))
    {
        // Unwrap object
        EventPortWrap* arg0_wrap = ObjectWrap::Unwrap<EventPortWrap>(info[0]->ToObject());
        CEventPort* arg0 = arg0_wrap->GetWrapped();

        // CEventPort(CEventPort& const arg0)
        m_EventPort = new CEventPort(*arg0);
    }
}

EventPortWrap::~EventPortWrap()
{
    delete m_EventPort;
}

NAN_MODULE_INIT(EventPortWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("EventPortWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getAccessMode", GetAccessMode);
    Nan::SetPrototypeMethod(tpl, "getPrincipalInterfaceType", GetPrincipalInterfaceType);
    Nan::SetPrototypeMethod(tpl, "read", Read);
    Nan::SetPrototypeMethod(tpl, "write", Write);
    Nan::SetPrototypeMethod(tpl, "setPortImpl", SetPortImpl);
    Nan::SetPrototypeMethod(tpl, "getSwapEndianess", GetSwapEndianess);
    Nan::SetPrototypeMethod(tpl, "invalidateNode", InvalidateNode);
    Nan::SetPrototypeMethod(tpl, "attachNode", AttachNode);
    Nan::SetPrototypeMethod(tpl, "detachNode", DetachNode);
    Nan::SetPrototypeMethod(tpl, "getEventIDLength", GetEventIDLength);
    Nan::SetPrototypeMethod(tpl, "checkEventID", CheckEventID);
    Nan::SetPrototypeMethod(tpl, "attachEvent", AttachEvent);
    Nan::SetPrototypeMethod(tpl, "detachEvent", DetachEvent);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CEventPort").ToLocalChecked(), function);
}

NAN_METHOD(EventPortWrap::AttachEvent)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "unsigned char")) && info[1]->IsNumber())
    {
        // Unwrap object
        unsigned charWrap* arg0_wrap = ObjectWrap::Unwrap<unsigned charWrap>(info[0]->ToObject());
        unsigned char* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());
    }
}

NAN_METHOD(EventPortWrap::AttachNode)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Unwrap object
        NodeWrap* arg0_wrap = ObjectWrap::Unwrap<NodeWrap>(info[0]->ToObject());
        INode* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(EventPortWrap::CheckEventID)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "unsigned char")) && info[1]->IsNumber())
    {
        // Unwrap object
        unsigned charWrap* arg0_wrap = ObjectWrap::Unwrap<unsigned charWrap>(info[0]->ToObject());
        unsigned char* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        __uint128_t arg0 = static_cast<__uint128_t>(info[0]->NumberValue());
    }
}

NAN_METHOD(EventPortWrap::DetachEvent)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::DetachNode)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::GetAccessMode)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::GetEventIDLength)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::GetPrincipalInterfaceType)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::GetSwapEndianess)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::InvalidateNode)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if (info.Length() == 0)
    {
    }
}

NAN_METHOD(EventPortWrap::Read)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "void")) && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // Unwrap object
        voidWrap* arg0_wrap = ObjectWrap::Unwrap<voidWrap>(info[0]->ToObject());
        void* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());
    }
}

NAN_METHOD(EventPortWrap::SetPortImpl)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();
    }
}

NAN_METHOD(EventPortWrap::Write)
{
    EventPortWrap* wrappedEventPort = ObjectWrap::Unwrap<EventPortWrap>(info.This());
    CEventPort* eventPort = wrappedEventPort->GetWrapped();

    if ((info.Length() == 3) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "void")) && info[1]->IsNumber() && info[2]->IsNumber())
    {
        // Unwrap object
        voidWrap* arg0_wrap = ObjectWrap::Unwrap<voidWrap>(info[0]->ToObject());
        void* arg0 = arg0_wrap->GetWrapped();

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());
    }
}
