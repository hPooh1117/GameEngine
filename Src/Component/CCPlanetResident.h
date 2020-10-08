#pragma once
#include "MoveComponent.h"
#include "./Engine/UIClient.h"

// @class CCPlanetResident
// @brief 地形が球状の場合、物理的な動作をするActorはこのComponentを利用する。
class CCPlanetResident final : public MoveComponent, public UIClient
{
private:
	std::shared_ptr<Actor> m_pPlanet;
	Vector3 mPos;
	Vector3 mUp;

private:
	CCPlanetResident(const std::shared_ptr<Actor>& owner);

public:
	static std::shared_ptr<CCPlanetResident> Initialize(const std::shared_ptr<Actor>& owner);

	virtual bool Create() override;

	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	virtual void RenderUI() override;

	void Move(float elapsed_time);

	void Integrate(float elapsed_time);
	void AddForce(const Vector3& force);

	void RegistActorAsPlanet(const std::shared_ptr<Actor>& p_owner);

	~CCPlanetResident();


};


inline void CCPlanetResident::RegistActorAsPlanet(const std::shared_ptr<Actor>& p_owner)
{
	if (m_pPlanet != nullptr) return; 
	m_pPlanet = p_owner; 
}
