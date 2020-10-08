#include "CCPlanetResident.h"

#include "./Utilities/MyArrayFromVector.h"

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

CCPlanetResident::CCPlanetResident(const std::shared_ptr<Actor>& owner) :MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CCPlanetResident> CCPlanetResident::Initialize(const std::shared_ptr<Actor>& owner)
{
    return std::shared_ptr<CCPlanetResident>(new CCPlanetResident(owner));
}

bool CCPlanetResident::Create()
{
    mPos = OwnerPtr->GetPosition();
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::Update(float elapsed_time)
{
    // Planet(惑星型地面)が登録されていない場合は更新しない。
    if (m_pPlanet == nullptr) return;
    // 無効オブジェクトなら更新しない。
    if (mIsValid == false) return;

    
    Move(elapsed_time);


    Integrate(elapsed_time);


}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::RenderUI()
{
    using namespace ImGui;
    MyArrayFromVector pos = m_pOwner.lock()->GetPosition();
    SliderFloat3("Target Position", pos.SetArray(), -100.0f, 100.0f);

    MyArrayFromVector up = mUp;
    SliderFloat3("Target Up", up.SetArray(), -1.0f, 1.0f);

    Text("Is Valid : %d", mIsValid);

    Text("Distance From Center : % .3f", (m_pPlanet->GetPosition() - mPos).getLength());

    MyArrayFromVector q = m_pOwner.lock()->GetQuaternion();
    SliderFloat4("Quaternion", q.SetArray(), -10.0f, 10.0f);
}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::Move(float elapsed_time)
{
    Quaternion prev = m_pOwner.lock()->GetQuaternion();
    Quaternion next;
    Matrix rot = Matrix::CreateFromQuaternion(prev);

    if (m_bOnGround)
    {
        mPos = m_pOwner.lock()->GetPosition();

        static float speed = 1.f;
        Vector3 forward(rot._31, rot._32, rot._33);

        mVelocity = speed * forward;
        Vector3 displacement = mVelocity * elapsed_time;
        mPos += displacement;
        m_pOwner.lock()->SetPosition(mPos);

    }


    // 重力方向を更新
    Vector3 gravityDir = Vector3::Normalize(m_pPlanet->GetPosition() - mPos);

    // アクターが重力と逆方向に回転
    mUp = -rot.GetYAxis(); // メッシュが90度回転しているものであるため、マイナスをつける
    Vector3 axis = Vector3::Cross(mUp, -gravityDir);
    float angle = acosf(Vector3::Dot(mUp, -gravityDir));

    if (angle > 0.001f * 0.001745f || angle < -0.001f * 0.001745f)
    {
        next.SetToRotateAxis(axis, angle);

        m_pOwner.lock()->SetQuaternion(Quaternion::Concatenate(prev, next));
    }

    AddForce(mMass * gravityDir * 9.8f * 2);

}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::Integrate(float elapsed_time)
{
    assert(mMass > 0);

    mAcceleration = mResultant / mMass;
    mVelocity += mAcceleration * elapsed_time;
    m_pOwner.lock()->SetPosition(m_pOwner.lock()->GetPosition() + mVelocity * elapsed_time);

    mResultant = {};

}

//----------------------------------------------------------------------------------------------------------------------------

void CCPlanetResident::AddForce(const Vector3& force)
{
    mResultant += force;
}

//----------------------------------------------------------------------------------------------------------------------------

CCPlanetResident::~CCPlanetResident()
{
}

//----------------------------------------------------------------------------------------------------------------------------
