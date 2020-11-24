#include "..\Blur.h"


#include "ComputedTexture.h"
#include "Shader.h"

#include "./Application/Helper.h"
#include "./Utilities/Log.h"
#include "./Utilities/ImGuiSelf.h"

BlurExecuter::BlurExecuter()
	:mKernelRange(4),
	mBlurStrength(1)
{
	mpBlurTex[0] = std::make_unique<ComputedTexture>();
	mpBlurTex[1] = std::make_unique<ComputedTexture>();
}


void BlurExecuter::Initialize(D3D::DevicePtr& p_device)
{
	auto result = p_device->CreateBuffer(
		&CD3D11_BUFFER_DESC(
			sizeof(CBForBlurSettings),
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0
		),
		nullptr,
		mpCBuffer.GetAddressOf()
	);
	if (FAILED(result))
	{
		Log::Error("[COMPUTED TEXTURE] Couldn't Create ConstantBuffer.");
		return;
	}

	for (UINT i = 0; i < BLUR_PASS_MAX; ++i)
	{
		mpBlurTex[i]->CreateTexture(p_device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
		mpBlurTex[i]->CreateTextureUAV(p_device, 0);
		mpBlurTex[i]->CreateSampler(p_device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	}
}

void BlurExecuter::CreateShader(D3D::DevicePtr& p_device)
{
	const ShaderStageDesc descHorizontal = {
		L"./Src/Shaders/CS_GaussianBlur.hlsl",
		{ { "HORIZONTAL", "1"},
		  { "VERTICAL", "0"},
		  { "SCREEN_X", std::to_string(SCREEN_WIDTH)},
		  { "SCREEN_Y", std::to_string(SCREEN_HEIGHT)},
		  { "THREAD_GROUP_SIZE_X", std::to_string(COMPUTE_KERNEL_DIMENSION)},
		  { "THREAD_GROUP_SIZE_Y", "1"},
		  { "THREAD_GROUP_SIZE_Z", "1"},
		  { "KERNEL_DIMENSION", std::to_string(COMPUTE_KERNEL_DIMENSION) },
		  { "KERNEL_RANGE", "7"},
		}
	};
	const ShaderStageDesc descVertical = {
		L"./Src/Shaders/CS_GaussianBlur.hlsl",
		{ { "HORIZONTAL", "0"},
		  { "VERTICAL", "1"},
		  { "SCREEN_X", std::to_string(SCREEN_WIDTH)},
		  { "SCREEN_Y", std::to_string(SCREEN_HEIGHT)},
		  { "THREAD_GROUP_SIZE_X", "1"},
		  { "THREAD_GROUP_SIZE_Y", std::to_string(COMPUTE_KERNEL_DIMENSION)},
		  { "THREAD_GROUP_SIZE_Z", "1"},
		  { "KERNEL_DIMENSION", std::to_string(COMPUTE_KERNEL_DIMENSION) },
		  { "KERNEL_RANGE", "7"},
		}
	};

	mpBlurTex[EHorizontal]->CreateShader(p_device, descHorizontal.filename.c_str(), "CSmain", descHorizontal.macros);
	mpBlurTex[EVertical]->CreateShader(p_device, descVertical.filename.c_str(), "CSmain", descVertical.macros);
}

void BlurExecuter::ChangeSetting(UINT strength, UINT kernel_range)
{
	mBlurStrength = strength;
	mKernelRange = kernel_range;
	CalculateKernelRange();
}

void BlurExecuter::ActivateBlur(D3D::DeviceContextPtr& p_imm_context, bool b_horizontal)
{
	CBForBlurSettings cb = {};
	cb.kernelRange = mKernelRange;
	cb.strength = mBlurStrength;
	cb.bHorizontal = b_horizontal;
	memcpy(cb.weight, mKernelWeights, sizeof(float) * 10);

	p_imm_context->UpdateSubresource(mpCBuffer.Get(), 0, nullptr, &cb, 0, 0);
	p_imm_context->CSSetConstantBuffers(1, 1, mpCBuffer.GetAddressOf());
}

void BlurExecuter::ExecuteBlur(D3D::DeviceContextPtr& p_imm_context)
{
	for (UINT i = 0; i < BLUR_PASS_MAX; ++i)
	{
		const UINT DISPATCH_GROUP_X = i % 2 == 0
			? (SCREEN_WIDTH + COMPUTE_KERNEL_DIMENSION - 1) / COMPUTE_KERNEL_DIMENSION/*SCREEN_WIDTH*/
			: SCREEN_WIDTH;
		const UINT DISPATCH_GROUP_Y = i % 2 != 0
			? (SCREEN_HEIGHT + COMPUTE_KERNEL_DIMENSION - 1) / COMPUTE_KERNEL_DIMENSION/*SCREEN_HEIGHT*/
			: SCREEN_HEIGHT;
		const UINT DISPATCH_GROUP_Z = 1;

		mpBlurTex[i]->ComputeUsingCurrentSetView(p_imm_context, DISPATCH_GROUP_X, DISPATCH_GROUP_Y, DISPATCH_GROUP_Z);
		mpBlurTex[i]->Set(p_imm_context, 0);
	}
}

void BlurExecuter::Deactivate(D3D::DeviceContextPtr& p_imm_context)
{

	ID3D11Buffer* nullbuf[1] = { nullptr };
	p_imm_context->CSSetConstantBuffers(1, 1, nullbuf);
}

void BlurExecuter::RenderUI()
{
	if (ImGui::TreeNode("Blur"))
	{
		for (UINT i = 0; i < BLUR_PASS_MAX; ++i)
		{
			if (ImGui::ImageButton((void*)mpBlurTex[i]->GetSRV().Get(), ImVec2(320, 180)))
			{

			}
		}
		ImGui::TreePop();
	}
}

void BlurExecuter::CalculateKernelRange()
{
	float t = 0.0f;
	float d = static_cast<float>(mKernelRange * mKernelRange) * 0.01f;

	UINT i = 0;
	for (i = 0; i < mKernelRange; ++i)
	{
		float r = 1.0f + 2.0f * i;
		float w = expf(-0.5f * (r * r) / d);

		mKernelWeights[i] = w;
		if (i > 0) w *= 2.0f;
		t += w;
	}
	for (i = 0; i < mKernelRange; ++i)
	{
		mKernelWeights[i] /= t;
	}
}
