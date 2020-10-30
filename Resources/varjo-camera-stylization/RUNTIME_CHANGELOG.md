# Varjo Runtime Changelog

This document lists all changes between the Varjo software stack releases
that affect the expected application behavior (common to apps built on various
SDK versions). CHANGELOG.md documents the changes on the SDK side and any
new supported functionality.

## 2.5.0

- No changes in runtime.

## 2.4.0

Fixed:
- Poses provided through Varjo World now take into account the reset transform,
  i.e. the poses provided are in local space. Developer must use the added
  `varjo_GetTrackingToLocalTransform` function to query the relationship
  between the tracking and local space and apply it to retain the old behavior.

Added:
- 2.4 Varjo software release introduces a tracking reset feature in Varjo Base.
  This feature can change the relationship between the client coordinate space
  (local space) and the tracking space, and effectively is the same as if
  active clients specifically called `varjo_ResetPose`. The transform returned
  via `varjo_GetTrackingToLocalTransform` should be applied to all poses
  coming outside Varjo API to transform them into same coordinate space as the
  poses provided by Varjo API.

## 2.3.0

Fixed:
- Fix a bug that `varjo_SyncProperties` had to be manually called at least
  once before `varjo_GetProperty*` functions returned valid property values
- Add support for D24S8 and D32S8 texture formats for DirectX 12 rendering
  path
- Improve swapchain validation (related to the new error codes)

Removed:
- Support for Varjo World visual markers. See CHANGELOG.md for more information
  about the new object marker API.


## 2.2.0

Fixed:
- Fix a bug in `varjo_StreamFrame::hmdPose`: the value was HMD center view
  matrix instead of world pose. If you need the old functionality, invert
  the value to get the view matrix.

Known issues:
- DirectX 12 rendering path does not support D24S8 and D32S8 depth formats.
  D32 is supported.


## 2.1.0

### Rendering

Changed:

- Improve swapchain config validation. This validation is only enabled if the
  application is built on 2.1 SDK or newer.
- Improve validation of `varjo_SubmitInfoLayers`. New error codes were
  added (see `CHANGELOG.md`). This validation is only enabled if the application
  is built on 2.1 SDK or newer.
- Implement various performance improvements
- Add occlusion mesh support for OpenVR

Fixed:

- Fix tearing and general choppiness of presented frames after screen recording
  was activated and deactivated. Fixing this previously required a restart
  of Varjo stack.
- Fix OpenGL depth decode, which resulted in wrong positional timewarp and
  depth testing behavior
- Fix Vsync behavior when video pass-through is active

### Pose tracking

Added:

- Added 3DoF tracking fallback in case the 6DoF tracking becomes unavailable.

Fixed:

- Usage of tracking plugin verifier tool to set active tracking plugin could
  previously cause rendering performance issues.

### Mixed Reality

Changed:

- Allow calls to MR API when MR capable device is not connected to make it
  possible to stop features on disconnected callback. Starting MR features
  is not allowed when disconnected.
- Improve quality of depth estimation


## 2.0.0

### Rendering

Changed:

- Increase the default occlusion mesh scaling from 1.0 to 1.3 to give
  timewarp some breathing room. Effectively, `varjo_CreateOcclusionMesh`
  returns a similar mesh as before, but with slight expansion.
- Introduce radial clamping to the occlusion mesh edges (i.e. clamp sample
  on the border instead of sampling the occlusion mesh contents). This will
  be enabled if applications use the layer rendering API and pass the
  `varjo_LayerFlag_UsingOcclusionMesh` flag. This flag also ensures that
  6-DOF timewarp is not applied within the occlusion mesh area.
- Force legacy rendering API (defined in `Varjo.h`) to always default to
  opaque rendering to make existing VR applications compatible with features
  introduced in Varjo Base 2.0. Applications which need alpha blending must
  transition to the layer rendering API.
- Change the default blending mode of the layer API to opaque. Applications
  must explicitly pass `varjo_LayerFlag_BlendMode_AlphaBlend` to enable
  (premultiplied) alpha blending.
- Increase maximum supported blended layer count from 4 to 6.
- `varjo_LayerMultiProj::views` can be now filled with just two views for
  stereo rendering. Runtime will automatically split the given views if the
  device has more displays than the provided view count.
- Far plane distance (`farZ`) in `varjo_ViewExtensionDepth` can now be assigned
  to the IEEE-compliant value of double-precision +infinity. This effectively
  sets the far clip plane of the frustum to infinity.

Fixed:

- Fix depth layer extension (`varjo_ViewExtensionDepth`) to properly use the
  values provided via nearZ and farZ parameters, which must be assigned to the
  near and far clipping distances correspondingly. For reversed depth buffer the
  values of nearZ and farZ must be swapped (i.e. nearZ receives the distance to
  the far clip plane and farZ receives the distance to the near clip plane).
- Fix compositor to stop rendering layers that application does not submit on
  the subsequent frames.
- Fix `varjo_GetViewDescription()` to not require graphics initialization. Correct
  values will be returned immediately after a session has been created.

### Eye tracking

Changed:

- Default gaze calibration (invoked by calling `varjo_RequestGazeCalibration`)
  model has changed to the new Fast model which halves the calibration duration
  and increases robustness to HMD shifts. The old 10 dot calibration sequence
  remains as an API option behind the `varjo_RequestGazeCalibrationWithParameters`
  extension (and may still be a better choice for certain medical conditions).
- Amend reported quality metrics algorithm for Fast calibration based on findings
  from large scale trials.

### Pose tracking

Added:

- Support for Tracking Plugin API which enables support of other HMD pose tracking
  solutions besides Steam.

### Mixed reality

Added:

- Initial support for mixed reality devices
