
#ifdef _WIN32

#include "../include/DirectXRenderer.h"
#include <windows.h>
#include <d3dcompiler.h>
#include <iostream>
#include <vector>
#include <cmath>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

using Microsoft::WRL::ComPtr;

// Simple cube vertex
struct Vertex {
    float x,y,z;
    float nx,ny,nz;
    float u,v;
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

DirectXRenderer::~DirectXRenderer(){ cleanup(); }

bool DirectXRenderer::initialize(HINSTANCE hInstance, int nCmdShow, int width, int height, const std::wstring& windowTitle){
    m_width = width; m_height = height;
    // register window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"CoreTechDXWindowClass";
    if(!RegisterClassEx(&wcex)){
        std::cerr << "Failed to register window class\\n";
        return false;
    }
    m_hwnd = CreateWindow(wcex.lpszClassName, windowTitle.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
    if(!m_hwnd){
        std::cerr << "Failed to create window\\n";
        return false;
    }
    ShowWindow(m_hwnd, nCmdShow);
    if(!createDeviceAndSwapChain(m_hwnd, width, height)) return false;
    if(!createResources()) return false;
    return true;
}

bool DirectXRenderer::createDeviceAndSwapChain(HWND hwnd, int width, int height){
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
        featureLevels, 1, D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, NULL, &m_context);
    if(FAILED(hr)){
        std::cerr << "D3D11CreateDeviceAndSwapChain failed: " << std::hex << hr << std::dec << "\\n";
        return false;
    }
    return true;
}

bool DirectXRenderer::createResources(){
    // create RTV
    ComPtr<ID3D11Texture2D> pBackBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if(FAILED(hr)) return false;
    hr = m_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_rtv);
    if(FAILED(hr)) return false;

    // create simple depth buffer
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_width; descDepth.Height = m_height;
    descDepth.MipLevels = 1; descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1; descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ComPtr<ID3D11Texture2D> depthTex;
    hr = m_device->CreateTexture2D(&descDepth, NULL, &depthTex);
    if(FAILED(hr)) return false;
    hr = m_device->CreateDepthStencilView(depthTex.Get(), NULL, &m_dsv);
    if(FAILED(hr)) return false;

    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());

    // create simple cube vertex buffer
    Vertex vertices[] = {
        // positions normals uv (only a few verts — for demo, a proper cube would be 36 verts)
        { -1,-1,-1,  0,0,-1, 0,0 }, { 1,-1,-1, 0,0,-1, 1,0 }, { 1,1,-1, 0,0,-1, 1,1 }, { -1,1,-1, 0,0,-1, 0,1 },
        // ... more faces omitted for brevity; use a simple triangle
        { -0.5f,-0.5f,0, 0,0,1, 0,0 }, { 0.5f,-0.5f,0, 0,0,1, 1,0 }, { 0.0f,0.5f,0, 0,0,1, 0.5f,1 }
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init = { vertices, 0, 0 };
    hr = m_device->CreateBuffer(&bd, &init, &m_vertexBuffer);
    if(FAILED(hr)) return false;

    // compile default HLSL shader if path provided
    if(!m_hlslPath.empty()){
        // attempt to compile VS/PS entry points from file
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;
        ID3DBlob* errBlob = nullptr;
        HRESULT r = D3DCompileFromFile(m_hlslPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", 0, 0, &vsBlob, &errBlob);
        if(FAILED(r)){
            if(errBlob) std::cerr << "VS compile error: " << (char*)errBlob->GetBufferPointer() << std::endl;
            return false;
        }
        r = D3DCompileFromFile(m_hlslPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", 0, 0, &psBlob, &errBlob);
        if(FAILED(r)){
            if(errBlob) std::cerr << "PS compile error: " << (char*)errBlob->GetBufferPointer() << std::endl;
            return false;
        }
        // create shaders
        hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs);
        if(FAILED(hr)) return false;
        hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps);
        if(FAILED(hr)) return false;
        // input layout (position,norm,uv)
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        hr = m_device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);
        if(FAILED(hr)) return false;
        vsBlob->Release();
        psBlob->Release();
    }

    // create constant buffer for per-object (simple)
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = 16 * 4; // 64 bytes enough for a few floats/matrices
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = m_device->CreateBuffer(&cbd, NULL, &m_cbPerObject);
    if(FAILED(hr)) return false;

    return true;
}

bool DirectXRenderer::compileHLSL(const std::wstring& hlslPath){
    m_hlslPath = hlslPath;
    // resources will be (re)created in createResources if needed
    return true;
}

void DirectXRenderer::setMaterial(const Material& mat){
    m_material = mat;
    // update constant buffer (very simplified)
    struct CBData { float albedo[4]; float spec[4]; } cb = {};
    cb.albedo[0]=mat.albedo[0]; cb.albedo[1]=mat.albedo[1]; cb.albedo[2]=mat.albedo[2]; cb.albedo[3]=1.0f;
    cb.spec[0]=mat.specPower; cb.spec[1]=mat.specIntensity;
    m_context->UpdateSubresource(m_cbPerObject.Get(), 0, NULL, &cb, 0, 0);
}

void DirectXRenderer::runMainLoop(){
    MSG msg = {0};
    while (WM_QUIT != msg.message){
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        float clearColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
        m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);
        m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        if(m_vs) m_context->VSSetShader(m_vs.Get(), NULL, 0);
        if(m_ps) m_context->PSSetShader(m_ps.Get(), NULL, 0);
        UINT stride = sizeof(Vertex); UINT offset = 0;
        m_context->IASetVertexBuffers(0,1,m_vertexBuffer.GetAddressOf(), &stride, &offset);
        m_context->IASetInputLayout(m_inputLayout.Get());
        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->RSSetViewports(1, &(D3D11_VIEWPORT){0.0f,0.0f,(FLOAT)m_width,(FLOAT)m_height,0.0f,1.0f});
        m_context->Draw(3, 0); // draws the demo triangle

        m_swapChain->Present(1, 0);
    }
}

void DirectXRenderer::cleanup(){
    if(m_context) m_context->ClearState();
    m_vs.Reset(); m_ps.Reset(); m_inputLayout.Reset(); m_vertexBuffer.Reset();
    m_rtv.Reset(); m_dsv.Reset(); m_swapChain.Reset(); m_context.Reset(); m_device.Reset();
}

#endif // _WIN32
