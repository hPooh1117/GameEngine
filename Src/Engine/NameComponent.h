#pragma once
#include "./Utilities/CommonInclude.h"
#include "./Engine/EntityComponentSystem.h"
#include <string>

namespace ECS
{
	struct NameComponent
	{
		std::string name;

		inline void operator=(const std::string& str) { name = str; }
		inline void operator=(std::string& str) { name = std::move(str); }
		inline bool operator==(const std::string& str) const { return name.compare(str) == 0; }

		void Serialize(Archive& archive, ECS::EntitySerializer& seri);
	};

}