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

#include "eventadaptergev.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> EventAdapterGEVWrap::prototype;
Nan::Persistent<Function> EventAdapterGEVWrap::constructor;

// Supported implementations
// CEventAdapterGEV(INodeMap* pNodeMap)
// CEventAdapterGEV(CEventAdapterGEV& const arg0)
EventAdapterGEVWrap::EventAdapterGEVWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_EventAdapterGEV(NULL)
{
    // Check constructor arguments
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // CEventAdapterGEV(INodeMap* pNodeMap)
        m_EventAdapterGEV = new CEventAdapterGEV(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CEventAdapterGEV")))
    {
        // Unwrap object
        EventAdapterGEVWrap* arg0_wrap = ObjectWrap::Unwrap<EventAdapterGEVWrap>(info[0]->ToObject());
        CEventAdapterGEV* arg0 = arg0_wrap->GetWrapped();

        // CEventAdapterGEV(CEventAdapterGEV& const arg0)
        m_EventAdapterGEV = new CEventAdapterGEV(*arg0);
    }
}

EventAdapterGEVWrap::~EventAdapterGEVWrap()
{
    delete m_EventAdapterGEV;
}

NAN_MODULE_INIT(EventAdapterGEVWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("EventAdapterGEVWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "deliverMessage", DeliverMessage);
    Nan::SetPrototypeMethod(tpl, "deliverEventMessage", DeliverEventMessage);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CEventAdapterGEV").ToLocalChecked(), function);
}

NAN_METHOD(EventAdapterGEVWrap::DeliverEventMessage)
{
    EventAdapterGEVWrap* wrappedEventAdapterGEV = ObjectWrap::Unwrap<EventAdapterGEVWrap>(info.This());
    CEventAdapterGEV* eventAdapterGEV = wrappedEventAdapterGEV->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GVCP_EVENTDATA_REQUEST_EXTENDED_ID")))
    {
        // Unwrap object
        GVCPEventdataRequestExtendedIdWrap* arg0_wrap = ObjectWrap::Unwrap<GVCPEventdataRequestExtendedIdWrap>(info[0]->ToObject());
        GVCP_EVENTDATA_REQUEST_EXTENDED_ID* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        eventAdapterGEV->DeliverEventMessage(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GVCP_EVENT_REQUEST_EXTENDED_ID")))
    {
        // Unwrap object
        GVCPEventRequestExtendedIdWrap* arg0_wrap = ObjectWrap::Unwrap<GVCPEventRequestExtendedIdWrap>(info[0]->ToObject());
        GVCP_EVENT_REQUEST_EXTENDED_ID* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        eventAdapterGEV->DeliverEventMessage(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GVCP_EVENTDATA_REQUEST")))
    {
        // Unwrap object
        GVCPEventdataRequestWrap* arg0_wrap = ObjectWrap::Unwrap<GVCPEventdataRequestWrap>(info[0]->ToObject());
        GVCP_EVENTDATA_REQUEST* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        eventAdapterGEV->DeliverEventMessage(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "GVCP_EVENT_REQUEST")))
    {
        // Unwrap object
        GVCPEventRequestWrap* arg0_wrap = ObjectWrap::Unwrap<GVCPEventRequestWrap>(info[0]->ToObject());
        GVCP_EVENT_REQUEST* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        eventAdapterGEV->DeliverEventMessage(arg0);
    }
}

NAN_METHOD(EventAdapterGEVWrap::DeliverMessage)
{
    EventAdapterGEVWrap* wrappedEventAdapterGEV = ObjectWrap::Unwrap<EventAdapterGEVWrap>(info.This());
    CEventAdapterGEV* eventAdapterGEV = wrappedEventAdapterGEV->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsString() && info[1]->IsNumber())
    {
        // Convert from string value
        unsigned char* arg0 = static_cast<unsigned char*>(pylon_v8::ToGCString(info[0]->ToString()).c_str());

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Call wrapped method
        eventAdapterGEV->DeliverMessage(arg0, arg1);
    }
}
