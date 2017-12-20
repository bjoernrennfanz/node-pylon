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

#include "pylonimagewindow.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> PylonImageWindowWrap::prototype;
Nan::Persistent<Function> PylonImageWindowWrap::constructor;

// Supported implementations
// CPylonImageWindow()
// CPylonImageWindow(CPylonImageWindow& const arg0)
PylonImageWindowWrap::PylonImageWindowWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_PylonImageWindow(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CPylonImageWindow()
        m_PylonImageWindow = new CPylonImageWindow();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "CPylonImageWindow")))
    {
        // Unwrap object
        PylonImageWindowWrap* arg0_wrap = ObjectWrap::Unwrap<PylonImageWindowWrap>(info[0]->ToObject());
        CPylonImageWindow* arg0 = arg0_wrap->GetWrapped();

        // CPylonImageWindow(CPylonImageWindow& const arg0)
        m_PylonImageWindow = new CPylonImageWindow(*arg0);
    }
}

PylonImageWindowWrap::~PylonImageWindowWrap()
{
    delete m_PylonImageWindow;
}

NAN_MODULE_INIT(PylonImageWindowWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("PylonImageWindowWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "getWindowHandle", GetWindowHandle);
    Nan::SetPrototypeMethod(tpl, "getWindowIndex", GetWindowIndex);
    Nan::SetPrototypeMethod(tpl, "isValid", IsValid);
    Nan::SetPrototypeMethod(tpl, "attach", Attach);
    Nan::SetPrototypeMethod(tpl, "detach", Detach);
    Nan::SetPrototypeMethod(tpl, "create", Create);
    Nan::SetPrototypeMethod(tpl, "show", Show);
    Nan::SetPrototypeMethod(tpl, "hide", Hide);
    Nan::SetPrototypeMethod(tpl, "close", Close);
    Nan::SetPrototypeMethod(tpl, "setImage", SetImage);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CPylonImageWindow").ToLocalChecked(), function);

    // Register static functions in Node JS
    Nan::Set(target, Nan::New<String>("displayImage").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonImageWindowWrap::DisplayImage)).ToLocalChecked());
}

NAN_METHOD(PylonImageWindowWrap::Attach)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        pylonImageWindow->Attach(arg0);
    }
}

NAN_METHOD(PylonImageWindowWrap::Close)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->Close();
    }
}

NAN_METHOD(PylonImageWindowWrap::Create)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if ((info.Length() == 5) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber() && info[4]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());

        // Convert from number value
        int arg2 = static_cast<int>(info[2]->NumberValue());

        // Convert from number value
        int arg3 = static_cast<int>(info[3]->NumberValue());

        // Convert from number value
        int arg4 = static_cast<int>(info[4]->NumberValue());

        // Call wrapped method
        pylonImageWindow->Create(arg0, arg1, arg2, arg3, arg4);
    }
}

NAN_METHOD(PylonImageWindowWrap::Detach)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->Detach();
    }
}

NAN_METHOD(PylonImageWindowWrap::GetWindowHandle)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->GetWindowHandle();
    }
}

NAN_METHOD(PylonImageWindowWrap::GetWindowIndex)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->GetWindowIndex();
    }
}

NAN_METHOD(PylonImageWindowWrap::Hide)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->Hide();
    }
}

NAN_METHOD(PylonImageWindowWrap::IsValid)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        pylonImageWindow->IsValid();
    }
}

NAN_METHOD(PylonImageWindowWrap::SetImage)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IImage")))
    {
        // Unwrap object
        ImageWrap* arg0_wrap = ObjectWrap::Unwrap<ImageWrap>(info[0]->ToObject());
        IImage* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        pylonImageWindow->SetImage(*arg0);
    }
}

NAN_METHOD(PylonImageWindowWrap::Show)
{
    PylonImageWindowWrap* wrappedPylonImageWindow = ObjectWrap::Unwrap<PylonImageWindowWrap>(info.This());
    CPylonImageWindow* pylonImageWindow = wrappedPylonImageWindow->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        int arg0 = static_cast<int>(info[0]->NumberValue());

        // Call wrapped method
        pylonImageWindow->Show(arg0);
    }
}

NAN_METHOD(PylonImageWindowWrap::DisplayImage)
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
    }
}
