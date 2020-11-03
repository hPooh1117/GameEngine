#pragma once
#include "D3D_Helper.h"

class Blender
{
public:
    enum BlendState
    {
        BLEND_NONE,
        BLEND_ALPHA,
        BLEND_ADD,
        BLEND_SUBTRACT,
        BLEND_REPLACE,
        BLEND_MULTIPLY,
        BLEND_LIGHTEN,
        BLEND_DARKEN,
        BLEND_SCREEN,
        BLEND_TYPE_ALL,
    };

    Blender() = default;
    Blender(Microsoft::WRL::ComPtr<ID3D11Device>& device);
    ~Blender();
    bool Initialize(D3D::DevicePtr& p_device);
    void SetBlendState(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, const int blendType);

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendStates[BLEND_TYPE_ALL];
};
