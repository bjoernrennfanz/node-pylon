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

#include "nodemapreft.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> NodeMapRefTWrap::prototype;
Nan::Persistent<Function> NodeMapRefTWrap::constructor;

// Supported implementations
// CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(gcstring& const DeviceName)
// CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>& const Them)
// CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(INodeMap* pNodeMap, gcstring& const DeviceName)
NodeMapRefTWrap::NodeMapRefTWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_NodeMapRefT(NULL)
{
    // Check constructor arguments
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(gcstring& const DeviceName)
        m_NodeMapRefT = new CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(*arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>")))
    {
        // Unwrap object
        NodeMapRefTWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapRefTWrap>(info[0]->ToObject());
        CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* arg0 = arg0_wrap->GetWrapped();

        // CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>& const Them)
        m_NodeMapRefT = new CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(*arg0);
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INodeMap")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[1]->ToObject());
        gcstring* arg1 = arg1_wrap->GetWrapped();

        // CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(INodeMap* pNodeMap, gcstring& const DeviceName)
        m_NodeMapRefT = new CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>(arg0, *arg1);
    }
}

NodeMapRefTWrap::~NodeMapRefTWrap()
{
    delete m_NodeMapRefT;
}

NAN_MODULE_INIT(NodeMapRefTWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("NodeMapRefTWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "_Destroy", _Destroy);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromFile", _LoadXMLFromFile);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromZIPFile", _LoadXMLFromZIPFile);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromZIPData", _LoadXMLFromZIPData);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromFileInject", _LoadXMLFromFileInject);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromString", _LoadXMLFromString);
    Nan::SetPrototypeMethod(tpl, "_LoadXMLFromStringInject", _LoadXMLFromStringInject);
    Nan::SetPrototypeMethod(tpl, "_GetSupportedSchemaVersions", _GetSupportedSchemaVersions);
    Nan::SetPrototypeMethod(tpl, "_GetDeviceName", _GetDeviceName);
    Nan::SetPrototypeMethod(tpl, "_Poll", _Poll);
    Nan::SetPrototypeMethod(tpl, "_ClearXMLCache", _ClearXMLCache);
    Nan::SetPrototypeMethod(tpl, "_GetNodes", _GetNodes);
    Nan::SetPrototypeMethod(tpl, "_GetNode", _GetNode);
    Nan::SetPrototypeMethod(tpl, "_InvalidateNodes", _InvalidateNodes);
    Nan::SetPrototypeMethod(tpl, "_Connect", _Connect);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>").ToLocalChecked(), function);

    // Register static functions in Node JS
    Nan::Set(target, Nan::New<String>("castToIDestroy").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(NodeMapRefTWrap::CastToIDestroy)).ToLocalChecked());
}

NAN_METHOD(NodeMapRefTWrap::_ClearXMLCache)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMapRefT->_ClearXMLCache();
    }
}

NAN_METHOD(NodeMapRefTWrap::_Connect)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[1]->ToObject());
        gcstring* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_Connect(arg0, *arg1);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_Connect(arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_Destroy)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMapRefT->_Destroy();
    }
}

NAN_METHOD(NodeMapRefTWrap::_GetDeviceName)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMapRefT->_GetDeviceName();
    }
}

NAN_METHOD(NodeMapRefTWrap::_GetNode)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_GetNode(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_GetNodes)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "node_vector")))
    {
        // Unwrap object
        NodeVectorWrap* arg0_wrap = ObjectWrap::Unwrap<NodeVectorWrap>(info[0]->ToObject());
        node_vector* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_GetNodes(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_GetSupportedSchemaVersions)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring_vector")))
    {
        // Unwrap object
        GCStringVectorWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringVectorWrap>(info[0]->ToObject());
        gcstring_vector* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_GetSupportedSchemaVersions(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_InvalidateNodes)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMapRefT->_InvalidateNodes();
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromFile)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromFile(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromFileInject)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[1]->ToObject());
        gcstring* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromFileInject(*arg0, *arg1);
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromString)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromString(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromStringInject)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[1]->ToObject());
        gcstring* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromStringInject(*arg0, *arg1);
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromZIPData)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsObject() && info[1]->IsNumber())
    {
        // TODO: Implement wrapper for void

        // Convert from number value
        unsigned int arg1 = static_cast<unsigned int>(info[1]->NumberValue());

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromZIPData(arg0, arg1);
    }
}

NAN_METHOD(NodeMapRefTWrap::_LoadXMLFromZIPFile)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMapRefT->_LoadXMLFromZIPFile(*arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::_Poll)
{
    NodeMapRefTWrap* wrappedNodeMapRefT = ObjectWrap::Unwrap<NodeMapRefTWrap>(info.This());
    CNodeMapRefT<GenApi_3_0_Basler_pylon_v5_0::CGeneric_XMLLoaderParams>* nodeMapRefT = wrappedNodeMapRefT->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        __int128_t arg0 = static_cast<__int128_t>(info[0]->NumberValue());

        // Call wrapped method
        nodeMapRefT->_Poll(arg0);
    }
}

NAN_METHOD(NodeMapRefTWrap::CastToIDestroy)
{
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped function
        CastToIDestroy(arg0);
    }
}
