#pragma once
#include "RenderPasses.h"
#include "./Utilities/Vector.h"

class GraphicsEngine;
class SubView;

class MakeCubeMapPass : public RenderPass
{
public:
	static constexpr UINT CUBE_MAP_WIDTH = 1024;
	static constexpr UINT CUBE_MAP_HEIGHT = 1024;

	struct CBufferForView
	{
		DirectX::XMFLOAT4X4 view[6];
		DirectX::XMFLOAT4X4 proj;
	};
	Vector3 mPos;
	float   mInterval;

private:
	std::unique_ptr<SubView> mpSubView;
	D3D::BufferPtr			 mpCBufferForView;
public:
	MakeCubeMapPass();
	~MakeCubeMapPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) override;

	void MakeCubeMap(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

	void RenderUI(bool b_open);

	void SetOriginPoint(const Vector3& pos, float interval);
};
