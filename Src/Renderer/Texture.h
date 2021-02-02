#pragma once
#include "./Application/Helper.h"
#include "D3D_Helper.h"
#include "./Utilities/Vector.h"



class Texture
{
private:
	D3D::SRVPtr mpSRV = nullptr;
public:
	Vector2 mTextureSize = {};
	unsigned int mID = 0;

public:
	Texture();
	~Texture();


	bool Load(D3D::DevicePtr& device, const wchar_t* filename);
	bool Load(D3D::DevicePtr& device);
	bool Create(D3D::DevicePtr& device, UINT width, UINT height, DXGI_FORMAT format);

	void Set(D3D::DeviceContextPtr& imm_context, int slot = 0, bool flag = true);

	inline Vector2 GetTexSize() { return mTextureSize; }
	inline auto&   GetSRV() { return mpSRV; }
};

