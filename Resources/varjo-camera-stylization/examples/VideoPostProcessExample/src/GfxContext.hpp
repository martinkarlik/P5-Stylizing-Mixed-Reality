
#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>
#include <d3d11.h>
#include <d3d12.h>

#include "Globals.hpp"

// NOTICE! For simplicity, we instantiate all contexts into this class. In real
// world use post processing should use the same graphics api context as rendering.

//! Wrapper class for graphics contexts used in this example.
class GfxContext
{
public:
    //! Constructor
    GfxContext(HWND hwnd);

    //! Destructor
    ~GfxContext() = default;

    //! Initialize gfx sessions
    void init(IDXGIAdapter* adapter);

    //! Returns D3D11 device
    ComPtr<ID3D11Device> getD3D11Device() const { return m_d3d11Device; }

    //! Returns D3D12 command queue
    ComPtr<ID3D12CommandQueue> getD3D12CommandQueue() const { return m_d3d12Queue; }

private:
    //! Initialize OpenGL
    void initGL();

    //! Initialize D3D11
    void initD3D11(IDXGIAdapter* adapter);

    //! Initialize D3D12
    void initD3D12(IDXGIAdapter* adapter);

private:
    HWND m_hwnd = NULL;
    HDC m_hdc = NULL;
    HGLRC m_hglrc = NULL;

    ComPtr<ID3D11Device> m_d3d11Device;                //!< D3D11 device
    ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;  //!< D3D11 device context

    ComPtr<ID3D12Device> m_d3d12Device;       //!< D3D12 device
    ComPtr<ID3D12CommandQueue> m_d3d12Queue;  //!< D3D12 command queue
};
