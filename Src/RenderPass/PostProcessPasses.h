#pragma once
#include "RenderPasses.h"
#include "./Renderer/D3D_Helper.h"
#include "./Renderer/Shader.h"
#include "./Utilities/Vector.h"

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
	std::unique_ptr<Shader>       mpCS_PostProcess;
	std::unique_ptr<BlurExecuter> mpBlurPass;

public:
	PostProcessPass();
	virtual ~PostProcessPass() = default;

	virtual void Initialize(Graphics::GraphicsDevice* device) override;

	void RenderPostProcess(Graphics::GraphicsDevice* device, float elapsed_time);
	void RenderUI();
	void RenderUIForSettings();
	void SetPostProcessed(bool flag) { mbIsPostProcessed = flag; }

private:
	void ChooseCurrentRenderTarget();
};