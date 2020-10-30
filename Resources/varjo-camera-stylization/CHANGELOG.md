# Varjo SDK Changelog

This file describes changes in Varjo SDK. See RUNTIME_CHANGELOG.md for
changes in the runtime behavior.

## 2.5.0

### Added

- (No additions)

### Changed

- SDK example code refactored
- SDK example vendor library linkage improved.

## 2.4.0

Note that this release introduces a separate Varjo experimental SDK
that can be downloaded from Varjo developer website.

### Added

- New `cameraCalibrationConstant` into `varjo_DistortedColorFrameMetadata`
  to allow clients to transform video pass-through pixels values into
  luminance and vice versa
- `varjo_DistortedColorFrameMetadata` now also contains full white balance
  normalization metadata. This is also used in all example renderers.
- New function `varjo_SessionSetPriority` to allow applications to change
  composition order dynamically. See use in MRExample.
- Velocity vector layer extension (`varjo_ViewExtensionVelocity`) to allow
  clients to submit velocity buffers with color swapchains. The only format
  currently supported is `varjo_VelocityTextureFormat_R8G8B8A8_UINT`.
- Benchmark example: Support for rendering left and right eyes on separate
  GPUs with Opengl and DirectX 12 (enable with `--use-sli`). Support for
  rendering all eye views on a slave gpu (enable with `--use-slave-gpu`).
- MRExample: Key bindings to change client priority and toggle lighting
- Getter (`varjo_GetTrackingtoLocalTransform`) for transform from tracking
  space to local space. Local space can deviate from tracking space when
  client calls `varjo_ResetPose`, or when offset is applied to all clients
  via Varjo Base (new feature in 2.4 software release). Please see Varjo
  developer documentation on http://developer.varjo.com for more details.

### Changed

- `whiteBalanceColorGains` array was moved into a struct. Instead
  of accessing `whiteBalanceColorGains`, one needs to now access
  `wbNormalizationData.whiteBalanceColorGains`, so this change requires
  minor application modification when upgrading SDK.
- Command line parsing in examples now uses cxxopts


## 2.3.0

### Added

- New mixed reality camera property `varjo_CameraPropertyType_Sharpness`
- Generic `varjo_Lock()` and `varjo_Unlock()` functions for exclusively
  locking MR features for the calling session. Related new error codes
  `varjo_Error_AlreadyLocked ` and `varjo_Error_NotLocked`.
- New function `varjo_GetPropertyString` to get a string property
- New function `varjo_GetPropertyStringSize` to get the size of a buffer
  for a string property
- New properties to get the product name (`varjo_PropertyKey_HMDProductName`)
  and serial number (`varjo_PropertyKey_HMDSerialNumber`) of the HMD
- `varjo_RenderAPI_D3D12` that was missing from 2.2 release
- New error codes for rendering data validation:
  - `varjo_Error_InvalidSwapChain`
  - `varjo_Error_D3D12CreateTextureFailed`
  - `varjo_Error_WrongSwapChainTextureFormat`
  - `varjo_Error_InvalidViewExtension`
- Benchmark example: occlusion mesh rendering for all APIs, and
  support for reversed depth (`--reverse-depth`)

### Changed

- Update examples to use the new lock API
- `varjo_GetSupportedTextureFormats` can now return `varjo_Error_InvalidRenderAPI`
- Refactor common example code: headless view and logging macros
- Stabilize Varjo World API (beta in 2.2). This is a breaking change;
  apps built on top of 2.2 VarjoLib will stop working gracefully (i.e. markers
  will not be returned anymore with new 2.3 runtime). The main change is in
  naming as visual markers are now called object markers. Also, the separate
  dynamic and static marker types have been removed; prediction mode similar
  to 2.2 dynamic marker is available to get similar behavior.

### Deprecated

- Old MR feature specific lock/unlock functions deprecated


## 2.2.0

### Added

- DirectX 12 support. New header `Varjo_d3d12.h` and a separate function for
  DX12 swapchain creation in `Varjo_layers.h`.
- DirectX 12 renderer for Benchmark example. Enable with `--renderer=d3d12`.
- API for controlling chroma keying (`Varjo_mr.h` and `Varjo_mr_types.h`),
  a new feature in 2.2 software release
- New example, ChromaKeyExample, for showing the usage of chroma key feature
- Varjo world API (`Varjo_world.h` and `Varjo_types_world.h`). This API allows
  applications to query objects exposed by Varjo stack and the related components
  (such as the pose of an object). This API is currently used for visual markers,
  a new feature in 2.2 software release.
- New example, MarkerExample, for showing the basic usage of Varjo World API and
  visual marker feature

### Changed

- Move reusable classes from `MRExample` to `Common` folder
- Wrap all example classes with `VarjoExamples` namespace


## 2.1.0

### Added

- New error codes `varjo_Error_ValidationFailure`, `varjo_Error_InvalidSwapChainRect`
  and `varjo_Error_ViewDepthExtensionValidationFailure`. These tie with the
  added validation for `varjo_SubmitInfoLayers`.
- Benchmark example app commandline parameter `--depth-format` for setting
  the depth-stencil texture format (d32|d24s8|d32s8)
- View extension (`varjo_ViewExtensionDepthTestRange`) to define ranges for
  which the depth test is active. Outside the given range the layer is alpha-blended
  in layer order without depth testing.
- `varjo_UpdateNearFarPlanes` to replace deprecated `varjo_UpdateClipPlaneDistances`.
  The implementation of this function is in `Varjo_math.h`.

