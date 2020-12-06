#pragma once

#include "D3D_Helper.h"

class Swapchain
{
private:
    D3D::SwapChainPtr                                 mpSwapChain          = nullptr;                   // �X���b�v�`�F�C��
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    mpRTV = nullptr;            // �`���̈�
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    mpDSV = nullptr;            // �`���̐[�x���

private:
    Microsoft::WRL::ComPtr<IDXGIDevice>               mpDxgiDevice = nullptr;
    Microsoft::WRL::ComPtr<IDXGIAdapter>              mpDxgiAdapther = nullptr;
    Microsoft::WRL::ComPtr<IDXGIFactory1>             mpDxgiFactory = nullptr;

public:
    Swapchain(D3D::DevicePtr& device, HWND& hwnd);
    ~Swapchain() = default;

    void Resize(D3D::DevicePtr& device, unsigned int width, unsigned int height);
    bool Present(bool vsync);
private:
    void ReloadBuffers(D3D::DevicePtr& device, unsigned int width, unsigned int height);

public:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return mpRTV; }
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return mpDSV; }
};

