\
#pragma once
#ifdef _WIN32
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <memory>

using Microsoft::WRL::ComPtr;

class RendererDX11 {
public:
    RendererDX11();
    ~RendererDX11();

    // Initialize Win32 window and D3D11 device. Returns false on failure.
    bool initialize(HINSTANCE hInstance, int nCmdShow, int width = 1280, int height = 720);

    // Main render loop (blocks until window closed)
    void runMainLoop(class PhysicsManager* phys);

    // Shutdown and cleanup
    void shutdown();

    // Load and compile HLSL shader from file (vertex+pixel entrypoints)
    bool compileShaderFromFile(const std::wstring& filename, const std::string& entryPoint, const std::string& target, ID3DBlob** blobOut);

private:
    HWND m_hwnd = nullptr;
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_rtv;

    // Simple resources for demo mesh
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_indexCount = 0;

    ComPtr<ID3D11Buffer> m_cbPerObject;
    ComPtr<ID3D11VertexShader> m_vs;
    ComPtr<ID3D11PixelShader> m_ps;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    int m_width = 1280;
    int m_height = 720;

    bool createDeviceAndSwapChain();
    void createRenderTarget();
    void createDemoMesh();
    void renderFrame();
    void cleanupSwapChain();
};

#endif // _WIN32
