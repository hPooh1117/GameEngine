#pragma once
#include "Component.h"
#include "./Utilities/Util.h"
#include "./Engine/UIClient.h"
//----------------------------------------------------------------------------------------------------------------------------
class CustomLight;
//----------------------------------------------------------------------------------------------------------------------------

// @class CCPlayerCollector
// @brief �v���C���[���^�[�Q�b�g���W�߂�Ƃ��̋��������߂�J�X�^���R���|�[�l���g�N���X
class CCPlayerCollector final : public Component, public UIClient
{
//=======================================================================================================
// CONSTANTS
//=======================================================================================================
private:
    static constexpr unsigned int START_LIGHT_TIMER_MAX = 60; // 1�b�Ń��C�g���X�^�[�g�A�b�v
    static constexpr unsigned int LIGHTING_TIMER_MAX = 300;   // 5�b�ԁA���C�g���Ƃ炵������
    static constexpr unsigned int END_LIGHT_TIMER_MAX = 30;   // 0.5�b�Ń��C�g���V���b�g�_�E��

//=======================================================================================================
// VARIABLES
//=======================================================================================================
private:
    // ���C�e�B���O�J�n���G�t�F�N�g�p
    unsigned int mTargetCounter = 0;


//=======================================================================================================
// FUNCTIONS
//=======================================================================================================
private:
//--< ESSENTIAL >----------------------------------------------------------------------------------------
    CCPlayerCollector(const std::shared_ptr<Actor>& owner);

public:
    static std::shared_ptr<CCPlayerCollector> Initialize(const std::shared_ptr<Actor>& owner);
    ~CCPlayerCollector() = default;

    virtual bool Create() override;
    virtual void Destroy() override;
    virtual void Update(float elapsed_time) override;

    
    virtual void RenderUI() override;

    void IncrementCounter();

//--< SETTER >--------------------------------------------------------------------------------------------

//--< GETTER >--------------------------------------------------------------------------------------------
    static constexpr int GetID() { return kCCPlayerCollector; }
};


inline void CCPlayerCollector::IncrementCounter()
{
    mTargetCounter++;
}






