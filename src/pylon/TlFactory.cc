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

#include "TlFactory.h"
#include "PylonDevice.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> TlFactoryWrap::prototype;
Nan::Persistent<Function> TlFactoryWrap::constructor;

TlFactoryWrap::TlFactoryWrap(Nan::NAN_METHOD_ARGS_TYPE info)
	: m_TlFactory(NULL)
{
    // Standalone constructor not implemented
    // internal use SetWrapped()
}

TlFactoryWrap::~TlFactoryWrap()
{
}

NAN_MODULE_INIT(TlFactoryWrap::Initialize)
{
    // Prepare constructor template
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("TlFactory").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);  

    // Prototype
    Nan::SetPrototypeMethod(tpl, "getInstance", GetInstance);
	Nan::SetPrototypeMethod(tpl, "createFirstDevice", CreateFirstDevice);

    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("TlFactory").ToLocalChecked(), function);
}

NAN_METHOD(TlFactoryWrap::New) 
{
    TlFactoryWrap* wrappedTlFactory = new TlFactoryWrap(info);
    wrappedTlFactory->Wrap(info.This());
}

Handle<Value> TlFactoryWrap::NewInstance(CTlFactory& tlFactory)
{
	Nan::EscapableHandleScope scope;

	Local<Object> instance = Nan::NewInstance(Nan::New(constructor), 0, NULL).ToLocalChecked();
	TlFactoryWrap* wrappedTlFactory = node::ObjectWrap::Unwrap<TlFactoryWrap>(instance);
	wrappedTlFactory->SetWrapped(&tlFactory);

	return scope.Escape(instance);
}

NAN_METHOD(TlFactoryWrap::GetInstance)
{
	// Get singleton instance of TlFactory and warp it.
	info.GetReturnValue().Set(TlFactoryWrap::NewInstance(CTlFactory::GetInstance()));
}

NAN_METHOD(TlFactoryWrap::CreateFirstDevice)
{
	TlFactoryWrap* wrappedTlFactory = ObjectWrap::Unwrap<TlFactoryWrap>(info.This());
	CTlFactory* tlFactory = wrappedTlFactory->GetWrapped();

	// Check if instance is valid
	if (tlFactory == nullptr)
	{
		return Nan::ThrowError(Exception::TypeError(Nan::New("TlFactoryWrap::CreateFirstDevice: instance not initialized.").ToLocalChecked()));
	}
	
	// Implement currently only default
	info.GetReturnValue().Set(PylonDeviceWrap::NewInstance(tlFactory->CreateFirstDevice()));
}