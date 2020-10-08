#include "CCPlayerMoveForPlanet.h"

#include "./Application/Input.h"
#include "./Utilities/MyArrayFromVector.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCPlayerMoveForPlanet> CCPlayerMoveForPlanet::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCPlayerMoveForPlanet>(new CCPlayerMoveForPlanet(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool CCPlayerMoveForPlanet::Create()
{
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMoveForPlanet::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMoveForPlanet::Update(float elapsed_time)
{
    // Planet(�f���^�n��)���o�^����Ă��Ȃ��ꍇ�͍X�V���Ȃ��B
    if (m_pPlanet == nullptr) return;
    // �����I�u�W�F�N�g�Ȃ�X�V���Ȃ��B
    if (mIsValid == false) return;

    Vector3 drag = -mVelocity * 0.5f;
    AddForce(drag);


    MovePlayer();


    // �d�͕������X�V
    mPos = m_pOwner.lock()->GetPosition();
    Vector3 gravityDir = Vector3::Normalize(m_pPlanet->GetPosition() - mPos);

    // �A�N�^�[���d�͂Ƌt�����ɉ�]
    Quaternion prev = m_pOwner.lock()->GetQuaternion();
    Quaternion next;
    Matrix rot = Matrix::CreateFromQuaternion(prev);
    mUp = rot.GetYAxis();
    Vector3 axis = Vector3::Cross(mUp, -gravityDir);
    float angle = acosf(Vector3::Dot(mUp, -gravityDir));
    next.SetToRotateAxis(axis, angle);

    m_pOwner.lock()->SetQuaternion(Quaternion::Concatenate(prev, next));
    m_pOwner.lock()->SetPosition(-gravityDir * 25 + m_pPlanet->GetPosition());
    //AddForce(mMass * gravityDir * 9.8f);

    Integrate(elapsed_time);


}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlayerMoveForPlanet::RenderUI()
{
    using namespace ImGui;

    MyArrayFromVector pos = MyArrayFromVector(m_pOwner.lock()->GetPosition());
    SliderFloat3("Player Position", pos.SetArray(), -1000.0f, 1000.0f);

    MyArrayFromVector up = MyArrayFromVector(mUp);
    SliderFloat3("Player Up", up.SetArray(), -1.0f, 1.0f);

}

//----------------------------------------------------------------------------------------------------------------------------

