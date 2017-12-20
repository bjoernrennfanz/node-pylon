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

#include "fileprotocoladapter.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> FileProtocolAdapterWrap::prototype;
Nan::Persistent<Function> FileProtocolAdapterWrap::constructor;

// Supported implementations
// FileProtocolAdapter()
// FileProtocolAdapter(FileProtocolAdapter& const arg0)
FileProtocolAdapterWrap::FileProtocolAdapterWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_FileProtocolAdapter(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // FileProtocolAdapter()
        m_FileProtocolAdapter = new FileProtocolAdapter();
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "FileProtocolAdapter")))
    {
        // Unwrap object
        FileProtocolAdapterWrap* arg0_wrap = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info[0]->ToObject());
        FileProtocolAdapter* arg0 = arg0_wrap->GetWrapped();

        // FileProtocolAdapter(FileProtocolAdapter& const arg0)
        m_FileProtocolAdapter = new FileProtocolAdapter(*arg0);
    }
}

FileProtocolAdapterWrap::~FileProtocolAdapterWrap()
{
    delete m_FileProtocolAdapter;
}

NAN_MODULE_INIT(FileProtocolAdapterWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("FileProtocolAdapterWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "attach", attach);
    Nan::SetPrototypeMethod(tpl, "openFile", openFile);
    Nan::SetPrototypeMethod(tpl, "closeFile", closeFile);
    Nan::SetPrototypeMethod(tpl, "write", write);
    Nan::SetPrototypeMethod(tpl, "read", read);
    Nan::SetPrototypeMethod(tpl, "getBufSize", getBufSize);
    Nan::SetPrototypeMethod(tpl, "deleteFile", deleteFile);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("FileProtocolAdapter").ToLocalChecked(), function);
}

NAN_METHOD(FileProtocolAdapterWrap::attach)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "INodeMap")))
    {
        // Unwrap object
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        fileProtocolAdapter->attach(arg0);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::closeFile)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Call wrapped method
        fileProtocolAdapter->closeFile(arg0);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::deleteFile)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Call wrapped method
        fileProtocolAdapter->deleteFile(arg0);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::getBufSize)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsString() && info[1]->IsNumber())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());

        // Call wrapped method
        fileProtocolAdapter->getBufSize(arg0, arg1);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::openFile)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsString() && info[1]->IsNumber())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        int arg1 = static_cast<int>(info[1]->NumberValue());

        // Call wrapped method
        fileProtocolAdapter->openFile(arg0, arg1);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::read)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 4) && info[0]->IsString() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());

        // Convert from string value
        char* arg3 = pylon_v8::ToGCString(info[3]->ToString()).c_str();

        // Call wrapped method
        fileProtocolAdapter->read(arg0, arg1, arg2, arg3);
    }
}

NAN_METHOD(FileProtocolAdapterWrap::write)
{
    FileProtocolAdapterWrap* wrappedFileProtocolAdapter = ObjectWrap::Unwrap<FileProtocolAdapterWrap>(info.This());
    FileProtocolAdapter* fileProtocolAdapter = wrappedFileProtocolAdapter->GetWrapped();

    if ((info.Length() == 4) && info[0]->IsString() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsString())
    {
        // Convert from string value
        char* arg0 = pylon_v8::ToGCString(info[0]->ToString()).c_str();

        // Convert from number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // Convert from number value
        __int128_t arg2 = static_cast<__int128_t>(info[2]->NumberValue());

        // Convert from string value
        char* arg3 = pylon_v8::ToGCString(info[3]->ToString()).c_str();

        // Call wrapped method
        fileProtocolAdapter->write(arg0, arg1, arg2, arg3);
    }
}