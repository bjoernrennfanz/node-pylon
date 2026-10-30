'use strict';
// pylon: image pipeline 1:1 logic ports (pypylon image, pylonimage,
// imageformatconverter, imagepersistence, imagedecompressor, pixeltype,
// pixeltypemapper, payloadtype, sfnc surface). Runs against the SDK
// emulator; device-independent enum checks always run.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const pylon = require('../../lib');
const { requireDevice, pickInt } = require('../helpers');

function openCamera() {
  const cam = new pylon.InstantCamera();
  cam.open();
  return cam;
}

describe('pixel types', () => {
  it('exposes stable enum values and names', () => {
    assert.ok(pylon.PixelType.Mono8 !== undefined);
    assert.ok(pylon.PixelType.RGB8 !== undefined);
    assert.equal(pylon.PixelType.Undefined, -1);
    assert.equal(pylon.getPixelTypeByName('Mono8'), pylon.PixelType.Mono8);
    assert.equal(pylon.getNameByPixelType(pylon.PixelType.Mono8), 'Mono8');
    assert.equal(pylon.getPixelTypeByName('RGB8'), pylon.PixelType.RGB8);
  });

  it('maps mono/color/packed correctly', () => {
    assert.equal(pylon.isMono('Mono8'), true);
    assert.equal(pylon.isMono('RGB8'), false);
    assert.equal(pylon.isColor('RGB8'), true);
    assert.equal(pylon.isColor('Mono8'), false);
    assert.equal(pylon.isPacked(pylon.PixelType.Mono12packed), true);
    assert.equal(pylon.isPacked('Mono8'), false);
    assert.throws(() => pylon.getPixelTypeByName('NoSuchFormat'));
  });

  it('exposes payload types', () => {
    assert.equal(typeof pylon.PayloadType.Image, 'number');
    assert.ok(pylon.PayloadType.Image !== pylon.PayloadType.GenDC);
  });
});

describe('pylon image', () => {
  it('creates valid images with geometry', () => {
    const img = pylon.PylonImage.create('Mono8', 64, 48);
    try {
      assert.equal(img.isValid(), true);
      assert.equal(img.isUnique(), true);
      assert.equal(img.getWidth(), 64);
      assert.equal(img.getHeight(), 48);
      assert.equal(img.getPixelType(), 'Mono8');
      assert.equal(img.getImageSize(), 64 * 48);
      assert.equal(img.getAllocatedBufferSize() >= 64 * 48, true);
      const buf = img.getBuffer();
      assert.ok(Buffer.isBuffer(buf));
      assert.equal(buf.length, img.getImageSize());
      const arr = img.getArray();
      assert.ok(arr instanceof Uint8Array);
      assert.equal(arr.byteLength, buf.length);
      const view = img.getMemoryView();
      assert.ok(view instanceof DataView);
      img.release();
      assert.equal(img.isValid(), false);
    } finally {
      img.release();
    }
  });

  it('copies grab results without camera round-trips', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const img = new pylon.PylonImage();
        try {
          img.copyImage(grab);
          assert.equal(img.isValid(), true);
          assert.equal(img.getWidth(), grab.getWidth());
          assert.equal(img.getHeight(), grab.getHeight());
          assert.equal(img.getPixelType(), grab.getPixelType());
          assert.equal(img.getBuffer().length, grab.getImageSize());
          // PylonImage-to-PylonImage copy preserves content.
          const dup = new pylon.PylonImage();
          try {
            dup.copyImage(img);
            assert.equal(dup.getWidth(), img.getWidth());
            assert.deepEqual(Array.from(dup.getBuffer()), Array.from(img.getBuffer()));
          } finally {
            dup.release();
          }
        } finally {
          img.release();
        }
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('round-trips through save and load', (t) => {
    if (!requireDevice(t)) return;
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'node-pylon-img-'));
    const file = path.join(dir, 'frame.png');
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const img = new pylon.PylonImage();
        try {
          img.copyImage(grab);
          assert.equal(img.canSaveWithoutConversion('Png'), true);
          img.save('Png', file);
          assert.ok(fs.existsSync(file));
          const loaded = new pylon.PylonImage();
          try {
            loaded.load(file);
            assert.equal(loaded.isValid(), true);
            assert.equal(loaded.getWidth(), img.getWidth());
            assert.equal(loaded.getHeight(), img.getHeight());
          } finally {
            loaded.release();
          }
        } finally {
          img.release();
        }
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
      fs.rmSync(dir, { recursive: true, force: true });
    }
  });
});

