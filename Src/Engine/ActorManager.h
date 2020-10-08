#pragma once
#include "Actor.h"
#include <unordered_map>

class ActorManager final
{
private:
	std::unordered_map<int, std::shared_ptr<Actor>> m_pActors;

public:
	ActorManager();
	std::shared_ptr<Actor> AddActor(int type);
	void AddActor(std::shared_ptr<Actor>& actor);
	std::shared_ptr<Actor>& GetActor(int type);
	void Update(float elapsed_time);

	inline unsigned int GetActorsSize() { return m_pActors.size(); }
	inline auto& GetActors() { return m_pActors; }

	~ActorManager();
};