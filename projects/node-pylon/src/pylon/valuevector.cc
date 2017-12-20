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

#include "valuevector.h"
#include "../pylon_v8.h"

using namespace v8;
using namespace GenApi_3_0_Basler_pylon_v5_0;
using namespace Pylon;

Nan::Persistent<FunctionTemplate> ValueVectorWrap::prototype;
Nan::Persistent<Function> ValueVectorWrap::constructor;

// Supported implementations
// value_vector()
// value_vector(unsigned int uiSize)
// value_vector(value_vector& const obj)
ValueVectorWrap::ValueVectorWrap(Nan::NAN_METHOD_ARGS_TYPE info)
  : m_ValueVector(NULL)
{
    // Check constructor arguments
    if (info.Length() == 0)
    {
        // value_vector()
        m_ValueVector = new value_vector();
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // value_vector(unsigned int uiSize)
        m_ValueVector = new value_vector(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "value_vector")))
    {
        // Unwrap object
        ValueVectorWrap* arg0_wrap = ObjectWrap::Unwrap<ValueVectorWrap>(info[0]->ToObject());
        value_vector* arg0 = arg0_wrap->GetWrapped();

        // value_vector(value_vector& const obj)
        m_ValueVector = new value_vector(*arg0);
    }
}

ValueVectorWrap::~ValueVectorWrap()
{
    delete m_ValueVector;
}

NAN_MODULE_INIT(ValueVectorWrap::Initialize)
{
    // Prepare constructor template
    Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ValueVectorWrap").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Register prototypes to template
    Nan::SetPrototypeMethod(tpl, "assign", assign);
    Nan::SetPrototypeMethod(tpl, "clear", clear);
    Nan::SetPrototypeMethod(tpl, "erase", erase);
    Nan::SetPrototypeMethod(tpl, "at", at);
    Nan::SetPrototypeMethod(tpl, "back", back);
    Nan::SetPrototypeMethod(tpl, "begin", begin);
    Nan::SetPrototypeMethod(tpl, "capacity", capacity);
    Nan::SetPrototypeMethod(tpl, "end", end);
    Nan::SetPrototypeMethod(tpl, "front", front);
    Nan::SetPrototypeMethod(tpl, "max_size", max_size);
    Nan::SetPrototypeMethod(tpl, "size", size);
    Nan::SetPrototypeMethod(tpl, "insert", insert);
    Nan::SetPrototypeMethod(tpl, "empty", empty);
    Nan::SetPrototypeMethod(tpl, "pop_back", pop_back);
    Nan::SetPrototypeMethod(tpl, "push_back", push_back);
    Nan::SetPrototypeMethod(tpl, "resize", resize);
    Nan::SetPrototypeMethod(tpl, "reserve", reserve);

    // Register template in Node JS
    prototype.Reset(tpl);
    Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();
    constructor.Reset(function);
    Nan::Set(target, Nan::New("value_vector").ToLocalChecked(), function);
}

NAN_METHOD(ValueVectorWrap::assign)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IValue")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        ValueWrap* arg1_wrap = ObjectWrap::Unwrap<ValueWrap>(info[1]->ToObject());
        IValue* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        valueVector->assign(arg0, arg1);
    }
}

NAN_METHOD(ValueVectorWrap::at)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        valueVector->at(arg0);
    }
    else if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        valueVector->at(arg0);
    }
}

NAN_METHOD(ValueVectorWrap::back)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->back();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->back();
    }
}

NAN_METHOD(ValueVectorWrap::begin)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->begin();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->begin();
    }
}

NAN_METHOD(ValueVectorWrap::capacity)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->capacity();
    }
}

NAN_METHOD(ValueVectorWrap::clear)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->clear();
    }
}

NAN_METHOD(ValueVectorWrap::empty)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->empty();
    }
}

NAN_METHOD(ValueVectorWrap::end)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->end();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->end();
    }
}

NAN_METHOD(ValueVectorWrap::erase)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        valueVector->erase(arg0);
    }
    else if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "iterator")))
    {
        // Unwrap object
        IteratorWrap* arg0_wrap = ObjectWrap::Unwrap<IteratorWrap>(info[0]->ToObject());
        iterator* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        valueVector->erase(arg0);
    }
}

NAN_METHOD(ValueVectorWrap::front)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->front();
    }
    else if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->front();
    }
}

NAN_METHOD(ValueVectorWrap::insert)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IValue")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        ValueWrap* arg1_wrap = ObjectWrap::Unwrap<ValueWrap>(info[1]->ToObject());
        IValue* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        valueVector->insert(arg0, arg1);
    }
    else if ((info.Length() == 2) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "iterator")) && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IValue")))
    {
        // Unwrap object
        IteratorWrap* arg0_wrap = ObjectWrap::Unwrap<IteratorWrap>(info[0]->ToObject());
        iterator* arg0 = arg0_wrap->GetWrapped();

        // Unwrap object
        ValueWrap* arg1_wrap = ObjectWrap::Unwrap<ValueWrap>(info[1]->ToObject());
        IValue* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        valueVector->insert(arg0, arg1);
    }
}

NAN_METHOD(ValueVectorWrap::max_size)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->max_size();
    }
}

NAN_METHOD(ValueVectorWrap::pop_back)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->pop_back();
    }
}

NAN_METHOD(ValueVectorWrap::push_back)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 1) && (info[0]->IsObject() && (pylon_v8::ToGCString(info[0]->ToObject()->GetConstructorName()) == "IValue")))
    {
        // Unwrap object
        ValueWrap* arg0_wrap = ObjectWrap::Unwrap<ValueWrap>(info[0]->ToObject());
        IValue* arg0 = arg0_wrap->GetWrapped();

        // Call wrapped method
        valueVector->push_back(arg0);
    }
}

NAN_METHOD(ValueVectorWrap::reserve)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 1) && info[0]->IsNumber())
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Call wrapped method
        valueVector->reserve(arg0);
    }
}

NAN_METHOD(ValueVectorWrap::resize)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if ((info.Length() == 2) && info[0]->IsNumber() && (info[1]->IsObject() && (pylon_v8::ToGCString(info[1]->ToObject()->GetConstructorName()) == "IValue")))
    {
        // Convert from number value
        unsigned int arg0 = static_cast<unsigned int>(info[0]->NumberValue());

        // Unwrap object
        ValueWrap* arg1_wrap = ObjectWrap::Unwrap<ValueWrap>(info[1]->ToObject());
        IValue* arg1 = arg1_wrap->GetWrapped();

        // Call wrapped method
        valueVector->resize(arg0, arg1);
    }
}

NAN_METHOD(ValueVectorWrap::size)
{
    ValueVectorWrap* wrappedValueVector = ObjectWrap::Unwrap<ValueVectorWrap>(info.This());
    value_vector* valueVector = wrappedValueVector->GetWrapped();

    if (info.Length() == 0)
    {
        // Call wrapped method
        valueVector->size();
    }
}
