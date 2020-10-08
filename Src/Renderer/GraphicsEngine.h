#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "D3D_Helper.h"

#ifdef _DEBUG
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#else
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#endif

class Sprite;
class Primitive;
class Blender;
class GeometricPrimiticeSelf;
class BasicCube;
class StaticMesh;
class SkinnedMesh;
class InputManager;

class Swapchain;

class GraphicsEngine
{
public:
    enum
    {
        DS_FALSE,
        DS_TRUE,
        DS_WRITE_FALSE,

        DS_TYPE_NUM_MAX,
    };

private:
    D3D::DevicePtr              m_pD3dDevice = nullptr;                      // D3D11の基幹システム
    D3D::DeviceContextPtr       m_pImmContext = nullptr;               // D3D11の描画の基幹システム
    D3D::DepthStencilPtr        m_pDepthStencilState[DS_TYPE_NUM_MAX];

    //Microsoft::WRL::ComPtr<IDXGISwapChain>            m_swapchain = nullptr;                   // スワップチェイン
    D3D_FEATURE_LEVEL mFeatureLevel;
private:
    //Microsoft::WRL::ComPtr<IDXGIDevice>               m_dxgi_device;
    //Microsoft::WRL::ComPtr<IDXGIAdapter>              m_dxgi_adapter;
    //Microsoft::WRL::ComPtr<IDXGIFactory1>              m_dxgi_factory;

    std::unique_ptr<Swapchain>                        m_pSwapChain = nullptr;

private:
    //Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    mRenderTargetView;            // 描画先領域
    //Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    mDepthStencilView;            // 描画先の深度情報
    //Microsoft::WRL::ComPtr<ID3D11Texture2D>           mDepthStencil;
    D3D11_VIEWPORT mViewport;



public:



    GraphicsEngine();
    ~GraphicsEngine();


    bool Initialize(HWND);
    void InitImGui(HWND hwnd);
    void ActivateRenderTarget();
    void BeginRender();
    void BeginImGuiRender();
    //void render(float elapsed_time/*Elapsed seconds from last frame*/);
    void EndImGuiRender();
    void EndRender();


    D3D::DevicePtr& GetDevicePtr();
    D3D::DeviceContextPtr& GetImmContextPtr();
    D3D::RenderTargetVPtr& GetRTVPtr();
    D3D::DepthStencilVPtr& GetDSVPtr();
    D3D::DepthStencilPtr& GetDepthStencilPtr(int ds_num);
    void SetViewport(float width, float height);
};
