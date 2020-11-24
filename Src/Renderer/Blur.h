#pragma once
#include <memory>
#include <vector>


#include "D3D_Helper.h"

class Shader;
class Texture;
class ComputedTexture;
struct ShaderMacro;



class BlurExecuter
{
public:
	enum BlurPass
	{
		EHorizontal,
		EVertical,

		BLUR_PASS_MAX
	};

	static constexpr UINT COMPUTE_KERNEL_DIMENSION = 1024;

private:
	struct CBForBlurSettings
	{
		UINT kernelRange;
		UINT strength;
		UINT bHorizontal;
		float weight[10];
		float param[3];
	};

	UINT mKernelRange;
	UINT mBlurStrength;
	float mKernelWeights[10] = {0};

	D3D::BufferPtr mpCBuffer;
	std::unique_ptr<ComputedTexture> mpBlurTex[BLUR_PASS_MAX];

public:
	BlurExecuter();
	~BlurExecuter() = default;

	void Initialize(D3D::DevicePtr& p_device);
	void CreateShader(D3D::DevicePtr& p_device);
	void ChangeSetting(UINT strength, UINT kernel_range);
	void ActivateBlur(D3D::DeviceContextPtr& p_imm_context, bool b_horizontal);
	void ExecuteBlur(D3D::DeviceContextPtr& p_imm_context);
	void Deactivate(D3D::DeviceContextPtr& p_imm_context);

	void RenderUI();
private:
	void CalculateKernelRange();
};