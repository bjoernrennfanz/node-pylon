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

#include <pylon/TlFactory.h>

class TlFactoryWrap : public node::ObjectWrap
{
public:
    static Nan::Persistent<v8::FunctionTemplate> prototype;
    static NAN_MODULE_INIT(Initialize);

    Pylon::CTlFactory* GetWrapped() const 
    { 
		return m_TlFactory;
    };

	void SetWrapped(Pylon::CTlFactory& tlFactory)
	{
		m_TlFactory = &tlFactory;
	};

private:
    static Nan::Persistent<v8::Function> constructor;
	TlFactoryWrap(Nan::NAN_METHOD_ARGS_TYPE info);
	~TlFactoryWrap();
    static NAN_METHOD(New);

    // Wrapped methods
    static NAN_METHOD(GetInstance);

	// Wrapped object
	Pylon::CTlFactory* m_TlFactory;
};