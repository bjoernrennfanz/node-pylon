'use strict';
// Image pipeline sample: grab a frame with the emulator, wrap it in the
// GrabResult API, copy it into a PylonImage, convert it and persist it.
// Run with: PYLON_CAMEMU=2 npm run sample:image
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const pylon = require('../lib');

function main() {
  if (pylon.enumerateDevices().length === 0) {
    console.log('no camera/emulator available (set PYLON_CAMEMU=2)');
    return;
  }
  const cam = new pylon.InstantCamera();
  cam.open();
  try {
    cam.startGrabbing();
    try {
      const grab = cam.retrieveGrabResult(10000);
      if (grab === null) {
        console.log('grab timeout');
        return;
      }
      console.log(`grab: ${grab.getWidth()}x${grab.getHeight()} ${grab.getPixelType()} ` +
        `payload=${grab.getPayloadSize()} ts=${grab.getTimestamp()} valid=${grab.isValid()}`);
      console.log(`payload type: ${grab.getPayloadType()}, components: ${grab.getDataComponentCount()}`);

      const img = new pylon.PylonImage();
      img.copyImage(grab);
      console.log(`image: ${img.getWidth()}x${img.getHeight()} ${img.getPixelType()} ` +
        `size=${img.getImageSize()}`);

      const conv = new pylon.ImageFormatConverter();
      conv.setOutputPixelFormat('BGR8');
      const converted = new pylon.PylonImage();
      conv.convert(converted, grab);
      console.log(`converted: ${converted.getPixelType()} size=${converted.getImageSize()}`);

      const file = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'node-pylon-')), 'frame.png');
      converted.save('Png', file);
      console.log(`saved ${file}`);
    } finally {
      cam.stopGrabbing();
    }
  } finally {
    cam.close();
  }
}

main();
