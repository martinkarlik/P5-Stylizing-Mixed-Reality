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

    // List of properties to control your post process effect

    float _Intensity;

    TEXTURE2D_X(_InputTexture);

    int radius = 15;

    float3 watercolor (uint2 positionSS, float3 outColor){

        // for water color
        float3 gl_FragColor = outColor;
        // water color effect
        float n = float((radius + 1) * (radius + 1));
        float4x3 mat;
        float4x3 matSquared;
        uint2 uv = positionSS;


        for (int i = 0; i < 4; i++) {
            mat[i] = float3(0.0f, 0.0f, 0.0f);
            matSquared[i] = float3(0.0f, 0.0f, 0.0f);
        }

        for (int y = -radius; y <= 0; y++)  {
            for (int x = -radius; x <= 0; x++)  {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).rgb;
                mat[0] += pixel;
                matSquared[0] += pixel * pixel;
            }
        }

        for (int y = -radius; y <= 0; y++)  {
            for (int x = 0; x <= radius; x++)  {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).rgb;
                mat[1] += pixel;
                matSquared[1] += pixel * pixel;
            }
        }

        for (int y = 0; y <= radius; y++)  {
            for (int x = 0; x <= radius; x++)  {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).rgb;
                mat[2] += pixel;
                matSquared[2] += pixel * pixel;
            }
        }

        for (int y = 0; y <= radius; y++)  {
            for (int x = -radius; x <= 0; x++)  {
                float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).rgb;
                mat[3] += pixel;
                matSquared[3] += pixel * pixel;
            }
        }

        float min_sigma2 = 100.0f;
        for (int i = 0; i < 4; i++) {
            mat[i] /= n;
            matSquared[i] = abs(matSquared[i] / n - mat[i] * mat[i]);

            float sigma2 = matSquared[i].r + matSquared[i].g + matSquared[i].b;
            if (sigma2 < min_sigma2) {
                min_sigma2 = sigma2;
                gl_FragColor.rgb = mat[i];
            }
        }

        return gl_FragColor.rgb;
    }

    float4 CustomPostProcess(VertexOutput input) : SV_Target {

        UNITY_SETUP_STEREO_EYE_INDEX_POST_VERTEX(input);

        uint2 positionSS = input.texcoord * _ScreenSize.xy;

        float3 outColor = LOAD_TEXTURE2D_X(_InputTexture, positionSS).xyz;

        // for water color
        float3 gl_FragColor = outColor;

        // for sketch color
        float4 FragColor;


        // water color 
        


        // sketch
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

        FragColor = float4(lerp(outColor, target, _Intensity), 1.0);

        // edge detection
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

        float edge_value = abs(pixel_sum_x / 3) + abs(pixel_sum_y / 3);
        float3 outputColor = outColor;

        if (edge_value > 0.05){
            outputColor = float3(0,0,0);
        }

        return float4(lerp(outColor, outputColor, _Intensity), 1);
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