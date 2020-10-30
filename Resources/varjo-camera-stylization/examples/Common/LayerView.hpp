// Copyright 2019-2020 Varjo Technologies Oy. All rights reserved.

#pragma once

#include <vector>
#include <memory>
#include <array>

#include <Varjo_layers.h>
#include <Varjo_types_layers.h>

#include "Globals.hpp"
#include "Renderer.hpp"
#include "SyncView.hpp"
#include "Scene.hpp"

namespace VarjoExamples
{
//! Graphics API independent base class for managing Varjo multi projection views and related resources
class LayerView : public SyncView
{
public:
    //! Clearing parameters structure
    struct ClearParams {
        //! Default constructor
        ClearParams() = default;

        //! Convinience constructor for clearing with given color
        ClearParams(const glm::vec4& colorClearValue)
            : colorValue(colorClearValue)
        {
        }

        //! Convinience constructor for clear flags
        ClearParams(bool clear, const glm::vec4& colorClearValue = {0.0f, 0.0f, 0.0f, 0.0f})
            : clearColor(clear)
            , clearDepth(clear)
            , clearStencil(clear)
            , colorValue(colorClearValue)
        {
        }

        bool clearColor = true;                           //!< Clear color buffer flag
        bool clearDepth = true;                           //!< Clear depth buffer flag
        bool clearStencil = true;                         //!< Clear stencil buffer flag
        glm::vec4 colorValue = {0.0f, 0.0f, 0.0f, 0.0f};  //!< Color clear value
        float depthValue = 1.0f;                          //!< Depth clear value
        uint8_t stencilValue = 0;                         //!< Stencil clear value
    };

    //! Render parameters struct
    struct RenderParams {
        //! Default constructor
        RenderParams() = default;

        //! Convinience constructor for just setting passId and clear flags
        RenderParams(int passId, void* userData = nullptr)
            : passId(passId)
            , userData(userData)
        {
        }

        int passId = 0;            //!< Render pass ID to be passed to scene
        void* userData = nullptr;  //!< User data pointer to be passed to scene render function
    };

    //! VR layer frame submit parameters
    struct SubmitParams {
        bool submitLayer = true;                                      //!< Enable VR layer submit
        bool alphaBlend = true;                                       //!< Enable alpha blending for submitted VR layer
        bool submitDepth = true;                                      //!< Enable depth buffer submit for VR layer
        bool depthTestEnabled = false;                                //!< Enable depth testing in compositor for submitted VR layer
        bool chromaKeyEnabled = false;                                //!< Enable chroma key masking in compositor for submitted VR layer
        bool depthTestRangeEnabled = true;                            //!< Enable depth test range limits for submitted VR layer
        std::array<double, 2> depthTestRangeLimits = {0.0, FLT_MAX};  //!< Depth test minimum and maximum limits for submitted VR layer
    };

    //! Destructor
    ~LayerView();

    // Disable copy, move and assign
    LayerView(const LayerView& other) = delete;
    LayerView(const LayerView&& other) = delete;
    LayerView& operator=(const LayerView& other) = delete;
    LayerView& operator=(const LayerView&& other) = delete;

    //! Return render target width
    glm::ivec2 getSize() const { return m_renderTargets[0]->getSize(); }

    //! Return view count
    int getViewCount() { return m_viewCount; }

    //! Return viewport for given view index
    const varjo_Viewport& getViewport(int i) const { return m_viewports.at(i); }

    //! From SyncFrame. Called to sync varjo frame before rendering.
    void syncFrame() override;

    //! Begin varjo frame. This must be called after syncFrame() and before rendering.
    void beginFrame(const SubmitParams& params);

    //! Render scene views to varjo render target
    void renderScene(const Scene& scene, const RenderParams& renderParams = {}) const;

    //! Clear view varjo render target
    void clear(const ClearParams& clearParams = {});

    //! Render scene views to external render target
    void renderSceneToTarget(Renderer::RenderTarget& renderTarget, const Scene& scene, const RenderParams& renderParams = {}) const;

    //! Clear given external render target
    void clearTarget(Renderer::RenderTarget& renderTarget, const ClearParams& clearParams = {});

    //! End varjo frame. This must be called after rendering to submit the frame.
    void endFrame();

protected:
    //! Protected constructor
    LayerView(varjo_Session* session, Renderer& renderer);

    //! Setup multi view structures. Called from implementing class constructor.
    void setupViews();

    //! Returns width of a texture which can fit all specified viewports
    static int32_t getTotalWidth(const std::vector<varjo_Viewport>& viewports);

    //! Returns height of a texture which can fit all specified viewports
    static int32_t getTotalHeight(const std::vector<varjo_Viewport>& viewports);

private:
    //! Calculates viewports of all views in an atlas, 2 views per row
    std::vector<varjo_Viewport> calculateViewports(varjo_Session* session) const;

protected:
    varjo_SwapChain* m_colorSwapChain = nullptr;                           //!< Swapchain for color buffers
    varjo_SwapChain* m_depthSwapChain = nullptr;                           //!< Swapchain for client depth
    std::vector<std::unique_ptr<Renderer::RenderTarget>> m_renderTargets;  //!< Render target for each swapchain image

    // View specific data
    int32_t m_viewCount = 0;                                                  //!< Number of views
    std::vector<varjo_Viewport> m_viewports;                                  //!< View port layout for each view
    std::vector<varjo_LayerMultiProjView> m_multiProjViews;                   //!< Multi projection views for each view
    std::vector<varjo_ViewExtensionDepth> m_extDepthViews;                    //!< Client depth extension for each view
    std::vector<varjo_ViewExtensionDepthTestRange> m_extDepthTestRangeViews;  //!< Client depth test range extension for each view

    const double c_nearClipPlane = .1;       //!< Near clip plane constant
    const double c_farClipPlane = 1e3;       //!< Far clip plane constant
    static const int32_t InvalidIndex = -1;  //!< Invalid swap chain index constant

private:
    Renderer& m_renderer;  //!< Renderer reference

    //! Update state structure
    struct {
        bool valid = false;                     //!< Update validity flag
        SubmitParams params{};                  //!< Render parameters
        std::vector<int> views;                 //!< List of updated views to be rendered
        int32_t swapChainIndex = InvalidIndex;  //!< Swap chain index for rendering
    } m_updateState;
};

}  // namespace VarjoExamples
