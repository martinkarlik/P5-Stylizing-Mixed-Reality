#ifndef VARJO_TYPES_MR_EXPERIMENTAL_H
#define VARJO_TYPES_MR_EXPERIMENTAL_H

#include "Varjo_types.h"
#include "Varjo_types_mr.h"

/*
 * *************** WARNING / DISCLAIMER ******************
 * Using these values is not guaranteed to work in the future. Please consult the MR team before using any of these.
 */

#if defined __cplusplus
extern "C" {
#endif

// =========================================================
// ============= Video Post Process Shader API =============
// =========================================================

static const varjo_LockType varjo_LockType_VideoPostProcessShader = 3;  //!< Lock for video-see-through post processing shader.

/**
 * Post process shader type constants.
 */
typedef int64_t varjo_ShaderType;
static const varjo_ShaderType varjo_ShaderType_VideoPostProcess = 1;

/**
 * Post process shader format constants.
 */
typedef int64_t varjo_ShaderFormat;
static const varjo_ShaderFormat varjo_ShaderFormat_None = 0;
static const varjo_ShaderFormat varjo_ShaderFormat_DxComputeBlob = 1;

/**
 * Post process shader input layout versioning for shader types.
 */
typedef int64_t varjo_ShaderInputLayout;
/*
 * This layout has these built-in inputs and outputs:
 * - Input RGBA texture that has the current video see through image.
 * - Output RGBA texture that should be written from the shader.
 * - Constant buffer with the following data:
 *       - Texture dimensions.
 *       - Source texture timestamp.
 *       - Currently rendered view index: 0 Left context, 1 Right Context,
 *         2 Left focus, 3 Right focus.
 *       - Destination rect. Area to render during a particular call to the shader.
 *         Source texture is guarateed to have have up-to-date data on this area.
 * - Linear clamping texture sampler
 * - Linear wrapping texture sampler
 *
 * HLSL register definitions:
 *
 * // Input buffer
 * Texture2D<float4> inputTex : register(t0);
 * // Output buffer
 * RWTexture2D<float4> outputTex : register(u0);
 * // Varjo generic constants
 * cbuffer ConstantBuffer : register(b0)
 * {
 *     int2 textureSize;   // Texture dimensions
 *     float sourceTime;   // Source texture timestamp
 *     int viewIndex;      // View to be rendered: 0=LC, 1=RC, 2=LF, 3=RF
 *     int4 destRect;      // Destination rectangle: x, y, w, h
 * }
 *
 * // Varjo generic texture samplers
 * SamplerState SamplerLinearClamp : register(s0);
 * SamplerState SamplerLinearWrap : register(s1);
 */
static const varjo_ShaderInputLayout varjo_ShaderInputLayout_VideoPostProcess_V1 = 1;

/**
 * Post process shader input flags.
 */
typedef int64_t varjo_ShaderFlags_VideoPostProcess;
static const varjo_ShaderFlags_VideoPostProcess varjo_ShaderFlag_VideoPostProcess_None = 0;

struct varjo_TextureConfig {
    varjo_TextureFormat format;
    uint64_t width;
    uint64_t height;
};

/**
 * Post process shader info structure defining shader properties and inputs
 * for video post process shader.
 */
struct varjo_ShaderParams_VideoPostProcess {
    varjo_ShaderFlags_VideoPostProcess inputFlags;  //!< Shader input flags.
    int64_t computeBlockSize;                       //!< Compute shader block size. Valid values: 8 or 16.
    int64_t samplingMargin;                         //!< Amount of lines left as margin for sampling kernel. Valid values [0, 64].
    int64_t constantBufferSize;                     //!< Constant buffer size in bytes. Must be divisible by 16.
    struct varjo_TextureConfig textures[16];        //!< Input texture configurations.
};

/**
 * Wrapper for different types of shader parameters.
 */
union varjo_ShaderParams {
    struct varjo_ShaderParams_VideoPostProcess videoPostProcess;  //!< Parameters for varjo_ShaderType_VideoPostProcess.
    int64_t reserved[128];                                        //!< Reserved for future use.
};

/**
 * Shader configuration structure.
 */
struct varjo_ShaderConfig {
    varjo_ShaderFormat format;            //!< Shader format.
    varjo_ShaderInputLayout inputLayout;  //!< Shader input layout version.
    union varjo_ShaderParams params;      //!< Shader parameters.
};

/*
 * DXGI texture format type. This is DXGI_FORMAT type, but we don't want to include dxghi.h for this here.
 */
typedef uint32_t varjo_DXGITextureFormat;

/*
 * OpenGL texture format structure. Formats are GLenum type, but we don't want to include GL.h for this here.
 */
struct varjo_GLTextureFormat {
    uint32_t baseFormat;      //!< Base texture format (GLenum type)
    uint32_t internalFormat;  //!< Internal texture format (GLenum type)
};

/**
 * Shader error codes.
 */
static const varjo_Error varjo_Error_InvalidShaderType = 10100;
static const varjo_Error varjo_Error_InvalidShaderFormat = 10101;
static const varjo_Error varjo_Error_InvalidInputLayout = 10102;
static const varjo_Error varjo_Error_InvalidComputeBlockSize = 10103;
static const varjo_Error varjo_Error_InvalidSamplingMargin = 10104;
static const varjo_Error varjo_Error_InvalidConstantBufferSize = 10105;
static const varjo_Error varjo_Error_InvalidTextureDimensions = 10106;
static const varjo_Error varjo_Error_InvalidTextureFormat = 10107;
static const varjo_Error varjo_Error_InvalidTextureIndex = 10108;
static const varjo_Error varjo_Error_TextureNotAcquired = 10109;
static const varjo_Error varjo_Error_TexturesLocked = 10110;
static const varjo_Error varjo_Error_InvalidConstantBuffer = 10111;
static const varjo_Error varjo_Error_RenderAPINotInitialized = 10112;
static const varjo_Error varjo_Error_InvalidShaderFlags = 10113;
static const varjo_Error varjo_Error_InvalidShaderSize = 10114;
static const varjo_Error varjo_Error_InvalidShader = 10115;
static const varjo_Error varjo_Error_InvalidIndexCount = 10116;
static const varjo_Error varjo_Error_TextureLockFailed = 10117;

#if defined __cplusplus
}
#endif

#endif  // VARJO_TYPES_MR_EXPERIMENTAL_H
