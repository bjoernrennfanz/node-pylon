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

#pragma once

#include <node.h>
#include <nan.h>

#include <pylon/ConfigurationEventHandler.h>
#include "acquirecontinuousconfiguration.h"

using namespace v8;
using namespace Pylon;

class ConfigurationEventHandlerWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CConfigurationEventHandler* GetWrapped() const
    {
        return m_ConfigurationEventHandler;
    };

    void SetWrapped(CConfigurationEventHandler* configurationEventHandler)
    {
        m_ConfigurationEventHandler = configurationEventHandler;
    };

    static v8::Handle<v8::Value> NewInstance(CConfigurationEventHandler* configurationEventHandler);

private:
    static Nan::Persistent<v8::Function> constructor;
    ConfigurationEventHandlerWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~ConfigurationEventHandlerWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(OnAttach);
    static NAN_METHOD(OnAttached);
    static NAN_METHOD(OnDetach);
    static NAN_METHOD(OnDetached);
    static NAN_METHOD(OnDestroy);
    static NAN_METHOD(OnDestroyed);
    static NAN_METHOD(OnOpen);
    static NAN_METHOD(OnOpened);
    static NAN_METHOD(OnClose);
    static NAN_METHOD(OnClosed);
    static NAN_METHOD(OnGrabStart);
    static NAN_METHOD(OnGrabStarted);
    static NAN_METHOD(OnGrabStop);
    static NAN_METHOD(OnGrabStopped);
    static NAN_METHOD(OnGrabError);
    static NAN_METHOD(OnCameraDeviceRemoved);
    static NAN_METHOD(OnConfigurationRegistered);
    static NAN_METHOD(OnConfigurationDeregistered);
    static NAN_METHOD(DestroyConfiguration);
    static NAN_METHOD(DebugGetEventHandlerRegistrationCount);

    // Wrapped object
    CConfigurationEventHandler* m_ConfigurationEventHandler;
};
