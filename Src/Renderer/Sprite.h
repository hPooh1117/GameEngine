#pragma once
#include "Texture.h"
#include "Shader.h"
#include "D3D_Helper.h"
#include "GraphicsDevice.h"
#include "./Utilities/Vector.h"

#include <sstream>
#include <memory>
#include <iostream>
#include <string>

#include <DirectXMath.h>


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
    //Microsoft::WRL::ComPtr<ID3D11Buffer>             m_pVertexBuffer;
    //Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_pRasterizerState;
    std::unique_ptr<Graphics::GPUBuffer> mpVertexBuffer;

private:
    std::unique_ptr<Texture> mpTexture;

public:
    Sprite(Graphics::GraphicsDevice* p_device); // テクスチャなし
    Sprite(Graphics::GraphicsDevice* p_device, const wchar_t*);
    ~Sprite();

    // メンバー関数
    void Render(
        Graphics::GraphicsDevice* p_device,
        Shader* p_shader,
        const Vector2& pos,
        const Vector2& size,
        const Vector2& tex_pos,
        const Vector2& tex_size,
        const float angle,
        const Vector4& color);

    void Render(
        Graphics::GraphicsDevice* p_device,
        Shader* p_shader,
        std::unique_ptr<Texture>& p_texture,
        const Vector2& pos,
        const Vector2& size,
        const Vector2& tex_pos,
        const Vector2& tex_size,
        const float angle,
        const Vector4& color);

    void RenderScreen(
        Graphics::GraphicsDevice* p_device,
        Shader* p_shader,
        const Vector2& pos,
        const Vector2& size);

    
    void TextOutput(
        Graphics::GraphicsDevice* p_device,
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
    Microsoft::WRL::ComPtr<ID3D11Buffer>             mpVertexBuffer;
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
        Graphics::GraphicsDevice* p_device,
        const wchar_t*, 
        const size_t maxInstances = 256);
    
    ~SpriteBatch();

    // メンバー関数
    void Begin(Graphics::GraphicsDevice* p_device);
    
    void Render(
        Graphics::GraphicsDevice* p_device,
        const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&,
        const DirectX::XMFLOAT2&, 
        const DirectX::XMFLOAT2&, 
        const float, 
        const DirectX::XMFLOAT4&);

    void RenderText(
        Graphics::GraphicsDevice* p_device,
        const std::string& text,
        const DirectX::XMFLOAT2& pos,
        const DirectX::XMFLOAT2& size,
        const DirectX::XMFLOAT4& color);
  
    void End(Graphics::GraphicsDevice* p_device);

    // 頂点フォーマット
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texcoord;
    };

};