#include "CCPlayerLightController.h"
#include "./Application/Input.h"

#include "./Utilities/MyArrayFromVector.h"

#include "./Engine/Actor.h"
#include "./Engine/LightController.h"

//----------------------------------------------------------------------------------------------------------------------------

CCPlayerLightController::CCPlayerLightController(const std::shared_ptr<Actor>& owner)
    :Component(owner),
    mStartLightingTimer(START_LIGHT_TIMER_MAX),
    mLightingTimer(LIGHTING_TIMER_MAX),
    mEndLightingTimer(END_LIGHT_TIMER_MAX)
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCPlayerLightController> CCPlayerLightController::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCPlayerLightController>(new CCPlayerLightController(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCPlayerLightController::Create()
{
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerLightController::Destroy()
{

}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerLightController::Update(float elapsed_time)
{
    assert(m_pCustomLight != nullptr);

    UpdateLightState();

    Vector3 spotLightDir = Vector3::Normalize(m_pPlanet->GetPosition() - m_pOwner.lock()->GetPosition());

    if (m_bIsLighting)
    {

        m_pCustomLight->SetLightColor(Math::Lerp(Vector3(0.7f, 0.7f, 0.7f), Vector3(0.35f, 0.35f, 0.35f), mLightUpRate));

        mPos = m_pOwner.lock()->GetPosition() + spotLightDir * 0.6f;

        // スポットライト設定
        m_pCustomLight->SetSpotData(
            0,
            mPos,
            Vector4(0.9f, 0.9f, 0.9f, 1.0f),
            spotLightDir,
            SPOTLIGHT_RANGE * mLightUpRate,
            0.9f, 0.8f);

        // ポイントライト設定
        m_pCustomLight->SetPointData(
            0,
            mPos,
            Vector4(0.9f, 0.9f, 0.9f, 1.0f),
            POINTLIGHT_RANGE * mLightUpRate
        );
    }
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerLightController::UpdateLightState(bool flag)
{
    if (m_bConstant == true)
    {
        // スポットライト関連の変数を初期化
        m_bIsLighting = true;

        mLightUpRate = mStartLightingTimer.GetTimeRate();


        mStartLightingTimer.Tick();

        return;
    }

    // ライティング処理に移行
    if (InputPtr->OnKeyTrigger("Z"))
    {
        // スポットライト関連の変数を初期化
        m_bIsLighting = true;
        mStartLightingTimer.Init();
        mLightUpRate = 0.0f;
        mLightingTimer.Init();
        mEndLightingTimer.Init();
    }

    if (InputPtr->OnKeyDown("Z") && m_bIsLighting)
    {
        mStartLightingTimer.Tick();

        if (!mStartLightingTimer.IsStopped())
            mLightUpRate = mStartLightingTimer.GetTimeRate();

        mLightingTimer.Tick();
    }

    // ライティング終了処理に移行
    if ((InputPtr->OnKeyUp("Z") || mLightingTimer.IsStopped()) && m_bIsLighting)
    {
        mEndLightingTimer.Tick();

        if (mEndLightingTimer.IsStopped())
        {
            m_bIsLighting = false;
            m_pCustomLight->ActivateSpotLight(0, false);
            m_pCustomLight->ActivatePointLight(0, false);
        }
        else
        {
            mLightUpRate = 1.0f - mEndLightingTimer.GetTimeRate();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerLightController::RenderUI()
{
    using namespace ImGui;

    Text("Start Timer     : %d[frame]", mStartLightingTimer.GetTime());

    Text("Lighting Timer  : %d[frame]", mLightingTimer.GetTime());

    Text("End Timer       : %d[frame]", mEndLightingTimer.GetTime());

    Text("Light Up Rate   : %.2f", mLightUpRate);

}

//----------------------------------------------------------------------------------------------------------------------------
