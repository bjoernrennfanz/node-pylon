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

#include "__Template__.h"

using namespace v8;

Nan::Persistent<FunctionTemplate> __Template__Wrap::prototype;
Nan::Persistent<Function> __Template__Wrap::constructor;

// Supported implementations:
//   __Template__()
__Template__Wrap::__Template__Wrap(Nan::NAN_METHOD_ARGS_TYPE args)
  : m___Template__(NULL)
{
    m___Template__ = new __Template__();
}

__Template__Wrap::~__Template__Wrap() 
{
    delete m___Template__;
}

NAN_MODULE_INIT(__Template__Wrap::Initialize)
{
    // Prepare constructor template
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("__Template__").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);  

    // Prototype
    Nan::SetPrototypeMethod(tpl, "example", Example);

    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("__Template__").ToLocalChecked(), function);
}

NAN_METHOD(__Template__Wrap::New) 
{
    __Template__Wrap* wrapped__Template__ = new __Template__Wrap(info);
    wrapped__Template__->Wrap(info.This());
}

Handle<Value> __Template__Wrap::NewInstance(C__Template__ __template__)
{
    Nan::EscapableHandleScope scope;
  
    Local<Object> instance = Nan::NewInstance(Nan::New(constructor), 0, NULL).ToLocalChecked();
    __Template__Wrap* wrapped__Template__ = node::ObjectWrap::Unwrap<__Template__Wrap>(instance);
    wrapped__Template__->SetWrapped(__template__);

    return scope.Escape(instance);
}

NAN_METHOD(__Template__Wrap::Example) 
{
    __Template__Wrap* wrapped__Template__ = ObjectWrap::Unwrap<__Template__Wrap>(info.This());
    __Template__* __template__ = wrapped__Template__->GetWrapped();

    // __template__->...?

    info.GetReturnValue().Set(Nan::Undefined());
}