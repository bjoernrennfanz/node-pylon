'use strict';
// RN contract checks: TS validity of the facade (via Node type stripping),
// codegen-compatibility of the TurboModule spec, core/spec parity, and the
// sample smoke test. Runs without camera, SDK or compiler.
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { test } = require('node:test');
const assert = require('node:assert/strict');
const { pathToFileURL } = require('node:url');

const root = path.join(__dirname, '..');

function read(rel) {
  return fs.readFileSync(path.join(root, rel), 'utf8');
}

// Every codegen-spec method must have a shared-core counterpart (no second
// grab path, no logic in the adapters). `null` = spec-only (version const).
const SPEC_TO_CORE = {
  getVersion: null, // core::kVersion
  getPylonVersion: 'PylonVersion',
  enumerateDevices: 'EnumerateDevices',
  getPixelTypeName: 'PixelTypeName',
  isMonoPixelType: 'IsMonoPixelType',
  isColorPixelType: 'IsColorPixelType',
  openCamera: 'OpenCamera',
  closeCamera: 'CloseCamera',
  isCameraOpen: 'IsCameraOpen',
  startGrabbing: 'StartGrabbing',
  stopGrabbing: 'StopGrabbing',
  isGrabbing: 'IsGrabbing',
  grabOne: 'GrabOne',
  retrieveResult: 'RetrieveResult',
  getNodeType: 'GetNodeType',
  isReadable: 'IsReadable',
  isWritable: 'IsWritable',
  isAvailable: 'IsAvailable',
  getInteger: 'GetInteger',
  setInteger: 'SetInteger',
  getFloat: 'GetFloat',
  setFloat: 'SetFloat',
  getBoolean: 'GetBoolean',
  setBoolean: 'SetBoolean',
  getString: 'GetString',
  setString: 'SetString',
  getEnum: 'GetEnum',
  setEnum: 'SetEnum',
  getEnumEntries: 'GetEnumEntries',
  executeCommand: 'ExecuteCommand',
  getIntegerMin: 'GetIntegerMin',
  getIntegerMax: 'GetIntegerMax',
  getIntegerInc: 'GetIntegerInc',
  getIntegerUnit: 'GetIntegerUnit',
  getIntegerRepresentation: 'GetIntegerRepresentation',
  setIntegerToMin: 'SetIntegerToMin',
  setIntegerToMax: 'SetIntegerToMax',
  tryMax: 'TryMax',
  getFloatMin: 'GetFloatMin',
  getFloatMax: 'GetFloatMax',
  hasFloatInc: 'HasFloatInc',
  getFloatInc: 'GetFloatInc',
  getFloatUnit: 'GetFloatUnit',
  getFloatRepresentation: 'GetFloatRepresentation',
  setFloatToMin: 'SetFloatToMin',
  setFloatToMax: 'SetFloatToMax',
  getDisplayName: 'GetDisplayName',
  getDescription: 'GetDescription',
  getValueAsString: 'GetValueAsString',
  setValueFromString: 'SetValueFromString',
  getStringMaxLength: 'GetStringMaxLength',
  getEnumIntValue: 'GetEnumIntValue',
  setEnumIntValue: 'SetEnumIntValue',
  isCommandDone: 'IsCommandDone',
  getCategoryFeatures: 'GetCategoryFeatures',
  isSelector: 'IsSelector',
  getSelectedFeatures: 'GetSelectedFeatures',
  getSelectingFeatures: 'GetSelectingFeatures',
  getRegisterLength: 'GetRegisterLength',
  getRegisterAddress: 'GetRegisterAddress',
  readRegister: 'ReadRegister',
  writeRegister: 'WriteRegister',
  getDeviceInfo: 'GetCameraDeviceInfo',
  isPylonDeviceAttached: 'IsPylonDeviceAttached',
  hasOwnership: 'HasOwnership',
  attachDevice: 'AttachDevice',
  detachDevice: 'DetachDevice',
  destroyDevice: 'DestroyDevice',
  isCameraDeviceRemoved: 'IsCameraDeviceRemoved',
  getCameraContext: 'GetCameraContext',
  setCameraContext: 'SetCameraContext',
  getQueuedBufferCount: 'GetQueuedBufferCount',
  getDiscoveryConstants: 'GetDiscoveryConstants',
};

// Codegen allows only these types in a spec (plus the declared interfaces).
const ALLOWED_TYPES = new Set([
  'string',
  'number',
  'boolean',
  'void',
  'Object',
  'Array',
  'DeviceInfo',
  'GrabImage',
]);

function specSignatureTypes(signature) {
  // Splits "name(a: T, b: U): R" into ['T', 'U', 'R'] with generics reduced
  // to their base ('Array<string>' -> 'Array').
  const m = signature.match(/^[A-Za-z0-9_]+\((.*)\):\s*(.+)$/);
  if (!m) return null;
  const [, params, ret] = m;
  const types = [];
  if (params.trim() !== '') {
    for (const p of params.split(',')) {
      if (p.trim() === '') continue;
      const t = p.split(':')[1];
      if (!t) return null;
      types.push(t.trim().replace(/<.*>$/, ''));
    }
  }
  types.push(ret.trim().replace(/<.*>$/, ''));
  return types;
}

