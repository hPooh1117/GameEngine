#include "TextureHolder.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"

#include "./Component/MeshComponent.h"

#include "./Renderer/Texture.h"

#include "./Utilities/misc.h"


bool TextureHolder::Initialize(D3D::DevicePtr& p_device)
{
    {
		FLOAT borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		auto hr = p_device->CreateSamplerState(
			&CD3D11_SAMPLER_DESC(
				D3D11_FILTER_ANISOTROPIC,
				D3D11_TEXTURE_ADDRESS_WRAP,
				D3D11_TEXTURE_ADDRESS_WRAP,
				D3D11_TEXTURE_ADDRESS_WRAP,
				0,
				16,
				D3D11_COMPARISON_ALWAYS,
				borderColor,
				0,
				D3D11_FLOAT32_MAX
			),
			mpSamplerTable[EWrap].GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

		hr = p_device->CreateSamplerState(
			&CD3D11_SAMPLER_DESC(
				D3D11_FILTER_ANISOTROPIC,
				D3D11_TEXTURE_ADDRESS_MIRROR,
				D3D11_TEXTURE_ADDRESS_MIRROR,
				D3D11_TEXTURE_ADDRESS_MIRROR,
				0,
				16,
				D3D11_COMPARISON_ALWAYS,
				borderColor,
				0,
				D3D11_FLOAT32_MAX
			),
			mpSamplerTable[EMirror].GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


		hr = p_device->CreateSamplerState(
			&CD3D11_SAMPLER_DESC(
				D3D11_FILTER_MIN_MAG_MIP_LINEAR,
				D3D11_TEXTURE_ADDRESS_CLAMP,
				D3D11_TEXTURE_ADDRESS_CLAMP,
				D3D11_TEXTURE_ADDRESS_CLAMP,
				0,
				16,
				D3D11_COMPARISON_ALWAYS,
				borderColor,
				0,
				D3D11_FLOAT32_MAX
			),
			mpSamplerTable[EClamp].GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

		borderColor[0] = 1.0f;
		borderColor[1] = 1.0f;
		borderColor[2] = 1.0f;
		borderColor[3] = 1.0f;

		hr = p_device->CreateSamplerState(
			&CD3D11_SAMPLER_DESC(
				D3D11_FILTER_MIN_MAG_MIP_LINEAR,
				D3D11_TEXTURE_ADDRESS_BORDER,
				D3D11_TEXTURE_ADDRESS_BORDER,
				D3D11_TEXTURE_ADDRESS_BORDER,
				0,
				16,
				D3D11_COMPARISON_NEVER,
				borderColor,
				0,
				D3D11_FLOAT32_MAX
			),
			mpSamplerTable[EBorder].GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));


		hr = p_device->CreateSamplerState(
			&CD3D11_SAMPLER_DESC(
				D3D11_FILTER_ANISOTROPIC,
				D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
				D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
				D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
				0,
				16,
				D3D11_COMPARISON_ALWAYS,
				borderColor,
				0,
				D3D11_FLOAT32_MAX
			),
			mpSamplerTable[EMirror].GetAddressOf()
		);
		_ASSERT_EXPR_A(SUCCEEDED(hr), hr_trace(hr));

	}
    return true;
}

void TextureHolder::RegisterTextureFromActors(D3D::DevicePtr& p_device, std::unique_ptr<ActorManager>& p_actors)
{
	for (auto i = 0u; i < p_actors->GetActorsSize(); ++i)
	{
		std::shared_ptr<NewMeshComponent> component = p_actors->GetActor(i)->GetComponent<NewMeshComponent>();
		const std::vector<TextureData>& table = component->GetTextureTable();
		
		for (auto i = 0u; i < table.size(); ++i)
		{
			auto it = mTextureTable.find(table[i].filename);
			if (it != mTextureTable.end())
			{
				continue;
			}

			std::unique_ptr<NewTexture> tex = std::make_unique<NewTexture>();
			if (table[i].filename == L"DUMMY" || table[i].filename == L"EMPTY")   tex->Load(p_device);
			else                                                                  tex->Load(p_device, table[i].filename.c_str());

			mTextureTable.emplace(table[i].filename.c_str(), std::move(tex));
		}
	}

}

void TextureHolder::ClearTextureTable()
{
	mTextureTable.clear();
}

void TextureHolder::Set(D3D::DeviceContextPtr& p_imm_context, std::wstring filename, unsigned int slot, bool flag)
{
	mTextureTable.find(filename)->second->Set(p_imm_context, slot, flag);
}

void TextureHolder::SetSampler(D3D::DeviceContextPtr& p_imm_context, unsigned int slot, unsigned int samplerid)
{
	if (samplerid >= SamplerID::ENUM_SAMPLER_MAX || samplerid < 0)
	{
		Log::Warning("Couldn't Set SamplerState. Out of range(%d).", samplerid);
		return;
	}
	p_imm_context->PSSetSamplers(slot, 1, mpSamplerTable[samplerid].GetAddressOf());
	p_imm_context->DSSetSamplers(slot, 1, mpSamplerTable[samplerid].GetAddressOf());
}
