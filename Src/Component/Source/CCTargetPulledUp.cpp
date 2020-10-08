#include "CCTargetPulledUp.h"

#include "CCSphereCollider.h"
#include "MoveComponent.h"
#include "CCPlayerLightController.h"

#include "./Engine/Actor.h"

//--------------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCTargetPulledUp> CCTargetPulledUp::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCTargetPulledUp>(new CCTargetPulledUp(owner));
}

//--------------------------------------------------------------------------------------------------------------------------------

bool CCTargetPulledUp::Create()
{
    mPulledUpTimer = FrameTimer(PULLEDUP_TIMER_MAX);
    mStandardScale = OwnerPtr->GetScale();
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void CCTargetPulledUp::Destroy()
{
}

//--------------------------------------------------------------------------------------------------------------------------------

void CCTargetPulledUp::Update(float elapsed_time)
{
    OnceInUpdate();

    // �X�|�b�g���C�g����Ă��Ȃ���Ώ������Ȃ�
    std::shared_ptr<CCPlayerLightController> light = m_pPlayer->GetComponent<CCPlayerLightController>();
    

    if (m_pColData->OnIntersection() && light->GetIsLighting()) // ���C�g�Ɠ������Ă���Ȃ�
    {

        // �ړ��n�R���|�[�l���g�𖳌���
        OwnerPtr->GetComponent<MoveComponent>()->BeInvalid();

        
        // �����ʒu�A�e�ƂȂ�A�N�^�[�̈ʒu���Z�o
        Vector3 lightPos = m_pPlayer->GetComponent<CCPlayerLightController>()->GetLightSourcePosition();
        
        if (!m_bIsPulledUp)
        {
            mStandardPos = m_pOwner.lock()->GetPosition();
            m_bIsPulledUp = true;
        }

        // �^�C�}�[�Ɉˑ����ăA�N�^�[�𕂏コ����
        OwnerPtr->SetPosition(Math::Lerp(mStandardPos, lightPos, mPulledUpTimer.GetTimeRate()));

        // �^�C�}�[�ˑ��ŃA�N�^�[�����[��]
        mAngle += Math::Lerp(ROTATE_YAW_MIN, ROTATE_YAW_MAX, mPulledUpTimer.GetTimeRate()) * elapsed_time;
        if (mAngle > 360 * 0.001745f) mAngle -= 360 * 0.001745f;

        Quaternion q;
        q.SetToRotateY(mAngle);
        Quaternion out = Quaternion::Concatenate(m_pOwner.lock()->GetQuaternion(), q);
        OwnerPtr->SetQuaternion(out);

        // �^�C�}�[�ˑ��ŃX�P�[�����O
        float scale = Math::Lerp(mStandardScale.x, 0.0f, mPulledUpTimer.GetTimeRate());
        OwnerPtr->SetScale(scale, scale, scale);

        mPulledUpTimer.Tick();
    }
    if (light->GetIsLighting() == false)
    {
        float scale = Math::Lerp(OwnerPtr->GetScaleValue(), mStandardScale.x, 0.01f);
        OwnerPtr->SetScale(scale, scale, scale);

        if (OwnerPtr->GetComponent<MoveComponent>()->GetIsValid()) return;

        OwnerPtr->GetComponent<MoveComponent>()->BeValid();
        mPulledUpTimer.Init();
        mAngle = 0.0f;
        m_bIsPulledUp = false;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------

void CCTargetPulledUp::OnceInUpdate()
{

    if (m_bOnceFlag) return;
        
    m_pColData = m_pOwner.lock()->GetComponent<CCSphereCollider>();
    m_bOnceFlag = true;
}

//--------------------------------------------------------------------------------------------------------------------------------

void CCTargetPulledUp::RenderUI()
{
    using namespace ImGui;

    Text("Timer(Pulled Up) : %d", mPulledUpTimer.GetTime());

    Text("TimeRate : %.3f", mPulledUpTimer.GetTimeRate());

    Vector3 scale = OwnerPtr->GetScale();
    Text("Scale : ( %.3f, %.3f, %.3f )", scale.x, scale.y, scale.z);


}

//--------------------------------------------------------------------------------------------------------------------------------
