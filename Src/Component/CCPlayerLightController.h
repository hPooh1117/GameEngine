#pragma once
#pragma once
#include "Component.h"
#include "./Utilities/Util.h"
#include "./Engine/UIClient.h"
//----------------------------------------------------------------------------------------------------------------------------
class LightController;
//----------------------------------------------------------------------------------------------------------------------------

// @class CCPlayerLightController
// @brief �v���C���[���^�[�Q�b�g���W�߂�Ƃ��̋��������߂�J�X�^���R���|�[�l���g�N���X
class CCPlayerLightController final : public Component, public UIClient
{
    //=============================
    // CONSTANTS
    //=============================
private:
    static constexpr unsigned int START_LIGHT_TIMER_MAX = 60; // 1�b�Ń��C�g���X�^�[�g�A�b�v
    static constexpr unsigned int LIGHTING_TIMER_MAX = 600;   // 5�b�ԁA���C�g���Ƃ炵������
    static constexpr unsigned int END_LIGHT_TIMER_MAX = 30;   // 0.5�b�Ń��C�g���V���b�g�_�E��

    static constexpr float SPOTLIGHT_RANGE = 10.0f;
    static constexpr float POINTLIGHT_RANGE = 5.0f;

   //==============================
   // VARIABLES                   
   //==============================
private:
    // ���C�e�B���O�J�n���G�t�F�N�g�p
    unsigned int mTargetCounter = 0;
    FrameTimer mStartLightingTimer;
    float        mLightUpRate = 0.0f;

    // ���C�e�B���O�p
    FrameTimer mLightingTimer;

    // ���C�e�B���O�I�����G�t�F�N�g�p
    FrameTimer mEndLightingTimer;


    bool m_bIsLighting = false;

    std::shared_ptr<LightController> m_pCustomLight;
    std::shared_ptr<Actor>       m_pPlanet;

    Vector3 mPos; // �����̈ʒu

    bool m_bConstant = false;
    //============================
    // FUNCTIONS
    //============================
private:
    //--< ESSENTIAL >----------------------------------------------------------------------------------------
    CCPlayerLightController(const std::shared_ptr<Actor>& owner);

public:
    static std::shared_ptr<CCPlayerLightController> Initialize(const std::shared_ptr<Actor>& owner);
    ~CCPlayerLightController() = default;

    virtual bool Create() override;
    virtual void Destroy() override;
    virtual void Update(float elapsed_time) override;

    void UpdateLightState(bool flag = false);

    virtual void RenderUI() override;

    //--< SETTER >--------------------------------------------------------------------------------------------
    void SetLight(const std::shared_ptr<LightController>& p_light);
    void RegisterActorAsPlanet(const std::shared_ptr<Actor>& p_planet);
    void SetConstantFlag(bool flag) { m_bConstant = flag; }

    //--< GETTER >--------------------------------------------------------------------------------------------
    static constexpr int GetID() { return kCCPlayerLightController; }
    bool GetIsLighting();
    Vector3& GetLightSourcePosition();
};


//----------------------------------------------------------------------------------------------------------------------------

inline void CCPlayerLightController::SetLight(const std::shared_ptr<LightController>& p_light)
{
    m_pCustomLight = p_light;
}

//----------------------------------------------------------------------------------------------------------------------------

inline void CCPlayerLightController::RegisterActorAsPlanet(const std::shared_ptr<Actor>& p_planet)
{
    m_pPlanet = p_planet;
}

//----------------------------------------------------------------------------------------------------------------------------

inline bool CCPlayerLightController::GetIsLighting()
{
    return m_bIsLighting;
}

//----------------------------------------------------------------------------------------------------------------------------

inline Vector3& CCPlayerLightController::GetLightSourcePosition()
{
    return mPos;
}