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

const util = require('util');
const pylonNode = require('..');

// Get GeniCam and pylon classes
const genicam = pylonNode.genicam;
const pylon = pylonNode.pylon;

// Number of images to be grabbed.
const countOfImagesToGrab = 100

// Initialize pylon
pylon.initialize();
    
// Print pylon Version
console.log("Using pylon Version " + pylon.getVersionString());
    
// Get TlFactory singleton instance
const tlFactoryInstance = new pylon.TlFactory().getInstance();

// Create an instant camera object with thecamera device found first.
var camera = new pylon.InstantCamera(tlFactoryInstance.createFirstDevice())

// Print the model name of the camera.
console.log("Using device ", camera.getDeviceInfo().getModelName())
