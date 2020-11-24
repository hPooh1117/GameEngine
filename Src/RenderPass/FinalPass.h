#pragma once
#include "RenderPasses.h"

class Sprite;
class GraphicsEngine;

class FinalPass : public RenderPass
{
private:
	std::unique_ptr<Sprite> mpScreen;

public:
	FinalPass();
	virtual ~FinalPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) override;

	void RenderResult(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
	void RenderUI();
};