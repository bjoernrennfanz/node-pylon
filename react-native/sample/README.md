# RN desktop sample

Sample against the real pylon SDK (requires the native TurboModule;
no fake values without it).

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

`App.tsx`: "list version + devices" (`enumerateDevices()` via the
`pylon_core_*` shared core) plus "grab one frame (device 0)"
(`InstantCamera.grabOne()` via the facade, `AcquisitionMode SingleFrame`).
The grab result shows `width x height`, pixel type, buffer bytes and
timestamp. With a camera or the SDK emulator (`PYLON_CAMEMU=2`) attached,
devices are listed and grabs succeed; otherwise the list is empty and
grab throws (real SDK error, no fake values).
