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

#include <pylon/InstantCamera.h>
#include "acquirecontinuousconfiguration.h"
#include "cameraeventhandler.h"
#include "deviceinfo.h"
#include "grabresultdata.h"
#include "grabresultptr.h"
#include "imageeventhandler.h"
#include "pylondevice.h"

using namespace v8;
using namespace Pylon;

class InstantCameraWrap : public node::ObjectWrap
{
public:
    static NAN_MODULE_INIT(Initialize);
    static Nan::Persistent<v8::FunctionTemplate> prototype;

    CInstantCamera* GetWrapped() const
    {
        return m_InstantCamera;
    };

    void SetWrapped(CInstantCamera* instantCamera)
    {
        m_InstantCamera = instantCamera;
    };

    static v8::Handle<v8::Value> NewInstance(CInstantCamera* instantCamera);

private:
    static Nan::Persistent<v8::Function> constructor;
    InstantCameraWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~InstantCameraWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(Attach);
    static NAN_METHOD(IsPylonDeviceAttached);
    static NAN_METHOD(IsCameraDeviceRemoved);
    static NAN_METHOD(HasOwnership);
    static NAN_METHOD(DestroyDevice);
    static NAN_METHOD(DetachDevice);
    static NAN_METHOD(Open);
    static NAN_METHOD(IsOpen);
    static NAN_METHOD(Close);
    static NAN_METHOD(StartGrabbing);
    static NAN_METHOD(RetrieveResult);
    static NAN_METHOD(StopGrabbing);
    static NAN_METHOD(IsGrabbing);
    static NAN_METHOD(GrabOne);
    static NAN_METHOD(GetQueuedBufferCount);
    static NAN_METHOD(GetGrabResultWaitObject);
    static NAN_METHOD(GetGrabStopWaitObject);
    static NAN_METHOD(GetCameraEventWaitObject);
    static NAN_METHOD(RegisterConfiguration);
    static NAN_METHOD(DeregisterConfiguration);
    static NAN_METHOD(RegisterImageEventHandler);
    static NAN_METHOD(DeregisterImageEventHandler);
    static NAN_METHOD(RegisterCameraEventHandler);
    static NAN_METHOD(DeregisterCameraEventHandler);
    static NAN_METHOD(WaitForFrameTriggerReady);
    static NAN_METHOD(CanWaitForFrameTriggerReady);
    static NAN_METHOD(ExecuteSoftwareTrigger);
    static NAN_METHOD(SetCameraContext);
    static NAN_METHOD(GetCameraContext);
    static NAN_METHOD(GetDeviceInfo);
    static NAN_METHOD(GetNodeMap);
    static NAN_METHOD(GetTLNodeMap);
    static NAN_METHOD(GetStreamGrabberNodeMap);
    static NAN_METHOD(GetEventGrabberNodeMap);
    static NAN_METHOD(GetInstantCameraNodeMap);
    static NAN_METHOD(SetBufferFactory);
    static NAN_METHOD(Is1394);
    static NAN_METHOD(IsGigE);
    static NAN_METHOD(IsUsb);
    static NAN_METHOD(IsCameraLink);
    static NAN_METHOD(GetSfncVersion);
    static NAN_METHOD(GetExtensionInterface);
    static NAN_METHOD(IsBcon);

    // Wrapped object
    CInstantCamera* m_InstantCamera;
};
