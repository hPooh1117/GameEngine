#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "D3D_Helper.h"

#include "./Renderer/GraphicsDevice.h"
#include "GraphicsDevice.h"
#include "Shader.h"
#include "Texture.h"
#include "./Engine/CameraController.h"
#include "./Engine/LightController.h"

class Shader;
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
	ShadowMap(Graphics::GraphicsDevice* device);

	void Activate(
		Graphics::GraphicsDevice* device,
		LightController* p_light,
		CameraController* p_camera);
	
	void Deactivate(Graphics::GraphicsDevice* device);

	void RenderUI();
	
	~ShadowMap();

	ShadowMap(const ShadowMap& other) = delete;
	ShadowMap& operator=(const ShadowMap& other) = delete;
};