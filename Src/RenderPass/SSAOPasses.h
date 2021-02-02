#pragma once
#include "RenderPasses.h"

class Sprite;
class AmbientOcclusion;
class BlurExecuter;

class SSAOPass : public RenderPass
{
private:
	static constexpr unsigned int GBUFFER_FOR_AO_MAX = 2;
	static constexpr unsigned int GBUFFER_FOR_BLUR_MAX = 2;

	std::unique_ptr<Sprite> mpScreen;
	std::unique_ptr<AmbientOcclusion> mpAOPreparation;
	std::unique_ptr<RenderTarget> mpBlurPassTargets;

	std::unique_ptr<BlurExecuter> mpBlurPass;
	bool mbIsUsingCS;
	bool mbIsUsingAlchemyAO;

public:
	SSAOPass();
	~SSAOPass() = default;

	virtual void Initialize(Graphics::GraphicsDevice* device) override;

	void ExecuteSSAO(Graphics::GraphicsDevice* device, float elapsed_time);

	void RenderUI(bool b_open);
	void RenderUIForSettings();
	void RenderUIForAnotherScreen();

	const std::unique_ptr<RenderTarget>& GetBlurPassTargetPtr() { return mpBlurPassTargets; }
};