#pragma once
#include <map>
#include <memory>
#include <sys/types.h>

class Actor;
class ActorManager final
{
private:
	std::map<unsigned int, std::unique_ptr<Actor>> m_pActors;

public:
	ActorManager();
	Actor* AddActor(int type);
	void   AddActor(Actor* actor, int id);
	Actor* GetActor(int type);
	void Update(float elapsed_time);

	void ClearAll();

	inline unsigned int GetActorsSize() { return m_pActors.size(); }
	inline auto& GetActors() { return m_pActors; }

	~ActorManager();
};