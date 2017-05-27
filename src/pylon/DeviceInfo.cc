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

#include "DeviceInfo.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> DeviceInfoWrap::prototype;
Nan::Persistent<Function> DeviceInfoWrap::constructor;

// Supported implementations:
//   DeviceInfo()
//   DeviceInfo(DeviceInfo)
DeviceInfoWrap::DeviceInfoWrap(Nan::NAN_METHOD_ARGS_TYPE info) 
    : m_DeviceInfo(NULL)
{
	GENICAM_NAMESPACE::gcstring arg0_constructor;
    if (info[0]->IsObject()) 
    {
		// DeviceInfo(DeviceInfo)
        arg0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
		if (arg0_constructor != "DeviceInfo")
		{
			Nan::ThrowError(Exception::TypeError(Nan::New("DeviceInfoWrap::DeviceInfoWrap: bad argument").ToLocalChecked()));
		}

		// Unwrap obj
		DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info[0]->ToObject());
		CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

		m_DeviceInfo = new CDeviceInfo(*deviceInfo);
    }
	else
    {
        // DeviceInfo()
        m_DeviceInfo = new CDeviceInfo();
        return;
    }
}

DeviceInfoWrap::~DeviceInfoWrap() 
{
	delete m_DeviceInfo;
}

NAN_MODULE_INIT(DeviceInfoWrap::Initialize) {
	// Prepare constructor template
	Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("DeviceInfo").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "getSerialNumber", GetSerialNumber);
	Nan::SetPrototypeMethod(tpl, "setSerialNumber", SetSerialNumber);
	Nan::SetPrototypeMethod(tpl, "isSerialNumberAvailable", IsSerialNumberAvailable);

	Nan::SetPrototypeMethod(tpl, "getUserDefinedName", GetUserDefinedName);
	Nan::SetPrototypeMethod(tpl, "setUserDefinedName", SetUserDefinedName);
	Nan::SetPrototypeMethod(tpl, "isUserDefinedNameAvailable", IsUserDefinedNameAvailable);

	Nan::SetPrototypeMethod(tpl, "getModelName", GetModelName);
	Nan::SetPrototypeMethod(tpl, "setModelName", SetModelName);
	Nan::SetPrototypeMethod(tpl, "isModelNameAvailable", IsModelNameAvailable);

	Nan::SetPrototypeMethod(tpl, "getDeviceVersion", GetDeviceVersion);
	Nan::SetPrototypeMethod(tpl, "setDeviceVersion", SetDeviceVersion);
	Nan::SetPrototypeMethod(tpl, "isDeviceVersionAvailable", IsDeviceVersionAvailable);

	Nan::SetPrototypeMethod(tpl, "getDeviceFactory", GetDeviceFactory);
	Nan::SetPrototypeMethod(tpl, "setDeviceFactory", SetDeviceFactory);
	Nan::SetPrototypeMethod(tpl, "isDeviceFactoryAvailable", IsDeviceFactoryAvailable);

	Nan::SetPrototypeMethod(tpl, "getXMLSource", GetXMLSource);
	Nan::SetPrototypeMethod(tpl, "setXMLSource", SetXMLSource);
	Nan::SetPrototypeMethod(tpl, "isXMLSourceAvailable", IsXMLSourceAvailable);

	Nan::SetPrototypeMethod(tpl, "setFriendlyName", SetFriendlyName);
	Nan::SetPrototypeMethod(tpl, "setFullName", SetFullName);
	Nan::SetPrototypeMethod(tpl, "setVendorName", SetVendorName);
	Nan::SetPrototypeMethod(tpl, "setDeviceClass", SetDeviceClass);

	Nan::SetPrototypeMethod(tpl, "setPropertyValue", SetPropertyValue);

	prototype.Reset(tpl);
	Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
	constructor.Reset(function);
	Nan::Set(target, Nan::New("DeviceInfo").ToLocalChecked(), function);
}

NAN_METHOD(DeviceInfoWrap::New) 
{
	DeviceInfoWrap* deviceInfoWrap = new DeviceInfoWrap(info);
	deviceInfoWrap->Wrap(info.This());
}

Handle<Value> DeviceInfoWrap::NewInstance(CDeviceInfo deviceInfo) 
{
	Nan::EscapableHandleScope scope;

	Local<Object> instance = Nan::NewInstance(Nan::New(constructor), 0, NULL).ToLocalChecked();
	DeviceInfoWrap* deviceInfoWrap = node::ObjectWrap::Unwrap<DeviceInfoWrap>(instance);
	deviceInfoWrap->SetWrapped(deviceInfo);

	return scope.Escape(instance);
}

NAN_METHOD(DeviceInfoWrap::GetSerialNumber)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetSerialNumber()));
}

NAN_METHOD(DeviceInfoWrap::SetSerialNumber)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetSerialNumber(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsSerialNumberAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsSerialNumberAvailable()));
}

NAN_METHOD(DeviceInfoWrap::GetUserDefinedName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetUserDefinedName()));
}

NAN_METHOD(DeviceInfoWrap::SetUserDefinedName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetUserDefinedName(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsUserDefinedNameAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsUserDefinedNameAvailable()));
}

NAN_METHOD(DeviceInfoWrap::GetModelName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetModelName()));
}

NAN_METHOD(DeviceInfoWrap::SetModelName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetModelName(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsModelNameAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsModelNameAvailable()));
}

NAN_METHOD(DeviceInfoWrap::GetDeviceVersion)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetDeviceVersion()));
}

NAN_METHOD(DeviceInfoWrap::SetDeviceVersion)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetDeviceVersion(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsDeviceVersionAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsDeviceVersionAvailable()));
}

NAN_METHOD(DeviceInfoWrap::GetDeviceFactory)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetDeviceFactory()));
}

NAN_METHOD(DeviceInfoWrap::SetDeviceFactory)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetDeviceFactory(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsDeviceFactoryAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsDeviceFactoryAvailable()));
}

NAN_METHOD(DeviceInfoWrap::GetXMLSource)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(pylon_v8::FromGCString(deviceInfo->GetXMLSource()));
}

NAN_METHOD(DeviceInfoWrap::SetXMLSource)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetXMLSource(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::IsXMLSourceAvailable)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	info.GetReturnValue().Set(Nan::New(deviceInfo->IsXMLSourceAvailable()));
}

NAN_METHOD(DeviceInfoWrap::SetFriendlyName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetFriendlyName(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::SetFullName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetFullName(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::SetVendorName)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetVendorName(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::SetDeviceClass)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetDeviceClass(pylon_v8::ToGCString(info[0]->ToString()));

	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DeviceInfoWrap::SetPropertyValue)
{
	DeviceInfoWrap* deviceInfoWrap = ObjectWrap::Unwrap<DeviceInfoWrap>(info.This());
	CDeviceInfo* deviceInfo = deviceInfoWrap->GetWrapped();

	deviceInfo->SetPropertyValue(pylon_v8::ToGCString(info[0]->ToString()), pylon_v8::ToGCString(info[1]->ToString()));

	info.GetReturnValue().Set(info.This());
}