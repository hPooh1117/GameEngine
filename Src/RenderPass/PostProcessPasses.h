#pragma once
#include "RenderPasses.h"
#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Vector.h"

class GraphicsEngine;
class Sprite;
class ComputedTexture;
class BlurExecuter;

class PostProcessPass : public RenderPass
{
private:

	std::unique_ptr<Sprite> mpScreen;
	bool mbIsPostProcessed;
	Vector3 mCorrectColor;
	float mContrastLevel;
	float mSaturationLevel;
	float mVignetteLevel;

	bool mbIsDesaturated;
	bool mbIsBlurred;
	bool mbIsDrawing;

	UINT mCurrentRenderTarget;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mpConstantBuffer;
	struct CBufferForPostProcess
	{
		DirectX::XMFLOAT3 correctColor;
		float contrast;
		float saturation;
		float vignette;
		float dummy0;
		float dummy1;
	};

	std::unique_ptr<ComputedTexture> mpPostProcessTex;
	std::unique_ptr<BlurExecuter> mpBlurPass;

public:
	PostProcessPass();
	virtual ~PostProcessPass() = default;

	virtual void Initialize(D3D::DevicePtr& device) override;

	void RenderPostProcess(std::unique_ptr<GraphicsEngine>& p_graphics, float elapsed_time);
	void RenderUI();
	void RenderUIForSettings();
	void SetPostProcessed(bool flag) { mbIsPostProcessed = flag; }

private:
	void ChooseCurrentRenderTarget();
};