// MIT License
// 
// Copyright (c) 2017 - 2018 Björn Rennfanz <bjoern@fam-rennfanz.de>
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

#include "pylon.h"
#include "pylon_v8.h"

#include "pylon/imageformatconverterparams.h"
#include "pylon/instantcameraparams.h"
#include "pylon/acquirecontinuousconfiguration.h"
#include "pylon/acquiresingleframeconfiguration.h"
#include "pylon/cameraeventhandler.h"
#include "pylon/chunkparser.h"
#include "pylon/configurationeventhandler.h"
#include "pylon/container.h"
#include "pylon/pylondevice.h"
#include "pylon/deviceaccessmode.h"
#include "pylon/devicefactory.h"
#include "pylon/deviceinfo.h"
#include "pylon/featurepersistence.h"
#include "pylon/grabresultdata.h"
#include "pylon/grabresultptr.h"
#include "pylon/image.h"
#include "pylon/imageeventhandler.h"
#include "pylon/imageformatconverter.h"
#include "pylon/info.h"
#include "pylon/instantcamera.h"
#include "pylon/instantcameraarray.h"
#include "pylon/pixeltype.h"
#include "pylon/pixelformatconverter.h"
#include "pylon/pylongui.h"
#include "pylon/pylonimage.h"
#include "pylon/pylonimagebase.h"
#include "pylon/result.h"
#include "pylon/reusableimage.h"
#include "pylon/softwaretriggerconfiguration.h"
#include "pylon/streamgrabber.h"
#include "pylon/tlfactory.h"
#include "pylon/transportlayer.h"
#include "pylon/accessmodeset.h"
#include "pylon/bitset.h"
#include "pylon/buffer.h"
#include "pylon/imageformatconverterparamsparams.h"
#include "pylon/infobase.h"
#include "pylon/instantcameraparamsparams.h"
#include "pylon/constiterator.h"
#include "pylon/pylonimagewindow.h"
#include "pylon/deviceinfolist.h"
#include "pylon/eventresult.h"
#include "pylon/interfaceinfolist.h"
#include "pylon/outputpixelformatenum.h"
#include "pylon/properties.h"
#include "pylon/selfreliantchunkparser.h"
#include "pylon/iterator.h"
#include "pylon/nodevector.h"
#include "pylon/pylonautoinitterm.h"
#include "pylon/tlinfolist.h"
#include "pylon/tlist.h"
#include "pylon/valuevector.h"

using namespace v8;
using namespace Pylon;

NAN_MODULE_INIT(PylonWrap::Initialize)
{
    // Register static functions in Node JS
    Nan::Set(target, Nan::New<String>("pylonInitialize").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::PylonInitialize)).ToLocalChecked());
    Nan::Set(target, Nan::New<String>("pylonTerminate").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::PylonTerminate)).ToLocalChecked());
    Nan::Set(target, Nan::New<String>("getPylonVersion").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::GetPylonVersion)).ToLocalChecked());
    Nan::Set(target, Nan::New<String>("getPylonVersionString").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(PylonWrap::GetPylonVersionString)).ToLocalChecked());

    // Initialize dynamic classes
    ImageformatconverterparamsWrap::Initialize(target);
    InstantcameraparamsWrap::Initialize(target);
    AcquireContinuousConfigurationWrap::Initialize(target);
    AcquireSingleFrameConfigurationWrap::Initialize(target);
    CameraEventHandlerWrap::Initialize(target);
    ChunkParserWrap::Initialize(target);
    ConfigurationEventHandlerWrap::Initialize(target);
    ContainerWrap::Initialize(target);
    PylonDeviceWrap::Initialize(target);
    DeviceaccessmodeWrap::Initialize(target);
    DeviceFactoryWrap::Initialize(target);
    DeviceInfoWrap::Initialize(target);
    FeaturePersistenceWrap::Initialize(target);
    GrabResultDataWrap::Initialize(target);
    GrabResultPtrWrap::Initialize(target);
    ImageWrap::Initialize(target);
    ImageEventHandlerWrap::Initialize(target);
    ImageFormatConverterWrap::Initialize(target);
    InfoWrap::Initialize(target);
    InstantCameraWrap::Initialize(target);
    InstantCameraArrayWrap::Initialize(target);
    PixeltypeWrap::Initialize(target);
    PixelFormatConverterWrap::Initialize(target);
    PylonguiWrap::Initialize(target);
    PylonImageWrap::Initialize(target);
    PylonImageBaseWrap::Initialize(target);
    ResultWrap::Initialize(target);
    ReusableImageWrap::Initialize(target);
    SoftwareTriggerConfigurationWrap::Initialize(target);
    StreamGrabberWrap::Initialize(target);
    TlFactoryWrap::Initialize(target);
    TransportLayerWrap::Initialize(target);
    AccessModeSetWrap::Initialize(target);
    BitsetWrap::Initialize(target);
    BufferWrap::Initialize(target);
    ImageFormatConverterParamsParamsWrap::Initialize(target);
    InfoBaseWrap::Initialize(target);
    InstantCameraParamsParamsWrap::Initialize(target);
    ConstIteratorWrap::Initialize(target);
    PylonImageWindowWrap::Initialize(target);
    DeviceInfoListWrap::Initialize(target);
    EventResultWrap::Initialize(target);
    InterfaceInfoListWrap::Initialize(target);
    OutputPixelFormatEnumWrap::Initialize(target);
    PropertiesWrap::Initialize(target);
    SelfReliantChunkParserWrap::Initialize(target);
    IteratorWrap::Initialize(target);
    NodeVectorWrap::Initialize(target);
    PylonAutoInitTermWrap::Initialize(target);
    TlInfoListWrap::Initialize(target);
    TListWrap::Initialize(target);
    ValueVectorWrap::Initialize(target);
}

NAN_METHOD(PylonWrap::GetPylonVersion)
{
    if ((info.Length() == 4) && info[0]->IsNumber() && info[1]->IsNumber() && info[2]->IsNumber() && info[3]->IsNumber())
    {
        // Convert from number value to pointer
        unsigned int arg0_value = static_cast<unsigned int>(info[0]->NumberValue());
        unsigned int* arg0 = &arg0_value;

        // Convert from number value to pointer
        unsigned int arg1_value = static_cast<unsigned int>(info[1]->NumberValue());
        unsigned int* arg1 = &arg1_value;

        // Convert from number value to pointer
        unsigned int arg2_value = static_cast<unsigned int>(info[2]->NumberValue());
        unsigned int* arg2 = &arg2_value;

        // Convert from number value to pointer
        unsigned int arg3_value = static_cast<unsigned int>(info[3]->NumberValue());
        unsigned int* arg3 = &arg3_value;

        // Call wrapped function
        GetPylonVersion(arg0, arg1, arg2, arg3);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(PylonWrap::GetPylonVersionString)
{
    if (info.Length() == 0)
    {
        // Call wrapped function
        const char* result = GetPylonVersionString();

        // Set return value
        info.GetReturnValue().Set(pylon_v8::FromGCString(result).ToLocalChecked());
    }
}

NAN_METHOD(PylonWrap::PylonInitialize)
{
    if (info.Length() == 0)
    {
        // Call wrapped function
        PylonInitialize();

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NAN_METHOD(PylonWrap::PylonTerminate)
{
    if ((info.Length() == 1) && info[0]->IsBoolean())
    {
        // Convert from boolean value
        bool arg0 = info[0]->BooleanValue();

        // Call wrapped function
        PylonTerminate(arg0);

        // Set return value to undefined
        info.GetReturnValue().SetUndefined();
    }
}

NODE_MODULE(pylon, PylonWrap::Initialize)
