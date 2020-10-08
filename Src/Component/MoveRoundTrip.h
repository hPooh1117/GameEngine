#pragma once
#include "MoveComponent.h"

class MoveRoundTrip final : public MoveComponent
{
private:
	float m_speed = 1.0f;

private:
	MoveRoundTrip(const std::shared_ptr<Actor>& owner);
	
public:
	virtual ~MoveRoundTrip();

	static std::shared_ptr<MoveRoundTrip> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	static constexpr int GetID() { return ComponentID::kMove; }

};