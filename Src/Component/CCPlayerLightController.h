#pragma once
#pragma once
#include "Component.h"
#include "./Utilities/Util.h"
#include "./Engine/UIClient.h"
//----------------------------------------------------------------------------------------------------------------------------
class LightController;
//----------------------------------------------------------------------------------------------------------------------------

// @class CCPlayerLightController
// @brief プレイヤーがターゲットを集めるときの挙動を決めるカスタムコンポーネントクラス
class CCPlayerLightController final : public Component, public UIClient
{
    //=============================
    // CONSTANTS
    //=============================
private:
    static constexpr unsigned int START_LIGHT_TIMER_MAX = 60; // 1秒でライトがスタートアップ
    static constexpr unsigned int LIGHTING_TIMER_MAX = 600;   // 5秒間、ライトを照らし続ける
    static constexpr unsigned int END_LIGHT_TIMER_MAX = 30;   // 0.5秒でライトがシャットダウン

    static constexpr float SPOTLIGHT_RANGE = 10.0f;
    static constexpr float POINTLIGHT_RANGE = 5.0f;

   //==============================
   // VARIABLES                   
   //==============================
private:
    // ライティング開始時エフェクト用
    unsigned int mTargetCounter = 0;
    FrameTimer mStartLightingTimer;
    float        mLightUpRate = 0.0f;

    // ライティング用
    FrameTimer mLightingTimer;

    // ライティング終了時エフェクト用
    FrameTimer mEndLightingTimer;


    bool m_bIsLighting = false;

    std::shared_ptr<LightController> m_pCustomLight;
    std::shared_ptr<Actor>       m_pPlanet;

    Vector3 mPos; // 光源の位置

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