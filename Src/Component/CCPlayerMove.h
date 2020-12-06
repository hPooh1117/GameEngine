#pragma once
#include "MoveComponent.h"

class CCPlayerMove : public MoveComponent
{
protected:
	float   mSpeed = 0.08f;
	Vector3 mDisplacement = {};

public:
	CCPlayerMove(Actor* owner);
	~CCPlayerMove() = default;

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time);

	void MovePlayer();

	void Integrate(float elapsed_time);
	void AddForce(const Vector3& force);

	static constexpr int GetID() { return ComponentID::kMove; }

};