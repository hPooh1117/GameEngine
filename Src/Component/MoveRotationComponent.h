#pragma once
#include "MoveComponent.h"
//_declspec(align(16))
class MoveRotationComponent final: public MoveComponent
{

public:
	MoveRotationComponent(Actor* owner);
	virtual ~MoveRotationComponent() = default;

	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }

};

