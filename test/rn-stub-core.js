'use strict';
// Shared test harness for the RN suites: an emulator-like in-test core stub
// (CamEmu semantics) plus the loader for the shipped TS facade.
//
// The stub stands in for the JSI/TurboModule layer only (test harness, never
// shipped): it implements the exact codegen-spec surface with the same
// error texts as pylon_core_* ("unknown node: X", "node is not an
// Integer: X", "node is not writable: X", "out of range", "invalid entry",
// "camera is not open", "Not grabbing"). The facade under test is always
// the unmodified shipped file, loaded via Node type stripping.
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { pathToFileURL } = require('node:url');

const root = path.join(__dirname, '..');

const ACCESS_MODES = [1, 3, 4, 5];

function defaultNodes() {
  return {
    Width: {
      type: 'Integer', value: 640, min: 16, max: 1920, inc: 2,
      unit: '', representation: 'Linear', display: 'Width',
      description: 'Image width.', readable: true, writable: true,
    },
    Height: {
      type: 'Integer', value: 480, min: 16, max: 1080, inc: 2,
      unit: '', representation: 'Linear', display: 'Height',
      description: 'Image height.', readable: true, writable: true,
    },
    PayloadSize: {
      type: 'Integer', value: 0, min: 0, max: 1 << 30, inc: 1,
      unit: '', representation: 'Linear', display: 'PayloadSize',
      description: 'Payload size.', readable: true, writable: false,
      computed: true,
    },
    ExposureTime: {
      type: 'Float', value: 5000, min: 10, max: 1000000, hasInc: false,
      unit: 'us', representation: 'Linear', display: 'ExposureTime',
      description: 'Exposure time.', readable: true, writable: true,
    },
    PixelFormat: {
      type: 'Enumeration', value: 'Mono8', entries: ['Mono8', 'RGB8'],
      intValues: { Mono8: 0x01080001, RGB8: 0x02180014 },
      display: 'PixelFormat', description: 'Pixel format.',
      readable: true, writable: true,
    },
    ChunkModeActive: {
      type: 'Boolean', value: false, display: 'ChunkModeActive',
      description: 'Chunk mode.', readable: true, writable: true,
    },
    DeviceUserID: {
      type: 'String', value: 'cam0', maxLength: 32, display: 'DeviceUserID',
      description: 'User ID.', readable: true, writable: true,
    },
    DeviceModelName: {
      type: 'String', value: 'Emulator', maxLength: 32,
      display: 'DeviceModelName', description: 'Model name.',
      readable: true, writable: false,
    },
    AcquisitionMode: {
      type: 'Enumeration', value: 'Continuous',
      entries: ['Continuous', 'SingleFrame'],
      intValues: { Continuous: 0, SingleFrame: 1 },
      display: 'AcquisitionMode', description: 'Acquisition mode.',
      readable: true, writable: true,
    },
    TriggerSelector: {
      type: 'Enumeration', value: 'AcquisitionStart',
      entries: ['AcquisitionStart', 'FrameStart'],
      intValues: { AcquisitionStart: 0, FrameStart: 1 },
      display: 'TriggerSelector', description: 'Trigger selector.',
      readable: true, writable: true, selector: true,
      selected: ['TriggerMode', 'TriggerSource'], selecting: [],
    },
    TriggerMode: {
      type: 'Enumeration', value: 'Off', entries: ['On', 'Off'],
      intValues: { On: 1, Off: 0 }, display: 'TriggerMode',
      description: 'Trigger mode.', readable: true, writable: true,
    },
    TriggerSource: {
      type: 'Enumeration', value: 'Software', entries: ['Software', 'Line1'],
      intValues: { Software: 0, Line1: 1 }, display: 'TriggerSource',
      description: 'Trigger source.', readable: true, writable: true,
    },
    AcquisitionStart: { type: 'Command', done: true, readable: true, writable: true },
    AcquisitionStop: { type: 'Command', done: true, readable: true, writable: true },
    TestRegister: {
      type: 'Register', bytes: [0x01, 0x02, 0x03, 0x04], address: 0x1000,
      readable: true, writable: true,
    },
    Root: {
      type: 'Category',
      features: ['Width', 'Height', 'PayloadSize', 'ExposureTime',
        'PixelFormat', 'ChunkModeActive', 'DeviceUserID', 'DeviceModelName',
        'AcquisitionMode', 'TriggerSelector', 'TriggerMode', 'TriggerSource',
        'AcquisitionStart', 'AcquisitionStop', 'TestRegister',
        'ImageFormatControl'],
    },
    ImageFormatControl: {
      type: 'Category', features: ['Width', 'Height', 'PixelFormat'],
    },
  };
}