### Changed

- Improve MRExample application
- Update Benchmark to use `varjo_UpdateNearFarPlanes`

### Deprecated

- Legacy rendering API. This includes types in `Varjo_types.h` and
  functions in `Varjo.h`, `Varjo_d3d11.h` and `Varjo_gl.h`. All applications
  need to migrate to Layer API (defined in `Varjo_layers.h`).


## 2.0.0

The main addition in this release is the API to control Varjo mixed
reality devices and the related data streams.

### Added

- Mixed reality API (`Varjo_mr.h` and `Varjo_mr_types.h`) for controlling
  mixed reality devices and cameras
- Mixed reality API related events:
  - `varjo_EventType_MRDeviceStatus`
  - `varjo_EventType_MRCameraPropertyChange`
- Mixed reality API related error codes:
  - `varjo_Error_RequestFailed`
  - `varjo_Error_OperationFailed`
  - `varjo_Error_NotAvailable`
  - `varjo_Error_CapabilityNotAvailable`
  - `varjo_Error_CameraAlreadyLocked`
  - `varjo_Error_CameraNotLocked`
  - `varjo_Error_CameraInvalidPropertyType`
  - `varjo_Error_CameraInvalidPropertyValue`
  - `varjo_Error_CameraInvalidPropertyMode`
- Property key for querying whether a mixed reality capable device is
  currently connected (`varjo_PropertyKey_MRAvailable`)
- Data stream API (`Varjo_datastream.h` and `Varjo_datastream_types.h`) for
  subscribing to color camera and lighting cubemap streams
- Data stream API related events:
  - `varjo_EventType_DataStreamStart`
  - `varjo_EventType_DataStreamStop`
- Data stream API related error codes:
  - `varjo_Error_DataStreamInvalidCallback`
  - `varjo_Error_DataStreamInvalidId`
  - `varjo_Error_DataStreamAlreadyInUse`
  - `varjo_Error_DataStreamNotInUse`
  - `varjo_Error_DataStreamBufferInvalidId`
  - `varjo_Error_DataStreamBufferAlreadyLocked`
  - `varjo_Error_DataStreamBufferNotLocked`
  - `varjo_Error_DataStreamFrameExpired`
  - `varjo_Error_DataStreamDataNotAvailable`
- `varjo_BeginFrameWithLayers` to begin a frame when using the layer
  rendering API. This is effectively the same `varjo_BeginFrame` but
  without the second parameter.
- New parameters for controlling gaze output filter
- Mixed reality example (`MRExample`)
- New options for the benchmark example to run with video see-through

### Removed

- Flag `varjo_LayerFlag_BlendMode_Inherit`. Inheriting has no effect
  anymore since runtime version 2.0, so the flag is obsolete.
- Flag `varjo_LayerFlag_BlendMode_Opaque`. Opaque is the default blending
  mode since runtime version 2.0, so the flag is obsolete. Applications
  utilizing alpha blending need to turn it on using
  `varjo_LayerFlag_BlendMode_AlphaBlend`.

### Changed

- `varjo_LayerMultiProj::views` can be filled with just two views for
  submission of a stereo pair. Varjo compositor will split the image for
  the quad display devices. This is a runtime change, but important enough
  to be documented here as well.
- Rename `varjo_LayerFlag_DepthSorting` to `varjo_LayerFlag_DepthTesting`.
- Examples are now built using CMake. Instructions under `examples/README.txt`.

### Deprecated

- Blending / depth testing related submission flags (`varjo_SubmitFlag_Opaque`,
  `varjo_SubmitFlag_InvertAlpha` and `varjo_SubmitFlag_DepthSorting`). Behavior
  is controlled now for each layer separately.
- `varjo_SubmitInfoLayers::flags`. This field has no effect.


## 1.4.0

### Added

- Experimental layer rendering API (`Varjo_layers.h` and `Varjo_layer_types.h`).
    - NOTE: This will be finalized in the next release and might still be subject
      to small changes. The supplied example uses both rendering APIs. Developer
      documentation will be added in the next release as well.
- `varjo_RequestGazeCalibrationWithParameters` to control the gaze calibration
- `varjo_GetViewCount` to query number of views
- `varjo_Error_GazeInvalidParameter` to denote error of passing invalid
  parameters to `varjo_RequestGazeCalibrationWithParameters`
- `varjo_Error_D3D11CreateTextureFailed`
- Properties for querying gaze calibration quality per eye
  (`varjo_PropertyKey_GazeEyeCalibrationQuality_Left` and
   `varjo_PropertyKey_GazeEyeCalibrationQuality_Right`)
- Property for HMD connection status (`varjo_PropertyKey_HMDConnected`)
- New texture formats `A8_UNORM`, `YUV422`, `RGBA16_FLOAT`, `D24_UNORM_S8_UINT`
  and `D32_FLOAT_S8_UINT`

## 1.3.0

- Add `varjo_EventType_Foreground`
- Improve documentation

## 1.2.0

- Add support for 32-bit applications
- Improve examples
- Native SDK projection matrix is now off-axis to provide improved image quality
    – NOTE: Applications that don’t support off-axis projection should call
      `varjo_SetCenteredProjection(...)` function to enable old behavior.
    – NOTE: Applications built with <1.1 behave as previously.
- Separate error messages for rendering and gaze
- VarjoLib does not require CPU with AVX support anymore
