#include "RenderSystem.h"

#include "Application.h"
#include "SwapChain.h"


RenderSystem::RenderSystem()
    :m_pSwapChain(nullptr)
{
    HRESULT hr = S_OK;
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
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    

    for (UINT driver_type_index = 0; driver_type_index < numDriverTypes;)
    {
        hr = D3D11CreateDevice(
            NULL,
            driverTypes[driver_type_index],
            NULL,
            NULL,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            mD3D_Device.GetAddressOf(),
            &mFeatureLevel,
            mDeviceContext.GetAddressOf()
            );

        if (SUCCEEDED(hr))
            break;
        ++driver_type_index;
    }

    if (FAILED(hr))
        return;

    mD3D_Device.As(&mDXGI_Device);
    mDXGI_Device->GetAdapter(&mDXGI_Adapter);
    mDXGI_Adapter->GetParent(IID_PPV_ARGS(&mDXGI_Factory));
}

void RenderSystem::createSwapChain()
{
    m_pSwapChain = std::make_unique<SwapChain>(this);
}

ID3D11Device* RenderSystem::getD3D11Device()
{
    return mD3D_Device.Get();
}

IDXGIFactory* RenderSystem::getIDXGIFactory()
{
    return mDXGI_Factory.Get();
}

ID3D11DeviceContext * RenderSystem::getImmidiatelyContext()
{
    return mDeviceContext.Get();
}


RenderSystem::~RenderSystem()
{
}
