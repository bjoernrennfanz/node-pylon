'use strict';

// JS-Fassade ueber das native N-API-Modul (analog pypylon Top-Level-API).
// Laedt build/Release/node_pylon.node; wirft mit klarem Hinweis,
// wenn nicht gebaut wurde (npm run rebuild).
let native = null;

function loadNative() {
  if (native) return native;
  try {
    // eslint-disable-next-line import/no-dynamic-require, global-require
    native = require('../build/Release/node_pylon.node');
  } catch (err) {
    const hint = 'Natives Modul nicht gefunden. Baue es mit `npm run rebuild` (Mock ohne Kamera) bzw. siehe README Bauabschnitt.';
    const e = new Error(`${hint} (${err.message})`);
    e.cause = err;
    throw e;
  }
  return native;
}

function version() {
  return loadNative().version();
}

function pylonVersion() {
  return loadNative().pylonVersion();
}

function isMock() {
  return Boolean(loadNative().mock);
}

function enumerateDevices() {
  return loadNative().enumerateDevices();
}

function grabMock(width = 64, height = 48) {
  return loadNative().grabMock(width, height);
}

module.exports = {
  version,
  pylonVersion,
  isMock,
  enumerateDevices,
  grabMock,
};
