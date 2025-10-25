
#pragma once
#ifdef _WIN32

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "Material.h"
#include "Renderer.h"

// Simple DirectX11 renderer skeleton that compiles HLSL at runtime via D3DCompile.
// This is Windows-only and requires linking with d3d11.lib and D3DCompiler.lib.
class DirectXRenderer {
public:
    DirectXRenderer() = default;
    ~DirectXRenderer();

    // Initialize Win32 window and D3D11 device. Returns false on failure.
    bool initialize(HINSTANCE hInstance, int nCmdShow, int width = 800, int height = 600, const std::wstring& windowTitle = L"CoreTech DX11 Demo");

    // Main render loop - returns when window is closed
    void runMainLoop();

    // Load and compile HLSL shader file (stores compiled blobs)
    bool compileHLSL(const std::wstring& hlslPath);

    // Set material (will map to constant buffer)
    void setMaterial(const Material& mat);

private:
    bool createDeviceAndSwapChain(HWND hwnd, int width, int height);
    bool createResources();
    void cleanup();

    HWND m_hwnd = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    // Simple pipeline objects
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerObject;

    Material m_material;
    int m_width = 800, m_height = 600;
    std::wstring m_hlslPath;
    std::vector<char> m_vsBlobData, m_psBlobData;
};

#endif // _WIN32
