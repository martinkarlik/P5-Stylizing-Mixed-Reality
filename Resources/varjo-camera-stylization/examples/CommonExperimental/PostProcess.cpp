
#include "PostProcess.hpp"

#include <wrl.h>
#include <d3dcompiler.h>
#include <fstream>

#include <Varjo_d3d11.h>

namespace
{
// Shader entrypoint
static const char* c_shaderEntrypoint = "main";

// Shader model
static const char* c_shaderTarget = "cs_5_0";

std::vector<char> loadFile(const std::string& filename)
{
    std::vector<char> data;
    std::ifstream file(filename, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        data.resize(size);
        if (!file.read(data.data(), size)) {
            LOGE("Loading file failed: %s", filename.c_str());
        }
    } else {
        LOGE("Opening file failed: %s", filename.c_str());
    }
    return data;
}
}  // namespace

namespace VarjoExamples
{
PostProcess::PostProcess(varjo_Session* session)
    : m_session(session)
{
}

void PostProcess::setEnabled(bool enabled)
{
    // Early exit if already same
    if (m_enabled == enabled) {
        return;
    }

    if (m_shaderSource == ShaderSource::None) {
        m_enabled = enabled;
    } else if (!enabled) {
        // Disable post processing
        varjo_MRSetShader(m_session, varjo_ShaderType_VideoPostProcess, varjo_False);
        CHECK_VARJO_ERR(m_session);

        // Reset post process flag
        m_enabled = false;
    } else {
        // Enable post processing
        varjo_MRSetShader(m_session, varjo_ShaderType_VideoPostProcess, varjo_True);
        CHECK_VARJO_ERR(m_session);

        // Set post process flag
        m_enabled = true;
    }
}

void PostProcess::reset()
{
    m_graphicsAPI = GraphicsAPI::None;
    m_shaderSource = ShaderSource::None;
    m_shaderParams = {};
    m_enabled = false;

    if (m_locked) {
        unlock();
    }
}

void PostProcess::initD3D11(ComPtr<ID3D11Device> device)
{
    // Store or reset D3D11 device
    m_d3d11Device = device;
}

void PostProcess::initD3D12(ComPtr<ID3D12CommandQueue> commandQueue)
{
    // Store or reset D3D12 command queue
    m_d3d12CommandQueue = commandQueue;
}

ComPtr<ID3D11Device> PostProcess::getD3D11Device() const
{
    // This can be null if d3d11 not initialized
    return m_d3d11Device;
}

ComPtr<ID3D12CommandQueue> PostProcess::getD3D12CommandQueue() const
{
    // This can be null if d3d12 not initialized
    return m_d3d12CommandQueue;
}

bool PostProcess::lock()
{
    m_locked = varjo_Lock(m_session, varjo_LockType_VideoPostProcessShader);
    CHECK_VARJO_ERR(m_session);
    return m_locked;
}

void PostProcess::unlock()
{
    varjo_Unlock(m_session, varjo_LockType_VideoPostProcessShader);
    m_locked = false;
    CHECK_VARJO_ERR(m_session);
}

bool PostProcess::loadShader(
    GraphicsAPI graphicsAPI, PostProcess::ShaderSource shaderSource, const std::string& shaderFilename, const ShaderParams& shaderParams)
{
    // Reset state before loading
    reset();

    // Lock post processing if not already locked
    if (!m_locked) {
        if (!lock()) {
            LOGE("Locking post processing failed.");
            return false;
        }
    }

    if ((shaderSource != ShaderSource::None) &&
        !(graphicsAPI == GraphicsAPI::D3D11 || graphicsAPI == GraphicsAPI::OpenGL || graphicsAPI == GraphicsAPI::D3D12)) {
        LOGE("Graphics API not supported: %d", graphicsAPI);
        return false;
    }

    if ((shaderSource != ShaderSource::None) && (graphicsAPI == GraphicsAPI::D3D11) && !m_d3d11Device) {
        LOGE("D3D11 support not initialized.");
        return false;
    }

    if ((shaderSource != ShaderSource::None) && (graphicsAPI == GraphicsAPI::D3D12) && !m_d3d12CommandQueue) {
        LOGE("D3D12 support not initialized.");
        return false;
    }

    // Shader data blob
    ComPtr<ID3DBlob> shaderBlob;

    switch (shaderSource) {
        case ShaderSource::None: {
            LOGI("Loading shader source: None");
            // Ignore loading
            m_shaderSource = ShaderSource::None;
            m_graphicsAPI = GraphicsAPI::None;
            m_shaderParams = {};
            return true;
        } break;

        case ShaderSource::Binary: {
            LOGI("Loading shader source: Compiled Binary: %s", shaderFilename.c_str());
            auto shaderData = loadFile(shaderFilename);
            if (!shaderData.empty()) {
                D3DCreateBlob(shaderData.size(), &shaderBlob);
                memcpy(shaderBlob->GetBufferPointer(), shaderData.data(), shaderData.size());
            } else {
                return false;
            }
        } break;

        case ShaderSource::Source: {
            LOGI("Loading shader source: HLSL Source: %s", shaderFilename.c_str());
            auto shaderData = loadFile(shaderFilename);
            if (!shaderData.empty()) {
                ComPtr<ID3DBlob> errorMsgs;
                HRESULT hr = D3DCompile(shaderData.data(), shaderData.size(), shaderFilename.c_str(), nullptr, nullptr, c_shaderEntrypoint, c_shaderTarget, 0,
                    0, &shaderBlob, &errorMsgs);
                if (FAILED(hr) && errorMsgs) {
                    std::string err(reinterpret_cast<char*>(errorMsgs->GetBufferPointer()), errorMsgs->GetBufferSize());
                    LOGE("Compiling post process shader failed: %s", err.c_str());
                    return false;
                }
            } else {
                return false;
            }
        } break;

        default: {
            CRITICAL("Unknown shader source: %d", shaderSource);
        } break;
    }

    // Apply compiled shader to Varjo API
    varjo_ShaderConfig shaderConfig{};
    shaderConfig.format = varjo_ShaderFormat_DxComputeBlob;
    shaderConfig.inputLayout = varjo_ShaderInputLayout_VideoPostProcess_V1;
    shaderConfig.params.videoPostProcess.inputFlags = varjo_ShaderFlag_VideoPostProcess_None;
    shaderConfig.params.videoPostProcess.computeBlockSize = shaderParams.blockSize;
    shaderConfig.params.videoPostProcess.samplingMargin = shaderParams.samplingMargin;
    shaderConfig.params.videoPostProcess.constantBufferSize = shaderParams.constantBufferSize;

    for (int i = 0; i < static_cast<int>(shaderParams.textures.size()); i++) {
        shaderConfig.params.videoPostProcess.textures[i] = {
            shaderParams.textures[i].format, static_cast<uint64_t>(shaderParams.textures[i].width), static_cast<uint64_t>(shaderParams.textures[i].height)};
    }

    switch (graphicsAPI) {
        case GraphicsAPI::D3D11: {
            LOGI("Shader texture API: D3D11");
            varjo_MRD3D11ConfigureShader(m_session, m_d3d11Device.Get(), varjo_ShaderType_VideoPostProcess, &shaderConfig,
                (shaderBlob ? static_cast<char*>(shaderBlob->GetBufferPointer()) : nullptr),
                shaderBlob ? static_cast<int32_t>(shaderBlob->GetBufferSize()) : 0);
        } break;
        case GraphicsAPI::OpenGL: {
            LOGI("Shader texture API: OpenGL");
            varjo_MRGLConfigureShader(m_session, varjo_ShaderType_VideoPostProcess, &shaderConfig,
                (shaderBlob ? static_cast<char*>(shaderBlob->GetBufferPointer()) : nullptr),
                shaderBlob ? static_cast<int32_t>(shaderBlob->GetBufferSize()) : 0);
        } break;
        case GraphicsAPI::D3D12: {
            LOGI("Shader texture API: D3D12");
            varjo_MRD3D12ConfigureShader(m_session, m_d3d12CommandQueue.Get(), varjo_ShaderType_VideoPostProcess, &shaderConfig,
                (shaderBlob ? static_cast<char*>(shaderBlob->GetBufferPointer()) : nullptr),
                shaderBlob ? static_cast<int32_t>(shaderBlob->GetBufferSize()) : 0);
        } break;
        default: {
            LOGE("Graphics API not supported: %d", m_graphicsAPI);
            return false;
        }
    }

    if (CHECK_VARJO_ERR(m_session) != varjo_NoError) {
        LOGE("Setting shader failed.");
        return false;
    }

    m_graphicsAPI = graphicsAPI;
    m_shaderSource = shaderSource;
    m_shaderParams = shaderParams;

    return true;
}

bool PostProcess::lockTextureBuffer(int textureIndex, varjo_Texture& varjoTexture)
{
    varjoTexture = varjo_MRAcquireShaderTexture(m_session, varjo_ShaderType_VideoPostProcess, textureIndex);
    if (CHECK_VARJO_ERR(m_session) != varjo_NoError) {
        LOGE("Locking texture failed: index=%d", textureIndex);
        return false;
    }
    return true;
}

void PostProcess::unlockTextureBuffer(int textureIndex)
{
    varjo_MRReleaseShaderTexture(m_session, varjo_ShaderType_VideoPostProcess, textureIndex);
    CHECK_VARJO_ERR(m_session);
}

void PostProcess::applyInputBuffers(char* constantBuffer, int contantBufferSize, const std::vector<int32_t>& textureIndices)
{
    // Apply updated textures and constants to Varjo API
    varjo_MRSubmitShaderInputs(m_session, varjo_ShaderType_VideoPostProcess, textureIndices.empty() ? nullptr : textureIndices.data(),
        static_cast<int32_t>(textureIndices.size()), (contantBufferSize == 0) ? nullptr : constantBuffer, contantBufferSize);
    CHECK_VARJO_ERR(m_session);
}

bool PostProcess::checkTextureFormat(GraphicsAPI api, varjo_TextureFormat format) const
{
    bool isSupported = false;

    if (api == GraphicsAPI::None) {
        return false;
    }

    varjo_RenderAPI renderAPI = static_cast<varjo_RenderAPI>(api);

    // List supported texture formats. This does not require the lock.
    std::vector<varjo_TextureFormat> formats;
    formats.resize(varjo_MRGetSupportedShaderTextureFormatCount(m_session, renderAPI, varjo_ShaderType_VideoPostProcess));
    CHECK_VARJO_ERR(m_session);

    varjo_MRGetSupportedShaderTextureFormats(m_session, renderAPI, varjo_ShaderType_VideoPostProcess, formats.data(), static_cast<int32_t>(formats.size()));
    CHECK_VARJO_ERR(m_session);

    for (const auto& f : formats) {
        if (f == format) {
            isSupported = true;
        }
    }

    if (!isSupported) {
        LOGE("Texture format not supported: %d.", static_cast<int>(format));
    }

    return isSupported;
}

varjo_DXGITextureFormat PostProcess::toDXGIFormat(varjo_TextureFormat format) const { return varjo_ToDXGIFormat(m_session, format); }

varjo_GLTextureFormat PostProcess::toGLFormat(varjo_TextureFormat format) const { return varjo_ToGLFormat(m_session, format); }

};  // namespace VarjoExamples
