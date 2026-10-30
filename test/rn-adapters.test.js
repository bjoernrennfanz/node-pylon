'use strict';
// RN desktop adapters: static checks for the out-of-tree platform layers.
// Runs without a camera, SDK or compiler (like rn-static.test.js).
// Mirrors the pypylon platform matrix (Windows/macOS/Linux) on the JS side:
// the shared core plus the per-platform JSI/TurboModule sketches must stay
// thin marshalling layers with no duplicated grab logic.
const fs = require('node:fs');
const path = require('node:path');
const { test } = require('node:test');
const assert = require('node:assert/strict');

const root = path.join(__dirname, '..');

function read(rel) {
  return fs.readFileSync(path.join(root, rel), 'utf8');
}

test('rn-desktop adapters: all platform layers present', () => {
  for (const f of [
    'react-native/shared/pylon_core.h',
    'react-native/shared/pylon_core.cpp',
    'react-native/specs/NativePylon.js',
    'react-native/src/index.ts',
    'react-native/windows/NativePylonModule.h',
    'react-native/macos/NativePylonModule.mm',
    'react-native/linux/NativePylonModule.h',
    'react-native/sample/App.tsx',
    'react-native/sample/README.md',
  ]) {
    assert.ok(fs.existsSync(path.join(root, f)), `missing: ${f}`);
  }
});

test('rn-desktop adapters: thin marshalling layers over the shared core', () => {
  for (const f of [
    'react-native/windows/NativePylonModule.h',
    'react-native/macos/NativePylonModule.mm',
    'react-native/linux/NativePylonModule.h',
  ]) {
    const text = read(f);
    assert.ok(/pylon_core/i.test(text), `${f} does not reference the shared core`);
    assert.ok(!/#include[^"\n]*napi\.h/i.test(text), `${f} must not include napi.h`);
    assert.ok(!/#include[^"\n]*node_api\.h/i.test(text), `${f} must not include node_api.h`);
    assert.ok(!/mock/i.test(text), `${f} still mentions mock`);
  }
});

test('rn-desktop adapters: core stays shareable (no N-API/JSI includes)', () => {
  for (const f of [
    'react-native/shared/pylon_core.h',
    'react-native/shared/pylon_core.cpp',
  ]) {
    const text = read(f);
    assert.ok(!/#include[^"\n]*napi\.h/i.test(text), `${f} must not include napi.h`);
    assert.ok(!/#include[^"\n]*jsi\//i.test(text), `${f} must not include jsi headers`);
    assert.ok(!/mock/i.test(text), `${f} still mentions mock`);
  }
});

test('rn-desktop adapters: TurboModule spec shape', () => {
  const spec = read('react-native/specs/NativePylon.js');
  assert.ok(spec.includes('TurboModuleRegistry'), 'spec: TurboModuleRegistry missing');
  assert.ok(
    spec.includes('codegenNativeModule') || spec.includes('TurboModule'),
    'spec: TurboModule marker missing',
  );
  for (const method of [
    'getVersion',
    'getPylonVersion',
    'enumerateDevices',
    'openCamera',
    'closeCamera',
    'startGrabbing',
    'grabOne',
    'retrieveResult',
    'getInteger',
    'setInteger',
    'getEnum',
    'setEnum',
    'executeCommand',
  ]) {
    assert.ok(spec.includes(method), `spec: ${method} missing`);
  }
});

test('rn-desktop adapters: facade only exposes spec methods', () => {
  const facade = read('react-native/src/index.ts');
  for (const method of [
    'version',
    'pylonVersion',
    'enumerateDevices',
    'InstantCamera',
    'grabOne',
    'retrieveResult',
    'getEnum',
    'setEnum',
    'GrabStrategy',
    'TimeoutHandling',
  ]) {
    assert.ok(facade.includes(method), `facade: ${method} missing`);
  }
  assert.ok(!/mock/i.test(facade), 'facade still mentions mock');
});

test('rn-desktop adapters: sample uses only the documented facade', () => {
  const app = read('react-native/sample/App.tsx');
  assert.ok(app.includes('enumerateDevices'), 'sample: enumerateDevices missing');
  assert.ok(!/mock/i.test(app), 'sample still mentions mock');
  assert.ok(!/specs\/NativePylon/.test(app), 'sample must not import the spec directly');
  assert.ok(!/TurboModuleRegistry/.test(app), 'sample must not touch TurboModuleRegistry');
});
