#pragma once
#include "CCPlayerMove.h"
#include "./Engine/UIClient.h"

// @class CCPlayerMoveForPlanet
// @brief ãÖèÛínå`ópPlayerà⁄ìÆComponent
class CCPlayerMoveForPlanet final : public CCPlayerMove, public UIClient
{
private:
	std::shared_ptr<Actor> m_pPlanet;

	Vector3 mPos;
	Vector3 mUp;

private:

	CCPlayerMoveForPlanet(const std::shared_ptr<Actor>& owner):CCPlayerMove(owner){}

public:
	static std::shared_ptr<CCPlayerMoveForPlanet> Initialize(const std::shared_ptr<Actor>& owner);
	virtual bool Create() override;
	virtual void Destroy() override;

	virtual void Update(float elapsed_time) override;

	virtual void RenderUI() override;

	inline void RegistActorAsPlanet(std::shared_ptr<Actor>& p_owner);

	static constexpr int GetID() { return ComponentID::kMove; }

	~CCPlayerMoveForPlanet(){}

	CCPlayerMoveForPlanet(const CCPlayerMoveForPlanet& other) = delete;
	CCPlayerMoveForPlanet& operator=(const CCPlayerMoveForPlanet& other) = delete;
};

void CCPlayerMoveForPlanet::RegistActorAsPlanet(std::shared_ptr<Actor>& p_owner)
{
	if (m_pPlanet != nullptr) return;
	m_pPlanet = p_owner;
}

