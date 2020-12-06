#pragma once
#include "MoveComponent.h"

class MoveForwardComponent final : public MoveComponent
{
private:
	float mSpeed = 1.0f;


public:
	MoveForwardComponent(Actor* owner);
	virtual ~MoveForwardComponent() = default;

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }
};