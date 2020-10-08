#pragma once
#include "MoveComponent.h"

class MoveForwardComponent final : public MoveComponent
{
private:
	float mSpeed = 1.0f;

private:
	MoveForwardComponent(const std::shared_ptr<Actor>& owner);

public:
	virtual ~MoveForwardComponent();

	static std::shared_ptr<MoveForwardComponent> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }
};