# API reference

Source: `lib/index.js`, types: `types/index.d.ts`.

| Function | Signature | Description |
|---|---|---|
| `version()` | `() => string` | Module version |
| `pylonVersion()` | `() => string` | pylon version from the SDK (e.g. `12.5.2.1565`) |
| `initialize()` / `terminate()` | `() => void` | Pylon lifecycle (refcounted; every `initialize()` needs a `terminate()`) |
| `enumerateDevices()` | `() => PylonDeviceInfo[]` | Device infos via TlFactory (empty without camera/emulator) |
| `createFirstCamera()` / `createFirst()` | `() => InstantCamera` | TlFactory.CreateFirstDevice — throws without a detected camera |
| `createCameraAt(i)` | `(number) => InstantCamera` | Attach enumerated device `i` (camera-array support) |
| `TlFactory` | object | `GetInstance()`/`EnumerateDevices()`/`CreateFirstDevice()` (+ lowercase aliases), like pypylon |
 | `InstantCamera` | class | `open(accessMode?)/close`, attach/detach/destroy (`isPylonDeviceAttached/hasOwnership/attachDevice/detachDevice/destroyDevice`), context (`getCameraContext/setCameraContext`), `isCameraDeviceRemoved`, `getQueuedBufferCount`, GenApi nodemap (`get/set*`, `tryMax`, `executeCommand`, `isAvailable`, `getDisplayName/getDescription`, integer/float `Inc/Unit/Representation` + `set*ToMin/Max`, `get/setValueAsString/FromString`, `getStringMaxLength`, enum `get/setEnumIntValue`, `isCommandDone`, `getCategoryFeatures`, `isSelector/getSelectedFeatures/getSelectingFeatures`, register `getRegisterLength/getRegisterAddress/readRegister/writeRegister`, node-change `registerCallback/getCallbackCount/deregisterCallback`), grab loop (`startGrabbing(strategy?/options)/startGrabbingMax/stopGrabbing/isGrabbing/retrieveResult(timeoutMs?, timeoutHandling?)/grabOne/waitForFrameTriggerReady/executeSoftwareTrigger`), event handlers (`registerConfiguration/deregisterConfiguration/registerImageEventHandler/deregisterImageEventHandler/registerCameraEventHandler/deregisterCameraEventHandler`), `grab()` |
| `InstantCameraArray` | class | `initialize(size)/getSize/getItem/iterator`, `open/close/destroyDevice`, `startGrabbing/stopGrabbing/isGrabbing`, `retrieveResult` (adds `cameraIndex`), like `CInstantCameraArray` |
| `AcquireContinuousConfiguration` / `AcquireSingleFrameConfiguration` / `SoftwareTriggerConfiguration` | classes | `static applyConfiguration(camera)` + `onOpened(camera)` (register via `registerConfiguration((ev) => cfg.onOpened(camera))`) |
| `ConfigurationHelper` | object | `disableAllTriggers/disableCompression/disableGenDC/selectRangeComponent/probePacketSize(camera)` |
| `GrabStrategy` / `TimeoutHandling` / `GrabLoop` / `RegistrationMode` / `Cleanup` / `AccessMode` | enum objects | Numeric values mirroring the C++ enums (`GrabStrategy.OneByOne = 0` …); native methods also accept the string names |
| `createAndOpenCamera()` | `() => InstantCamera` | Short form: creates + opens a camera |

`PylonDeviceInfo`: `{ friendlyName, modelName, serialNumber, vendorName, deviceClass, fullName?, [key: string]: unknown }`.
`retrieveResult(timeoutMs, timeoutHandling)` returns `null` on timeout with
`TimeoutHandling.Return` (default), throws on timeout with
`TimeoutHandling.ThrowException`.
Grab results are unique plain objects: `{ isValid, succeeded, grabSucceeded(),
width, height, pixelType, pixelFormat, payloadSize, buffer, blockId,
frameNumber, timestamp, errorCode?, errorDescription? }`.
Event handlers are synchronous JS callbacks: configuration handlers receive
lifecycle names (`OnOpened/OnClosed/OnGrabStarted/OnGrabStopped/OnAttached/
OnDetached/OnDestroyed`), image handlers receive each retrieved result.
`register*` returns a numeric id; `deregister*` accepts the id or the
function and returns whether it was removed.
`open(accessMode?)` validates `Control/Stream/Event/Exclusive` (or `1/3/4/5`)
and opens with the default mode (`CInstantCamera::Open()` takes no mode).
Camera-device event dispatch (node-specific callbacks from the event-grab
thread) is not wired yet: `registerCameraEventHandler` stores the
registration for lifecycle parity; delivery via a ThreadSafeFunction polling
thread is a follow-up.

## Image pipeline APIs (grab result, image, converter, decompressor)