describe('image format converter', () => {
  it('configures output format and alignment', () => {
    const conv = new pylon.ImageFormatConverter();
    conv.setOutputPixelFormat('RGB8');
    assert.equal(conv.getOutputPixelFormat(), 'RGB8');
    conv.setOutputBitAlignment('MsbAligned');
    assert.equal(conv.getOutputBitAlignment(), 'MsbAligned');
    conv.setOutputBitAlignment('LsbAligned');
    assert.equal(conv.getOutputBitAlignment(), 'LsbAligned');
    assert.equal(pylon.ImageFormatConverter.isSupportedInputFormat('Mono8'), true);
    assert.equal(pylon.ImageFormatConverter.isSupportedOutputFormat('RGB8'), true);
    assert.throws(() => conv.setOutputPixelFormat('NoSuchFormat'));
    assert.throws(() => conv.setOutputBitAlignment('Sideways'));
  });

  it('converts emulator frames between pixel types', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const conv = new pylon.ImageFormatConverter();
        const srcFormat = grab.getPixelType();
        assert.equal(pylon.ImageFormatConverter.isSupportedInputFormat(srcFormat), true);
        conv.setOutputPixelFormat('BGR8');
        assert.equal(conv.imageHasDestinationFormat(grab), false);
        assert.ok(conv.getBufferSizeForConversion(grab) > 0);
        const dst = new pylon.PylonImage();
        try {
          conv.convert(dst, grab);
          assert.equal(dst.isValid(), true);
          assert.equal(dst.getWidth(), grab.getWidth());
          assert.equal(dst.getHeight(), grab.getHeight());
          assert.equal(dst.getPixelType(), 'BGR8');
          assert.equal(dst.getImageSize(), grab.getWidth() * grab.getHeight() * 3);
          assert.equal(conv.imageHasDestinationFormat(dst), true);
          // Converting an already-converted image is a no-op copy path.
          const again = new pylon.PylonImage();
          try {
            conv.convert(again, dst);
            assert.equal(again.getPixelType(), 'BGR8');
          } finally {
            again.release();
          }
        } finally {
          dst.release();
        }
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });

  it('scales payload with the output format', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    const origW = cam.getInteger('Width');
    const origH = cam.getInteger('Height');
    try {
      const w = pickInt(cam.getIntegerMin('Width'), cam.getIntegerMax('Width'), 64);
      const h = pickInt(cam.getIntegerMin('Height'), cam.getIntegerMax('Height'), 48);
      cam.setInteger('Width', w);
      cam.setInteger('Height', h);
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const conv = new pylon.ImageFormatConverter();
        conv.setOutputPixelFormat('RGB8');
        const dst = new pylon.PylonImage();
        try {
          conv.convert(dst, grab);
          assert.equal(dst.getImageSize(), w * h * 3);
        } finally {
          dst.release();
        }
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      try { cam.setInteger('Width', origW); } catch { /* restore best effort */ }
      try { cam.setInteger('Height', origH); } catch { /* restore best effort */ }
      cam.close();
    }
  });
});

describe('image decompressor', () => {
  it('describes uncompressed emulator frames', (t) => {
    if (!requireDevice(t)) return;
    const cam = openCamera();
    try {
      cam.startGrabbing();
      try {
        const grab = cam.retrieveGrabResult(10000);
        assert.ok(grab !== null);
        const dec = new pylon.ImageDecompressor();
        const info = dec.describe(grab);
        assert.equal(info.hasCompressedImage, false);
        assert.equal(typeof info.width, 'number');
        assert.equal(typeof info.height, 'number');
        assert.equal(pylon.ImageDecompressor.getCompressionMode(), 'Off');
      } finally {
        cam.stopGrabbing();
      }
    } finally {
      cam.close();
    }
  });
});
