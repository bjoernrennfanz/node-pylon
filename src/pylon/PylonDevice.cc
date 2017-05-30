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

#include "PylonDevice.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> PylonDeviceWrap::prototype;
Nan::Persistent<Function> PylonDeviceWrap::constructor;

// Supported implementations:
//   PylonDevice()
PylonDeviceWrap::PylonDeviceWrap(Nan::NAN_METHOD_ARGS_TYPE args)
  : m_PylonDevice(NULL)
{
    // Standalone constructor not implemented
    // internal use SetWrapped()
}

PylonDeviceWrap::~PylonDeviceWrap() 
{
}

NAN_MODULE_INIT(PylonDeviceWrap::Initialize)
{
    // Prepare constructor template
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("PylonDevice").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);  

    // Prototype

    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("PylonDevice").ToLocalChecked(), function);
}

NAN_METHOD(PylonDeviceWrap::New) 
{
    PylonDeviceWrap* wrappedPylonDevice = new PylonDeviceWrap(info);
    wrappedPylonDevice->Wrap(info.This());
}

Handle<Value> PylonDeviceWrap::NewInstance(IPylonDevice* ptrPylonDevice)
{
    Nan::EscapableHandleScope scope;
  
    Local<Object> instance = Nan::NewInstance(Nan::New(constructor), 0, NULL).ToLocalChecked();
    PylonDeviceWrap* wrappedPylonDevice = node::ObjectWrap::Unwrap<PylonDeviceWrap>(instance);
    wrappedPylonDevice->SetWrapped(ptrPylonDevice);

    return scope.Escape(instance);
}