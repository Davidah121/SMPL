#pragma once

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_5.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

class DXSingleton
{
public:

    static IDXGISwapChain* getSwapChain();
    static ID3D11Device* getDevice();
    static ID3D11DeviceContext* getContext();
    static ID3D11RenderTargetView* getBackBuffer();

    static void init(HWND outputWindowHandle, bool windowed);
    static void dispose();
    
private:
    DXSingleton(HWND outputWindowHandle, bool windowed);
    ~DXSingleton();

    IDXGISwapChain* d3dSwapChain = nullptr;
    ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* d3dContext = nullptr;
    ID3D11RenderTargetView* backBuffer = nullptr; 

    static DXSingleton* singleton;
};

inline DXSingleton* DXSingleton::singleton = nullptr;

inline void DXSingleton::init(HWND outputWindowHandle, bool windowed)
{
    if(singleton != nullptr)
        delete singleton;

    singleton = new DXSingleton(outputWindowHandle, windowed);
}

inline void DXSingleton::dispose()
{
    if(singleton != nullptr)
        delete singleton;

    singleton = nullptr;
}

inline DXSingleton::DXSingleton(HWND outputWindowHandle, bool windowed)
{
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.SampleDesc.Count = 1;
    scd.Windowed = windowed;
    scd.OutputWindow = outputWindowHandle;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &d3dSwapChain, &d3dDevice, NULL, &d3dContext);

    ID3D11Texture2D* pBackBuff;

    d3dSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuff);
    d3dDevice->CreateRenderTargetView(pBackBuff, NULL, &backBuffer);
    pBackBuff->Release();

    d3dContext->OMSetRenderTargets(1, &backBuffer, nullptr);
}

inline DXSingleton::~DXSingleton()
{
    if(backBuffer != nullptr)
        backBuffer->Release();
    if(d3dSwapChain != nullptr)
        d3dSwapChain->Release();
    if(d3dDevice != nullptr)
        d3dDevice->Release();
    if(d3dContext != nullptr)
        d3dContext->Release();

    d3dSwapChain = nullptr;
    d3dDevice = nullptr;
    d3dContext = nullptr;
    backBuffer = nullptr; 
}

inline IDXGISwapChain* DXSingleton::getSwapChain()
{
    if(singleton != nullptr)
    {
        return singleton->d3dSwapChain;
    }
    return nullptr;
}
inline ID3D11Device* DXSingleton::getDevice()
{
    if(singleton != nullptr)
    {
        return singleton->d3dDevice;
    }
    return nullptr;
}
inline ID3D11DeviceContext* DXSingleton::getContext()
{
    if(singleton != nullptr)
    {
        return singleton->d3dContext;
    }
    return nullptr;
}
inline ID3D11RenderTargetView* DXSingleton::getBackBuffer()
{
    if(singleton != nullptr)
    {
        return singleton->backBuffer;
    }
    return nullptr;
}