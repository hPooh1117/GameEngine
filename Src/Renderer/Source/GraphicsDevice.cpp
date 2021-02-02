#include "GraphicsDevice.h"

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


using namespace DirectX;

namespace Graphics
{
    const void* const _nullBlob[128] = {};

    static const std::string SAMPLER_STATE_NAMES[SS_TYPE_NUM_MAX] =
    {
        "LINEAR_WRAP",
        "LINEAR_CLAMP",
        "LINEAR_MIRROR",

        "POINT_WRAP",
        "POINT_CLAMP",
        "POINT_MIRROR",

        "ANISO_WRAP",
        "ANISO_CLAMP",
        "ANISO_MIRROR",

        "COMPARISON_DEPTH"
    };

    static const std::string RASTERIZER_STATE_NAMES[RS_TYPE_NUM_MAX] =
    {
        "SOLID_FRONT",
        "SOLID_BACK",
        "SOLID_DOUBLESIDED",
        
        "WIRE_FRONT",
        "WIRE_BACK",
        "WIRE_SMOOTH",
        "WIRE_DOUBLESIDED"
        "WIRE_DOUBLESIDED_SMOOTH",

        "SPRITE",
        "SKYBOX",
        "MAYA_SOLID_BACK",
        "MAYA_WIRE_BACK",
    };

    constexpr uint32_t _ParseBindFlags(uint32_t value)
    {
        uint32_t _flag = 0;

        if (value & BIND_VERTEX_BUFFER)
            _flag |= D3D11_BIND_VERTEX_BUFFER;
        if (value & BIND_INDEX_BUFFER)
            _flag |= D3D11_BIND_INDEX_BUFFER;
        if (value & BIND_CONSTANT_BUFFER)
            _flag |= D3D11_BIND_CONSTANT_BUFFER;
        if (value & BIND_SHADER_RESOURCE)
            _flag |= D3D11_BIND_SHADER_RESOURCE;
        if (value & BIND_STREAM_OUTPUT)
            _flag |= D3D11_BIND_STREAM_OUTPUT;
        if (value & BIND_RENDER_TARGET)
            _flag |= D3D11_BIND_RENDER_TARGET;
        if (value & BIND_DEPTH_STENCIL)
            _flag |= D3D11_BIND_DEPTH_STENCIL;
        if (value & BIND_UNORDERED_ACCESS)
            _flag |= D3D11_BIND_UNORDERED_ACCESS;

        return _flag;
    }

    constexpr uint32_t _ParseCPUAccessFlags(uint32_t value)
    {
        uint32_t _flag = 0;

        if (value & CPU_ACCESS_WRITE)
            _flag |= D3D11_CPU_ACCESS_WRITE;
        if (value & CPU_ACCESS_READ)
            _flag |= D3D11_CPU_ACCESS_READ;

        return _flag;
    }