| Function | Signature | Description |
|---|---|---|
| `camera.retrieveGrabResult(timeoutMs?, timeoutHandling?)` | `(number, TimeoutHandlingValue?) => GrabResult \| null` | Full `CGrabResultPtr` wrapper (same acquisition as `retrieveResult`, fires image handlers); `null` on timeout with `Return` |
| `GrabResult` | class | `isValid/isUnique/release`, `succeeded/grabSucceeded`, geometry (`getWidth/getHeight/getOffsetX/getOffsetY/getPaddingX/getPaddingY`), `getPixelType/getPixelTypeValue/getPayloadType`, sizes (`getPayloadSize/getBufferSize/getImageSize`), `getTimestamp/getBlockId/getId/getImageNumber/getSkippedImages`, `getErrorCode/getErrorDescription`, `isChunkDataAvailable/hasCrc/checkCrc/getStride`, zero-copy `getBuffer/getArray/getMemoryView` over the grab buffer (shared ownership, no copy), GenDC `getDataComponentCount/getDataComponent/getDataComponentBuffer`, `toObject`; plain fields (`succeeded`, `width`, `pixelType`, `buffer`, …) are also set for property access |
| `PylonImage` | class | `static create(pixelType, width, height, paddingX?)`, `isValid/isUnique/release`, `copyImage(PylonImage\|GrabResult)`, `attachGrabResult(grab)`, `getWidth/getHeight/getPaddingX/getOrientation/getPixelType/getPixelTypeValue/getImageSize/getAllocatedBufferSize/getStride/isSupportedPixelType`, zero-copy `getBuffer/getArray/getMemoryView`, `save(format, file)/load(file)/canSaveWithoutConversion(format)` via `CImagePersistence`, `toObject` |
| `ImageFormatConverter` | class | `setOutputPixelFormat/getOutputPixelFormat`, `setOutputBitAlignment('LsbAligned'│'MsbAligned')/getOutputBitAlignment`, `convert(dst, src)`, `getBufferSizeForConversion(src)`, `imageHasDestinationFormat(src)`, `static isSupportedInputFormat/isSupportedOutputFormat`; sources are `PylonImage` or `GrabResult` (grab buffers attach without a pixel copy) |
| `ImageDecompressor` | class | `describe(src) → CompressionInfo`, `decompress(dst, src)`, `static getCompressionMode()` (`'Off'` without a compression-enabled nodemap) |
| `PixelType` / `PayloadType` | enum objects | Numeric `EPixelType`/`EPayloadType` values from the SDK (`PixelType.Mono8`, `PayloadType.Image`, …) |
| `getPixelTypeByName/getNameByPixelType/isMono/isColor/isPacked` | functions | `CPixelTypeMapper` name lookup plus `IsMono/IsColorImage/IsPacked` predicates; accept SFNC names, short aliases (`Mono8`, `RGB8`) or numeric values |
| `ImageFileFormat` | object | `'Bmp'/'Tiff'/'Jpeg'/'Png'/'Raw'/'Dng'` for `PylonImage.save/load` |

`CompressionInfo`: `{ hasCompressedImage, lossy, width, height, offsetX, offsetY, decompressedImageSize, decompressedPayloadSize, compressionStatus }`.
`DataComponentInfo`: `{ index, componentIndex, componentType, componentTypeName, width, height, offsetX, offsetY, pixelType, pixelTypeValue, dataSize, timestamp, sourceId }`.
DataProcessing recipes stay out of scope (MVP).

## Infra APIs (buffer factory, wait objects, feature persistence, defect pixels, type mappings)

| Function | Signature | Description |
|---|---|---|
| `BufferFactory` | class | malloc-backed `IBufferFactory` with `allocationCount()/freeCount()/lastAllocationSize()/isReleased()`; one attach per instance |
| `camera.setBufferFactory(f)` / `getBufferFactory()` | `(BufferFactory\|null) => void` / `() => BufferFactory\|null` | Attach/detach on the grab engine (`Cleanup_Delete`, deferred destroy while buffers are outstanding); holdable results from `retrieveGrabResult()` carry `getBufferContext()/getBufferFactory()` provenance (context id, or `null` without a factory) |
| `WaitObject` / `WaitObjects` | classes | `isValid/wait(timeoutMs)/waitEx(timeoutMs, alertable?)/static sleep(ms)`; `add/removeAll/waitForAll/waitForAny/waitForAllEx/waitForAnyEx` (all sync, ms timeouts; `withIndex=true` yields `[result, index]`); camera `getGrabResultWaitObject()/getGrabStopWaitObject()` |
| `WaitExResult` / `waitex_*` / `waitForever` | enum object + constants | `Timeout 0 / Signaled 1 / Abandoned 2 / Alerted -1`; `waitForever 0xFFFFFFFF` |
| `FeaturePersistence` | object | `save/load(file, camera, validate?)/saveToString/loadFromString` against the open nodemap (+ `PascalCase` aliases) |
| `StaticDefectPixelCorrection` | object | `get/setDefectPixelList(camera, pixels, listType?)/normalizePixelList(camera, pixels)` → `[success, StaticDefectPixelVector]`; `ListType_Factory 0 / ListType_User 1`; entries are `[x, y] / [x, y, type]` or `{x, y, type?}` (validated: `TypeError`/`RangeError`; SDK failures surface as `RuntimeException`, e.g. without user-list support) |
| `StaticDefectPixel` / `StaticDefectPixelVector` | classes | `X/Y/Type` struct (+ lowercase aliases); `length/size/capacity/empty/reserve/resize/clear/swap/at/push/toArray` + `Symbol.iterator` |
| `StaticDefectPixelType_Reserved` | `0` | Pixel-type constant |
| `DeviceInfo` | class | Settable `FriendlyName/ModelName/VendorName/SerialNumber/DeviceClass/FullName` (UTF-8 strings), `GetPropertyNames()` → `[count, names]`, `IsPersistentIpActive()` |
| `GenericException` + 9 subclasses / `genicam` | classes / namespace | Identical class objects on both (`pylon.X === pylon.genicam.X`); SDK throws are translated by dynamic type (`TimeoutException` on grab timeouts, `InvalidArgumentException` from `IsPersistentIpActive` without IP config, …). Node-accessor guards keep their plain `Error/TypeError/RangeError` shapes. |

Deviations from pypylon: JS subclass overrides of `BufferFactory` (`AllocateBuffer` returning custom context objects) are not supported — the native factory assigns numeric context ids. Empty `StaticDefectPixelVector` is truthy (all JS objects are); use `empty()`. The pypylon background-thread wait sections run sequentially (a synchronous wait blocks the JS loop, so no timer could fire mid-wait); the asserted SDK state transitions are identical.
