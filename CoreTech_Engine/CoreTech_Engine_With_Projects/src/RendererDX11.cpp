\
#ifdef _WIN32
#include "../include/RendererDX11.h"
#include <d3dcompiler.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
#include <filesystem>
#include <unordered_map>

#include <stdexcept>
#include <chrono>
#include <thread>
#include "../include/PhysicsManager.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Simple vertex structure
struct SimpleVertex {
    float x,y,z;
    float nx,ny,nz;
    float u,v;
};

// Per-object constant buffer
struct CBPerObject {
    float mWorld[16];
    float mView[16];
    float mProj[16];
};

// Forward declarations for Win32
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

RendererDX11::RendererDX11(){}
RendererDX11::~RendererDX11(){ shutdown(); }

bool RendererDX11::initialize(HINSTANCE hInstance, int nCmdShow, int width, int height){
    m_width = width; m_height = height;
    // Register class
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"CoreTechWindowClass";
    if(!RegisterClassEx(&wcex)) return false;

    RECT rc = {0,0,width,height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hwnd = CreateWindow(L"CoreTechWindowClass", L"CoreTech Demo (DirectX11)", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, hInstance, NULL);
    if(!m_hwnd) return false;
    ShowWindow(m_hwnd, nCmdShow);

    if(!createDeviceAndSwapChain()) return false;
    createRenderTarget();
    createDemoMesh();
    return true;
}

bool RendererDX11::createDeviceAndSwapChain(){
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_width;
    sd.BufferDesc.Height = m_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
                                               NULL, 0, D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, &featureLevel, &m_context);
    if(FAILED(hr)) return false;
    return true;
}

void RendererDX11::createRenderTarget(){
    ComPtr<ID3D11Texture2D> pBackBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf());
    m_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_rtv.GetAddressOf());
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), NULL);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)m_width;
    vp.Height = (FLOAT)m_height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_context->RSSetViewports(1, &vp);
}

void RendererDX11::createDemoMesh(){
    // Simple quad (two triangles) as demo mesh
    SimpleVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0,0,1, 0,1},
        {-1.0f,  1.0f, 0.0f, 0,0,1, 0,0},
        { 1.0f,  1.0f, 0.0f, 0,0,1, 1,0},
        { 1.0f, -1.0f, 0.0f, 0,0,1, 1,1},
    };
    UINT indices[] = {0,1,2, 0,2,3};
    m_indexCount = 6;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = vertices;
    m_device->CreateBuffer(&bd, &initData, m_vertexBuffer.GetAddressOf());

    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;
    m_device->CreateBuffer(&bd, &initData, m_indexBuffer.GetAddressOf());

    // Constant buffer
    D3D11_BUFFER_DESC cbd;
    ZeroMemory(&cbd, sizeof(cbd));
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(CBPerObject);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_device->CreateBuffer(&cbd, NULL, m_cbPerObject.GetAddressOf());

    // Compile simple passthrough shaders from existing HLSL (BlinnPhong.hlsl uses PSMain/VSMain)
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    wchar_t bp[] = L"shaders/hlsl/BlinnPhong.hlsl";
    m_shaderTimestamps[bp] = std::filesystem::exists(bp) ? std::filesystem::last_write_time(bp).time_since_epoch().count() : 0;
    if(FAILED(D3DCompileFromFile(L"shaders/hlsl/BlinnPhong.hlsl", NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vsBlob, NULL))){
        OutputDebugStringA("Failed to compile VS from BlinnPhong.hlsl\\n");
    } else {
        m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, m_vs.GetAddressOf());
    }
    if(FAILED(D3DCompileFromFile(L"shaders/hlsl/BlinnPhong.hlsl", NULL, NULL, "PSMain", "ps_5_0", 0, 0, &psBlob, NULL))){
        OutputDebugStringA("Failed to compile PS from BlinnPhong.hlsl\\n");
    } else {
        m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, m_ps.GetAddressOf());
    }

    // Create input layout matching SimpleVertex
    if(vsBlob){
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
        vsBlob->Release();
    }
    if(psBlob) psBlob->Release();
}


