#pragma once
#include <d3d11.h>
#include <wrl/client.h>

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

    Blender(Microsoft::WRL::ComPtr<ID3D11Device>& device);
    ~Blender();
    void SetBlendState(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, const int blendType);

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendStates[BLEND_TYPE_ALL];
};
