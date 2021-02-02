#pragma once
#include "EntityComponentSystem.h"

namespace ECS
{
	struct LayerComponent
	{
		uint32_t layerMask = ~0;

		inline uint32_t GetLayerMask() const { return layerMask; }

		void Serialize(Archive& archive, EntitySerializer& seri);
	};

}