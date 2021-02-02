#pragma once
#include "./Utilities/CommonInclude.h"
#include "EntityComponentSystem.h"
#include "NameComponent.h"
#include "LayerComponent.h"
#include "TransformComponent.h"
#include "HierarchyComponent.h"

#include "./Application/SpinLock.h"
namespace ECS
{
	struct Scene
	{
		ECS::ComponentManager<NameComponent> names;
		ECS::ComponentManager<LayerComponent> layers;
		ECS::ComponentManager<TransformComponent> transforms;
		ECS::ComponentManager<HierarchyComponent> hierarchy;
		SpinLock locker;

		void Update(float dt);
		void Clear();
		void Merge(Scene& other);

		void RemoveEntity(Entity entity);
		Entity FindEntityByName(const std::string& name);
		Entity SerializeEntityInScene(Archive& archive, Entity entity = INVALID_ENTITY);
		Entity CreateEntityObject(const std::string& name);

		void AttachComponent(Entity entity, Entity parent, bool child_already_in_local_space = false);
		void DetachComponent(Entity entity);
		void DetachChildrenComponent(Entity parent);

		void Serialize(Archive& archive);

		void RunTransformUpdateSystem(JobSystem::Context& ctx);
	};

	inline Scene& GetScene()
	{
		static Scene scene;
		return scene;
	}

	Entity LoadModel(const std::string& filename, const XMMATRIX& transformMatrix = XMMatrixIdentity(), bool attached = false);
	Entity LoadModel(Scene& scene, const std::string& fileName, const XMMATRIX& transformMatrix = XMMatrixIdentity(), bool attached = false);
}