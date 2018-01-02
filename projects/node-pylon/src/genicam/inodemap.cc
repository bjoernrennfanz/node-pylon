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

#include "inodemap.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> NodeMapWrap::prototype;
Nan::Persistent<Function> NodeMapWrap::constructor;

// Supported implementations
// INodeMap()
// INodeMap(INodeMap& const arg0)
NodeMapWrap::NodeMapWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_NodeMap(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // INodeMap()
        m_NodeMap = new INodeMap();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // INodeMap(INodeMap& const arg0)
        m_NodeMap = new INodeMap(*arg0);
    }
}

NodeMapWrap::~NodeMapWrap()
{
    delete m_NodeMap;
}

NAN_MODULE_INIT(NodeMapWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("NodeMapWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getNodes", GetNodes);
    Nan::SetPrototypeMethod(tpl, "getNode", GetNode);
    Nan::SetPrototypeMethod(tpl, "invalidateNodes", InvalidateNodes);
    Nan::SetPrototypeMethod(tpl, "connect", Connect);
    Nan::SetPrototypeMethod(tpl, "getDeviceName", GetDeviceName);
    Nan::SetPrototypeMethod(tpl, "poll", Poll);
    Nan::SetPrototypeMethod(tpl, "getLock", GetLock);
    Nan::SetPrototypeMethod(tpl, "getNumNodes", GetNumNodes);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("INodeMap").ToLocalChecked(), function);
}

NAN_METHOD(NodeMapWrap::Connect)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        GCStringWrap* arg1_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[1]->ToObject());
        gcstring* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        bool result = nodeMap->Connect(arg0, *arg1);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IPort")))
    {
        // Unwrap object
        PortWrap* arg0_wrap = ObjectWrap::Unwrap<PortWrap>(info[0]->ToObject());
        IPort* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        bool result = nodeMap->Connect(arg0);

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(NodeMapWrap::GetDeviceName)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMap->GetDeviceName();
    }
}

NAN_METHOD(NodeMapWrap::GetLock)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        // TODO: Implement return value wrapper for nodeMap->GetLock()
    }
}

NAN_METHOD(NodeMapWrap::GetNode)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "gcstring")))
    {
        // Unwrap object
        GCStringWrap* arg0_wrap = ObjectWrap::Unwrap<GCStringWrap>(info[0]->ToObject());
        gcstring* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMap->GetNode(*arg0);
    }
}

NAN_METHOD(NodeMapWrap::GetNodes)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "node_vector")))
    {
        // Unwrap object
        NodeVectorWrap* arg0_wrap = ObjectWrap::Unwrap<NodeVectorWrap>(info[0]->ToObject());
        node_vector* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeMap->GetNodes(*arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeMapWrap::GetNumNodes)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        __uint128_t result = nodeMap->GetNumNodes();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(NodeMapWrap::InvalidateNodes)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeMap->InvalidateNodes();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeMapWrap::Poll)
{
    NodeMapWrap* wrappedNodeMap = ObjectWrap::Unwrap<NodeMapWrap>(info.This());
    INodeMap* nodeMap = wrappedNodeMap->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        __int128_t arg0 = static_cast<__int128_t>(info[0]->NumberValue());

        // Call wrapped method
        nodeMap->Poll(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
