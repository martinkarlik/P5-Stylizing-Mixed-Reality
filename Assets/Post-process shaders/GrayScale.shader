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

    float4 gl_FragColor;

    int radius = 10;

    float4 CustomPostProcess(VertexOutput input) : SV_Target {

        UNITY_SETUP_STEREO_EYE_INDEX_POST_VERTEX(input);

        uint2 positionSS = input.texcoord * _ScreenSize.xy;

        float3 outColor = LOAD_TEXTURE2D_X(_InputTexture, positionSS).xyz;

        // float3x3 gx = float3x3(
        // -1, 0, 1,
        // -2, 0, 2,
        // -1, 0, 1
        // );

        // float3x3 gy = float3x3(
        //     -1, -2, -1,
        //     0, 0, 0,
        //     1, 2, 1
        // );

        // float pixel_sum_x = 0.0;
        // float pixel_sum_y = 0.0;

        // for (int y = -1; y < 2; y++) {
        //     for (int x = -1; x < 2; x++) {

                
        //         float3 pixel = LOAD_TEXTURE2D_X(_InputTexture, uint2((positionSS.x + x),(positionSS.y + y))).xyz; 

        //         float grayscale_value = (pixel.r + pixel.g + pixel.b) / 3;
            
        //         pixel_sum_x += grayscale_value * gx[y + 1][x + 1];
        //         pixel_sum_y += grayscale_value * gy[y + 1][x + 1];
        //     }
        // }

        // float edge_value = abs(pixel_sum_x / 3) + abs(pixel_sum_y / 3);
        // float3 outputColor = outColor;

        // if (edge_value > 0.05){
        //     outputColor = float3(0,0,0);
        // }

        // water color
        float2 uv = positionSS;
        float n = float((radius + 1) * (radius + 1));

        float4x3 m;
        float4x3 s;
        for (int k = 0; k < 4; ++k) {
            m[k] = float3(0.0,0.0,0.0);
            s[k] = float3(0.0,0.0,0.0);
        }

        for (int j = -radius; j <= 0; ++j)  {
            for (int i = -radius; i <= 0; ++i)  {
                float3 c = LOAD_TEXTURE2D_X(_InputTexture, (uv + uint2(i,j)) / _ScreenSize).rgb;
                m[0] += c;
                s[0] += c * c;
            }
        }

        for (int j = -radius; j <= 0; ++j)  {
            for (int i = 0; i <= radius; ++i)  {
                float3 c = LOAD_TEXTURE2D_X(_InputTexture, (uv + uint2(i,j)) / _ScreenSize).rgb;
                m[1] += c;
                s[1] += c * c;
            }
        }

        for (int j = 0; j <= radius; ++j)  {
            for (int i = 0; i <= radius; ++i)  {
                float3 c = LOAD_TEXTURE2D_X(_InputTexture, (uv + uint2(i,j)) / _ScreenSize).rgb;
                m[2] += c;
                s[2] += c * c;
            }
        }

        for (int j = 0; j <= radius; ++j)  {
            for (int i = -radius; i <= 0; ++i)  {
                float3 c = LOAD_TEXTURE2D_X(_InputTexture, (uv + uint2(i,j)) / _ScreenSize).rgb;
                m[3] += c;
                s[3] += c * c;
            }
        }


        float min_sigma2 = 1e+2;
        for (int k = 0; k < 4; ++k) {
            m[k] /= n;
            s[k] = abs(s[k] / n - m[k] * m[k]);

            float sigma2 = s[k].r + s[k].g + s[k].b;
            if (sigma2 < min_sigma2) {
                min_sigma2 = sigma2;
                gl_FragColor = float4(m[k], 1.0);
            }
        }

        return float4(gl_FragColor);

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