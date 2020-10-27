// Copyright 2020 Varjo Technologies Oy. All rights reserved.

// This is example shader for showcasing how to use video post process filters from
// your own application. In your application, implement your own shader that suits
// your needs.

// Compute shader thread block size
#define BLOCK_SIZE (8)

// Debug texture output modes
#define DEBUG_TEXTURE_OUT_NONE (0)
#define DEBUG_TEXTURE_OUT_RGB (1)
#define DEBUG_TEXTURE_OUT_RG_A (2)

// TODO: We could get this mode in constant buffer to be able to switch runtime
#define DEBUG_TEXTURE_OUT (DEBUG_TEXTURE_OUT_NONE)
//#define DEBUG_TEXTURE_OUT (DEBUG_TEXTURE_OUT_RG_A)

// -------------------------------------------------------------------------

// Shader input layout V1 built-in bindings

// Input buffers: t0 = Camera image input
Texture2D<float4> inputTex : register(t0);  // NOTICE! This is sRGBA data bound as RGBA so fetched values are in screen gamma.

// Output buffers: u0 = Camera image output
RWTexture2D<float4> outputTex : register(u0);

// Texture samplers
SamplerState SamplerLinearClamp : register(s0);
SamplerState SamplerLinearWrap : register(s1);

// Varjo generic constants
cbuffer ConstantBuffer : register(b0)
{
    int2 sourceSize;   // Source texture dimensions
    float sourceTime;  // Source texture timestamp
    int viewIndex;     // View to be rendered: 0=LC, 1=RC, 2=LF, 3=RF
    int4 destRect;     // Destination rectangle: x, y, w, h
}

// -------------------------------------------------------------------------

// Shader specific constants
cbuffer ConstantBuffer : register(b1)
{
    // Color grading
    float colorFactor;             // Color grading factor: 0=off, 1=full
    float colorPreserveSaturated;  // Color grading saturated preservation
    float2 _padding_b1_0;          // Padding
    float4 colorValue;             // Color grading value
    float4 colorExp;               // Color grading exponent

    // Noise texture
    float noiseAmount;  // Noise amount: 0=off, 1=full
    float noiseScale;   // Noise scale

    // Blur filter
    float blurScale;     // Blur kernel scale: 0=off, 1=full
    int blurKernelSize;  // Blur kernel size
}

// Shader specific textures
Texture2D<float4> noiseTexture : register(t1);

// -------------------------------------------------------------------------

static const float Epsilon = 1e-10;

float3 convertRGBtoHCV(in float3 RGB)
{
    float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
    float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
    return float3(H, C, Q.x);
}

// Converts given RGB color to HSV color. RGB = [0..1], HSV = [0..1].
float3 convertRGBtoHSV(in float3 RGB)
{
    float3 HCV = convertRGBtoHCV(RGB);
    float S = HCV.y / (HCV.z + Epsilon);
    return float3(HCV.x, S, HCV.z);
}

// Converts HSV hue channel value to RGB color. H = [0..1].
float3 hueToRGB(in float H)
{
    float R = abs(H * 6.0f - 3.0f) - 1.0f;
    float G = 2.0f - abs(H * 6.0f - 2.0f);
    float B = 2.0f - abs(H * 6.0f - 4.0f);
    return saturate(float3(R, G, B));
}

// Converts given HSV color to RGB color. HSV = [0..1], RGB = [0..1].
float3 hsvToRGB(in float3 HSV)
{
    float3 RGB = hueToRGB(HSV.x);
    return ((RGB - 1.0f) * HSV.y + 1.0f) * HSV.z;
}

// -------------------------------------------------------------------------

// Shader entrypoint
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)] void main(uint3 dispatchThreadID
                                                  : SV_DispatchThreadID) {
    // Calculate thread coordinates
    const int2 thisThread = dispatchThreadID.xy + int2(destRect.xy);
    const float2 uv = float2(thisThread) / sourceSize;

    // Load source sample
    float4 origColor = inputTex.Load(int3(thisThread.xy, 0)).rgba;
    float4 color = origColor;

    // Apply box blur
    if (blurScale > 0.0) {
        // Kernel radius and count
        const int kernelD = blurKernelSize;
        const int kernelR = (kernelD >> 1);
        const int kernelN = (kernelD * kernelD);

        const float2 kernelScale = float2(1.0, 1.0) / sourceSize;
        const float2 kernelOffs = (float2(kernelD, kernelD) * 0.5 - 0.5);

        color = float4(0.0, 0.0, 0.0, 0.0);
        for (int y = 0; y < kernelD; y++) {
            for (int x = 0; x < kernelD; x++) {
                const float2 uvOffs = ((float2(x, y) - kernelOffs) * blurScale) * kernelScale;
                color += inputTex.SampleLevel(SamplerLinearClamp, uv + uvOffs, 0.0, 0.0);
            }
        }
        color /= kernelN;
    }

    // Apply color grading
    if (colorFactor > 0.0) {
        float f = colorFactor;

        if (colorPreserveSaturated > 0.0) {
            float3 hsv = convertRGBtoHSV(color.rgb);
            const float floorS = 0.2;
            const float floorV = 0.2;
            const float expS = 2.0;
            const float expV = 2.0;
            float preserveS = pow(saturate(hsv.y - floorS) / (1.0 - floorS), expS);
            float preserveV = pow(saturate(hsv.z - floorV) / (1.0 - floorV), expV);
            f *= lerp(1.0, 1.0 - saturate(preserveS * preserveV), colorPreserveSaturated);
        }

        color.rgb = lerp(color.rgb, pow(max(0.0, color.rgb), colorExp.rgb) * colorValue.rgb, f);
    }

    // Apply noise texture
    if (noiseAmount > 0.0) {
        float4 noise = noiseTexture.SampleLevel(SamplerLinearWrap, (uv - 0.5) * noiseScale + 0.5, 0.0, 0.0);
        color.rgb += (noise.rgb - 0.5) * noiseAmount;

#if (DEBUG_TEXTURE_OUT == DEBUG_TEXTURE_OUT_RGB)
        // Debug: Write texture out as is
        color.rgb = noise.rgb;
#elif (DEBUG_TEXTURE_OUT == DEBUG_TEXTURE_OUT_RG_A)
        // Debug: Write texture alpha to RGB as is
        color.rgb = float3(noise.r, noise.g, noise.a);
#endif
    }

    // NOTICE! This is written to sRGBA texture so we need to write this in screen gamma.
    // If we linearized input value, we need to gamma correct it back here before write.

    // Write output pixel. Alpha contains chroma key mask if enabled, so we keep original.

    float value = (color.r + color.g + color.b) / 3;
    outputTex[thisThread.xy] = float4(value, value, value, 1.0);
}