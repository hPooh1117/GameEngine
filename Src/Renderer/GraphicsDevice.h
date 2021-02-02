#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "D3D_Helper.h"
#include "Blender.h"
#include "GPUBuffer.h"
#include "EnumsForRenderer.h"
#include "./Utilities/PerfTimer.h"
#include "./Utilities/CommonInclude.h"

#ifdef _DEBUG
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#else
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#endif

class Swapchain;
class RenderTargetView;

namespace Graphics
{
    enum DepthStencilID
    {
        DS_FALSE,
        DS_TRUE,
        DS_WRITE_FALSE,
        DS_TRUE_LESS_EQUAL,

        DS_TYPE_NUM_MAX,
    };

    enum SamplerStateID
    {
        SS_LINEAR_WRAP,
        SS_LINEAR_CLAMP,
        SS_LINEAR_MIRROR,

        SS_POINT_WRAP,
        SS_POINT_CLAMP,
        SS_POINT_MIRROR,

        SS_ANISO_WRAP,
        SS_ANISO_CLAMP,
        SS_ANISO_MIRROR,

        SS_CMP_DEPTH,

        SS_TYPE_NUM_MAX,
    };

    enum RasterizerStateID
    {
        RS_FRONT,
        RS_BACK,
        RS_DOUBLESIDED,
        
        RS_WIRE_FRONT,
        RS_WIRE_BACK,
        RS_WIRE_SMOOTH,
        RS_WIRE_DOUBLESIDED,
        RS_WIRE_DOUBLESIDED_SMOOTH,

        RS_SPRITE,
        RS_SKYBOX,
        RS_MAYA_BACK,
        RS_MAYA_WIRE_BACK,

        RS_TYPE_NUM_MAX,
    };

    enum PrimitiveTopologyID
    {
        TRIANGLELIST,
        TRIANGLESTRIP,
        POINTLIST,
        LINELIST,
        LINESTRIP,
        PATCHLIST,
    };

class GraphicsDevice
{
public:

private:
    D3D::DevicePtr              mpD3dDevice = nullptr;             
    D3D::DeviceContextPtr       mpImmContext = nullptr;            
    D3D::DepthStencilPtr        m_pDepthStencilState[DS_TYPE_NUM_MAX];
    D3D::SamplerStatePtr        mpSampler[SS_TYPE_NUM_MAX];
    D3D::RasterizerPtr          mpRasterizerStates[RS_TYPE_NUM_MAX];
    D3D_FEATURE_LEVEL           mFeatureLevel;

    std::unique_ptr<Swapchain>  mpSwapChain = nullptr;
    PerfTimer mTimer;

    D3D11_VIEWPORT mViewport;

public:
    static Blender              mBlender;


public:
    GraphicsDevice();
    ~GraphicsDevice();


    bool Initialize(HWND);
    void InitImGui(HWND hwnd);

    void CreateBuffer(
        const Graphics::GPUBufferDesc* p_desc,
        const Graphics::SubresourceData* p_data,
        Graphics::GPUBuffer* p_buffer);

private:
    void CreateDepthStencilStates();
    void CreateSamplerStates();
    void CreateRasterizerStates();

public:

    void UpdateBuffer(GPUBuffer* p_buffer, const void* data, int data_size);

    void BindVertexBuffer(
        const Graphics::GPUBuffer* const* vertex_buffers,
        uint32_t slot,
        uint32_t count,
        const uint32_t* strides, 
        const uint32_t* offsets);

    void BindIndexBuffer(
        const Graphics::GPUBuffer* index_buffer,
        const IndexBufferFormat format,
        uint32_t offset);

    void BindConstantBuffer(
        Graphics::ShaderStage stage,
        const Graphics::GPUBuffer* buffer,
        uint32_t slot);
    void BindPrimitiveTopology(
        PrimitiveTopologyID primitive_topology);

    void ActivateBackBuffer();
    void BeginRender();
    void BeginImGuiRender();

    void Draw(
        uint32_t vertex_count, 
        uint32_t start_vertex_location);
    void DrawIndexed(
        uint32_t index_count, 
        uint32_t start_index_location,
        uint32_t base_vertex_location);
    void DrawInstanced(
        uint32_t vertex_count,
        uint32_t instance_count,
        uint32_t start_vertex_location,
        uint32_t start_instance_location);
    void DrawIndexedInstanced(
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t start_index_location,
        uint32_t base_vertex_location,
        uint32_t start_instance_location);

    void EndImGuiRender();
    void EndRender();

    void RenderUI();

    D3D::DevicePtr&        GetDevicePtr();
    D3D::DeviceContextPtr& GetImmContextPtr();
    D3D::RTVPtr& GetRTVPtr();
    D3D::DSVPtr& GetDSVPtr();
    std::unique_ptr<Swapchain>& GetSwapchain() { return mpSwapChain; }

    void SetSamplers(uint16_t sampler_id, uint16_t slot);
    void RSSetState(uint16_t rasterizer_id);
    void OMSetDepthStencilState(uint16_t type);
    void RSSetViewports(float width, float height);
};

}
