#pragma once
#include "D3D_Helper.h"
#include "GraphicsDevice.h"

enum UsageID
{
	ELinearZ,
	EDS2x,
	EDS2xAtlas,
	EDS4x,
	EDS4xAtlas,

	USAGE_ID_COUNT,
};

class AODepthPreparation
{
private:
	D3D::Texture2DPtr mpTex2D[USAGE_ID_COUNT];
	D3D::SRVPtr mpSRV[USAGE_ID_COUNT];
	D3D::UAVPtr mpUAV[USAGE_ID_COUNT];

	D3D::BufferPtr mpCBuffer;
	
	struct CBuffer
	{
		float zMagic;
	};

public:
	AODepthPreparation() {}
	~AODepthPreparation() {}

	void Initialize(Graphics::GraphicsDevice* device, uint32_t width, uint32_t height);
	void Execute(Graphics::GraphicsDevice* device);
};