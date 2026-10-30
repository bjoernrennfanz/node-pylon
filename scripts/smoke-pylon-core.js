'use strict';
// Builds and runs the shared React-Native core smoke test
// (react-native/shared/pylon_core_smoke.cpp + pylon_core.cpp) against the
// real pylon SDK and the camera emulator: no stubs, no mocks, no JS.
// Used by the `rn-build` CI job after scripts/compile-pylon-core.js.
// Link/run flags come from build-config.gypi (run
// `node scripts/configure-build.js` first). Runtime needs the emulator
// environment (PYLON_CAMEMU=2 plus the per-OS library paths — same as the
// `unit-tests` job sets).
const { spawnSync } = require('node:child_process');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');

const REPO_ROOT = path.join(__dirname, '..');
const CORE = path.join('react-native', 'shared', 'pylon_core.cpp');
const SMOKE = path.join('react-native', 'shared', 'pylon_core_smoke.cpp');

function run(cmd, args, opts) {
  console.log(`$ ${cmd} ${args.join(' ')}`);
  const res = spawnSync(cmd, args, { stdio: 'inherit', ...opts });
  if (res.status !== 0) {
    console.error(`failed (exit ${res.status}): ${cmd}`);
    process.exit(res.status || 1);
  }
}

function main() {
  const cfg = JSON.parse(
    fs.readFileSync(path.join(REPO_ROOT, 'build-config.gypi'), 'utf8'),
  );
  const includeDirs = cfg.include_dirs || [];
  const libraries = cfg.libraries || [];
  const ldflags = cfg.ldflags || [];
  for (const src of [CORE, SMOKE]) {
    if (!fs.existsSync(path.join(REPO_ROOT, src))) {
      console.error(`missing source: ${src}`);
      process.exit(1);
    }
  }
  const out = path.join(
    os.tmpdir(),
    process.platform === 'win32' ? 'pylon_core_smoke.exe' : 'pylon_core_smoke',
  );
  if (process.platform === 'win32') {
    run(
      'cl',
      [
        '/nologo', '/std:c++17', '/EHsc', '/GR',
        ...includeDirs.map((d) => `/I${d}`),
        CORE, SMOKE,
        ...libraries,
        `/Fe${out}`,
      ],
      { cwd: REPO_ROOT, shell: true },
    );
  } else {
    run(
      'c++',
      [
        '-std=c++17', '-fexceptions', '-frtti',
        ...includeDirs.map((d) => `-I${d}`),
        CORE, SMOKE,
        ...libraries,
        ...ldflags,
        '-o', out,
      ],
      { cwd: REPO_ROOT },
    );
  }
  console.log('--- running core smoke test (real SDK, emulator) ---');
  run(out, [], { env: process.env });
  console.log('core smoke test passed');
}

main();