    constexpr uint32_t _ParseResourceMiscFlags(uint32_t value)
    {
        uint32_t _flag = 0;

        if (value & RESOURCE_MISC_SHARED)
            _flag |= D3D11_RESOURCE_MISC_SHARED;
        if (value & RESOURCE_MISC_TEXTURECUBE)
            _flag |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        if (value & RESOURCE_MISC_INDIRECT_ARGS)
            _flag |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        if (value & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
            _flag |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        if (value & RESOURCE_MISC_BUFFER_STRUCTURED)
            _flag |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        if (value & RESOURCE_MISC_TILED)
            _flag |= D3D11_RESOURCE_MISC_TILED;

        return _flag;
    }

    constexpr D3D11_USAGE _ConvertUsage(uint32_t value)
    {
        switch (value)
        {
        case USAGE_DEFAULT:
            return D3D11_USAGE_DEFAULT;
            break;
        case USAGE_IMMUTABLE:
            return D3D11_USAGE_IMMUTABLE;
            break;
        case USAGE_DYNAMIC:
            return D3D11_USAGE_DYNAMIC;
            break;
        case USAGE_STAGING:
            return D3D11_USAGE_STAGING;
            break;
        default:
            break;
        }
        return D3D11_USAGE_DEFAULT;
    }

    constexpr DXGI_FORMAT _ConvertFormat(uint32_t value)
    {
        switch (value)
        {
        case FORMAT_UNKNOWN:
            return DXGI_FORMAT_UNKNOWN;
            break;
        case FORMAT_R32G32B32A32_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case FORMAT_R32G32B32A32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
            break;
        case FORMAT_R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
            break;
        case FORMAT_R32G32B32_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case FORMAT_R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
            break;
        case FORMAT_R32G32B32_SINT:
            return DXGI_FORMAT_R32G32B32_SINT;
            break;
        case FORMAT_R16G16B16A16_FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case FORMAT_R16G16B16A16_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
            break;
        case FORMAT_R16G16B16A16_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case FORMAT_R16G16B16A16_SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
            break;
        case FORMAT_R16G16B16A16_SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case FORMAT_R32G32_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
            break;
        case FORMAT_R32G32_UINT:
            return DXGI_FORMAT_R32G32_UINT;
            break;
        case FORMAT_R32G32_SINT:
            return DXGI_FORMAT_R32G32_SINT;
            break;
        case FORMAT_R32G8X24_TYPELESS:
            return DXGI_FORMAT_R32G8X24_TYPELESS;
            break;
        case FORMAT_D32_FLOAT_S8X24_UINT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;
        case FORMAT_R10G10B10A2_UNORM:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
            break;
        case FORMAT_R10G10B10A2_UINT:
            return DXGI_FORMAT_R10G10B10A2_UINT;
            break;
        case FORMAT_R11G11B10_FLOAT:
            return DXGI_FORMAT_R11G11B10_FLOAT;
            break;
        case FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case FORMAT_R8G8B8A8_UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case FORMAT_R8G8B8A8_SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
        case FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case FORMAT_R16G16_FLOAT:
            return DXGI_FORMAT_R16G16_FLOAT;
            break;
        case FORMAT_R16G16_UNORM:
            return DXGI_FORMAT_R16G16_UNORM;
            break;
        case FORMAT_R16G16_UINT:
            return DXGI_FORMAT_R16G16_UINT;
            break;
        case FORMAT_R16G16_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
            break;
        case FORMAT_R16G16_SINT:
            return DXGI_FORMAT_R16G16_SINT;
            break;
        case FORMAT_R32_TYPELESS:
            return DXGI_FORMAT_R32_TYPELESS;
            break;
        case FORMAT_D32_FLOAT:
            return DXGI_FORMAT_D32_FLOAT;
            break;
        case FORMAT_R32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
            break;
        case FORMAT_R32_UINT:
            return DXGI_FORMAT_R32_UINT;
            break;
        case FORMAT_R32_SINT:
            return DXGI_FORMAT_R32_SINT;
            break;
        case FORMAT_R24G8_TYPELESS:
            return DXGI_FORMAT_R24G8_TYPELESS;
            break;
        case FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;
        case FORMAT_R8G8_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
            break;
        case FORMAT_R8G8_UINT:
            return DXGI_FORMAT_R8G8_UINT;
            break;
        case FORMAT_R8G8_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
            break;
        case FORMAT_R8G8_SINT:
            return DXGI_FORMAT_R8G8_SINT;
            break;
        case FORMAT_R16_TYPELESS:
            return DXGI_FORMAT_R16_TYPELESS;
            break;
        case FORMAT_R16_FLOAT:
            return DXGI_FORMAT_R16_FLOAT;
            break;
        case FORMAT_D16_UNORM:
            return DXGI_FORMAT_D16_UNORM;
            break;
        case FORMAT_R16_UNORM:
            return DXGI_FORMAT_R16_UNORM;
            break;
        case FORMAT_R16_UINT:
            return DXGI_FORMAT_R16_UINT;
            break;
        case FORMAT_R16_SNORM:
            return DXGI_FORMAT_R16_SNORM;
            break;
        case FORMAT_R16_SINT:
            return DXGI_FORMAT_R16_SINT;
            break;
        case FORMAT_R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;
            break;
        case FORMAT_R8_UINT:
            return DXGI_FORMAT_R8_UINT;
            break;
        case FORMAT_R8_SNORM:
            return DXGI_FORMAT_R8_SNORM;
            break;
        case FORMAT_R8_SINT:
            return DXGI_FORMAT_R8_SINT;
            break;
        case FORMAT_BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM;
            break;
        case FORMAT_BC1_UNORM_SRGB:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
            break;
        case FORMAT_BC2_UNORM:
            return DXGI_FORMAT_BC2_UNORM;
            break;
        case FORMAT_BC2_UNORM_SRGB:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
            break;
        case FORMAT_BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM;
            break;
        case FORMAT_BC3_UNORM_SRGB:
            return DXGI_FORMAT_BC3_UNORM_SRGB;
            break;
        case FORMAT_BC4_UNORM:
            return DXGI_FORMAT_BC4_UNORM;
            break;
        case FORMAT_BC4_SNORM:
            return DXGI_FORMAT_BC4_SNORM;
            break;
        case FORMAT_BC5_UNORM:
            return DXGI_FORMAT_BC5_UNORM;
            break;
        case FORMAT_BC5_SNORM:
            return DXGI_FORMAT_BC5_SNORM;
            break;
        case FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        case FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            break;
        case FORMAT_BC6H_UF16:
            return DXGI_FORMAT_BC6H_UF16;
            break;
        case FORMAT_BC6H_SF16:
            return DXGI_FORMAT_BC6H_SF16;
            break;
        case FORMAT_BC7_UNORM:
            return DXGI_FORMAT_BC7_UNORM;
            break;
        case FORMAT_BC7_UNORM_SRGB:
            return DXGI_FORMAT_BC7_UNORM_SRGB;
            break;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

    inline D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData)
    {
        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = pInitialData.pSysMem;
        data.SysMemPitch = pInitialData.SysMemPitch;
        data.SysMemSlicePitch = pInitialData.SysMemSlicePitch;

        return data;
    }

    Blender GraphicsDevice::mBlender = Blender();

    GraphicsDevice::GraphicsDevice() :mViewport({})
    {
    }

    bool GraphicsDevice::Initialize(HWND hwnd)
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
                mpD3dDevice.GetAddressOf(),
                &mFeatureLevel,
                mpImmContext.GetAddressOf()
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
        mpSwapChain = std::make_unique<Swapchain>(mpD3dDevice, hwnd);

        CreateDepthStencilStates();
        CreateRasterizerStates();
        CreateSamplerStates();
        mBlender.Initialize(mpD3dDevice);

        InitImGui(hwnd);

        return true;
    }

    void GraphicsDevice::InitImGui(HWND hwnd)
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
        ImGui_ImplDX11_Init(mpD3dDevice.Get(), mpImmContext.Get());
        ImGui::StyleColorsClassic();


        Log::Info("[ImGui] Initialized");
#endif //USE_IMGUI

    }

