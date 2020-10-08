#pragma once
#include "MoveComponent.h"
//_declspec(align(16))
class MoveRotationComponent final: public MoveComponent
{
private:
	MoveRotationComponent(const std::shared_ptr<Actor>& owner);

public:
	virtual ~MoveRotationComponent();

	static std::shared_ptr<MoveRotationComponent> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }

};

