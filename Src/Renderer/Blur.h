#pragma once
#include <memory>
#include <vector>


#include "D3D_Helper.h"
#include "GraphicsDevice.h"
#include "./Renderer/Shader.h"

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
	static constexpr UINT BLUR_STRENGTH_MAX = 4;

private:
	//struct CBForBlurSettings
	//{
	//	UINT kernelSize;
	//	UINT strength;
	//	UINT bHorizontal;
	//	float weight[10];
	//	float param[3];
	//};

	UINT mKernelSize;
	int mBlurStrength;
	float mKernelWeights[10] = {0};

	//D3D::BufferPtr mpCBuffer;
	std::unique_ptr<ComputedTexture> mpBlurTex[BLUR_PASS_MAX];
	std::unique_ptr<ComputedTexture> mpBlurWideTex[BLUR_PASS_MAX];

	std::unique_ptr<Shader> mpCS_Blur[BLUR_PASS_MAX + BLUR_PASS_MAX];
public:
	BlurExecuter();
	~BlurExecuter() = default;

	void Initialize(D3D::DevicePtr& p_device);
	void CreateShader(D3D::DevicePtr& p_device);
	void ChangeSetting(UINT strength, UINT kernel_range);
	void ActivateBlur(D3D::DeviceContextPtr& p_imm_context, bool b_horizontal);
	void ExecuteBlur(Graphics::GraphicsDevice* p_device, const D3D::SRVPtr& srv, UINT slot = 0);
	void Deactivate(D3D::DeviceContextPtr& p_imm_context);

	void RenderUI();

	int GetBlurStrength() { return mBlurStrength; }
	void SetBlurStrength(int param) { mBlurStrength = param; }

private:
	void CalculateKernelRange();
};