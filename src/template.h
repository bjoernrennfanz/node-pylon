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

#include <pylon/__Template__.h>

class __Template__Wrap : public node::ObjectWrap 
{
public:
    static Nan::Persistent<v8::FunctionTemplate> prototype;
    static NAN_MODULE_INIT(Initialize);

    Pylon::C__Template__* GetWrapped() const 
    { 
        return m___Template__; 
    };

    void SetWrapped(Pylon::C__Template__ __template__) 
    { 
        if (m___Template__) delete m___Template__; 
        m___Template__ = new Pylon::C__Template__(__template__); 
    };

    static v8::Handle<v8::Value> NewInstance(Pylon::C__Template__ __template__);

private:
    static Nan::Persistent<v8::Function> constructor;
    __Template__Wrap(Nan::NAN_METHOD_ARGS_TYPE info);
    ~__Template__Wrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(Example);

    // Wrapped object
    __Template__* m___Template__;
};