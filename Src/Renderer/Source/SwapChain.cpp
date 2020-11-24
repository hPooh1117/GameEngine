#include "SwapChain.h"

#include <exception>

#include "./Application/Application.h"
#include "./Application/Helper.h"

#include "./Utilities/misc.h"
#include "./Utilities/Log.h"


Swapchain::Swapchain(Microsoft::WRL::ComPtr<ID3D11Device>& device, HWND& hwnd)
{
    HRESULT result = S_OK;
    //{
    //    DXGI_SAMPLE_DESC MSAA;
    //    for (auto i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++i)
    //    {
    //        UINT quality;
    //        result = device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &quality);
    //        if (quality > 0)
    //        {
    //            MSAA.Count = i;
    //            MSAA.Quality = quality - 1;
    //        }
    //    }

    //}
    {
        DXGI_SWAP_CHAIN_DESC scDesc = {};
        ZeroMemory(&scDesc, sizeof(scDesc));
        scDesc.BufferDesc.Width = SCREEN_WIDTH;
        scDesc.BufferDesc.Height = SCREEN_HEIGHT;
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferDesc.RefreshRate.Numerator = 60;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.BufferCount = 8;
        scDesc.OutputWindow = hwnd;
        scDesc.Windowed = TRUE;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        scDesc.Flags = 0;

        result = device.Get()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(m_dxgi_device.GetAddressOf()));
        result = m_dxgi_device->GetAdapter(m_dxgi_adapter.GetAddressOf());
        result = m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_dxgi_factory.GetAddressOf()));
        result = m_dxgi_factory->CreateSwapChain(
            device.Get(),
            &scDesc,
            m_swapchain.GetAddressOf()
        );
        if (FAILED(result))
        {
            Log::Error("Couldn't Create Swapchain.");
        }
    }
    //hr = ->getIDXGIFactory()->CreateSwapChain(renderSystem->getD3D11Device(), &scDesc, mSwapChain.GetAddressOf());
    //if (FAILED(hr))
    //{
    //    throw std::exception("Swapchain not created successfully");
    //}

   ID3D11Texture2D* pTexture2D;
    result = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&pTexture2D));
    pTexture2D->Release();
    _ASSERT_EXPR(SUCCEEDED(result), hr_trace(result));
    
    result = device->CreateRenderTargetView(pTexture2D, NULL, m_render_target_view.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create RenderTargetView(BackBuffer).");
    }

    D3D11_TEXTURE2D_DESC txDesc = {};
    txDesc.Width = SCREEN_WIDTH;
    txDesc.Height = SCREEN_HEIGHT;
    txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    txDesc.Usage = D3D11_USAGE_DEFAULT;
    txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    txDesc.MipLevels = 1;
    txDesc.SampleDesc.Count = 1;
    txDesc.SampleDesc.Quality = 0;
    txDesc.MiscFlags = 0;
    txDesc.ArraySize = 1;
    txDesc.CPUAccessFlags = 0;

    result = device->CreateTexture2D(&txDesc, nullptr, &pTexture2D);
    _ASSERT_EXPR(SUCCEEDED(result), hr_trace(result));

    result = device->CreateDepthStencilView(pTexture2D, NULL, m_depth_stencil_view.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create DepthStencilView.");
    }


}

void Swapchain::Resize(D3D::DevicePtr& device, unsigned int width, unsigned int height)
{
    if (m_render_target_view) m_render_target_view.Reset();
    if (m_depth_stencil_view) m_depth_stencil_view.Reset();

    m_swapchain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ReloadBuffers(device, width, height);
}

bool Swapchain::present(bool vsync)
{
    m_swapchain->Present(vsync, 0);
    return true;
}

void Swapchain::ReloadBuffers(D3D::DevicePtr& device, unsigned int width, unsigned int height)
{
    ID3D11Texture2D* pTexture2D;
    auto result = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&pTexture2D));
    pTexture2D->Release();
    _ASSERT_EXPR(SUCCEEDED(result), hr_trace(result));

    result = device->CreateRenderTargetView(pTexture2D, NULL, m_render_target_view.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create RenderTargetView(BackBuffer).");
    }

    D3D11_TEXTURE2D_DESC txDesc = {};
    txDesc.Width = width;
    txDesc.Height = height;
    txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    txDesc.Usage = D3D11_USAGE_DEFAULT;
    txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    txDesc.MipLevels = 1;
    txDesc.SampleDesc.Count = 1;
    txDesc.SampleDesc.Quality = 0;
    txDesc.MiscFlags = 0;
    txDesc.ArraySize = 1;
    txDesc.CPUAccessFlags = 0;

    result = device->CreateTexture2D(&txDesc, nullptr, &pTexture2D);
    _ASSERT_EXPR(SUCCEEDED(result), hr_trace(result));

    result = device->CreateDepthStencilView(pTexture2D, NULL, m_depth_stencil_view.GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create DepthStencilView.");
    }

}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& Swapchain::getRenderTargetView()
{
    return m_render_target_view;
}


Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& Swapchain::getDepthStencilView()
{
    return m_depth_stencil_view;
}


Swapchain::~Swapchain()
{
}
