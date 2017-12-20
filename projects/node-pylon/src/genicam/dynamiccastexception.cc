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

#include "dynamiccastexception.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> DynamicCastExceptionWrap::prototype;
Nan::Persistent<Function> DynamicCastExceptionWrap::constructor;

// Supported implementations
// DynamicCastException(DynamicCastException& const arg0)
// DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine)
// DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine, char* const pExceptionType)
// DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine, char* const pEntryPoint, char* const pErrorNodeName, char* const pExceptionType)
DynamicCastExceptionWrap::DynamicCastExceptionWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_DynamicCastException(NULL)
{
    // Check constructor arguments
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "DynamicCastException")))
    {
        // Unwrap object
        DynamicCastExceptionWrap* arg0_wrap = ObjectWrap::Unwrap<DynamicCastExceptionWrap>(info[0]->ToObject());
        DynamicCastException* arg0 = arg0_wrap->GetWrapped();

        // DynamicCastException(DynamicCastException& const arg0)
        m_DynamicCastException = new DynamicCastException(*arg0);
    }
    else if ((info.Length() == 3) && info[0]->IsString() && info[1]->IsString() && info[2]->IsNumber())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from string value
        char* arg1 = pylon_v8::ToGCString(info[1]->ToString()).c_str();

        // Convert from number value
        int arg2 = static_cast<int>(info[2]->NumberValue());

        // DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine)
        m_DynamicCastException = new DynamicCastException(arg0, arg1, arg2);
    }
    else if ((info.Length() == 4) && info[0]->IsString() && info[1]->IsString() && info[2]->IsNumber() && info[3]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from string value
        char* arg1 = pylon_v8::ToGCString(info[1]->ToString()).c_str();

        // Convert from number value
        int arg2 = static_cast<int>(info[2]->NumberValue());

        // Convert from string value
        char* arg3 = pylon_v8::ToGCString(info[3]->ToString()).c_str();

        // DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine, char* const pExceptionType)
        m_DynamicCastException = new DynamicCastException(arg0, arg1, arg2, arg3);
    }
    else if ((info.Length() == 6) && info[0]->IsString() && info[1]->IsString() && info[2]->IsNumber() && info[3]->IsString() && info[4]->IsString() && info[5]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from string value
        char* arg1 = pylon_v8::ToGCString(info[1]->ToString()).c_str();

        // Convert from number value
        int arg2 = static_cast<int>(info[2]->NumberValue());

        // Convert from string value
        char* arg3 = pylon_v8::ToGCString(info[3]->ToString()).c_str();

        // Convert from string value
        char* arg4 = pylon_v8::ToGCString(info[4]->ToString()).c_str();

        // Convert from string value
        char* arg5 = pylon_v8::ToGCString(info[5]->ToString()).c_str();

        // DynamicCastException(char* const pDescription, char* const pSourceFileName, int SourceLine, char* const pEntryPoint, char* const pErrorNodeName, char* const pExceptionType)
        m_DynamicCastException = new DynamicCastException(arg0, arg1, arg2, arg3, arg4, arg5);
    }
}

DynamicCastExceptionWrap::~DynamicCastExceptionWrap()
{
    delete m_DynamicCastException;
}

NAN_MODULE_INIT(DynamicCastExceptionWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("DynamicCastExceptionWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("DynamicCastException").ToLocalChecked(), function);
}