// Copyright 2019-2020 Varjo Technologies Oy. All rights reserved.

#include "LayerView.hpp"

#include <cassert>
#include <Varjo_math.h>

#include "Scene.hpp"

namespace VarjoExamples
{
LayerView::LayerView(varjo_Session* session, Renderer& renderer)
    : SyncView(session, true)
    , m_renderer(renderer)
{
    // Number of views to render
    m_viewCount = varjo_GetViewCount(getSession());

    // Allocate view specific data
    m_viewports = calculateViewports(getSession());
    m_multiProjViews.resize(m_viewCount);
    m_extDepthViews.resize(m_viewCount);
    m_extDepthTestRangeViews.resize(m_viewCount);
}

LayerView::~LayerView()
{
    // These just free allocated resource, so no session or error check needed
    varjo_FreeSwapChain(m_colorSwapChain);
    varjo_FreeSwapChain(m_depthSwapChain);
}

void LayerView::setupViews()
{
    // Setup views
    for (int i = 0; i < m_viewCount; ++i) {
        // Viewport layout
        const varjo_Viewport& viewport = m_viewports[i];
        m_multiProjViews[i].viewport = varjo_SwapChainViewport{m_colorSwapChain, viewport.x, viewport.y, viewport.width, viewport.height, 0};

        // Notice that we set extension links nullptr here and do the actual linking later
        // in render call according to the render parameters.

        // Depth extension
        m_extDepthViews[i].header.type = varjo_ViewExtensionDepthType;
        m_extDepthViews[i].header.next = nullptr;
        m_extDepthViews[i].minDepth = 0.0;
        m_extDepthViews[i].maxDepth = 1.0;
        m_extDepthViews[i].nearZ = c_nearClipPlane;
        m_extDepthViews[i].farZ = c_farClipPlane;
        m_extDepthViews[i].viewport = varjo_SwapChainViewport{m_depthSwapChain, viewport.x, viewport.y, viewport.width, viewport.height, 0};

        // Depth test range extension
        m_extDepthTestRangeViews[i].header.type = varjo_ViewExtensionDepthTestRangeType;
        m_extDepthTestRangeViews[i].header.next = nullptr;
        m_extDepthTestRangeViews[i].nearZ = 0.0;
        m_extDepthTestRangeViews[i].farZ = 1.0;
    }
}

std::vector<varjo_Viewport> LayerView::calculateViewports(varjo_Session* session) const
{
    const int viewsPerRow = 2;
    const int32_t viewCount = varjo_GetViewCount(session);
    CHECK_VARJO_ERR(getSession());
    std::vector<varjo_Viewport> viewports;
    viewports.reserve(viewCount);

    int x = 0, y = 0;
    for (int i = 0; i < viewCount; i++) {
        const varjo_ViewDescription viewDesc = varjo_GetViewDescription(session, static_cast<int32_t>(i));
        CHECK_VARJO_ERR(getSession());
        const varjo_Viewport viewport{x, y, static_cast<int32_t>(viewDesc.width), static_cast<int32_t>(viewDesc.height)};
        viewports.push_back(viewport);
        x += viewport.width;
        if (i > 0 && viewports.size() % viewsPerRow == 0) {
            x = 0;
            y += viewport.height;
        }
    }
    return viewports;
}

int32_t LayerView::getTotalWidth(const std::vector<varjo_Viewport>& viewports)
{
    const int viewsPerRow = 2;
    int maxWidth = 0;
    for (size_t i = 0; i < viewports.size() / viewsPerRow; i++) {
        int rowWidth = 0;
        for (size_t k = 0; k < viewsPerRow; k++) {
            rowWidth += viewports[i * viewsPerRow + k].width;
        }

        maxWidth = (std::max)(maxWidth, rowWidth);
    }

    return maxWidth;
}

int32_t LayerView::getTotalHeight(const std::vector<varjo_Viewport>& viewports)
{
    const int viewsPerRow = 2;
    int32_t totalHeight = 0;
    for (size_t i = 0; i < viewports.size() / viewsPerRow; i++) {
        totalHeight += viewports[i * viewsPerRow].height;
    }

    return totalHeight;
}

void LayerView::syncFrame()
{
    // Handle frame timing in base class
    SyncView::syncFrame();

    // Reset update state
    m_updateState = {};

    // Iterate views to update
    for (int i = 0; i < m_viewCount; i++) {
        // Get the view information for this frame.
        varjo_ViewInfo view = getFrameInfo()->views[i];
        if (!view.enabled) {
            continue;  // Skip a view if it is not enabled.
        }
        m_updateState.views.push_back(i);

        // Change the near and far clip distances in projection matrix
        varjo_UpdateNearFarPlanes(view.projectionMatrix, varjo_ClipRangeZeroToOne, c_nearClipPlane, c_farClipPlane);

        // Copy view and projection matrices to multiproj views
        std::copy(std::begin(view.projectionMatrix), std::end(view.projectionMatrix), std::begin(m_multiProjViews[i].projection.value));
        std::copy(std::begin(view.viewMatrix), std::end(view.viewMatrix), std::begin(m_multiProjViews[i].view.value));

        // Reset optional view extensions
        m_multiProjViews[i].extension = nullptr;
    }

    // Set update state valid
    m_updateState.valid = true;
}

void LayerView::beginFrame(const SubmitParams& params)
{
    // Check that previous update is not valid
    assert(m_updateState.valid);

    // Store params
    m_updateState.params = params;

    // Handle optional view extensions
    for (int i = 0; i < m_viewCount; i++) {
        // Add depth extension if submit depth enabled
        if (m_updateState.params.submitDepth) {
            m_multiProjViews[i].extension = &m_extDepthViews[i].header;
            m_extDepthViews[i].header.next = nullptr;
            // Add depth test range extension if enabled
            if (m_updateState.params.depthTestRangeEnabled) {
                m_extDepthViews[i].header.next = &m_extDepthTestRangeViews[i].header;
                m_extDepthTestRangeViews[i].header.next = nullptr;
                m_extDepthTestRangeViews[i].nearZ = m_updateState.params.depthTestRangeLimits[0];
                m_extDepthTestRangeViews[i].farZ = m_updateState.params.depthTestRangeLimits[1];
            }
        }
    }

    // Begin rendering frame
    varjo_BeginFrameWithLayers(getSession());
    if (CHECK_VARJO_ERR(getSession()) != varjo_NoError) {
        return;
    }

    // Get swap chain index only if VR layer submit is enabled
    m_updateState.swapChainIndex = InvalidIndex;
    if (m_updateState.params.submitLayer) {
        int32_t colorIndex = 0;
        varjo_AcquireSwapChainImage(m_colorSwapChain, &colorIndex);
        int32_t depthIndex = 0;
        varjo_AcquireSwapChainImage(m_depthSwapChain, &depthIndex);
        assert(colorIndex == depthIndex);

        m_updateState.swapChainIndex = colorIndex;
    }
}

void LayerView::endFrame()
{
    // Check that update state is valid
    assert(m_updateState.valid);

    // Submit info structures
    varjo_SubmitInfoLayers submitInfoLayers{};
    varjo_LayerMultiProj multiProjectionLayer{};
    varjo_LayerHeader* renderedLayers[] = {&multiProjectionLayer.header};

    // Reset submit info
    submitInfoLayers.frameNumber = getFrameInfo()->frameNumber;
    submitInfoLayers.layerCount = 0;
    submitInfoLayers.layers = nullptr;

    // Only fill in submit info if we actually rendered
    if (m_updateState.swapChainIndex != InvalidIndex) {
        // Send the textures and viewport information to the compositor.
        // Also give the view information the frame was rendered with.
        varjo_LayerHeader header{};
        header.type = varjo_LayerMultiProjType;
        header.flags = 0;

        // We need to set this flag to enable Varjo compositor to alpha blend
        // the VR content over VST image (or other VR layers). If we render opaque
        // VR background we dont want to set it.
        if (m_updateState.params.alphaBlend) {
            header.flags |= varjo_LayerFlag_BlendMode_AlphaBlend;
        }

        // Enable Varjo compositor to depth test this layer agains other layers (e.g. VST)
        // with depth information. Depth layer must be submitted for this to work.
        if (m_updateState.params.depthTestEnabled && m_updateState.params.submitDepth) {
            header.flags |= varjo_LayerFlag_DepthTesting;
        }

        // Enable chroma keying for this layer
        if (m_updateState.params.chromaKeyEnabled) {
            header.flags |= varjo_LayerFlag_ChromaKeyMasking;
        }

        multiProjectionLayer.header = header;
        multiProjectionLayer.space = varjo_SpaceLocal;
        multiProjectionLayer.viewCount = m_viewCount;

        // We use const-cast here to access member data through non-const pointer without copying it.
        multiProjectionLayer.views = const_cast<varjo_LayerMultiProjView*>(m_multiProjViews.data());

        // Add frame layers to submit info
        submitInfoLayers.layerCount = 1;
        submitInfoLayers.layers = renderedLayers;

        // Release swap chain images after rendering
        varjo_ReleaseSwapChainImage(m_colorSwapChain);
        CHECK_VARJO_ERR(getSession());
        varjo_ReleaseSwapChainImage(m_depthSwapChain);
        CHECK_VARJO_ERR(getSession());
    }

    // Finish frame and submit layers info
    varjo_EndFrameWithLayers(getSession(), &submitInfoLayers);
    CHECK_VARJO_ERR(getSession());

    // Reset update state to invalidate
    m_updateState = {};
}

void LayerView::renderScene(const Scene& scene, const RenderParams& params) const
{
    // Check that update state is valid
    assert(m_updateState.valid);

    // Render frame if we have swapchain image
    if (m_updateState.swapChainIndex != InvalidIndex) {
        // Get render target for swapchain index
        Renderer::RenderTarget& target = *m_renderTargets[m_updateState.swapChainIndex];

        // Render updated scene views to target
        renderSceneToTarget(target, scene, params);
    }
}

void LayerView::renderSceneToTarget(Renderer::RenderTarget& renderTarget, const Scene& scene, const RenderParams& params) const
{
    // Check that update state is valid
    assert(m_updateState.valid);

    // Bind render target
    m_renderer.bindRenderTarget(renderTarget);

    // Iterate updated views for render
    for (auto i : m_updateState.views) {
        // Set viewport
        const varjo_Viewport& viewport = m_viewports[i];
        m_renderer.setViewport(viewport.x, viewport.y, viewport.width, viewport.height);

        // Render viewport
        scene.render(m_renderer, renderTarget, fromVarjoMatrix(m_multiProjViews[i].view), fromVarjoMatrix(m_multiProjViews[i].projection), params.userData);
    }

    // Unbind render target
    m_renderer.unbindRenderTarget();
}

void LayerView::clear(const ClearParams& params)
{
    // Check that update state is valid
    assert(m_updateState.valid);

    // Render frame if we have swapchain image
    if (m_updateState.swapChainIndex != InvalidIndex) {
        // Get render target for swapchain index
        Renderer::RenderTarget& target = *m_renderTargets[m_updateState.swapChainIndex];

        // Clear view target
        clearTarget(target, params);
    }
}

void LayerView::clearTarget(Renderer::RenderTarget& renderTarget, const ClearParams& params)
{
    // Check that update state is valid
    assert(m_updateState.valid);

    // Clear render target
    m_renderer.clear(renderTarget, params.colorValue, params.clearColor, params.clearDepth, params.clearStencil, params.depthValue, params.stencilValue);
}

}  // namespace VarjoExamples
