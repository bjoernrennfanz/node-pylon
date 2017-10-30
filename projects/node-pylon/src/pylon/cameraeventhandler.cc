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

#include "cameraeventhandler.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> CameraEventHandlerWrap::prototype;
Nan::Persistent<Function> CameraEventHandlerWrap::constructor;

// Supported implementations
// CCameraEventHandler()
// CCameraEventHandler(CCameraEventHandler& const arg0)
CameraEventHandlerWrap::CameraEventHandlerWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_CameraEventHandler(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // CCameraEventHandler()
        m_CameraEventHandler = new CCameraEventHandler();
    }
    else if (info[0]->IsObject())
    {
        gcstring info0_constructor = pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName());
        if (info0_constructor != "CCameraEventHandler")
        {
            ThrowException(Exception::TypeError(String::New("CCameraEventHandler::CCameraEventHandler: bad argument")));
        }

        // Unwrap obj
        CameraEventHandlerWrap* arg0_wrap = ObjectWrap::Unwrap<CameraEventHandlerWrap>(info[0]->ToObject());
        CCameraEventHandler* arg0 = arg0_wrap->GetWrapped();

        // CCameraEventHandler(CCameraEventHandler& const arg0)
        m_CameraEventHandler = new CCameraEventHandler(*arg0);
    }
}
