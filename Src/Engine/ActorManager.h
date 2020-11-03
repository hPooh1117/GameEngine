#pragma once
#include <map>
#include <memory>
#include <sys/types.h>

class Actor;
class ActorManager final
{
private:
	std::map<unsigned int, std::shared_ptr<Actor>> m_pActors;

public:
	ActorManager();
	std::shared_ptr<Actor> AddActor(int type);
	void AddActor(std::shared_ptr<Actor>& actor);
	std::shared_ptr<Actor>& GetActor(int type);
	void Update(float elapsed_time);

	void ClearAll();

	inline unsigned int GetActorsSize() { return m_pActors.size(); }
	inline auto& GetActors() { return m_pActors; }

	~ActorManager();
};