
#include <sstream>
#include <memory>
#include <iostream>
#include <string>

#include <DirectXMath.h>
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"

class NewTexture;
class Shader;

class Sprite
{
private:
    // 頂点フォーマット
    struct vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texcoord;
        DirectX::XMFLOAT4 color;
    };


private:
    // メンバー変数
    Microsoft::WRL::ComPtr<ID3D11Buffer>             m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_pRasterizerState;

private:
    std::unique_ptr<NewTexture> m_pTexture;

public:
    Sprite(D3D::DevicePtr& device); // テクスチャなし
    Sprite(D3D::DevicePtr& device, const wchar_t*);
    ~Sprite();

    // メンバー関数
    void Render(
        D3D::DeviceContextPtr& imm_context,
        Shader* p_shader,
        const Vector2& pos,
        const Vector2& size,
        const Vector2& tex_pos,
        const Vector2& tex_size,
        const float angle,
        const Vector4& color);

    void Render(
        D3D::DeviceContextPtr& imm_context,
        Shader* p_shader,
        std::unique_ptr<NewTexture>& p_texture,
        const Vector2& pos,
        const Vector2& size,
        const Vector2& tex_pos,
        const Vector2& tex_size,
        const float angle,
        const Vector4& color);

    void RenderScreen(
        D3D::DeviceContextPtr& imm_context,
        Shader* p_shader,
        const Vector2& pos,
        const Vector2& size);

    
    void TextOutput(
        D3D::DeviceContextPtr& imm_context,
        const std::string& str, 
        Shader* p_shader,
        const Vector2& pos, 
        const Vector2& size,
        const Vector4& color);


    Sprite(const Sprite& other) = delete;
    Sprite& operator=(const Sprite& other) = delete;
};

class SpriteBatch
{
private:
    // メンバー変数
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        mInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    mRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRView;            
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       mSamplerState;      
    D3D11_TEXTURE2D_DESC                             mTexDesc;           
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  mDepthStencilState; 
                            
    const size_t MAX_INSTANCES = 256;
    struct Instance {
        DirectX::XMFLOAT4X4 ndc_transform;
        DirectX::XMFLOAT4 texcoord_transform;
        DirectX::XMFLOAT4 color;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mInstanceBuffer;

    D3D11_VIEWPORT mViewport;
    UINT mCountInstances = 0;
    Instance* mInstance;


public:
    SpriteBatch(
        Microsoft::WRL::ComPtr<ID3D11Device>& device,
        const wchar_t*, 
        const size_t maxInstances = 256);
    
    ~SpriteBatch();

    // メンバー関数
    void Begin(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);
    
    void Render(
        D3D::DeviceContextPtr& imm_context,
        const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, 
        const DirectX::XMFLOAT2&, 
        const float, 
        const DirectX::XMFLOAT4&);

    void RenderText(
        D3D::DeviceContextPtr& imm_context,
        const std::string& text,
        const DirectX::XMFLOAT2& pos,
        const DirectX::XMFLOAT2& size,
        const DirectX::XMFLOAT4& color);
  
    void End(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& imm_context);

    // 頂点フォーマット
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texcoord;
    };

};