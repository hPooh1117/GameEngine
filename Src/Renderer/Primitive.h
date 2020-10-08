#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
class Shader;

class Primitive
{
private:
    // メンバー変数
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        mInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    mRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  mDepthStencilState;

public:

    Primitive(Microsoft::WRL::ComPtr<ID3D11Device>& device);
    ~Primitive();

    // render for 2D game
    void Render(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context,
        DirectX::XMFLOAT2&,
        DirectX::XMFLOAT2&, 
        float, 
        DirectX::XMFLOAT4&, 
        std::shared_ptr<Shader>& shader,
        bool isSolid = true);


    struct Vertex 
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT4 color;
    };
};

class PrimitiveBatch
{
private:
    // メンバー変数
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        mInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    mRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  mDepthStencilState;

    const UINT MAX_INSTANCES = 256;
    struct Instance
    {
        DirectX::XMFLOAT4X4 ndc_transform;
        DirectX::XMFLOAT4 color;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mInstanceBuffer;
    Instance* mInstances;
    UINT mCountInstances;
    D3D11_VIEWPORT mViewport;

public:

    PrimitiveBatch(Microsoft::WRL::ComPtr<ID3D11Device>& device, UINT numInstances = 256);
    ~PrimitiveBatch();

    void Begin(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
    void DrawRect(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context, DirectX::XMFLOAT2&, DirectX::XMFLOAT2&, float, DirectX::XMFLOAT4&);
    void End(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);

    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
    };

};