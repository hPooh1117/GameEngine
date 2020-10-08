#pragma once
#include "MoveComponent.h"

class CCEnemyMove : public MoveComponent
{
private:
	CCEnemyMove(const std::shared_ptr<Actor>& owner);

public:
	static std::shared_ptr<CCEnemyMove> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;


	~CCEnemyMove();

	CCEnemyMove(const CCEnemyMove& other) = delete;
	CCEnemyMove& operator=(const CCEnemyMove& other) = delete;
};