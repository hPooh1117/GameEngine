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

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBufferForShadow;

	struct CBufferForShadow
	{
		DirectX::XMFLOAT4X4 shadowVP;
	};

	DirectX::XMFLOAT4X4 mLightViewProj;

	float mDistanceToLight;

public:
	ShadowMap(D3D::DevicePtr& device);

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