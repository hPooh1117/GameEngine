#pragma once

#include <memory>
#include <DirectXMath.h>
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"
#include "./Engine/UIClient.h"

#include "GraphicsDevice.h"
#include "./Renderer/Shader.h"


class Texture;
class CameraController;
class ComputedTexture;

class AmbientOcclusion : public UIClient
{
public:
	enum AOType
	{
		EOldSSAO,
		EAlchemyAO,
	};

private:
	static constexpr unsigned int MAX_SAMPLES = 16;
	static constexpr float SAMPLE_RADIUS = 0.675f;
	static const Vector2 NOISE_TEX_RESOLUTION;

private:
	D3D::RTVPtr    m_pRTV;
	D3D::DSVPtr    m_pDSV;
	D3D::BufferPtr           m_pCBufferForAO;
	std::unique_ptr<ComputedTexture> mpSSAOTex;
	std::unique_ptr<ComputedTexture> mpAlchemyAOTex;
	std::unique_ptr<Shader> mpCS_SSAO;
	std::unique_ptr<Shader> mpCS_AlchemyAO;

	DirectX::XMFLOAT4 mSamplePos[MAX_SAMPLES] = { {} };
	
	D3D::Texture2DPtr        m_pNoiseTexture;
	D3D::SRVPtr   m_pNoiseResourceView;
	Vector2                  mNoiseScale;
	float                    mSampleRadius;
	float                    mPower;
	float                    mBias;
	float					 mBlurSize;
	float                    mKernelSize;
	float                    mKernelSizeRcp;
	Vector2                  mScreenSize;
	UINT                     mAOType;

private:
	struct CBufferForAO
	{
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 view;

		DirectX::XMFLOAT2   screenSize;
		float               radius;
		float               power;

		float               kernelSize;
		float				cameraNearZ;
		float				cameraFarZ;
		float               kernelSize_rcp;
		
		DirectX::XMFLOAT2  screenSize_rcp;
		DirectX::XMFLOAT2  noiseScale;
		DirectX::XMFLOAT4  samplePos[MAX_SAMPLES];

		float              bias;
		float dummy = 0;
		float dummy1 = 0;
		float dummy2 = 0;
	};

public:
	AmbientOcclusion();
	bool Initialize(Graphics::GraphicsDevice* device);
	void Activate(Graphics::GraphicsDevice* device, CameraController* p_camera);
	void ExecuteOcclusion(Graphics::GraphicsDevice* device, D3D::SRVPtr& p_srv);
	void Deactivate(Graphics::GraphicsDevice* device, UINT slot);

	virtual void RenderUI() override;



	~AmbientOcclusion();
};
