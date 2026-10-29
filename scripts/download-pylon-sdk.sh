#!/usr/bin/env bash
# Download + extract the Basler pylon SDK (v26.09) for node-pylon.
#
# The SDK source lives behind the secret environment variable
# PYLON_SDK_BASE_URL (local agent secret / GitHub Actions secret of the
# same name). This script appends the exact per-OS archive name and never
# prints the base URL (no leak into logs, docs or commits).
#
# Usage:
#   scripts/download-pylon-sdk.sh [--os linux|macos|windows] [--arch x64|arm64]
#                                 [--out DIR] [--print-root] [--force]
#
# Defaults: host OS/arch auto-detected, OUT=<repo>/.pylon-sdk.
# Prints only the local SDK root directory (safe, no secret).
set -euo pipefail

VERSION="26.09"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DEFAULT_OUT="$REPO_ROOT/.pylon-sdk"

OS_OVERRIDE=""
ARCH_OVERRIDE=""
OUT="$DEFAULT_OUT"
PRINT_ROOT_ONLY=0
FORCE=0

while [ $# -gt 0 ]; do
  case "$1" in
    --os) OS_OVERRIDE="$2"; shift 2 ;;
    --arch) ARCH_OVERRIDE="$2"; shift 2 ;;
    --out) OUT="$2"; shift 2 ;;
    --print-root) PRINT_ROOT_ONLY=1; shift ;;
    --force) FORCE=1; shift ;;
    -h|--help)
      echo "Usage: $(basename "$0") [--os linux|macos|windows] [--arch x64|arm64] [--out DIR] [--print-root] [--force]"
      exit 0
      ;;
    *) echo "error: unknown argument: $1" >&2; exit 2 ;;
  esac
done

fail() { echo "error: $1" >&2; exit 1; }

# The local SDK cache wins over the network: a present SDK is reused
# without requiring the secret (offline rebuilds, repeated CI steps).
# Only a missing cache (or --force) needs PYLON_SDK_BASE_URL.

detect_os() {
  case "$(uname -s)" in
    Linux*) echo "linux" ;;
    Darwin*) echo "macos" ;;
    MINGW*|MSYS*|CYGWIN*|Windows_NT*) echo "windows" ;;
    *) fail "unsupported host OS: $(uname -s) (override with --os)" ;;
  esac
}

detect_arch() {
  case "$(uname -m)" in
    x86_64|amd64) echo "x64" ;;
    arm64|aarch64) echo "arm64" ;;
    *) fail "unsupported host arch: $(uname -m) (override with --arch)" ;;
  esac
}

OS="${OS_OVERRIDE:-$(detect_os)}"
ARCH="${ARCH_OVERRIDE:-$(detect_arch)}"

case "$OS/$ARCH" in
  linux/x64) ARCHIVE="pylon-sdk-26.09-linux_x86_64.tar.gz" ;;
  linux/arm64) ARCHIVE="pylon-sdk-26.09-linux_aarch64.tar.gz" ;;
  macos/*) ARCHIVE="pylon-sdk-26.09-macos_universal.tar.gz" ;;
  windows/x64) ARCHIVE="pylon-sdk-26.09-windows_x86_64.tar.gz" ;;
  *) fail "unsupported OS/arch combination: $OS/$ARCH" ;;
esac

# The base URL is resolved only after the secret check below (set -u safe).
if [ "$PRINT_ROOT_ONLY" = "1" ]; then
  echo "$OUT"
  exit 0
fi

if [ -f "$OUT/.pylon-sdk-ready" ] && [ "$FORCE" != "1" ]; then
  CACHED_ROOT="$OUT"
  if [ "$OS" = "windows" ] && command -v cygpath >/dev/null 2>&1; then
    CACHED_ROOT="$(cygpath -w "$OUT")"
  fi
  echo "pylon SDK already present at $CACHED_ROOT (use --force to re-download)."
  echo "PYLON_ROOT=$CACHED_ROOT"
  if [ -n "${GITHUB_ENV:-}" ]; then
    echo "PYLON_ROOT=$CACHED_ROOT" >> "$GITHUB_ENV"
  fi
  exit 0
fi

# Only a fresh download needs the secret (see header: never echoed).
if [ -z "${PYLON_SDK_BASE_URL:-}" ]; then
  fail "PYLON_SDK_BASE_URL is not set. Provide the pylon SDK base URL via the PYLON_SDK_BASE_URL environment variable (agent secret locally, GitHub Actions secret in CI), then retry."
fi

# Strip a single trailing slash; the archive name carries the version.
BASE_URL="${PYLON_SDK_BASE_URL%/}"

command -v curl >/dev/null 2>&1 || fail "curl is required to download the pylon SDK."
command -v tar >/dev/null 2>&1 || fail "tar is required to extract the pylon SDK."

mkdir -p "$OUT"
TMP_FILE="$(mktemp "${TMPDIR:-/tmp}/pylon-sdk-XXXXXX.tar.gz")"
trap 'rm -f "$TMP_FILE"' EXIT

# NOTE: the URL is intentionally never echoed (secret). curl runs quiet
# with fail-on-HTTP-error so no URL appears in logs.
if ! curl -fsSL --retry 3 -o "$TMP_FILE" "$BASE_URL/$ARCHIVE"; then
  fail "download of the pylon SDK archive failed (HTTP error or network issue). Check PYLON_SDK_BASE_URL and network access, then retry."
fi

tar -xzf "$TMP_FILE" -C "$OUT"
rm -f "$TMP_FILE"

# Sanity check per OS layout (headers must exist after extraction).
case "$OS" in
  linux) [ -f "$OUT/include/pylon/PylonIncludes.h" ] || fail "extraction done but include/pylon/PylonIncludes.h is missing." ;;
  macos) [ -f "$OUT/Frameworks/pylon.framework/Headers/PylonIncludes.h" ] || [ -f "$OUT/Frameworks/pylon.framework/Versions/A/Headers/PylonIncludes.h" ] || fail "extraction done but pylon.framework headers are missing." ;;
  windows) [ -f "$OUT/Development/include/pylon/PylonIncludes.h" ] || fail "extraction done but Development/include/pylon/PylonIncludes.h is missing." ;;
esac

date -u +"%Y-%m-%dT%H:%M:%SZ $ARCHIVE" > "$OUT/.pylon-sdk-ready"

# Export root: on Windows runners prefer a native path for MSVC/node-gyp.
EXPORT_ROOT="$OUT"
if [ "$OS" = "windows" ] && command -v cygpath >/dev/null 2>&1; then
  EXPORT_ROOT="$(cygpath -w "$OUT")"
fi

echo "pylon SDK $VERSION ($OS/$ARCH) ready at $EXPORT_ROOT."
echo "PYLON_ROOT=$EXPORT_ROOT"

# Export for GitHub Actions without ever touching the secret value.
if [ -n "${GITHUB_ENV:-}" ]; then
  echo "PYLON_ROOT=$EXPORT_ROOT" >> "$GITHUB_ENV"
fi
