// Copyright 2020 Varjo Technologies Oy. All rights reserved.
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "PostProcess.hpp"

// This is example shader for showcasing how to use video post process filters from
// your own application. In your application, implement your own shader that suits
// your needs.

//! Post process constant buffer. Must match with the shader exactly!
struct PostProcessConstantBuffer {

    int clusterSize = 0;
    float outlineIntensity = 0.0f;

    int watercolorRadius = 0;
    float sketchIntensity = 0.0f;

    float pointilismStep = 0.0f;
    float pointilismThreshold = 0.0f;

    int grayscale = 0;
    int puzzle = 0;
    float mirrorMatrix[2] = {0.0f, 0.0f};
    
    float _padding0[2];
 
};

// Shader parameters
static const VarjoExamples::PostProcess::ShaderParams c_postProcessShaderParams = {  //
    8,                                                                               // Block size
    3,                                                                               // Sampling margin
    sizeof(PostProcessConstantBuffer),                                               // Size of constant buffer
    {
        // Texture size and format. Enable one of these for testing the format.
        {256, 256, varjo_TextureFormat_R8G8B8A8_UNORM},
        //{256, 256, varjo_TextureFormat_R8G8B8A8_SRGB},
        //{256, 256, varjo_TextureFormat_R32_UINT},
        //{256, 256, varjo_TextureFormat_R32_FLOAT},
        //{256, 256, varjo_MaskTextureFormat_A8_UNORM},

        // | Varjo Format   | D3D11     | GL        | D3D12     |
        // |----------------|-----------|-----------|-----------|
        // | R8G8B8A8_UNORM | CPU + GPU | CPU + GPU | CPU + ??? |
        // | R8G8B8A8_SRGB  | CPU + --- | CPU + --- | CPU + ??? | (Binding sRGB directly for compute shader not working.)
        // | R32_UINT       | CPU + GPU | CPU + GPU | CPU + ??? |
        // | R32_FLOAT      | CPU + GPU | CPU + --- | CPU + ??? | (GL compute does not output this for unknown reason.)
        // | A8_UNORM       | CPU + GPU | CPU + --- | CPU + ??? | (GL compute does not output this for unknown reason.)
        // |----------------|-----------|-----------|-----------|

        // NOTICE:
        // 1) Set DEBUG_TEXTURE_OUT in HLSL shader to see the raw texture.
        // 2) GPU support for D3D11 and GL is now tested using compute shaders. This might limit support.
        //    It is possible that some of the formats could be used directly as render targets for normal
        //    geometry rendering, or with different shader texture bindings, or using intermediate texture
        //    or texture view. Highly dependent on rendering API as well.
        // 3) D3D12 GPU support in the example not implemented so marked ???. Should be the same as D3D11.
    }};

// Shader source filenames
const std::unordered_map<VarjoExamples::PostProcess::ShaderSource, std::string> c_postProcessShaderSources = {
    {VarjoExamples::PostProcess::ShaderSource::None, ""},
    {VarjoExamples::PostProcess::ShaderSource::Binary, "vstPostProcess.cso"},
    {VarjoExamples::PostProcess::ShaderSource::Source, "vstPostProcess.hlsl"},
};