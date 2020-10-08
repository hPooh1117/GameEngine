#include "GraphicsEngine.h"

#include <algorithm>
#include <deque>

#include "SwapChain.h"
#include "sprite.h"
#include "Primitive.h"
#include "Blender.h"
#include "GeometricPrimitiveSelf.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"

#include "./Application/Application.h"
#include "./Application/Helper.h"

#include "./Utilities/ImguiSelf.h"
#include "./Utilities/Log.h"
#include "./Utilities/misc.h"

// Unit 8

using namespace DirectX;
// ウィンドウハンドル　：　管理番号

GraphicsEngine::GraphicsEngine() :mViewport({})
{
}

bool GraphicsEngine::Initialize(HWND hwnd)
{
    HRESULT result = S_OK;
    UINT deviceFlag = 0;

#ifdef _DEBUG
    deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif


    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    // usually used 11_0 level
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    D3D_FEATURE_LEVEL levels = D3D_FEATURE_LEVEL_11_0;

    //-----------------------------------------
    // Create Device, DeviceContext
    //-----------------------------------------
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        result = D3D11CreateDevice(
            NULL, 
            driverTypes[driverTypeIndex],
            NULL, 
            NULL,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            m_pD3dDevice.GetAddressOf(),
            &mFeatureLevel,
            m_pImmContext.GetAddressOf()
            );
        if (SUCCEEDED(result))
            break;
    }
    if (FAILED(result))
    {
        Log::Error("Couldn't create Device and DeviceContext");
        return false;
    }


    //-----------------------------------------
    // Create SwapChain
    //-----------------------------------------
    m_pSwapChain = std::make_unique<Swapchain>(m_pD3dDevice, hwnd);

    //-----------------------------------------
    // Create DepthStencilState
    //-----------------------------------------

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = FALSE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xff;
    dsDesc.StencilWriteMask = 0xff;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    result = m_pD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_FALSE].GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create DepthStencilState(DS_FALSE).");
    }

    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    result = m_pD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_TRUE].GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create DepthStencilState(DS_TRUE).");
    }

    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    result = m_pD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_WRITE_FALSE].GetAddressOf());
    if (FAILED(result))
    {
        Log::Error("Couldn't Create DepthStencilState(DS_WRITE_FALSE).");
    }


    InitImGui(hwnd);

    return true;
}

void GraphicsEngine::InitImGui(HWND hwnd)
{
#ifdef USE_IMGUI

    // set up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // set japanese font
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, glyphRangesJapanese);

    // set up platform / renderer
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_pD3dDevice.Get(), m_pImmContext.Get());
    ImGui::StyleColorsDark();

    Log::Info("[ImGui] Initialized");
#endif //USE_IMGUI

}

void GraphicsEngine::ActivateRenderTarget()
{
    // Unit1 7-①　ビューポートの設定 （ビューポート：描画範囲）
    SetViewport(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Unit1 7-②　レンダーターゲットのクリア
    //float ClearColor[4] = { 0.796f, 0.796f, 0.597f, 1.0f }; //R, G, B, A
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f }; //R, G, B, A

    m_pImmContext->ClearRenderTargetView(m_pSwapChain->getRenderTargetView().Get(), ClearColor);

    // Unit1 7-③　深度ステンシル　リソースのクリア
    m_pImmContext->ClearDepthStencilView(m_pSwapChain->getDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Unit1 7-④　レンダーターゲットと深度ステンシルバッファ―のバインド
    m_pImmContext->OMSetRenderTargets(1, m_pSwapChain->getRenderTargetView().GetAddressOf(), m_pSwapChain->getDepthStencilView().Get());

    m_pImmContext->OMSetDepthStencilState(m_pDepthStencilState[TRUE].Get(), 1);

}

void GraphicsEngine::BeginRender()
{
    //BeginImGuiRender();

    ActivateRenderTarget();

}

void GraphicsEngine::BeginImGuiRender()
{
#ifdef  USE_IMGUI

    // imgui new frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(350, 450), ImGuiCond_Once);

#endif //  USE_IMGUI

}



void GraphicsEngine::EndImGuiRender()
{
#ifdef  USE_IMGUI

    //imgui render
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif //  USE_IMGUI

}

void GraphicsEngine::EndRender()
{
    //EndImGuiRender();
    // Flip
    m_pSwapChain->present(1);
}

Microsoft::WRL::ComPtr<ID3D11Device>& GraphicsEngine::GetDevicePtr()
{
    return m_pD3dDevice;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GraphicsEngine::GetImmContextPtr()
{
    return m_pImmContext;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GraphicsEngine::GetRTVPtr()
{
    return m_pSwapChain->getRenderTargetView();
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GraphicsEngine::GetDSVPtr()
{
    return m_pSwapChain->getDepthStencilView();
}

D3D::DepthStencilPtr& GraphicsEngine::GetDepthStencilPtr(int ds_num)
{
    assert(ds_num < DS_TYPE_NUM_MAX&& ds_num >= 0);
    return m_pDepthStencilState[ds_num];
}

void GraphicsEngine::SetViewport(float width, float height)
{
    mViewport.Width = width;
    mViewport.Height = height;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    m_pImmContext->RSSetViewports(1, &mViewport);


}


GraphicsEngine::~GraphicsEngine()
{

}