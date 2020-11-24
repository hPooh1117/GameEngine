#pragma once

#include <memory>
#include <DirectXMath.h>
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"
#include "./Engine/UIClient.h"

class GraphicsEngine;
class Shader;
class Light;
class Camera;
class Texture;
class ShadowMap;
class CameraController;


class AmbientOcclusion : public UIClient
{
private:
	static constexpr unsigned int MAX_SAMPLES = 16;
	static constexpr float SAMPLE_RADIUS = 0.675f;
	static const Vector2 NOISE_TEX_RESOLUTION;

private:
	D3D::RTVPtr    m_pRTV;
	D3D::DSVPtr    m_pDSV;
	D3D::BufferPtr           m_pCBufferForAO;


	DirectX::XMFLOAT4 mSamplePos[MAX_SAMPLES];
	
	D3D::Texture2DPtr        m_pNoiseTexture;
	D3D::SRVPtr   m_pNoiseResourceView;
	D3D::SamplerStatePtr     m_pWrapSampler;
	Vector2                  mNoiseScale;
	float                    mSampleRadius;
	float                    mPower;
	float                    mBias;
	float					 mBlurSize;
private:
	struct CBufferForAO
	{
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT2   screenSize;
		DirectX::XMFLOAT2   noiseScale;
		float               radius;
		float               power;
		float               kernelSize;
		float               ambientBias;
		DirectX::XMFLOAT4   samplePos[MAX_SAMPLES];
		float               blurTimes;
		DirectX::XMFLOAT3   dummy;
	};

public:
	AmbientOcclusion();
	bool Initialize(D3D::DevicePtr& p_device);
	void Activate(std::unique_ptr<GraphicsEngine>& p_graphics, CameraController* p_camera);
	void Deactivate(std::unique_ptr<GraphicsEngine>& p_graphics);

	virtual void RenderUI() override;
	~AmbientOcclusion();
};