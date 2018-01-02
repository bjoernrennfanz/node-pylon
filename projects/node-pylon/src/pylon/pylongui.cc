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

#include "pylongui.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

NAN_MODULE_INIT(PylonguiWrap::Initialize)
{
    // Register static functions in Node JS
    Nan::Set(target, Nan::New<String>("displayImage").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonguiWrap::DisplayImage)).ToLocalChecked());
}

NAN_METHOD(PylonguiWrap::DisplayImage)
{
    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        ImageWrap* arg1_wrap = ObjectWrap::Unwrap<ImageWrap>(info[1]->ToObject());
        IImage* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped function
        DisplayImage(arg0, *arg1);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}
