// Copyright 2020 Varjo Technologies Oy. All rights reserved.

#pragma once

#include <vector>
#include <array>
#include <d3d11_1.h>
#include <d3d12.h>

#include <Varjo_mr.h>
#include <Varjo_mr_experimental.h>

#include "Globals.hpp"

namespace VarjoExamples
{
// NOTICE! This example class has a support for all supported graphics APIs (D3D11, GL, and D3D12),
// but in real use-case you should only implement the one you are already using for other graphics in
// your application.

//! Wrapper class for VST post processing.
class PostProcess
{
public:
    //! Enumeration for shader source types
    enum class ShaderSource {
        None = 0,  //!< No shader
        Binary,    //!< Load from compiled binary
        Source,    //!< Load from HLSL source
    };

    //! Shader params structure
    struct ShaderParams {
        struct TextureParams {
            int width = 0;                   //!< Texture width
            int height = 0;                  //!< Texture height
            varjo_TextureFormat format = 0;  //!< Texture pixel format
        };

        int blockSize = 0;                    //!< Compute shader block size
        int samplingMargin = 0;               //!< Input texture sampling margin in scan lines
        int constantBufferSize = 0;           //!< Constant buffer size in bytes
        std::vector<TextureParams> textures;  //!< User input texture parameters
    };

    //! Graphics API types for input texture support.
    enum class GraphicsAPI {
        None = -1,                      //!< Not defined
        D3D11 = varjo_RenderAPI_D3D11,  //!< Use D3D11 API
        OpenGL = varjo_RenderAPI_GL,    //!< Use OpenGL API
        D3D12 = varjo_RenderAPI_D3D12,  //!< Use D3D12 API
    };

public:
    //! Constructor
    PostProcess(varjo_Session* session);

    //! Destructor
    ~PostProcess() = default;

    //! Init optional D3D11 support. This is required before loading D3D11 type shader.
    void initD3D11(ComPtr<ID3D11Device> device);

    //! Init optional D3D12 support. This is required before loading D3D12 type shader.
    void initD3D12(ComPtr<ID3D12CommandQueue> commandQueue);

    //! Return D3D11 device pointer or nullptr if not initialized.
    ComPtr<ID3D11Device> PostProcess::getD3D11Device() const;

    //! Return D3D12 device pointer or nullptr if not initialized.
    ComPtr<ID3D12CommandQueue> PostProcess::getD3D12CommandQueue() const;

    //! Reset post processor
    void reset();

    //! Enable/disable VST post processing
    void setEnabled(bool enabled);

    //! Returns if post processing is enabled
    bool isEnabled() const { return m_enabled; }

    //! Is post processor active
    bool isActive() const { return (m_enabled && m_shaderSource != ShaderSource::None); }

    //! Check texture format
    bool checkTextureFormat(GraphicsAPI api, varjo_TextureFormat format) const;

    //! Convert varjo_TextureFormat to DXGI_FORMAT
    varjo_DXGITextureFormat toDXGIFormat(varjo_TextureFormat format) const;

    //! Convert varjo_TextureFormat to GL texture format
    varjo_GLTextureFormat toGLFormat(varjo_TextureFormat format) const;

    //! Load from given source. Texture support is determined by given graphicsAPI type
    bool loadShader(GraphicsAPI graphicsAPI, PostProcess::ShaderSource shaderSource, const std::string& shaderFilename, const ShaderParams& shaderParams);

    //! Lock texture for writing. Texture is written in given reference, and true is returned if succeeded.
    bool lockTextureBuffer(int textureIndex, varjo_Texture& varjoTexture);

    //! Unlock a previously locked texture
    void unlockTextureBuffer(int textureIndex);

    //! Apply shader input buffers
    void applyInputBuffers(char* constantBuffer, int contantBufferSize, const std::vector<int32_t>& textureIndices);

    //! Returns current shader source
    ShaderSource getShaderSource() const { return m_shaderSource; }

private:
    //! Lock post processing feature for this session
    bool lock();

    //! Unlock post processing feature for this session
    void unlock();

private:
    varjo_Session* m_session = nullptr;                //!< Varjo session
    bool m_enabled = false;                            //!< Feature enabled flag
    bool m_locked = false;                             //!< Feature locked
    GraphicsAPI m_graphicsAPI = GraphicsAPI::None;     //!< Current graphics API
    ShaderSource m_shaderSource = ShaderSource::None;  //!< Current shader source
    ShaderParams m_shaderParams{};                     //!< Shader parameters
    ComPtr<ID3D11Device> m_d3d11Device;                //!< D3D11 device instance
    ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;    //!< D3D12 command queue instance
};

}  // namespace VarjoExamples