bool RendererDX11::loadTextureFromFile(const std::wstring& path, ID3D11ShaderResourceView** outSRV){
    if(!m_device) return false;
    // avoid reloading if already loaded
    auto it = m_textureSRVs.find(path);
    if(it != m_textureSRVs.end()){
        *outSRV = it->second.Get();
        if(*outSRV) (*outSRV)->AddRef();
        return true;
    }
    IWICImagingFactory* wicFactory = nullptr;
    if(FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)))){
        return false;
    }
    IWICBitmapDecoder* decoder = nullptr;
    if(FAILED(wicFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder))){
        wicFactory->Release();
        return false;
    }
    IWICBitmapFrameDecode* frame = nullptr;
    if(FAILED(decoder->GetFrame(0, &frame))){
        decoder->Release(); wicFactory->Release();
        return false;
    }
    IWICFormatConverter* converter = nullptr;
    if(FAILED(wicFactory->CreateFormatConverter(&converter))){
        frame->Release(); decoder->Release(); wicFactory->Release();
        return false;
    }
    if(FAILED(converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom))){
        converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release();
        return false;
    }
    UINT width, height;
    converter->GetSize(&width, &height);
    std::vector<BYTE> pixels(width * height * 4);
    if(FAILED(converter->CopyPixels(NULL, width*4, (UINT)pixels.size(), pixels.data()))){
        converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release();
        return false;
    }
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width; desc.Height = height; desc.MipLevels = 1; desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; desc.SampleDesc.Count = 1; desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA initData = {}; initData.pSysMem = pixels.data(); initData.SysMemPitch = width * 4;
    ID3D11Texture2D* tex = nullptr;
    HRESULT hr = m_device->CreateTexture2D(&desc, &initData, &tex);
    if(FAILED(hr)){
        converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release();
        return false;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    srvd.Format = desc.Format; srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; srvd.Texture2D.MipLevels = 1;
    ID3D11ShaderResourceView* srv = nullptr;
    hr = m_device->CreateShaderResourceView(tex, &srvd, &srv);
    tex->Release();
    converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release();
    if(FAILED(hr) || !srv) return false;
    m_textureSRVs[path] = srv;
    *outSRV = srv; (*outSRV)->AddRef();
    return true;
}

void RendererDX11::checkShaderHotReload(){
    // iterate known shader files and recompile if newer
    for(auto &kv : m_shaderTimestamps){
        std::wstring path = kv.first;
        std::time_t prev = kv.second;
        std::time_t mod = 0;
        try {
            auto ftime = std::filesystem::last_write_time(std::wstring(path));
            mod = decltype(ftime)::clock::to_time_t(ftime);
        } catch(...) { continue; }
        if(mod > prev){
            // attempt recompilation for VSMain/PSMain
            ID3DBlob* vsBlob = nullptr; ID3DBlob* psBlob = nullptr;
            HRESULT hr = D3DCompileFromFile(path.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", 0, 0, &vsBlob, NULL);
            if(SUCCEEDED(hr) && vsBlob){
                m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, m_vs.GetAddressOf());
                vsBlob->Release();
            }
            hr = D3DCompileFromFile(path.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", 0, 0, &psBlob, NULL);
            if(SUCCEEDED(hr) && psBlob){
                m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, m_ps.GetAddressOf());
                psBlob->Release();
            }
            kv.second = mod;
        }
    }
}

