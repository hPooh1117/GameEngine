#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "D3D_Helper.h"
#include "Blender.h"

#ifdef _DEBUG
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#else
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#endif

class Swapchain;
class RenderTargetView;

class GraphicsEngine
{
public:
    enum
    {
        DS_FALSE,
        DS_TRUE,
        DS_WRITE_FALSE,
        DS_TRUE_LESS_EQUAL,

        DS_TYPE_NUM_MAX,
    };

private:
    D3D::DevicePtr              m_pD3dDevice = nullptr;             
    D3D::DeviceContextPtr       m_pImmContext = nullptr;            
    D3D::DepthStencilPtr        m_pDepthStencilState[DS_TYPE_NUM_MAX];

    D3D_FEATURE_LEVEL           mFeatureLevel;

    std::unique_ptr<Swapchain>  m_pSwapChain = nullptr;


    D3D11_VIEWPORT mViewport;

public:
    static Blender              mBlender;


public:
    GraphicsEngine();
    ~GraphicsEngine();


    bool Initialize(HWND);
    void InitImGui(HWND hwnd);
    void ActivateBackBuffer();
    void BeginRender();
    void BeginImGuiRender();

    void EndImGuiRender();
    void EndRender();


    D3D::DevicePtr&        GetDevicePtr();
    D3D::DeviceContextPtr& GetImmContextPtr();
    D3D::RTVPtr& GetRTVPtr();
    D3D::DSVPtr& GetDSVPtr();
    D3D::DepthStencilPtr&  GetDepthStencilPtr(int ds_num);
    std::unique_ptr<Swapchain>& GetSwapchain() { return m_pSwapChain; }

    void SetDepthStencil(unsigned int type);
    void SetViewport(float width, float height);
};
