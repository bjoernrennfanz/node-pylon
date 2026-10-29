// RN static check: runs in CI without camera/SDK/compiler.
// Verifies that docs + shared core + TurboModule spec + adapter sketches
// + sample exist, that the core exposes the required symbols, and that no
// mock API remains in the RN surface.
'use strict';
const fs = require('node:fs');
const path = require('node:path');
const {test} = require('node:test');
const assert = require('node:assert');

const root = path.join(__dirname, '..');
const mustExist = [
  'docs/rn-architecture.md',
  'react-native/shared/pylon_core.h',
  'react-native/shared/pylon_core.cpp',
  'react-native/specs/NativePylon.js',
  'react-native/src/index.ts',
  'react-native/windows/NativePylonModule.h',
  'react-native/macos/NativePylonModule.mm',
  'react-native/linux/NativePylonModule.h',
  'react-native/sample/App.tsx',
  'react-native/sample/README.md',
];

test('rn-desktop: all files present', () => {
  for (const f of mustExist) {
    assert.ok(fs.existsSync(path.join(root, f)), `missing: ${f}`);
  }
});

test('rn-desktop: core without JSI/V8 includes (shareable)', () => {
  const h = fs.readFileSync(path.join(root, 'react-native/shared/pylon_core.h'), 'utf8');
  assert.ok(h.includes('EnumerateDevices'), 'core: EnumerateDevices missing');
  assert.ok(h.includes('PylonVersion'), 'core: PylonVersion missing');
  assert.ok(!h.includes('napi.h'), 'core must not include napi.h');
  assert.ok(!h.includes('jsi/'), 'core must not include jsi');
});

test('rn-desktop: spec uses TurboModuleRegistry', () => {
  const spec = fs.readFileSync(path.join(root, 'react-native/specs/NativePylon.js'), 'utf8');
  assert.ok(spec.includes('TurboModuleRegistry'), 'spec: TurboModuleRegistry missing');
  assert.ok(spec.includes('codegenNativeModule') || spec.includes('TurboModule'), 'spec: TurboModule marker missing');
});

test('rn-desktop: no mock API in RN surface', () => {
  for (const f of [
    'react-native/shared/pylon_core.h',
    'react-native/shared/pylon_core.cpp',
    'react-native/specs/NativePylon.js',
    'react-native/src/index.ts',
    'react-native/sample/App.tsx',
  ]) {
    const text = fs.readFileSync(path.join(root, f), 'utf8');
    assert.ok(!/mock/i.test(text), `${f} still mentions mock`);
  }
});

test('rn-desktop: architecture doc mentions out-of-tree platforms', () => {
  const doc = fs.readFileSync(path.join(root, 'docs/rn-architecture.md'), 'utf8');
  for (const needle of ['react-native-windows', 'react-native-macos', 'out-of-tree']) {
    assert.ok(doc.includes(needle), `docs: ${needle} missing`);
  }
});
