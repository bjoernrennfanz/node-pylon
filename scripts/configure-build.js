'use strict';
// Generates build-config.gypi (flavor-dependent build settings) before
// node-gyp runs. Plain file generation instead of gyp conditions: this
// gyp version evaluates target conditions before external variables
// (command line, include files, command expansions) are visible, so a
// condition-based flavor switch silently never matches. Generating the
// file keeps the switch explicit and testable.
//
//   node scripts/configure-build.js          -> real SDK 26.09 (default)
//   node scripts/configure-build.js --mock   -> mock opt-in
//   NODE_PYLON_USE_MOCK=1 node scripts/configure-build.js -> mock opt-in
// (CLI flag and env var are equivalent; the flag exists so npm scripts
// work on Windows cmd.exe, which has no VAR=x command prefix.)
//
// Real mode ensures the SDK is present (runs scripts/download-pylon-sdk.sh
// when PYLON_ROOT is missing) and records per-OS include/lib paths with
// the exact 26.09 library names. Mock mode records no SDK paths at all.
// Never prints PYLON_SDK_BASE_URL (handled only by the download script).
const { execFileSync } = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');

const REPO_ROOT = path.join(__dirname, '..');
const OUT_FILE = path.join(REPO_ROOT, 'build-config.gypi');

function fail(msg) {
  console.error(`error: ${msg}`);
  process.exit(1);
}

function isMock() {
  if (process.argv.includes('--mock')) return true;
  if (process.argv.includes('--real')) return false;
  return process.env.NODE_PYLON_USE_MOCK === '1';
}

function runDownloadScript() {
  try {
    execFileSync('bash', [path.join(__dirname, 'download-pylon-sdk.sh')], {
      cwd: REPO_ROOT,
      stdio: 'inherit',
      env: process.env,
    });
  } catch {
    fail('pylon SDK download failed (see above). Provide PYLON_SDK_BASE_URL and retry.');
  }
}

function resolvePylonRoot() {
  if (process.env.PYLON_ROOT && process.env.PYLON_ROOT.length > 0) {
    return process.env.PYLON_ROOT;
  }
  return path.join(REPO_ROOT, '.pylon-sdk');
}

function checkHeaders(pylonRoot, os) {
  const candidates =
    os === 'win'
      ? ['Development/include/pylon/PylonIncludes.h']
      : os === 'mac'
        ? ['Frameworks/pylon.framework/Headers/PylonIncludes.h']
        : ['include/pylon/PylonIncludes.h'];
  for (const rel of candidates) {
    if (fs.existsSync(path.join(pylonRoot, rel))) return;
  }
  fail(
    `pylon SDK headers not found under ${pylonRoot} (${candidates[0]} missing). ` +
      'Run scripts/download-pylon-sdk.sh (needs PYLON_SDK_BASE_URL) or set PYLON_ROOT.'
  );
}

function realConfig(pylonRoot) {
  // Exact Basler pylon SDK 26.09 library names, verified against all four
  // archives (linux x64/arm64, macOS universal framework, Windows x64).
  const winLib = (name) => `${pylonRoot}/Development/lib/x64/${name}`;
  const base = {
    defines: ['NODE_PYLON_MOCK=0'],
    include_dirs: [],
    libraries: [],
  };
  addExceptionFlags(base);
  if (process.platform === 'win32') {
    base.include_dirs.push(`${pylonRoot}/Development/include`);
    base.libraries.push(
      winLib('PylonBase_v12.lib'),
      winLib('PylonUtility_v12.lib'),
      winLib('GenApi_MD_VC142_v3_5_Basler_pylon_v1.lib'),
      winLib('GCBase_MD_VC142_v3_5_Basler_pylon_v1.lib')
    );
  } else if (process.platform === 'darwin') {
    // Physical Versions/A paths (not the top-level framework symlinks:
    // symlink restoration is extractor-dependent, the versioned dirs are
    // always real).
    const fwLib = `${pylonRoot}/Frameworks/pylon.framework/Versions/A/Libraries`;
    base.include_dirs.push(`${pylonRoot}/Frameworks/pylon.framework/Versions/A/Headers`);
    base.libraries.push(
      `-L${fwLib}`,
      '-lpylonbase',
      '-lpylonutility',
      '-lGenApi_gcc_v3_5_Basler_pylon_v1',
      '-lGCBase_gcc_v3_5_Basler_pylon_v1'
    );
    // C++ exceptions + RTTI (node-gyp make generator ignores xcode_settings).
    base.ldflags = [`-Wl,-rpath,${fwLib}`];
    base.xcode_settings = {
      OTHER_LDFLAGS: [`-Wl,-rpath,${fwLib}`],
    };
  } else {
    base.include_dirs.push(`${pylonRoot}/include`);
    base.libraries.push(
      `-L${pylonRoot}/lib`,
      '-lpylonbase',
      '-lpylonutility',
      '-lGenApi_gcc_v3_5_Basler_pylon_v1',
      '-lGCBase_gcc_v3_5_Basler_pylon_v1'
    );
    base.ldflags = ['-Wl,-E', `-Wl,-rpath,${pylonRoot}/lib`];
  }
  return base;
}

// C++ exceptions + RTTI for gcc/clang (node-gyp make generator ignores
// xcode_settings and defaults both off; the pylon headers and the mock
// translation units need them). MSVC is covered by binding.gyp.
function addExceptionFlags(config) {
  if (process.platform === 'win32') return;
  config['cflags!'] = ['-fno-exceptions', '-fno-rtti'];
  config['cflags_cc!'] = ['-fno-exceptions', '-fno-rtti'];
  config.cflags_cc = ['-fexceptions', '-frtti'];
}

function main() {
  let config;
  if (isMock()) {
    config = { defines: ['NODE_PYLON_MOCK=1'] };
    addExceptionFlags(config);
    console.log('build flavor: mock (explicit opt-in, no pylon SDK)');
  } else {
    let root = resolvePylonRoot();
    const os = process.platform === 'win32' ? 'win' : process.platform === 'darwin' ? 'mac' : 'linux';
    if (!fs.existsSync(path.join(root, '.pylon-sdk-ready')) && !process.env.PYLON_ROOT) {
      runDownloadScript();
      root = resolvePylonRoot();
    }
    checkHeaders(root, os);
    config = realConfig(root);
    console.log(`build flavor: real pylon SDK 26.09 (PYLON_ROOT=${root})`);
  }
  fs.writeFileSync(OUT_FILE, `${JSON.stringify(config, null, 2)}\n`);
  console.log(`wrote ${path.basename(OUT_FILE)}`);
}

main();
