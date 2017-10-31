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

#include "chunkadapter.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;

Nan::Persistent<FunctionTemplate> ChunkAdapterWrap::prototype;
Nan::Persistent<Function> ChunkAdapterWrap::constructor;

// Supported implementations
// CChunkAdapter(CChunkAdapter& const arg0)
// CChunkAdapter(INodeMap* pNodeMap, __int128_t MaxChunkCacheSize)
ChunkAdapterWrap::ChunkAdapterWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ChunkAdapter(NULL)
{
    // Check constructor arguments
    if (info[0]->IsObject())
    {
        gcstring info0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
        if (info0_constructor != "CChunkAdapter")
        {
            ThrowException(Exception::TypeError(String::New("CChunkAdapter::CChunkAdapter: bad argument")));
        }

        // Unwrap obj
        ChunkAdapterWrap* arg0_wrap = ObjectWrap::Unwrap<ChunkAdapterWrap>(info[0]->ToObject());
        CChunkAdapter* arg0 = arg0_wrap->GetWrapped();

        // CChunkAdapter(CChunkAdapter& const arg0)
        m_ChunkAdapter = new CChunkAdapter(*arg0);
    }
    else if ((info[0]->IsObject()) && (info[1]->IsNumber()))
    {
        gcstring info0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
        if (info0_constructor != "INodeMap")
        {
            ThrowException(Exception::TypeError(String::New("CChunkAdapter::CChunkAdapter: bad argument")));
        }

        // Unwrap obj
        NodeMapWrap* arg0_wrap = ObjectWrap::Unwrap<NodeMapWrap>(info[0]->ToObject());
        INodeMap* arg0 = arg0_wrap->GetWrapped();

        // Convert number value
        __int128_t arg1 = static_cast<__int128_t>(info[1]->NumberValue());

        // CChunkAdapter(INodeMap* pNodeMap, __int128_t MaxChunkCacheSize)
        m_ChunkAdapter = new CChunkAdapter(arg0, arg1);
    }
}

ChunkAdapterWrap::~ChunkAdapterWrap()
{
    delete m_ChunkAdapter;
}

NAN_MODULE_INIT(ChunkAdapterWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ChunkAdapterWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "attachNodeMap", AttachNodeMap);
    Nan::SetPrototypeMethod(tpl, "detachNodeMap", DetachNodeMap);
    Nan::SetPrototypeMethod(tpl, "checkBufferLayout", CheckBufferLayout);
    Nan::SetPrototypeMethod(tpl, "attachBuffer", AttachBuffer);
    Nan::SetPrototypeMethod(tpl, "detachBuffer", DetachBuffer);
    Nan::SetPrototypeMethod(tpl, "updateBuffer", UpdateBuffer);
    Nan::SetPrototypeMethod(tpl, "clearCaches", ClearCaches);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("CChunkAdapter").ToLocalChecked(), function);
}
