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

#pragma once

#include <node.h>
#include <nan.h>

#include <GenApi/INode.h>
#include "container.h"
#include "ibase.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

class NodeWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    INode* GetWrapped() const
    {
        return m_Node;
    };

    void SetWrapped(INode* node)
    {
        m_Node = node;
    };

    static v8::Handle<v8::Value> NewInstance(INode* node);

private:
    static Nan::Persistent<v8::Function> constructor;
    NodeWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~NodeWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetName);
    static NAN_METHOD(GetNameSpace);
    static NAN_METHOD(GetVisibility);
    static NAN_METHOD(InvalidateNode);
    static NAN_METHOD(IsCachable);
    static NAN_METHOD(IsAccessModeCacheable);
    static NAN_METHOD(GetCachingMode);
    static NAN_METHOD(GetPollingTime);
    static NAN_METHOD(GetToolTip);
    static NAN_METHOD(GetDescription);
    static NAN_METHOD(GetDisplayName);
    static NAN_METHOD(GetDeviceName);
    static NAN_METHOD(GetChildren);
    static NAN_METHOD(GetParents);
    static NAN_METHOD(RegisterCallback);
    static NAN_METHOD(DeregisterCallback);
    static NAN_METHOD(GetNodeMap);
    static NAN_METHOD(GetEventID);
    static NAN_METHOD(IsStreamable);
    static NAN_METHOD(GetPropertyNames);
    static NAN_METHOD(GetProperty);
    static NAN_METHOD(ImposeAccessMode);
    static NAN_METHOD(ImposeVisibility);
    static NAN_METHOD(GetAlias);
    static NAN_METHOD(GetCastAlias);
    static NAN_METHOD(GetDocuURL);
    static NAN_METHOD(IsDeprecated);
    static NAN_METHOD(GetPrincipalInterfaceType);
    static NAN_METHOD(IsFeature);

private:
    // Wrapped functions
    static NAN_METHOD(IsReadable);
    static NAN_METHOD(IsWritable);
    static NAN_METHOD(IsImplemented);
    static NAN_METHOD(IsAvailable);
    static NAN_METHOD(Combine);
    static NAN_METHOD(IsVisible);
    static NAN_METHOD(IsCacheable);

    // Wrapped object
    INode* m_Node;
};
