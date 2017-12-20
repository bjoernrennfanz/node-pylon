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

#include "exceptionreporter.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenICam_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> ExceptionReporterWrap::prototype;
Nan::Persistent<Function> ExceptionReporterWrap::constructor;

// Supported implementations
// ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>& const arg0)
// ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(char* const pSourceFileName, int SourceLine)
// ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(char* const pSourceFileName, int SourceLine, char* const pExceptionType)
ExceptionReporterWrap::ExceptionReporterWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ExceptionReporter(NULL)
{
    // Check constructor arguments
    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>")))
    {
        // Unwrap object
        ExceptionReporterWrap* arg0_wrap = ObjectWrap::Unwrap<ExceptionReporterWrap>(info[0]->ToObject());
        ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>* arg0 = arg0_wrap->GetWrapped();

        // ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>& const arg0)
        m_ExceptionReporter = new ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(*arg0);
    }
    else if ((info.Length() == 2) && info[0]->IsString() && info[1]->IsNumber())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());

        // ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(char* const pSourceFileName, int SourceLine)
        m_ExceptionReporter = new ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(arg0, arg1);
    }
    else if ((info.Length() == 3) && info[0]->IsString() && info[1]->IsNumber() && info[2]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());

        // Convert from string value
        char* arg2 = pylon_v8::ToGCString(info[2]->ToString()).c_str();

        // ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(char* const pSourceFileName, int SourceLine, char* const pExceptionType)
        m_ExceptionReporter = new ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>(arg0, arg1, arg2);
    }
}

ExceptionReporterWrap::~ExceptionReporterWrap()
{
    delete m_ExceptionReporter;
}

NAN_MODULE_INIT(ExceptionReporterWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ExceptionReporterWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "report", Report);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>").ToLocalChecked(), function);
}

NAN_METHOD(ExceptionReporterWrap::Report)
{
    ExceptionReporterWrap* wrappedExceptionReporter = ObjectWrap::Unwrap<ExceptionReporterWrap>(info.This());
    ExceptionReporter<GenICam_3_0_Basler_pylon_v5_0::RuntimeException>* exceptionReporter = wrappedExceptionReporter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsObject())
    {
        // TODO: Implement wrapper for basic_stringstream<char, std::char_traits<char>, std::allocator<char> >

        // Call wrapped method
        exceptionReporter->Report(*arg0);
    }
    else if ((info.Length() == 1) && info[0]->IsObject())
    {
        // TODO: Implement wrapper for basic_string<char, std::char_traits<char>, std::allocator<char> >

        // Call wrapped method
        exceptionReporter->Report(*arg0);
    }
    else if ((info.Length() == 1) && info[0]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Call wrapped method
        exceptionReporter->Report(arg0);
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        exceptionReporter->Report();
    }
}
