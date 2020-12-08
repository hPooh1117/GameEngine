#pragma once
#include <array>
#include <unordered_map>
#include <memory>
#include <string>


#include "D3D_Helper.h"
#include "TextureHelper.h"


using TextureID = unsigned int;

enum SamplerID
{
	EWrap,
	EMirror,
	EClamp,
	EBorder,
	EMirrorOnce,

	ENUM_SAMPLER_MAX,
};


class Texture;
class ActorManager;

class TextureHolder
{
private:
	std::unordered_map<std::wstring, std::unique_ptr<Texture>> mTextureTable;
	std::array < D3D::SamplerStatePtr, SamplerID::ENUM_SAMPLER_MAX> mpSamplerTable;
	
public:
	TextureHolder() = default;
	~TextureHolder() = default;

	bool Initialize(D3D::DevicePtr& p_device);

	void RegisterTextureFromActors(D3D::DevicePtr& p_device, ActorManager* p_actors);


	void ClearTextureTable();

	void Set(D3D::DeviceContextPtr& p_imm_context, const wchar_t* filename, unsigned int slot, bool flag = true);
	void SetSampler(D3D::DeviceContextPtr& p_imm_context, unsigned int slot, unsigned int samplerid = SamplerID::EWrap);
};