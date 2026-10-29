# RN desktop sample

Working sample without a camera (mock fallback in `react-native/src/index.ts`).

## Start per platform (out-of-tree)

```bash
# Windows (react-native-windows, C++/WinAppSDK, Fabric)
npx react-native run-windows --arch x64

# macOS (react-native-macos)
npx react-native run-macos

# Linux (community port react-native-linux; evaluation ongoing —
# fallback: Electron app loading build/Release/node_pylon.node)
npx react-native run-linux   # only if the fork registers the platform
```

Metro resolves `NativePylon.windows.js` / `.macos.js` next to
`react-native/specs/NativePylon.js` when platform-specific JS is needed
(`rnpm.haste.platforms`, see `docs/rn-architecture.md`).

## What the sample shows

`App.tsx`: two buttons — "list version + devices" (`enumerateDevices`,
mock: `[]`) and "grab mock" (64x48 Mono8, deterministic gradient, base64).
With a native TurboModule the values come from `pylon_core_*`; without a native
module the JS mock kicks in — the sample runs in both cases.

## Real camera operation

Build the core behind `NODE_PYLON_USE_REAL` against `PYLON_ROOT`, then
`enumerateDevices()` maps to `CTlFactory` and `grabMock()` maps to a
`RetrieveResult` promise via `CallInvoker` (never blocks the JS loop).
