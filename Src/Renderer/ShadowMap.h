#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "D3D_Helper.h"

class GraphicsEngine;
class Shader;
class Light;
class CameraController;
class Texture;
class LightController;


class ShadowMap
{
private:
	//std::unique_ptr<Shader> m_pShadowMapShader;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pShadowMapTexture;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pShadowMapDepth;
	//D3D::RenderTargetVPtr m_pRTVShadowMap;
	//D3D::DepthStencilVPtr m_pDSVShadowMap;
	//std::unique_ptr<Texture> m_pShadowTex;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBufferForShadow;

	struct CBufferForShadow
	{
		DirectX::XMFLOAT4X4 shadowVP;
		//DirectX::XMFLOAT4X4 nearProjection;
		//DirectX::XMFLOAT4X4 midProjection;
		//DirectX::XMFLOAT4X4 farProjection;
	};

	DirectX::XMFLOAT4X4 mLightViewProj;

	float mDistanceToLight;

public:
	ShadowMap(D3D::DevicePtr& device/*Microsoft::WRL::ComPtr<ID3D11Device>& device*/);

	//void Activate(
	//	std::unique_ptr<GraphicsEngine>& p_graphics, 
	//	std::shared_ptr<Light>& p_light,
	//	std::shared_ptr<CameraController>& p_camera);

	void Activate(
		const std::unique_ptr<GraphicsEngine>& p_graphics,
		LightController* p_light,
		CameraController* p_camera);
	
	void Deactivate(std::unique_ptr<GraphicsEngine>& p_graphics);

	void RenderUI();
	
	~ShadowMap();

	ShadowMap(const ShadowMap& other) = delete;
	ShadowMap& operator=(const ShadowMap& other) = delete;
};