'use strict';
// Compile-only check for the shared React-Native core
// (react-native/shared/pylon_core.cpp). Used by the `rn-build` CI job to
// prove the core compiles on every OS against the real pylon SDK without
// building the full Node addon: `-fsyntax-only` on POSIX, `cl /c` on
// Windows. Include dirs come from build-config.gypi (run
// `node scripts/configure-build.js` first, like the npm scripts do).
const { spawnSync } = require('node:child_process');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

const SRC = path.join('react-native', 'shared', 'pylon_core.cpp');

function main() {
  const cfg = JSON.parse(fs.readFileSync('build-config.gypi', 'utf8'));
  const includeDirs = cfg.include_dirs || [];
  if (!fs.existsSync(SRC)) {
    console.error(`missing source: ${SRC}`);
    process.exit(1);
  }
  let cmd;
  let args;
  if (process.platform === 'win32') {
    cmd = 'cl';
    args = [
      '/nologo', '/std:c++17', '/EHsc', '/GR', '/c',
      ...includeDirs.map((d) => `/I${d}`),
      SRC,
      `/Fo${path.join(os.tmpdir(), 'pylon_core.obj')}`,
    ];
  } else {
    cmd = 'c++';
    args = [
      '-std=c++17', '-fsyntax-only',
      ...includeDirs.map((d) => `-I${d}`),
      SRC,
    ];
  }
  console.log(`compiling ${SRC} (${process.platform})`);
  console.log(`$ ${cmd} ${args.join(' ')}`);
  const res = spawnSync(cmd, args, {
    stdio: 'inherit',
    shell: process.platform === 'win32',
  });
  if (res.status !== 0) {
    console.error(`pylon_core compile check failed (exit ${res.status})`);
    process.exit(res.status || 1);
  }
  console.log('pylon_core compile check passed');
}

main();
