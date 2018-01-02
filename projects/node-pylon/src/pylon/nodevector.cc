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

#include "nodevector.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> NodeVectorWrap::prototype;
Nan::Persistent<Function> NodeVectorWrap::constructor;

// Supported implementations
// node_vector()
// node_vector(unsigned int uiSize)
// node_vector(node_vector& const obj)
NodeVectorWrap::NodeVectorWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_NodeVector(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // node_vector()
        m_NodeVector = new node_vector();
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // node_vector(unsigned int uiSize)
        m_NodeVector = new node_vector(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "node_vector")))
    {
        // Unwrap object
        NodeVectorWrap* arg0_wrap = ObjectWrap::Unwrap<NodeVectorWrap>(info[0]->ToObject());
        node_vector* arg0 = arg0_wrap->GetWrapped();

        // node_vector(node_vector& const obj)
        m_NodeVector = new node_vector(*arg0);
    }
}

NodeVectorWrap::~NodeVectorWrap()
{
    delete m_NodeVector;
}

NAN_MODULE_INIT(NodeVectorWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("NodeVectorWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "assign", assign);
    Nan::SetPrototypeMethod(tpl, "clear", clear);
    Nan::SetPrototypeMethod(tpl, "erase", erase);
    Nan::SetPrototypeMethod(tpl, "at", at);
    Nan::SetPrototypeMethod(tpl, "back", back);
    Nan::SetPrototypeMethod(tpl, "begin", begin);
    Nan::SetPrototypeMethod(tpl, "capacity", capacity);
    Nan::SetPrototypeMethod(tpl, "end", end);
    Nan::SetPrototypeMethod(tpl, "front", front);
    Nan::SetPrototypeMethod(tpl, "max_size", max_size);
    Nan::SetPrototypeMethod(tpl, "size", size);
    Nan::SetPrototypeMethod(tpl, "insert", insert);
    Nan::SetPrototypeMethod(tpl, "empty", empty);
    Nan::SetPrototypeMethod(tpl, "pop_back", pop_back);
    Nan::SetPrototypeMethod(tpl, "push_back", push_back);
    Nan::SetPrototypeMethod(tpl, "resize", resize);
    Nan::SetPrototypeMethod(tpl, "reserve", reserve);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("node_vector").ToLocalChecked(), function);
}

NAN_METHOD(NodeVectorWrap::assign)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        NodeWrap* arg1_wrap = ObjectWrap::Unwrap<NodeWrap>(info[1]->ToObject());
        INode* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeVector->assign(arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::at)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        nodeVector->at(arg0);
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        nodeVector->at(arg0);
    }
}

NAN_METHOD(NodeVectorWrap::back)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->back();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->back();
    }
}

NAN_METHOD(NodeVectorWrap::begin)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->begin();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->begin();
    }
}

NAN_METHOD(NodeVectorWrap::capacity)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = nodeVector->capacity();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(NodeVectorWrap::clear)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->clear();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::empty)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        bool result = nodeVector->empty();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Boolean>(result));
    }
}

NAN_METHOD(NodeVectorWrap::end)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->end();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->end();
    }
}

NAN_METHOD(NodeVectorWrap::erase)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        nodeVector->erase(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "iterator")))
    {
        // Unwrap object
        IteratorWrap* arg0_wrap = ObjectWrap::Unwrap<IteratorWrap>(info[0]->ToObject());
        iterator* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeVector->erase(arg0);
    }
}

NAN_METHOD(NodeVectorWrap::front)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->front();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->front();
    }
}

NAN_METHOD(NodeVectorWrap::insert)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        NodeWrap* arg1_wrap = ObjectWrap::Unwrap<NodeWrap>(info[1]->ToObject());
        INode* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeVector->insert(arg0, arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "iterator")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Unwrap object
        IteratorWrap* arg0_wrap = ObjectWrap::Unwrap<IteratorWrap>(info[0]->ToObject());
        iterator* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        NodeWrap* arg1_wrap = ObjectWrap::Unwrap<NodeWrap>(info[1]->ToObject());
        INode* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        nodeVector->insert(arg0, arg1);
    }
}

NAN_METHOD(NodeVectorWrap::max_size)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = nodeVector->max_size();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}

NAN_METHOD(NodeVectorWrap::pop_back)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        nodeVector->pop_back();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::push_back)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INode")))
    {
        // Unwrap object
        NodeWrap* arg0_wrap = ObjectWrap::Unwrap<NodeWrap>(info[0]->ToObject());
        INode* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        nodeVector->push_back(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::reserve)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        nodeVector->reserve(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::resize)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        nodeVector->resize(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(NodeVectorWrap::size)
{
    NodeVectorWrap* wrappedNodeVector = ObjectWrap::Unwrap<NodeVectorWrap>(info.This());
    node_vector* nodeVector = wrappedNodeVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        unsigned int result = nodeVector->size();

        // Set return value
        info.GetReturnValue().Set(Nan::New<Number>(result));
    }
}
