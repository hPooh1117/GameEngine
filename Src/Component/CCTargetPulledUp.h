#pragma once
#include "Component.h"
#include "./Utilities/Util.h"
#include "./Engine/UIClient.h"

class CCSphereCollider;

class CCTargetPulledUp final : public Component, public UIClient
{
private:
	static constexpr unsigned int PULLEDUP_TIMER_MAX = 300;
	static constexpr float ROTATE_YAW_MIN = 30.0f * 0.001745f;
	static constexpr float ROTATE_YAW_MAX = 60.0f * 0.001745f;

private:
	std::shared_ptr<CCSphereCollider> m_pColData;
	std::shared_ptr<Actor> m_pPlayer;

	Vector3 mStandardScale;
	float mAngle = 0.0f;
	FrameTimer mPulledUpTimer;
	bool m_bIsPulledUp = false;
	Vector3 mStandardPos;
	bool m_bOnceFlag = false;
private:
	CCTargetPulledUp(const std::shared_ptr<Actor>& owner) : Component(owner) {};
public:
	static std::shared_ptr<CCTargetPulledUp> Initialize(const std::shared_ptr<Actor>& owner);
	~CCTargetPulledUp() = default;

	virtual bool Create() override;
	virtual void Destroy() override;
	virtual void Update(float elapsed_time) override;
	void OnceInUpdate();

	virtual void RenderUI() override;


	static constexpr int GetID() { return kCCTargetPulledUp; }

	void SetPlayer(std::shared_ptr<Actor>& p_owner);

	bool IsDone();

	CCTargetPulledUp(const CCTargetPulledUp& other) = delete;
	CCTargetPulledUp& operator=(const CCTargetPulledUp& other) = delete;
};


inline void CCTargetPulledUp::SetPlayer(std::shared_ptr<Actor>& p_owner)
{
	m_pPlayer = p_owner;
}

inline bool CCTargetPulledUp::IsDone()
{
	return mPulledUpTimer.IsStopped();
}