test('rn-contract: facade is valid TypeScript (loads via type stripping)', async () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'rn-tscheck-'));
  fs.writeFileSync(
    path.join(dir, 'rn-stub.mjs'),
    'export const TurboModuleRegistry = { getEnforcing() { throw new Error("no native module"); } };\n',
  );
  const src = read('react-native/src/index.ts').replace(
    "from 'react-native'",
    "from './rn-stub.mjs'",
  );
  // `import type` is erased by stripping; the spec file itself is Flow-typed
  // and must never be loaded at runtime from the facade.
  assert.ok(src.includes('import type'), 'facade must use import type for spec types');
  fs.writeFileSync(path.join(dir, 'rn-facade.ts'), src);
  const facade = await import(
    pathToFileURL(path.join(dir, 'rn-facade.ts')).href
  );
  for (const name of [
    'version',
    'pylonVersion',
    'enumerateDevices',
    'getPixelTypeName',
    'isMonoPixelType',
    'isColorPixelType',
    'InstantCamera',
    'createFirstCamera',
    'createCameraAt',
    'GrabStrategy',
    'TimeoutHandling',
    'AcquireContinuousConfiguration',
    'AcquireSingleFrameConfiguration',
    'SoftwareTriggerConfiguration',
    'ConfigurationHelper',
    'RN_COVERAGE_GAPS',
  ]) {
    assert.ok(name in facade, `facade export missing: ${name}`);
  }
  for (const method of [
    'isOpen',
    'close',
    'startGrabbing',
    'stopGrabbing',
    'isGrabbing',
    'grabOne',
    'retrieveResult',
    'getInteger',
    'setInteger',
    'getFloat',
    'setFloat',
    'getBoolean',
    'setBoolean',
    'getString',
    'setString',
    'getEnum',
    'setEnum',
    'getEnumEntries',
    'executeCommand',
  ]) {
    assert.equal(
      typeof facade.InstantCamera.prototype[method],
      'function',
      `InstantCamera.${method} missing`,
    );
  }
});

test('rn-contract: spec is codegen-compatible (allowed types only)', () => {
  const spec = read('react-native/specs/NativePylon.js');
  assert.ok(spec.includes('TurboModuleRegistry'), 'spec: TurboModuleRegistry missing');
  assert.ok(spec.includes('export interface Spec extends TurboModule'));
  assert.ok(spec.includes('export interface DeviceInfo'));
  const signatures = [];
  const re = /([A-Za-z0-9_]+)\(([^)]*)\)\s*:\s*([^;]+);/g;
  let m;
  while ((m = re.exec(spec)) !== null) {
    if (m[1] === 'getEnforcing') continue;
    signatures.push(`${m[1]}(${m[2].replace(/\s+/g, ' ').trim()}): ${m[3].trim()}`);
  }
  assert.ok(
    signatures.length >= Object.keys(SPEC_TO_CORE).length,
    `spec too small: ${signatures.length} methods`,
  );
  for (const sig of signatures) {
    const types = specSignatureTypes(sig);
    assert.ok(types, `spec: unparseable signature: ${sig}`);
    for (const t of types) {
      assert.ok(
        ALLOWED_TYPES.has(t),
        `spec: codegen-incompatible type "${t}" in: ${sig}`,
      );
    }
  }
  for (const banned of ['enum ', 'class ', 'namespace ', '?.', 'mock']) {
    assert.ok(!spec.includes(banned), `spec: forbidden "${banned}" present`);
  }
});

test('rn-contract: every spec method maps to the shared core', () => {
  const header = read('react-native/shared/pylon_core.h');
  const spec = read('react-native/specs/NativePylon.js');
  assert.ok(!/#include[^"\n]*napi\.h/i.test(header), 'core must not include napi.h');
  assert.ok(!/#include[^"\n]*jsi\//i.test(header), 'core must not include jsi');
  const impl = read('react-native/shared/pylon_core.cpp');
  for (const [method, core] of Object.entries(SPEC_TO_CORE)) {
    assert.ok(spec.includes(method), `spec method missing: ${method}`);
    if (core === null) continue;
    assert.ok(
      header.includes(core),
      `core declaration missing for spec method ${method}: ${core}`,
    );
    assert.ok(
      impl.includes(core),
      `core implementation missing for spec method ${method}: ${core}`,
    );
  }
  for (const sym of ['GrabImage', 'CameraHandle', 'PylonError', 'GrabStrategy', 'TimeoutHandling', 'DiscoveryConstants', 'FullDeviceInfo']) {
    assert.ok(header.includes(sym), `core header missing: ${sym}`);
  }
});

test('rn-contract: adapters stay thin marshalling layers over the core', () => {
  for (const f of [
    'react-native/windows/NativePylonModule.h',
    'react-native/macos/NativePylonModule.mm',
    'react-native/linux/NativePylonModule.h',
  ]) {
    const text = read(f);
    assert.ok(/pylon_core/i.test(text), `${f} does not reference the shared core`);
    for (const sym of ['OpenCamera', 'GrabOne', 'GetEnum']) {
      assert.ok(text.includes(sym), `${f} missing core call ${sym}`);
    }
    assert.ok(!/#include[^"\n]*napi\.h/i.test(text), `${f} must not include napi.h`);
    assert.ok(!/#include[^"\n]*node_api\.h/i.test(text), `${f} must not include node_api.h`);
    assert.ok(!/mock/i.test(text), `${f} still mentions mock`);
  }
});

test('rn-contract: sample smoke (grab via facade, image info shown)', () => {
  const app = read('react-native/sample/App.tsx');
  assert.ok(!/mock/i.test(app), 'sample still mentions mock');
  assert.ok(
    app.includes("from '../src'"),
    'sample must use the facade (../src), not the native module directly',
  );
  assert.ok(!/specs\/NativePylon/.test(app), 'sample must not import the spec directly');
  assert.ok(!/TurboModuleRegistry/.test(app), 'sample must not touch TurboModuleRegistry');
  for (const needle of ['InstantCamera', 'grabOne', 'enumerateDevices', 'width', 'height', 'buffer']) {
    assert.ok(app.includes(needle), `sample: ${needle} missing`);
  }
});