    void GraphicsDevice::CreateDepthStencilStates()
    {
        //-----------------------------------------
        // Create DepthStencilState
        //-----------------------------------------

        HRESULT result = S_OK;

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
        result = mpD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_FALSE].GetAddressOf());
        if (FAILED(result))
        {
            Log::Error("Couldn't Create DepthStencilState(DS_FALSE).");
        }

        dsDesc.DepthEnable = TRUE;
        //dsDesc.StencilEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        result = mpD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_TRUE].GetAddressOf());
        if (FAILED(result))
        {
            Log::Error("Couldn't Create DepthStencilState(DS_TRUE).");
        }

        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        result = mpD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_TRUE_LESS_EQUAL].GetAddressOf());
        if (FAILED(result))
        {
            Log::Error("Couldn't Create DepthStencilState(DS_TRUE_LESS_EQUAL).");
        }


        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        result = mpD3dDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState[DS_WRITE_FALSE].GetAddressOf());
        if (FAILED(result))
        {
            Log::Error("Couldn't Create DepthStencilState(DS_WRITE_FALSE).");
        }
        
    }

    void GraphicsDevice::CreateSamplerStates()
    {
        HRESULT result = S_OK;

        // LINEAR - WRAP
        D3D11_SAMPLER_DESC sd = {};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        sd.MinLOD = 0;
        sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sd.BorderColor[0] = 0;
        sd.BorderColor[1] = 0;
        sd.BorderColor[2] = 0;
        sd.BorderColor[3] = 0;
        sd.MipLODBias = 0;
        sd.MaxAnisotropy = 0;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_LINEAR_WRAP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_LINEAR_WRAP]);
        
        // LINEAR - CLAMP
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_LINEAR_CLAMP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_LINEAR_CLAMP]);

        // LINEAR - MIRROR
        sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_LINEAR_MIRROR].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_LINEAR_MIRROR]);

        // POINT - WRAP
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_POINT_WRAP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_POINT_WRAP]);

        // POINT - CLAMP
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_POINT_CLAMP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_POINT_CLAMP]);

        // POINT - MIRROR
        sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_POINT_MIRROR].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_POINT_MIRROR]);

        // ANISOTROPIC - WRAP
        sd.Filter = D3D11_FILTER_ANISOTROPIC;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.MaxAnisotropy = 16;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_ANISO_WRAP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_ANISO_WRAP]);

        // ANISOTROPIC - CLAMP
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxAnisotropy = 16;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_ANISO_CLAMP].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_ANISO_CLAMP]);

        // ANISOTROPIC - MIRROR
        sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        sd.MaxAnisotropy = 16;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_ANISO_MIRROR].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_ANISO_MIRROR]);

        // COMPARISON DEPTH (using shadow)
        sd.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
        sd.MaxAnisotropy = 0;
        mpD3dDevice->CreateSamplerState(&sd, mpSampler[SS_CMP_DEPTH].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create sampler state ( %s )", SAMPLER_STATE_NAMES[SS_CMP_DEPTH]);

    }

    void GraphicsDevice::CreateRasterizerStates()
    {
        HRESULT result = S_OK;

        D3D11_RASTERIZER_DESC rd = {};
        rd.FillMode = D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_FRONT;
        rd.FrontCounterClockwise = TRUE;
        rd.DepthBias = 0;
        rd.DepthBiasClamp = 0.0f;
        rd.SlopeScaledDepthBias = 0.0f;
        rd.DepthClipEnable = TRUE;
        rd.ScissorEnable = FALSE;
        rd.MultisampleEnable = FALSE;
        rd.AntialiasedLineEnable = FALSE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_FRONT].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_FRONT]);

        rd.FrontCounterClockwise = FALSE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_SKYBOX].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_SKYBOX]);

        rd.FrontCounterClockwise = FALSE;
        rd.CullMode = D3D11_CULL_BACK;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_BACK].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_BACK]);

        rd.CullMode = D3D11_CULL_NONE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_DOUBLESIDED].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_DOUBLESIDED]);


        rd.FillMode = D3D11_FILL_WIREFRAME;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_WIRE_DOUBLESIDED].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_WIRE_DOUBLESIDED]);

        rd.AntialiasedLineEnable = TRUE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_WIRE_DOUBLESIDED_SMOOTH].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_WIRE_DOUBLESIDED_SMOOTH]);


        rd.CullMode = D3D11_CULL_BACK;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_WIRE_SMOOTH].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_WIRE_SMOOTH]);

        rd.AntialiasedLineEnable = FALSE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_WIRE_BACK].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_WIRE_BACK]);

        rd.CullMode = D3D11_CULL_FRONT;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_WIRE_FRONT].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_WIRE_FRONT]);

        rd.FillMode = D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_NONE;
        rd.DepthClipEnable = FALSE;
        rd.FrontCounterClockwise = FALSE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_SPRITE].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_SPRITE]);

        rd.CullMode = D3D11_CULL_BACK;
        rd.FrontCounterClockwise = TRUE;
        rd.DepthClipEnable = TRUE;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_MAYA_BACK].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_MAYA_BACK]);

        rd.FillMode = D3D11_FILL_WIREFRAME;
        mpD3dDevice->CreateRasterizerState(&rd, mpRasterizerStates[RS_MAYA_WIRE_BACK].GetAddressOf());
        if (FAILED(result)) Log::Error("Couldn't create rasterizer state ( %s )", RASTERIZER_STATE_NAMES[RS_MAYA_WIRE_BACK]);
    }

    void GraphicsDevice::CreateBuffer(
        const GPUBufferDesc* p_desc, 
        const SubresourceData* p_initial_data,
        GPUBuffer* p_buffer)
    {
        HRESULT result = S_OK;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = p_desc->ByteWidth;
        desc.Usage = _ConvertUsage(p_desc->Usage);
        desc.BindFlags = _ParseBindFlags(p_desc->BindFlags);
        desc.CPUAccessFlags = _ParseCPUAccessFlags(p_desc->CPUAccessFlags);
        desc.MiscFlags = _ParseResourceMiscFlags(p_desc->MiscFlags);
        desc.StructureByteStride = p_desc->StructureByteStride;

        D3D11_SUBRESOURCE_DATA data = {};
        if (p_initial_data != nullptr)
        {
            data = _ConvertSubresourceData(*p_initial_data);
        }

        p_buffer->desc = *p_desc;

        result = mpD3dDevice->CreateBuffer(
            &desc,
            &data,
            p_buffer->buffer.GetAddressOf());
        assert(SUCCEEDED(result) && "GPUBuffer couldn't create.");
    }

    void GraphicsDevice::UpdateBuffer(
        GPUBuffer* p_buffer, 
        const void* data, 
        int data_size)
    {
        assert(p_buffer->desc.Usage != USAGE_IMMUTABLE && "Cannot update immutable GPUbuffer.");
        assert(static_cast<int>(p_buffer->desc.ByteWidth) >= data_size || data_size < 0 && "Data size is too big!");

        if (data_size == 0)
        {
            return;
        }

        data_size = (std::min)(static_cast<int>(p_buffer->desc.ByteWidth), data_size);

        if (p_buffer->desc.Usage == USAGE_DYNAMIC)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource = {};
            HRESULT hr = mpImmContext->Map(p_buffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            assert(SUCCEEDED(hr) && "GPUBuffer mapping failed!");
            memcpy(mappedResource.pData, data, (data_size >= 0 ? data_size : p_buffer->desc.ByteWidth));
            mpImmContext->Unmap(p_buffer->buffer.Get(), 0);
        }
        else if (p_buffer->desc.BindFlags & BIND_CONSTANT_BUFFER || data_size < 0)
        {
            mpImmContext->UpdateSubresource(p_buffer->buffer.Get(), 0, nullptr, data, 0, 0);
        }
        else
        {
            D3D11_BOX box = {};
            box.left = 0;
            box.right = static_cast<uint32_t>(data_size);
            box.top = 0;
            box.bottom = 1;
            box.front = 0;
            box.back = 1;
            mpImmContext->UpdateSubresource(p_buffer->buffer.Get(), 0, &box, data, 0, 0);
        }
    }

    void GraphicsDevice::BindVertexBuffer(
        const GPUBuffer* const* vertex_buffers,
        uint32_t slot,
        uint32_t count, 
        const uint32_t* strides, 
        const uint32_t* offsets)
    {
        assert(count <= 8);
        ID3D11Buffer* res[8] = { 0 };
        for (uint32_t i = 0; i < count; ++i)
        {
            res[i] = vertex_buffers[i] != nullptr ? vertex_buffers[i]->buffer.Get() : nullptr;
        }
        mpImmContext->IASetVertexBuffers(slot, count, res, strides, (offsets != nullptr ? offsets : reinterpret_cast<const uint32_t*>(_nullBlob)));
    }

    void GraphicsDevice::BindIndexBuffer(
        const GPUBuffer* p_index_buffer, 
        const IndexBufferFormat format,
        uint32_t offset)
    {
        ID3D11Buffer* res = p_index_buffer != nullptr ? p_index_buffer->buffer.Get() : nullptr;
        mpImmContext->IASetIndexBuffer(res, (format == IndexBufferFormat::INDEXFORMAT_16BIT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), offset);
    }

    void GraphicsDevice::BindConstantBuffer(
        ShaderStage stage, 
        const GPUBuffer* p_constant_buffer,
        uint32_t slot)
    {
        ID3D11Buffer* res = p_constant_buffer != nullptr ? p_constant_buffer->buffer.Get() : nullptr;
        switch (stage)
        {
        case Graphics::EVertexShader:
            mpImmContext->VSSetConstantBuffers(slot, 1, &res);
            break;
        case Graphics::EHullShader:
            mpImmContext->HSSetConstantBuffers(slot, 1, &res);
            break;
        case Graphics::EDomainShader:
            mpImmContext->DSSetConstantBuffers(slot, 1, &res);
            break;
        case Graphics::EGeometryShader:
            mpImmContext->GSSetConstantBuffers(slot, 1, &res);
            break;
        case Graphics::EPixelShader:
            mpImmContext->PSSetConstantBuffers(slot, 1, &res);
            break;
        case Graphics::EComputeShader:
            mpImmContext->CSSetConstantBuffers(slot, 1, &res);
            break;
        default:
            assert(0);
            break;
        }
    }

    void GraphicsDevice::BindPrimitiveTopology(PrimitiveTopologyID primitive_topology)
    {
        D3D11_PRIMITIVE_TOPOLOGY d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        switch (primitive_topology)
        {
        case TRIANGLELIST:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case TRIANGLESTRIP:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        case POINTLIST:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case LINELIST:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case LINESTRIP:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        case PATCHLIST:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
            break;
        default:
            d3dType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
            break;
        }

        mpImmContext->IASetPrimitiveTopology(d3dType);
    }


    void GraphicsDevice::ActivateBackBuffer()
    {
        RSSetViewports(SCREEN_WIDTH, SCREEN_HEIGHT);

        float ClearColor[4] = { 0.456f, 0.456f, 0.456f, 1.0f };

        mpImmContext->ClearRenderTargetView(mpSwapChain->GetRenderTargetView().Get(), ClearColor);
        mpImmContext->ClearDepthStencilView(mpSwapChain->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
        mpImmContext->OMSetRenderTargets(1, mpSwapChain->GetRenderTargetView().GetAddressOf(), mpSwapChain->GetDepthStencilView().Get());
    }


    void GraphicsDevice::BeginRender()
    {
        ActivateBackBuffer();
    }

    void GraphicsDevice::BeginImGuiRender()
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

    void GraphicsDevice::Draw(uint32_t vertex_count, uint32_t start_vertex_location)
    {
        mpImmContext->Draw(vertex_count, start_vertex_location);
    }

    void GraphicsDevice::DrawIndexed(uint32_t index_count, uint32_t start_index_location, uint32_t base_vertex_location)
    {
        mpImmContext->DrawIndexed(index_count, start_index_location, base_vertex_location);
    }

    void GraphicsDevice::DrawInstanced(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex_location, uint32_t start_instance_location)
    {
        mpImmContext->DrawInstanced(vertex_count, instance_count, start_vertex_location, start_instance_location);
    }

    void GraphicsDevice::DrawIndexedInstanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index_location, uint32_t base_vertex_location, uint32_t start_instance_location)
    {
        mpImmContext->DrawIndexedInstanced(index_count, instance_count, start_index_location, base_vertex_location, start_instance_location);
    }



    void GraphicsDevice::EndImGuiRender()
    {
#ifdef  USE_IMGUI

        //imgui render
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif //  USE_IMGUI

    }

    void GraphicsDevice::EndRender()
    {
        //EndImGuiRender();
        // Flip
        mTimer.Start();
        mpSwapChain->Present(1);
        mTimer.Stop();


    }

    void GraphicsDevice::RenderUI()
    {
        ImGui::Text("Elapsed Time on GPU : %.5f", mTimer.GetDeltaTime());
    }

    Microsoft::WRL::ComPtr<ID3D11Device>& GraphicsDevice::GetDevicePtr()
    {
        return mpD3dDevice;
    }

    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GraphicsDevice::GetImmContextPtr()
    {
        return mpImmContext;
    }

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GraphicsDevice::GetRTVPtr()
    {
        return mpSwapChain->GetRenderTargetView();
    }

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GraphicsDevice::GetDSVPtr()
    {
        return mpSwapChain->GetDepthStencilView();
    }


    void GraphicsDevice::RSSetViewports(float width, float height)
    {
        mViewport.Width = width;
        mViewport.Height = height;
        mViewport.MinDepth = 0.0f;
        mViewport.MaxDepth = 1.0f;
        mViewport.TopLeftX = 0;
        mViewport.TopLeftY = 0;
        mpImmContext->RSSetViewports(1, &mViewport);
    }

    void GraphicsDevice::SetSamplers(uint16_t sampler_id, uint16_t slot)
    {
        mpImmContext->PSSetSamplers(slot, 1, mpSampler[sampler_id].GetAddressOf());
        mpImmContext->CSSetSamplers(slot, 1, mpSampler[sampler_id].GetAddressOf());
    }

    void GraphicsDevice::RSSetState(uint16_t rasterizer_id)
    {
        mpImmContext->RSSetState(mpRasterizerStates[rasterizer_id].Get());
    }

    void GraphicsDevice::OMSetDepthStencilState(uint16_t depth_stencil_id)
    {
        if (depth_stencil_id >= DS_TYPE_NUM_MAX)
        {
            Log::Error("Couldn't set depth stencil state. type is out of range.");
            return;
        }
        mpImmContext->OMSetDepthStencilState(m_pDepthStencilState[depth_stencil_id].Get(), 1);
    }

    GraphicsDevice::~GraphicsDevice()
    {

    }
}
