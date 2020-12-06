#pragma once
#include "MoveComponent.h"

class CCPhysicalMove final : public MoveComponent
{

private:

public:
	CCPhysicalMove(Actor* owner);
	virtual ~CCPhysicalMove() = default;

	virtual bool Create() override;

	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;


	void Integrate(float elapsed_time);
	void AddForce(const Vector3& force);
};