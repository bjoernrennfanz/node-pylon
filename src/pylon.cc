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

#include "pylon/DeviceInfo.h"
#include "pylon/PylonDevice.h"
#include "pylon/TlFactory.h"

#include <pylon/PylonBase.h>

using namespace v8;

class PylonWrap
{
public:
	static NAN_METHOD(Initialize)
	{
		Pylon::PylonInitialize();
		info.GetReturnValue().Set(Nan::Undefined());
	}

	static NAN_METHOD(Terminate)
	{
		Pylon::PylonTerminate();
		info.GetReturnValue().Set(Nan::Undefined());
	}

	static NAN_METHOD(GetVersionString)
	{
		info.GetReturnValue().Set(Nan::New<v8::String>(Pylon::GetPylonVersionString()).ToLocalChecked());
	}
};

NAN_MODULE_INIT(InitPylonWrapper)
{
	// Initialize static methods
	Nan::Set(target, Nan::New<String>("initialize").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::Initialize)).ToLocalChecked());
	Nan::Set(target, Nan::New<String>("terminate").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::Terminate)).ToLocalChecked());
	Nan::Set(target, Nan::New<String>("getVersionString").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::GetVersionString)).ToLocalChecked());

	// Initialize dynamic classes
	DeviceInfoWrap::Initialize(target);
	PylonDeviceWrap::Initialize(target);
	TlFactoryWrap::Initialize(target);
}

NODE_MODULE(pylon, InitPylonWrapper)