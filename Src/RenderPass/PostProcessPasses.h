#pragma once
#include "RenderPasses.h"
#include "./Renderer/D3D_Helper.h"

class GraphicsEngine;
class Sprite;

class PostProcessPass : public RenderPass
{
	std::unique_ptr<Sprite> mpScreen;
public:
	PostProcessPass();
	~PostProcessPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) override;

	void RenderPostProcess(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
};