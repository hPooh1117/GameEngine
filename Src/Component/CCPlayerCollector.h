#pragma once
#include "Component.h"
#include "./Utilities/Util.h"
#include "./Engine/UIClient.h"
//----------------------------------------------------------------------------------------------------------------------------
class CustomLight;
//----------------------------------------------------------------------------------------------------------------------------

// @class CCPlayerCollector
// @brief プレイヤーがターゲットを集めるときの挙動を決めるカスタムコンポーネントクラス
class CCPlayerCollector final : public Component, public UIClient
{
//=======================================================================================================
// CONSTANTS
//=======================================================================================================
private:
    static constexpr unsigned int START_LIGHT_TIMER_MAX = 60; // 1秒でライトがスタートアップ
    static constexpr unsigned int LIGHTING_TIMER_MAX = 300;   // 5秒間、ライトを照らし続ける
    static constexpr unsigned int END_LIGHT_TIMER_MAX = 30;   // 0.5秒でライトがシャットダウン

//=======================================================================================================
// VARIABLES
//=======================================================================================================
private:
    // ライティング開始時エフェクト用
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






