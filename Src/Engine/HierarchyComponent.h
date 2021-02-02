#pragma once
#include "EntityComponentSystem.h"

namespace ECS
{
	struct HierarchyComponent
	{
		Entity parentID = INVALID_ENTITY;
		uint32_t layerMask_bind;

		void Serialize(Archive& archive, EntitySerializer& seri);
	};
}