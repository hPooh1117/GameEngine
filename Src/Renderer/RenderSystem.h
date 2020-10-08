#pragma once
#include <memory>
#include <wrl\client.h>
#include <d3d11.h>

class SwapChain;

class RenderSystem
{
private:
    Microsoft::WRL::ComPtr<ID3D11Device>              mD3D_Device    = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>       mDeviceContext = nullptr;               // D3D11‚Ì•`‰æ‚ÌŠîŠ²ƒVƒXƒeƒ€
    D3D_FEATURE_LEVEL                                 mFeatureLevel  = D3D_FEATURE_LEVEL_11_0;
private:
    std::unique_ptr<SwapChain>                        m_pSwapChain;

private:
    Microsoft::WRL::ComPtr<IDXGIDevice>               mDXGI_Device  = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter>              mDXGI_Adapter = nullptr;
    Microsoft::WRL::ComPtr<IDXGIFactory>              mDXGI_Factory = nullptr;


public:
    RenderSystem();

    void createSwapChain();
    ID3D11Device* getD3D11Device();
    IDXGIFactory* getIDXGIFactory();
    ID3D11DeviceContext* getImmidiatelyContext();
    ~RenderSystem();
};

