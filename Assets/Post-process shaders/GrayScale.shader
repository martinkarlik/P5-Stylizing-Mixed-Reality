Shader "Hidden/Shader/GrayScale"
{
    HLSLINCLUDE

    #pragma target 4.5

    #pragma only_renderers d3d11 playstation xboxone vulkan metal switch

    #include "Packages/com.unity.render-pipelines.core/ShaderLibrary/Common.hlsl"

    #include "Packages/com.unity.render-pipelines.core/ShaderLibrary/Color.hlsl"

    #include "Packages/com.unity.render-pipelines.high-definition/Runtime/ShaderLibrary/ShaderVariables.hlsl"

    #include "Packages/com.unity.render-pipelines.high-definition/Runtime/PostProcessing/Shaders/FXAA.hlsl"

    #include "Packages/com.unity.render-pipelines.high-definition/Runtime/PostProcessing/Shaders/RTUpscale.hlsl"

    struct VertexIntput {

        uint vertexID : SV_VertexID;

        UNITY_VERTEX_INPUT_INSTANCE_ID

    };

    struct VertexOutput {

        float4 positionCS : SV_POSITION;

        float2 texcoord   : TEXCOORD0;

        UNITY_VERTEX_OUTPUT_STEREO

    };

    // Type and name, so VertexOutput is the name of the structure, Vert is the name of the function
    VertexOutput Vert(VertexIntput input) {

        VertexOutput output;

        UNITY_SETUP_INSTANCE_ID(input);

        UNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(output);

        output.positionCS = GetFullScreenTriangleVertexPosition(input.vertexID);

        output.texcoord = GetFullScreenTriangleTexCoord(input.vertexID);

        return output;

    }

    
    float3 WaterColor(uint2 positionSS, int radius) {

        float3 outColor = LOAD_TEXTURE2D_X(_InputTexture, positionSS).rgb;

        float3 matTR = float3(0.0f, 0.0f, 0.0f);
        float3 matTR2 = float3(0.0f, 0.0f, 0.0f);
        float3 matTL = float3(0.0f, 0.0f, 0.0f);
        float3 matTL2 = float3(0.0f, 0.0f, 0.0f);
        float3 matBL = float3(0.0f, 0.0f, 0.0f);
        float3 matBL2 = float3(0.0f, 0.0f, 0.0f);
        float3 matBR = float3(0.0f, 0.0f, 0.0f);
        float3 matBR2 = float3(0.0f, 0.0f, 0.0f);

        float3 pixel_sum = float3(0.0f, 0.0f, 0.0f);
        int n = (radius + 1) * (radius + 1);


        for (int ii = -radius; ii <= 0; ii++) {
            for (int jj = -radius; jj <= 0; jj++) {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, int2(positionSS.x + ii, positionSS.y + jj)).rgb;
                matTL += pixel;
                matTL2 += pixel * pixel;
            }
        }

        for (int ii = -radius; ii <= 0; ii++) {
            for (int jj = 0; jj <= radius; jj++) {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, int2(positionSS.x + ii, positionSS.y + jj)).rgb;
                matTR += pixel;
                matTR2 += pixel * pixel;
            }
        }

        for (int ii = 0; ii <= radius; ii++) {
            for (int jj = 0; jj <= radius; jj++) {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, int2(positionSS.x + ii, positionSS.y + jj)).rgb;
                matBL += pixel;
                matBL2 += pixel * pixel;
            }
        }

        for (int ii = 0; ii <= radius; ii++) {
            for (int jj = -radius; jj <= 0; jj++) {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, int2(positionSS.x + ii, positionSS.y + jj)).rgb;
                matBR += pixel;
                matBR2 += pixel * pixel;
            }
        }

        float min_sigma2 = 100.0f;
        matTL /= n;
        matTL2 = abs(matTL2 / n - matTL * matTL);
        float sigma2 = matTL2.r + matTL2.g + matTL2.b;
        if (sigma2 < min_sigma2) {
            min_sigma2 = sigma2;
            outColor = matTL;
        }

        matTR /= n;
        matTR2 = abs(matTR2 / n - matTR * matTR);
        sigma2 = matTR2.r + matTR2.g + matTR2.b;
        if (sigma2 < min_sigma2) {
            min_sigma2 = sigma2;
            outColor = matTR;
        }

        matBR /= n;
        matBR2 = abs(matBR2 / n - matBR * matBR);
        sigma2 = matBR2.r + matBR2.g + matBR2.b;
        if (sigma2 < min_sigma2) {
            min_sigma2 = sigma2;
            outColor = matBR;
        }

        matBL /= n;
        matBL2 = abs(matBL2 / n - matBL * matBL);
        sigma2 = matBL2.r + matBL2.g + matBL2.b;
        if (sigma2 < min_sigma2) {
            min_sigma2 = sigma2;
            outColor = matBL;
        }

        return outColor;
    }

    float3 Outline(TEXTURE2D_X(_InputTexture), uint2 positionSS, float3 outColor, int _LineStrength, int radius){
        float3x3 gx = float3x3(
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
        );

        float3x3 gy = float3x3(
            -1, -2, -1,
            0, 0, 0,
            1, 2, 1
        );

        float pixel_sum_x = 0.0;
        float pixel_sum_y = 0.0;
        for (int y = -1; y < 2; y++) {
            for (int x = -1; x < 2; x++) {

                
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).xyz; 

                float grayscale_value = (pixel.r + pixel.g + pixel.b) / 3;
            
                pixel_sum_x += grayscale_value * gx[y + 1][x + 1];
                pixel_sum_y += grayscale_value * gy[y + 1][x + 1];
            }
        }
        float value = 20 - _LineStrength * 19;
        float edge_value = abs(pixel_sum_x / value) + abs(pixel_sum_y / value);
        float3 outputColor = outColor;

        if (edge_value > 0.05){
            return outputColor = float3(0,0,0);
        }
        return outColor;
    }

    float3 Sketch(TEXTURE2D_X(_InputTexture), uint2 positionSS){
        float3 W = float3(0.2125, 0.7154, 0.0721);
        float2 stp0 = float2(1.0 / _ScreenSize[1], 0.0);
        float2 st0p = float2(0.0, 1.0 / _ScreenSize[0]);
        float2 stpp = float2(1.0 / _ScreenSize[1], 1.0 / _ScreenSize[0]);
        float2 stpm = float2(1.0 / _ScreenSize[1], -1.0 / _ScreenSize[0]);

        float im1m1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy - stpp)).rgb, W);
        float ip1p1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy + stpp)).rgb, W);
        float im1p1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy - stpm)).rgb, W);
        float ip1m1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy + stpm)).rgb, W);
        float im10 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy - stp0)).rgb, W);
        float ip10 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy + stp0)).rgb, W);
        float i0m1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy - st0p)).rgb, W);
        float i0p1 = dot( LOAD_TEXTURE2D_X(_InputTexture, uint2(positionSS.xy + st0p)).rgb, W);
        float h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
        float v = -im1m1 - 2.0 * im10 - im1p1 + ip1m1 + 2.0 * ip10 + ip1p1;

        float mag = 1.0 - length(float2(h, v));
        float3 target = float3(mag, mag, mag);
        return target;
    }

    float3 Blur(TEXTURE2D_X(_InputTexture), uint2 positionSS, int radius){
        float3 pixel_sum = float3(0,0,0);
        int n = (radius*2+1)*(radius*2+1);
        for (int yy = -radius; yy < radius; yy++) {
            for (int xx = -radius; xx < radius; xx++) {

                pixel_sum += LOAD_TEXTURE2D_X(_InputTexture, int2(positionSS.x + yy,positionSS.y + xx)).xyz; 
            }
        }
        return pixel_sum/n;
    }

    // List of properties to control your post process effect

    float _Intensity;

    float _LineStrength;

    int _Radius;

    TEXTURE2D_X(_InputTexture);

    float4 CustomPostProcess(VertexOutput input) : SV_Target {

        UNITY_SETUP_STEREO_EYE_INDEX_POST_VERTEX(input);

        uint2 positionSS = input.texcoord * _ScreenSize.xy;

        float3 outColor = LOAD_TEXTURE2D_X(_InputTexture, positionSS).xyz;

        float3 outputColor = outColor;

        // Blur
        float3 blurColor = Blur(_InputTexture, positionSS, _Radius);

        // for water color
        float3 waterColor = WaterColor(positionSS, _Radius);

        // for sketch color
        float3 sketchColor = Sketch(_InputTexture, positionSS);

        // edge detection
        float3 linecolor = Outline(_InputTexture, positionSS, outColor, _LineStrength, _Radius);


        return float4(blurColor, 1);
        // return float4(watercolor(positionSS, outColor),1);
    }


    ENDHLSL

    SubShader {

        Pass {

            Name "GrayScale"

            ZWrite Off

            ZTest Always

            Blend Off

            Cull Off

            HLSLPROGRAM

                #pragma fragment CustomPostProcess

                #pragma vertex Vert

            ENDHLSL }
            
        }

    Fallback Off

}