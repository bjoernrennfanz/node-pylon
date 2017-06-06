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

#pragma once

#include <node.h>
#include <nan.h>

#include <pylon/DeviceInfo.h>

class DeviceInfoWrap : public node::ObjectWrap 
{
public:
    static Nan::Persistent<v8::FunctionTemplate> prototype;
    static NAN_MODULE_INIT(Initialize);

    Pylon::CDeviceInfo* GetWrapped() const 
    { 
        return m_DeviceInfo; 
    };

    void SetWrapped(Pylon::CDeviceInfo deviceInfo) 
    { 
        if (m_DeviceInfo) delete m_DeviceInfo; 
        m_DeviceInfo = new Pylon::CDeviceInfo(deviceInfo); 
    };

    static v8::Handle<v8::Value> NewInstance(Pylon::CDeviceInfo deviceInfo);

 private:
    static Nan::Persistent<v8::Function> constructor;
    DeviceInfoWrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~DeviceInfoWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetSerialNumber);
    static NAN_METHOD(SetSerialNumber);
    static NAN_METHOD(IsSerialNumberAvailable);
    
    static NAN_METHOD(GetUserDefinedName);
    static NAN_METHOD(SetUserDefinedName);
    static NAN_METHOD(IsUserDefinedNameAvailable);
    
    static NAN_METHOD(GetModelName);
    static NAN_METHOD(SetModelName);
    static NAN_METHOD(IsModelNameAvailable);
    
    static NAN_METHOD(GetDeviceVersion);
    static NAN_METHOD(SetDeviceVersion);
    static NAN_METHOD(IsDeviceVersionAvailable);
    
    static NAN_METHOD(GetDeviceFactory);
    static NAN_METHOD(SetDeviceFactory);
    static NAN_METHOD(IsDeviceFactoryAvailable);

    static NAN_METHOD(GetXMLSource);
    static NAN_METHOD(SetXMLSource);
    static NAN_METHOD(IsXMLSourceAvailable);

    static NAN_METHOD(SetFriendlyName);
    static NAN_METHOD(SetFullName);
    static NAN_METHOD(SetVendorName);
    static NAN_METHOD(SetDeviceClass);

    static NAN_METHOD(SetPropertyValue);

    // Wrapped object
    Pylon::CDeviceInfo* m_DeviceInfo;
};