
#include "GfxContext.hpp"
#include "Globals.hpp"

GfxContext::GfxContext(HWND hwnd)
{
    LOGI("Initializing graphics..");

    // Initializing window
    m_hwnd = hwnd;
    m_hdc = GetDC(m_hwnd);
    if (m_hdc == NULL) {
        CRITICAL("Failed to get DC.");
    }
}

void GfxContext::init(IDXGIAdapter* adapter)
{
    // Initialize GL context
    initGL();

    // Initialize D3D11 context
    initD3D11(adapter);

    // Initialize D3D12 context
    initD3D12(adapter);
}

void GfxContext::initD3D11(IDXGIAdapter* adapter)
{
    LOGI("Initializing D3D11 context..");

    HRESULT hr = D3D11CreateDevice(adapter, adapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
        &m_d3d11Device, nullptr, &m_d3d11DeviceContext);
    if (FAILED(hr)) {
        CRITICAL("Creating D3D11 device failed.");
    }
}

void GfxContext::initD3D12(IDXGIAdapter* adapter)
{
    HRESULT hr = 0;

    LOGI("Initializing D3D12 context..");

#ifdef _DEBUG
    {
        // Enable the D3D12 debug layer.
        ComPtr<ID3D12Debug> debugController;
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        if (FAILED(hr)) {
            CRITICAL("Getting D3D12 debug interface failed.");
        }

        // Enable debug layer
        debugController->EnableDebugLayer();
    }
#endif

    // Create D3D12 device
    hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12Device));
    if (FAILED(hr)) {
        CRITICAL("Creating D3D12 device failed.");
    }

    // Create D3D12 command queue
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.NodeMask = 0;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    hr = m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12Queue));
    if (FAILED(hr)) {
        CRITICAL("Creating D3D12 command queue failed.");
    }

    LOGI("D3D12 initialized!");
}

void GfxContext::initGL()
{
    LOGI("Initializing GL context..");

    if (wglewInit() != GLEW_OK) {
        CRITICAL("Failed to initialize WGLEW.");
    }

    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,  // Flags
        PFD_TYPE_RGBA,                                               // The kind of framebuffer. RGBA or palette.
        32,                                                          // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24,  // Number of bits for the depthbuffer
        8,   // Number of bits for the stencilbuffer
        0,   // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE, 0, 0, 0, 0};

    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);

    if (SetPixelFormat(m_hdc, pixelFormat, &pfd) == false) {
        CRITICAL("Failed to set pixel format.");
    }

    m_hglrc = wglCreateContext(m_hdc);
    if (m_hglrc == NULL) {
        CRITICAL("Failed to create OpenGL context.");
    }

    if (wglMakeCurrent(m_hdc, m_hglrc) == false) {
        CRITICAL("Failed to set current OpenGL context.");
    }

    if (glewInit() != GLEW_OK) {
        CRITICAL("Failed to initialize GLEW.");
    }
}
