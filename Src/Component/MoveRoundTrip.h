#pragma once
#include "MoveComponent.h"

class MoveRoundTrip final : public MoveComponent
{
private:
	float mSpeed = 1.0f;


public:
	MoveRoundTrip(Actor* owner);
	virtual ~MoveRoundTrip() = default;

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }

};