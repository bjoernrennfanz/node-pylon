// RN-Statikcheck (ZAU-153): laeuft in CI ohne Kamera/SDK/Compiler.
// Prueft, dass Doku + geteilter Core + TurboModule-Spec + Adapter-Skizzen
// + Sample vorhanden sind und der Core die noetigen Symbole enthaelt.
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

test('rn-desktop: alle Dateien vorhanden', () => {
  for (const f of mustExist) {
    assert.ok(fs.existsSync(path.join(root, f)), `fehlt: ${f}`);
  }
});

test('rn-desktop: core ohne JSI/V8-Includes (teilbar)', () => {
  const h = fs.readFileSync(path.join(root, 'react-native/shared/pylon_core.h'), 'utf8');
  assert.ok(h.includes('EnumerateDevices'), 'core: EnumerateDevices fehlt');
  assert.ok(h.includes('GrabMockFrame'), 'core: GrabMockFrame fehlt');
  assert.ok(h.includes('PylonVersion'), 'core: PylonVersion fehlt');
  assert.ok(!h.includes('napi.h'), 'core darf kein napi.h enthalten');
  assert.ok(!h.includes('jsi/'), 'core darf kein jsi enthalten');
});

test('rn-desktop: spec nutzt TurboModuleRegistry', () => {
  const spec = fs.readFileSync(path.join(root, 'react-native/specs/NativePylon.js'), 'utf8');
  assert.ok(spec.includes('TurboModuleRegistry'), 'spec: TurboModuleRegistry fehlt');
  assert.ok(spec.includes('codegenNativeModule') || spec.includes('TurboModule'), 'spec: TurboModule-Marker fehlt');
});

test('rn-desktop: architektur-doku nennt out-of-tree-plattformen', () => {
  const doc = fs.readFileSync(path.join(root, 'docs/rn-architecture.md'), 'utf8');
  for (const needle of ['react-native-windows', 'react-native-macos', 'out-of-tree']) {
    assert.ok(doc.includes(needle), `doku: ${needle} fehlt`);
  }
});
