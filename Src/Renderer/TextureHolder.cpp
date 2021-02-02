#include "TextureHolder.h"

#include "./Engine/Actor.h"
#include "./Engine/ActorManager.h"

#include "./Component/MeshComponent.h"

#include "./Renderer/Texture.h"

#include "./Utilities/misc.h"


bool TextureHolder::Initialize(D3D::DevicePtr& p_device)
{
    return true;
}

void TextureHolder::RegisterTextureFromActors(D3D::DevicePtr& p_device, ActorManager* p_actors)
{
	for (auto i = 0u; i < p_actors->GetActorsSize(); ++i)
	{
		MeshComponent* component = p_actors->GetActor(i)->GetComponent<MeshComponent>();
		const std::vector<TextureData>& table = component->GetTextureTable();

		for (auto i = 0u; i < table.size(); ++i)
		{
			auto it = mTextureTable.find(table[i].filename);
			if (it != mTextureTable.end())
			{
				continue;
			}

			std::unique_ptr<Texture> tex = std::make_unique<Texture>();
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

void TextureHolder::Set(D3D::DeviceContextPtr& p_imm_context, const wchar_t* filename, unsigned int slot, bool flag)
{
	mTextureTable.find(filename)->second->Set(p_imm_context, slot, flag);
}