void RendererDX11::updateCamera(float dt){
    // simple static camera for demo: set view/projection matrices to identity-like values
    // For real camera controls, process input and update matrices accordingly.
    // set world to identity
    for(int i=0;i<16;i++) m_worldMat[i] = (i%5==0) ? 1.0f : 0.0f;
    // simple view: camera at (0,0,-5) looking at origin
    // column-major for HLSL: create look-at manually
    float eyeX=0, eyeY=0, eyeZ=-5.0f;
    float atX=0, atY=0, atZ=0;
    float upX=0, upY=1, upZ=0;
    // compute forward vector
    float fx = atX - eyeX, fy = atY - eyeY, fz = atZ - eyeZ;
    float fl = sqrtf(fx*fx + fy*fy + fz*fz);
    fx/=fl; fy/=fl; fz/=fl;
    // right = normalize(cross(up, forward))
    float rx = upY*fz - upZ*fy;
    float ry = upZ*fx - upX*fz;
    float rz = upX*fy - upY*fx;
    float rl = sqrtf(rx*rx + ry*ry + rz*rz);
    if(rl==0) rl=1.0f;
    rx/=rl; ry/=rl; rz/=rl;
    // corrected up = cross(forward, right)
    float ux = fy*rz - fz*ry;
    float uy = fz*rx - fx*rz;
    float uz = fx*ry - fy*rx;
    // view matrix (row-major)
    m_viewMat[0]=rx; m_viewMat[1]=ux; m_viewMat[2]=fx; m_viewMat[3]=0.0f;
    m_viewMat[4]=ry; m_viewMat[5]=uy; m_viewMat[6]=fy; m_viewMat[7]=0.0f;
    m_viewMat[8]=rz; m_viewMat[9]=uz; m_viewMat[10]=fz; m_viewMat[11]=0.0f;
    m_viewMat[12]=-(rx*eyeX + ry*eyeY + rz*eyeZ);
    m_viewMat[13]=-(ux*eyeX + uy*eyeY + uz*eyeZ);
    m_viewMat[14]=-(fx*eyeX + fy*eyeY + fz*eyeZ);
    m_viewMat[15]=1.0f;
    // projection simple perspective
    float fov = 70.0f * 3.14159265f/180.0f;
    float aspect = (float)m_width / (float)m_height;
    float zn = 0.1f, zf = 1000.0f;
    float f = 1.0f / tanf(fov*0.5f);
    m_projMat[0] = f/aspect; m_projMat[1]=0; m_projMat[2]=0; m_projMat[3]=0;
    m_projMat[4]=0; m_projMat[5]=f; m_projMat[6]=0; m_projMat[7]=0;
    m_projMat[8]=0; m_projMat[9]=0; m_projMat[10]=zf/(zf-zn); m_projMat[11]=1.0f;
    m_projMat[12]=0; m_projMat[13]=0; m_projMat[14]=(-zn*zf)/(zf-zn); m_projMat[15]=0;
}

void RendererDX11::renderFrame(){
    float clearColor[4] = {0.2f,0.2f,0.3f,1.0f};
    m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0,1,m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->VSSetShader(m_vs.Get(), NULL, 0);
    m_context->PSSetShader(m_ps.Get(), NULL, 0);
    m_context->VSSetConstantBuffers(0,1,m_cbPerObject.GetAddressOf());

    m_context->DrawIndexed(m_indexCount, 0, 0);

    m_swapChain->Present(1, 0);
}

void RendererDX11::runMainLoop(PhysicsManager* phys){
    MSG msg = {0};
    bool running = true;
    while(running){
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if(msg.message == WM_QUIT) running = false;
        }
        // step physics if provided
        if(phys && phys->dynamicsWorld()) phys->dynamicsWorld()->stepSimulation(1.0f/60.0f);
        if(phys) phys->processCollisionCallbacks();

        // update camera and check shader hot-reload
        updateCamera(1.0f/60.0f);
        checkShaderHotReload();

        renderFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void RendererDX11::shutdown(){
    cleanupSwapChain();
    m_vertexBuffer.Reset(); m_indexBuffer.Reset();
    m_vs.Reset(); m_ps.Reset(); m_inputLayout.Reset();
    m_context.Reset(); m_device.Reset();
}

void RendererDX11::cleanupSwapChain(){
    if(m_rtv) m_rtv.Reset();
    if(m_swapChain) m_swapChain.Reset();
}

bool RendererDX11::compileShaderFromFile(const std::wstring& filename, const std::string& entryPoint, const std::string& target, ID3DBlob** blobOut){
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(filename.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), 0, 0, &shaderBlob, &errorBlob);
    if(FAILED(hr)){
        if(errorBlob){
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        if(shaderBlob) shaderBlob->Release();
        return false;
    }
    *blobOut = shaderBlob;
    if(errorBlob) errorBlob->Release();
    return true;
}

// Win32 message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

#endif // _WIN32
