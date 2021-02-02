#include "Scene_ECS.h"

namespace ECS
{
	void NameComponent::Serialize(Archive& archive, EntitySerializer& seri)
	{
		if (archive.IsReadMode())
		{
			archive >> name;
		}
		else
		{
			archive << name;
		}
	}

	void LayerComponent::Serialize(Archive& archive, EntitySerializer& seri)
	{
		if (archive.IsReadMode())
		{
			archive >> layerMask;
		}
		else
		{
			archive << layerMask;
		}
	}

	void TransformComponent::Serialize(Archive& archive, EntitySerializer& seri)
	{
		if (archive.IsReadMode())
		{
			archive >> flags;
			archive >> scaleLocal;
			archive >> rotationLocal;
			archive >> translationLocal;

			SetDirty();
			UpdateTransform();
		}
		else
		{
			archive << flags;
			archive << scaleLocal;
			archive << rotationLocal;
			archive << translationLocal;
		}
	}

	void HierarchyComponent::Serialize(Archive& archive, EntitySerializer& seri)
	{
		SerializeEntity(archive, parentID, seri);

		if (archive.IsReadMode())
		{
			archive >> layerMask_bind;
			
		}
		else
		{
			archive << layerMask_bind;
		}
	}

	Entity Scene::SerializeEntityInScene(Archive& archive, Entity entity)
	{
		EntitySerializer seri;
		SerializeEntity(archive, entity, seri);

		seri.allow_remap = false;

		if (archive.IsReadMode())
		{
			{// NameComponent
				bool component_exists;
				archive >> component_exists;
				if (component_exists)
				{
					auto& component = names.Create(entity);
					component.Serialize(archive, seri);
				}
			}
			{// TransformComponent
				bool component_exists;
				archive >> component_exists;
				if (component_exists)
				{
					auto& component = transforms.Create(entity);
					component.Serialize(archive, seri);
				}
			}
		}
		else
		{
			{// NameComponent
				auto component = names.GetComponent(entity);
				if (component != nullptr)
				{
					archive << true;
					component->Serialize(archive, seri);
				}
				else
				{
					archive << false;
				}
			}
			{// TransformComponent
				auto component = transforms.GetComponent(entity);
				if (component != nullptr)
				{
					archive << true;
					component->Serialize(archive, seri);
				}
				else
				{
					archive << false;
				}
			}
		}
		return entity;
	}


	void Scene::Serialize(Archive& archive)
	{
		if (archive.IsReadMode())
		{
			uint32_t reserved;
			archive >> reserved;
		}
		else
		{
			uint32_t reserved = 0;
			archive << reserved;
		}

		EntitySerializer seri;

		names.Serialize(archive, seri);
		transforms.Serialize(archive, seri);
	}
}