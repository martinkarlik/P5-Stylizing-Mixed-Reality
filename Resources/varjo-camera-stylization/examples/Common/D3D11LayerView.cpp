// Copyright 2019-2020 Varjo Technologies Oy. All rights reserved.

#include "D3D11LayerView.hpp"

#include <Varjo_d3d11.h>

namespace VarjoExamples
{
D3D11LayerView::D3D11LayerView(varjo_Session* session, D3D11Renderer& renderer)
    : LayerView(session, renderer)
{
    // Create color texture swap chain for DX11
    varjo_SwapChainConfig2 colorConfig;
    colorConfig.numberOfTextures = 4;
    colorConfig.textureArraySize = 1;
    colorConfig.textureFormat = varjo_TextureFormat_R8G8B8A8_SRGB;
    colorConfig.textureWidth = getTotalWidth(m_viewports);
    colorConfig.textureHeight = getTotalHeight(m_viewports);

    m_colorSwapChain = varjo_D3D11CreateSwapChain(getSession(), renderer.getD3DDevice(), &colorConfig);
    CHECK_VARJO_ERR(getSession());

    // Create depth texture swap chain for DX11
    varjo_SwapChainConfig2 depthConfig{colorConfig};
    depthConfig.textureFormat = varjo_DepthTextureFormat_D32_FLOAT;

    m_depthSwapChain = varjo_D3D11CreateSwapChain(getSession(), renderer.getD3DDevice(), &depthConfig);
    CHECK_VARJO_ERR(getSession());

    // Create a DX11 render target for each swap chain texture
    for (int i = 0; i < colorConfig.numberOfTextures; ++i) {
        // Get swap chain textures for render target
        const varjo_Texture colorTexture = varjo_GetSwapChainImage(m_colorSwapChain, i);
        CHECK_VARJO_ERR(getSession());

        const varjo_Texture depthTexture = varjo_GetSwapChainImage(m_depthSwapChain, i);
        CHECK_VARJO_ERR(getSession());

        // Create render target instance
        m_renderTargets.emplace_back(std::make_unique<D3D11Renderer::RenderTarget>(renderer.getD3DDevice(), colorConfig.textureWidth, colorConfig.textureHeight,
            varjo_ToD3D11Texture(colorTexture), varjo_ToD3D11Texture(depthTexture)));
    }

    // Setup views
    setupViews();
}

ComPtr<IDXGIAdapter> D3D11LayerView::getAdapter(varjo_Session* session)
{
    varjo_Luid luid = varjo_D3D11GetLuid(session);

    ComPtr<IDXGIFactory> factory = nullptr;
    const HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (SUCCEEDED(hr)) {
        UINT i = 0;
        while (true) {
            ComPtr<IDXGIAdapter> adapter = nullptr;
            if (factory->EnumAdapters(i++, &adapter) == DXGI_ERROR_NOT_FOUND) break;
            DXGI_ADAPTER_DESC desc;
            if (SUCCEEDED(adapter->GetDesc(&desc)) && desc.AdapterLuid.HighPart == luid.high && desc.AdapterLuid.LowPart == luid.low) {
                return adapter;
            }
        }
    }
    return nullptr;
}

}  // namespace VarjoExamples
