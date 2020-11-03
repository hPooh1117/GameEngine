#pragma once
#include "RenderPasses.h"

class GraphicsEngine;
class Sprite;
class AmbientOcclusion;


class SSAOPass : public RenderPass
{
private:
	static constexpr unsigned int GBUFFER_FOR_AO_MAX = 2;
	static constexpr unsigned int GBUFFER_FOR_BLUR_MAX = 2;

	std::unique_ptr<Sprite> mpScreen;
	std::unique_ptr<AmbientOcclusion> mpAOPreparation;
	std::unique_ptr<RenderTarget> mpBlurPassTargets;


public:
	SSAOPass();
	~SSAOPass() = default;

	virtual void Initialize(D3D::DevicePtr& p_device) override;

	void ExecuteSSAO(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);

	void RenderUI();
	void RenderUIForRuntime();

	const std::unique_ptr<RenderTarget>& GetBlurPassTargetPtr() { return mpBlurPassTargets; }
};
