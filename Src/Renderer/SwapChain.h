#pragma once

#include "D3D_Helper.h"

class Swapchain
{
private:
    D3D::SwapChainPtr                                 m_swapchain          = nullptr;                   // スワップチェイン
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_render_target_view = nullptr;            // 描画先領域
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_depth_stencil_view = nullptr;            // 描画先の深度情報

private:
    Microsoft::WRL::ComPtr<IDXGIDevice>               m_dxgi_device = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter>              m_dxgi_adapter = nullptr;
    Microsoft::WRL::ComPtr<IDXGIFactory1>              m_dxgi_factory = nullptr;

public:
    Swapchain(D3D::DevicePtr& device, HWND& hwnd);
    void Resize(D3D::DevicePtr& device, unsigned int width, unsigned int height);
    bool present(bool vsync);
private:
    void ReloadBuffers(D3D::DevicePtr& device, unsigned int width, unsigned int height);

public:

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& getRenderTargetView();

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& getDepthStencilView();
    ~Swapchain();
};

