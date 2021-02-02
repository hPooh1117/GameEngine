#include "Scene_ECS.h"

namespace ECS
{
	void Scene::Update(float dt)
	{
		JobSystem::Context ctx;

		RunTransformUpdateSystem(ctx);

		JobSystem::Wait(ctx);
	}

	void Scene::Clear()
	{
		names.Clear();
		transforms.Clear();
	}

	void Scene::Merge(Scene& other)
	{
		names.Merge(other.names);
		transforms.Merge(other.transforms);
	}

	void Scene::RemoveEntity(Entity entity)
	{
		DetachComponent(entity);

		names.Remove(entity);
		transforms.Remove(entity);
	}

	Entity Scene::FindEntityByName(const std::string& name)
	{
		for (size_t i = 0; i < names.GetCount(); ++i)
		{
			if (names[i] == name)
			{
				return names.GetEntity(i);
			}
		}
		return INVALID_ENTITY;
	}

	Entity Scene::CreateEntityObject(const std::string& name)
	{
		Entity entity = CreateEntity();
		names.Create(entity) = name;
		transforms.Create(entity);
		return entity;
	}

	void Scene::AttachComponent(Entity entity, Entity parent, bool child_already_in_local_space)
	{
		assert(entity != parent);

		if (hierarchy.Contains(entity))
		{
			DetachComponent(entity);
		}
	}

	void Scene::DetachComponent(Entity entity)
	{
	}

	void Scene::DetachChildrenComponent(Entity parent)
	{
	}

	void Scene::RunTransformUpdateSystem(JobSystem::Context& ctx)
	{
	}
	Entity LoadModel(const std::string& filename, const XMMATRIX& transformMatrix, bool attached)
	{
		Scene scene;
		Entity root = LoadModel(scene, filename, transformMatrix, attached);
		GetScene().Merge(scene);
		return root;
	}

	Entity LoadModel(Scene& scene, const std::string& filename, const XMMATRIX& transformMatrix, bool attached)
	{
		Archive archive(filename, true);
		if (archive.IsOpen())
		{
			scene.Serialize(archive);

			Entity root = CreateEntity();
			scene.transforms.Create(root);
			scene.layers.Create(root).layerMask = ~0;

			{
				for (size_t i = 0; i < scene.transforms.GetCount() - 1; ++i)
				{
					Entity entity = scene.transforms.GetEntity(i);
					if (!scene.hierarchy.Contains(entity))
					{
						scene.AttachComponent(entity, root);
					}
				}

				scene.transforms.GetComponent(root)->MatrixTransform(transformMatrix);
				scene.Update(0);
			}
			if (!attached)
			{
				scene.DetachChildrenComponent(root);
				scene.RemoveEntity(root);
				root = INVALID_ENTITY;
			}
			return root;
		}
		return INVALID_ENTITY;
	}

}