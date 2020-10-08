#pragma once

#include <wrl\client.h>
#include <d3d11.h>


class Swapchain
{
private:
    Microsoft::WRL::ComPtr<IDXGISwapChain>            m_swapchain        = nullptr;                   // スワップチェイン
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_render_target_view = nullptr;            // 描画先領域
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_depth_stencil_view = nullptr;            // 描画先の深度情報

private:
    Microsoft::WRL::ComPtr<IDXGIDevice>               m_dxgi_device = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter>              m_dxgi_adapter = nullptr;
    Microsoft::WRL::ComPtr<IDXGIFactory1>              m_dxgi_factory = nullptr;

public:
    Swapchain(Microsoft::WRL::ComPtr<ID3D11Device>& device, HWND& hwnd);
    bool present(bool vsync);
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& getRenderTargetView();

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& getDepthStencilView();
    ~Swapchain();
};