const ARTICLE = { Integer: 'an', Enumeration: 'an' };

function createMockCore() {
  const devices = [
    {
      friendlyName: 'Emulator 0', modelName: 'Emulator',
      serialNumber: 'EMU000001', vendorName: 'Basler',
      deviceClass: 'BaslerCamEmu',
    },
    {
      friendlyName: 'Emulator 1', modelName: 'Emulator',
      serialNumber: 'EMU000002', vendorName: 'Basler',
      deviceClass: 'BaslerCamEmu',
    },
  ];
  let nextHandle = 1;
  const cams = new Map();
  const state = { returnBase64: false };

  function need(handle) {
    const cam = cams.get(handle);
    if (!cam) throw new Error('unknown camera handle');
    return cam;
  }

  function needOpen(cam) {
    if (!cam.open) throw new Error('camera is not open');
  }

  function nodeOf(cam, name) {
    const node = cam.nodes[name];
    if (!node) throw new Error(`unknown node: ${name}`);
    return node;
  }

  function typed(cam, name, want) {
    const node = nodeOf(cam, name);
    if (node.type !== want) {
      const article = ARTICLE[want] || 'a';
      throw new Error(`node is not ${article} ${want}: ${name}`);
    }
    return node;
  }

  function writable(node, name) {
    if (node.writable === false) throw new Error(`node is not writable: ${name}`);
  }

  function payloadOf(cam) {
    const w = cam.nodes.Width.value;
    const h = cam.nodes.Height.value;
    const bpp = cam.nodes.PixelFormat.value === 'RGB8' ? 3 : 1;
    return { w, h, bpp, size: w * h * bpp };
  }

  function frame(cam) {
    const { w, h, size } = payloadOf(cam);
    const buf = new Uint8Array(size);
    const seq = cam.frames;
    for (let i = 0; i < buf.length; i += 1) buf[i] = (i ^ (seq * 37)) & 0xff;
    const frameNumber = 100 + seq;
    const timestamp = 1000000 + seq * 1000;
    const image = {
      succeeded: true,
      width: w,
      height: h,
      pixelType: cam.nodes.PixelFormat.value,
      payloadSize: size,
      timestamp,
      blockId: 1000 + seq,
      frameNumber,
      buffer: state.returnBase64 ? Buffer.from(buf).toString('base64') : buf,
    };
    if (cam.nodes.ChunkModeActive.value === true) {
      image.chunk = { modeActive: true, timestamp, frameID: frameNumber };
    }
    cam.frames += 1;
    cam.grabCount += 1;
    cam.nodes.PayloadSize.value = size;
    return image;
  }

  function emptyImage() {
    return {
      succeeded: false, width: 0, height: 0, pixelType: '',
      payloadSize: 0, timestamp: 0, blockId: 0, frameNumber: 0,
      buffer: state.returnBase64 ? '' : new Uint8Array(0),
    };
  }

  return {
    state,
    getVersion: () => '0.2.0-core',
    getPylonVersion: () => '12.5.2.1565',
    enumerateDevices: () => devices.map((d) => ({ ...d })),
    getPixelTypeName: (v) => (v === 0x01080001 ? 'Mono8' : v === 0x02180014 ? 'RGB8' : ''),
    isMonoPixelType: (v) => v === 0x01080001,
    isColorPixelType: (v) => v === 0x02180014,
    getDiscoveryConstants: () => ({
      strings: {
        BaslerCamEmuDeviceClass: 'BaslerCamEmu',
        BaslerIpCamDeviceClass: 'BaslerIPCam',
        BaslerCameraLinkDeviceClass: 'BaslerCameraLink',
        BaslerUsbDeviceClass: 'BaslerUsb',
        BaslerGenTlGevDeviceClass: 'BaslerGTC/Basler/GEV',
        BaslerGenTlU3vDeviceClass: 'BaslerGTC/Basler/U3V',
        BaslerGenTlCxpDeviceClass: 'BaslerGTC/Basler/CXP',
        BaslerGenTlBlazeDeviceClass:
          'BaslerGTC/Basler/GenTL_Producer_for_Basler_blaze_101_cameras',
        BaslerGenTlStaDeviceClass: 'BaslerGTC/Basler/basler_xw',
        TLTypeGigE: 'GigE',
        TLTypeUSB: 'USB',
        TLTypeCL: 'CL',
        TLTypeCamEmu: 'CamEmu',
        TLTypeCXP: 'CXP',
        TLTypeCustom: 'Custom',
        FriendlyNameKey: 'FriendlyName',
        FullNameKey: 'FullName',
        VendorNameKey: 'VendorName',
        DeviceClassKey: 'DeviceClass',
        TLTypeKey: 'TLType',
        InterfaceIDKey: 'InterfaceID',
        SerialNumberKey: 'SerialNumber',
        UserDefinedNameKey: 'UserDefinedName',
        ModelNameKey: 'ModelName',
        DeviceVersionKey: 'DeviceVersion',
        DeviceFactoryKey: 'DeviceFactory',
        XMLSourceKey: 'XMLSource',
        FileNameKey: 'FileName',
        InfoIDKey: 'InfoID',
        VersionKey: 'Version',
      },
      integers: {
        DeviceAccessMode_Control: 0x1,
        DeviceAccessMode_Stream: 0x3,
        DeviceAccessMode_Event: 0x4,
        DeviceAccessMode_Exclusive: 0x5,
        Accessibility_Unknown: 0,
        Accessibility_Ok: 1,
        Accessibility_Opened: 2,
        Accessibility_OpenedExclusively: 3,
        Accessibility_NotReachable: 4,
      },
    }),
    openCamera(deviceIndex, accessMode) {
      if (!ACCESS_MODES.includes(accessMode)) {
        throw new Error('Unknown access mode (Control/Stream/Event/Exclusive)');
      }
      if (deviceIndex >= devices.length) {
        throw new Error('device index out of range: no device attached');
      }
      const idx = deviceIndex < 0 ? 0 : deviceIndex;
      const handle = nextHandle++;
      cams.set(handle, {
        open: true, grabbing: false, maxImages: 0, grabCount: 0,
        attached: true, ownership: true,
        removed: false, context: 0, queued: 0, frames: 0,
        deviceIndex: idx, nodes: defaultNodes(),
      });
      return handle;
    },
    closeCamera(handle) {
      const cam = need(handle);
      cam.grabbing = false;
      cam.open = false;
      cams.delete(handle);
    },
    isCameraOpen: (handle) => need(handle).open,
    getDeviceInfo(handle) {
      const cam = need(handle);
      if (!cam.attached) throw new Error('no device attached');
      const d = devices[cam.deviceIndex];
      return {
        friendlyName: d.friendlyName, modelName: d.modelName,
        serialNumber: d.serialNumber, vendorName: d.vendorName,
        deviceClass: d.deviceClass,
        fullName: `${d.modelName} ${d.serialNumber}`,
      };
    },
    isPylonDeviceAttached: (handle) => need(handle).attached,
    hasOwnership: (handle) => need(handle).ownership,
    attachDevice(handle, deviceIndex) {
      const cam = need(handle);
      if (deviceIndex >= devices.length) {
        throw new Error('device index out of range: no device attached');
      }
      cam.deviceIndex = deviceIndex < 0 ? 0 : deviceIndex;
      cam.attached = true;
    },
    detachDevice(handle) {
      const cam = need(handle);
      if (!cam.attached) throw new Error('no device attached');
      cam.attached = false;
      cam.open = false;
      cam.grabbing = false;
    },
    destroyDevice(handle) {
      const cam = need(handle);
      cam.attached = false;
      cam.open = false;
      cam.grabbing = false;
    },
    isCameraDeviceRemoved: (handle) => need(handle).removed,
    getCameraContext: (handle) => need(handle).context,
    setCameraContext(handle, context) {
      need(handle).context = context;
    },
    getQueuedBufferCount(handle) {
      const cam = need(handle);
      if (!cam.attached) throw new Error('no device attached');
      return cam.queued;
    },
    startGrabbing(handle, strategy, maxImages) {
      const cam = need(handle);
      needOpen(cam);
      cam.grabbing = true;
      cam.grabCount = 0;
      cam.maxImages = maxImages || 0;
    },
    stopGrabbing(handle) {
      need(handle).grabbing = false;
    },
    isGrabbing(handle) {
      const cam = need(handle);
      if (!cam.grabbing) return false;
      if (cam.maxImages > 0 && cam.grabCount >= cam.maxImages) {
        cam.grabbing = false;
        return false;
      }
      return true;
    },
    grabOne(handle, timeoutMs, timeoutHandling) {
      const cam = need(handle);
      needOpen(cam);
      if (timeoutMs === 0 && timeoutHandling === 0) return emptyImage();
      return frame(cam);
    },
    retrieveResult(handle, timeoutMs, timeoutHandling) {
      const cam = need(handle);
      needOpen(cam);
      if (!cam.grabbing) {
        throw new Error('Not grabbing (grab loop is not running)');
      }
      if (timeoutMs === 0 && timeoutHandling === 0) return emptyImage();
      return frame(cam);
    },
    getNodeType: (handle, name) => nodeOf(need(handle), name).type,
    isReadable(handle, name) {
      return Boolean(nodeOf(need(handle), name));
    },
    isWritable(handle, name) {
      return nodeOf(need(handle), name).writable !== false;
    },
    isAvailable(handle, name) {
      return Boolean(nodeOf(need(handle), name));
    },
    getInteger(handle, name) {
      const cam = need(handle);
      const node = typed(cam, name, 'Integer');
      if (node.computed) {
        node.value = payloadOf(cam).size;
        return node.value;
      }
      return node.value;
    },
    setInteger(handle, name, value) {
      const node = typed(need(handle), name, 'Integer');
      writable(node, name);
      if (value < node.min || value > node.max) {
        throw new Error(`SetInteger(${name}) out of range`);
      }
      node.value = value;
    },
    getIntegerMin: (handle, name) => typed(need(handle), name, 'Integer').min,
    getIntegerMax: (handle, name) => typed(need(handle), name, 'Integer').max,
    getIntegerInc: (handle, name) => typed(need(handle), name, 'Integer').inc,
    getIntegerUnit: (handle, name) => typed(need(handle), name, 'Integer').unit,
    getIntegerRepresentation: (handle, name) =>
      typed(need(handle), name, 'Integer').representation,
    setIntegerToMin(handle, name) {
      const node = typed(need(handle), name, 'Integer');
      writable(node, name);
      node.value = node.min;
    },
    setIntegerToMax(handle, name) {
      const node = typed(need(handle), name, 'Integer');
      writable(node, name);
      node.value = node.max;
    },
    tryMax(handle, name) {
      const node = typed(need(handle), name, 'Integer');
      writable(node, name);
      node.value = node.max;
      return node.max;
    },
    getFloat: (handle, name) => typed(need(handle), name, 'Float').value,
    setFloat(handle, name, value) {
      const node = typed(need(handle), name, 'Float');
      writable(node, name);
      if (value < node.min || value > node.max) {
        throw new Error(`SetFloat(${name}) out of range`);
      }
      node.value = value;
    },
    getFloatMin: (handle, name) => typed(need(handle), name, 'Float').min,
    getFloatMax: (handle, name) => typed(need(handle), name, 'Float').max,
    hasFloatInc: (handle, name) =>
      typed(need(handle), name, 'Float').hasInc === true,
    getFloatInc(handle, name) {
      const node = typed(need(handle), name, 'Float');
      if (node.hasInc !== true) throw new Error(`GetFloatInc(${name}) failed: no increment`);
      return 1;
    },
    getFloatUnit: (handle, name) => typed(need(handle), name, 'Float').unit,
    getFloatRepresentation: (handle, name) =>
      typed(need(handle), name, 'Float').representation,
    setFloatToMin(handle, name) {
      const node = typed(need(handle), name, 'Float');
      writable(node, name);
      node.value = node.min;
    },
    setFloatToMax(handle, name) {
      const node = typed(need(handle), name, 'Float');
      writable(node, name);
      node.value = node.max;
    },
    getDisplayName: (handle, name) =>
      nodeOf(need(handle), name).display || name,
    getDescription: (handle, name) =>
      nodeOf(need(handle), name).description || '',
    getValueAsString(handle, name) {
      const cam = need(handle);
      const node = nodeOf(cam, name);
      switch (node.type) {
        case 'Integer':
          return String(
            node.computed ? payloadOf(cam).size : node.value,
          );
        case 'Float':
        case 'Enumeration':
        case 'String':
          return String(node.value);
        case 'Boolean':
          return node.value ? 'true' : 'false';
        default:
          throw new Error(`node has no value interface: ${name}`);
      }
    },
    setValueFromString(handle, name, value) {
      const cam = need(handle);
      const node = nodeOf(cam, name);
      writable(node, name);
      switch (node.type) {
        case 'Integer': {
          const v = Number.parseInt(value, 10);
          if (!Number.isFinite(v)) throw new Error(`SetValueFromString(${name}) failed: bad integer`);
          if (v < node.min || v > node.max) {
            throw new Error(`SetValueFromString(${name}) out of range`);
          }
          node.value = v;
          return;
        }
        case 'Float': {
          const v = Number.parseFloat(value);
          if (!Number.isFinite(v)) throw new Error(`SetValueFromString(${name}) failed: bad float`);
          node.value = v;
          return;
        }
        case 'Enumeration':
          if (!node.entries.includes(value)) {
            throw new Error(`SetValueFromString(${name}) failed: invalid entry ${value}`);
          }
          node.value = value;
          return;
        case 'Boolean':
          node.value = value === 'true';
          return;
        case 'String':
          node.value = value;
          return;
        default:
          throw new Error(`node has no value interface: ${name}`);
      }
    },
    getBoolean: (handle, name) => typed(need(handle), name, 'Boolean').value,
    setBoolean(handle, name, value) {
      const node = typed(need(handle), name, 'Boolean');
      writable(node, name);
      node.value = value;
    },
    getString: (handle, name) => typed(need(handle), name, 'String').value,
    setString(handle, name, value) {
      const node = typed(need(handle), name, 'String');
      writable(node, name);
      node.value = value;
    },
    getStringMaxLength: (handle, name) =>
      typed(need(handle), name, 'String').maxLength,
    getEnum: (handle, name) => typed(need(handle), name, 'Enumeration').value,
    setEnum(handle, name, value) {
      const node = typed(need(handle), name, 'Enumeration');
      writable(node, name);
      if (!node.entries.includes(value)) {
        throw new Error(`SetEnum(${name}) failed: invalid entry ${value}`);
      }
      node.value = value;
    },
    getEnumEntries: (handle, name) =>
      typed(need(handle), name, 'Enumeration').entries.slice(),
    getEnumIntValue(handle, name) {
      const node = typed(need(handle), name, 'Enumeration');
      return node.intValues[node.value];
    },
    setEnumIntValue(handle, name, value) {
      const node = typed(need(handle), name, 'Enumeration');
      writable(node, name);
      const entry = Object.keys(node.intValues).find(
        (k) => node.intValues[k] === value,
      );
      if (entry === undefined) {
        throw new Error(`SetEnumIntValue(${name}) failed: invalid entry`);
      }
      node.value = entry;
    },
    executeCommand(handle, name) {
      const node = typed(need(handle), name, 'Command');
      writable(node, name);
      node.done = true;
    },
    isCommandDone(handle, name) {
      return typed(need(handle), name, 'Command').done === true;
    },
    getCategoryFeatures(handle, name) {
      const node = typed(need(handle), name, 'Category');
      return node.features.slice();
    },
    isSelector(handle, name) {
      return nodeOf(need(handle), name).selector === true;
    },
    getSelectedFeatures(handle, name) {
      const node = nodeOf(need(handle), name);
      if (node.selector !== true) throw new Error(`node is not a Selector: ${name}`);
      return node.selected.slice();
    },
    getSelectingFeatures(handle, name) {
      const node = nodeOf(need(handle), name);
      if (node.selector !== true) throw new Error(`node is not a Selector: ${name}`);
      return node.selecting.slice();
    },
    getRegisterLength: (handle, name) =>
      typed(need(handle), name, 'Register').bytes.length,
    getRegisterAddress: (handle, name) =>
      typed(need(handle), name, 'Register').address,
    readRegister(handle, name) {
      const node = typed(need(handle), name, 'Register');
      return Buffer.from(node.bytes).toString('base64');
    },
    writeRegister(handle, name, dataBase64) {
      const node = typed(need(handle), name, 'Register');
      writable(node, name);
      node.bytes = Array.from(Buffer.from(dataBase64, 'base64'));
    },
  };
}

// Loads the shipped facade with the stubbed TurboModule. Returns the
// facade module plus the mock core (for toggles like returnBase64).
async function loadFacade(mock) {
  const core = mock || createMockCore();
  globalThis.__rnMock = core;
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'rn-facade-'));
  fs.writeFileSync(
    path.join(dir, 'rn-stub.mjs'),
    'export const TurboModuleRegistry = { getEnforcing() { return globalThis.__rnMock; } };\n',
  );
  const src = fs
    .readFileSync(path.join(root, 'react-native/src/index.ts'), 'utf8')
    .replace("from 'react-native'", "from './rn-stub.mjs'");
  fs.writeFileSync(path.join(dir, 'rn-facade.ts'), src);
  const facade = await import(pathToFileURL(path.join(dir, 'rn-facade.ts')).href);
  return { facade, mock: core };
}

module.exports = { createMockCore, loadFacade };
