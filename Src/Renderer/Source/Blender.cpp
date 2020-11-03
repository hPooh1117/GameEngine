#include "Blender.h"

#include "./Utilities/misc.h"

Blender::Blender(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{

}

Blender::~Blender()
{
}

bool Blender::Initialize(D3D::DevicePtr& p_device)
{
    HRESULT hr = S_OK;

    // BLEND_NONE
    D3D11_BLEND_DESC bDesc = {};
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = false;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_NONE].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_ALPHA
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_ALPHA].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


    // BLEND_ADD
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_ADD].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_SUBSTRACT
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_SUBTRACT].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_REPLACE
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_REPLACE].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    //BLEND_MULTIPLY
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_MULTIPLY].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_LIGHTEN
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_LIGHTEN].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_DARKEN
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_DARKEN].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    // BLEND_SCREEN
    bDesc.AlphaToCoverageEnable = false;
    bDesc.IndependentBlendEnable = false;
    bDesc.RenderTarget[0].BlendEnable = true;
    bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
    bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = p_device->CreateBlendState(&bDesc, mBlendStates[BLEND_SCREEN].GetAddressOf());
    _ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

    return true;
}

void Blender::SetBlendState(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, const int blendType)
{
    imm_context->OMSetBlendState(mBlendStates[blendType].Get(), 0, 0xffffffff);
}
