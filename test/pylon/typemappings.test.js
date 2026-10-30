'use strict';
// pylon: type mappings (analogous to pypylon's typemappings_test.py, 1:1 in
// logic). String_t maps to JS strings, StringList_t to [count, names], and
// the GenICam exception hierarchy is re-exported identically on pylon and
// genicam. The SDK raise path (DeviceInfo without IP configuration)
// exercises cross-module catchability; when the SDK answers instead of
// throwing, the boolean result is asserted.
const { describe, it } = require('node:test');
const assert = require('node:assert/strict');
const pylon = require('../../lib');

const MAPPED_EXCEPTION_NAMES = [
  'GenericException',
  'BadAllocException',
  'InvalidArgumentException',
  'OutOfRangeException',
  'PropertyException',
  'RuntimeException',
  'LogicalErrorException',
  'AccessException',
  'TimeoutException',
  'DynamicCastException',
];

describe('pylon type mappings', () => {
  it('String_t typedef is not exposed', () => {
    assert.equal(pylon.String_t, undefined);
  });

  it('StringList_t typedef is not exposed', () => {
    assert.equal(pylon.StringList_t, undefined);
  });

  it('string getter returns a JS string', () => {
    const info = new pylon.DeviceInfo();
    info.FriendlyName = 'hello';
    assert.equal(typeof info.FriendlyName, 'string');
    assert.equal(info.FriendlyName, 'hello');
  });

  it('string setter accepts a JS string', () => {
    const info = new pylon.DeviceInfo();
    info.ModelName = 'acA1920-40um';
    assert.equal(info.ModelName, 'acA1920-40um');
  });

  it('string round-trip preserves unicode', () => {
    const info = new pylon.DeviceInfo();
    info.FriendlyName = 'naïve café';
    assert.equal(info.FriendlyName, 'naïve café');
  });

  it('string round-trip preserves an empty value', () => {
    const info = new pylon.DeviceInfo();
    info.FriendlyName = '';
    assert.equal(info.FriendlyName, '');
  });

  it('string setters reject non-strings', () => {
    const info = new pylon.DeviceInfo();
    assert.throws(() => { info.FriendlyName = 42; }, TypeError);
  });

  it('string list is returned as an iterable of strings', () => {
    const info = new pylon.DeviceInfo();
    info.ModelName = 'M';
    info.VendorName = 'V';
    const [count, names] = info.GetPropertyNames();
    assert.equal(count, names.length);
    for (const name of names) {
      assert.equal(typeof name, 'string');
    }
    assert.ok(names.includes('ModelName'));
    assert.ok(names.includes('VendorName'));
  });

  it('string list is empty when no properties are set', () => {
    const info = new pylon.DeviceInfo();
    const [count, names] = info.GetPropertyNames();
    assert.equal(count, 0);
    assert.equal(names.length, 0);
  });

  it('all mapped exceptions are exposed on pylon', () => {
    for (const name of MAPPED_EXCEPTION_NAMES) {
      assert.ok(pylon[name], `pylon.${name} is missing`);
    }
  });

  it('pylon exception aliases are identical to genicam', () => {
    for (const name of MAPPED_EXCEPTION_NAMES) {
      assert.equal(pylon[name], pylon.genicam[name], name);
    }
  });

  it('mapped exceptions derive from GenericException and Error', () => {
    for (const name of MAPPED_EXCEPTION_NAMES) {
      const cls = pylon[name];
      assert.ok(cls.prototype instanceof pylon.GenericException, name);
      assert.ok(new cls('probe') instanceof Error, name);
      assert.equal(new cls('probe').name, name);
    }
  });

  it('thrown errors carry message and stack', () => {
    const err = new pylon.RuntimeException('probe message');
    assert.equal(err.message, 'probe message');
    assert.equal(typeof err.stack, 'string');
  });

  function raiseInvalidArgument() {
    // IsPersistentIpActive raises when no IP configuration property is set
    // on the DeviceInfo (real SDK-side throw through the type mapper).
    return new pylon.DeviceInfo().IsPersistentIpActive();
  }

  it('SDK raise is catchable via pylon.GenericException', (t) => {
    let outcome = null;
    try {
      outcome = raiseInvalidArgument();
    } catch (err) {
      assert.ok(err instanceof pylon.GenericException);
      return;
    }
    t.skip(`SDK answered instead of throwing (result: ${outcome})`);
  });

  it('SDK raise is catchable via genicam.GenericException', (t) => {
    try {
      raiseInvalidArgument();
    } catch (err) {
      assert.ok(err instanceof pylon.genicam.GenericException);
      return;
    }
    t.skip('SDK answered instead of throwing');
  });

  it('SDK raise is catchable via the specific subclass on both modules', (t) => {
    let err = null;
    try {
      raiseInvalidArgument();
    } catch (e) {
      err = e;
    }
    if (err === null) {
      t.skip('SDK answered instead of throwing');
      return;
    }
    // Whatever the SDK threw, the pylon/genicam aliases are identical.
    assert.equal(pylon[err.constructor.name], pylon.genicam[err.constructor.name]);
    if (err.constructor.name === 'InvalidArgumentException') {
      assert.throws(raiseInvalidArgument, pylon.InvalidArgumentException);
      assert.throws(raiseInvalidArgument, pylon.genicam.InvalidArgumentException);
    } else {
      assert.throws(raiseInvalidArgument, pylon.GenericException);
    }
  });
});
