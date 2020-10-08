#pragma once
#include "MoveComponent.h"

class CCPhysicalMove final : public MoveComponent
{

private:
	CCPhysicalMove(const std::shared_ptr<Actor>& owner);

public:
	static std::shared_ptr<CCPhysicalMove> Initialize(const std::shared_ptr<Actor>& owner);

	virtual bool Create() override;

	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;


	void Integrate(float elapsed_time);
	void AddForce(const Vector3& force);



	~CCPhysicalMove();
};