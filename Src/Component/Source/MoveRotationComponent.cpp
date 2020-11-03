#include "MoveRotationComponent.h"

#include <iostream>

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveRotationComponent::MoveRotationComponent(const std::shared_ptr<Actor>& owner):MoveComponent(owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

MoveRotationComponent::~MoveRotationComponent()
{
}

//----------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<MoveRotationComponent> MoveRotationComponent::Initialize(const std::shared_ptr<Actor>& owner)
{
	return std::shared_ptr<MoveRotationComponent>(new MoveRotationComponent(owner));
}

//----------------------------------------------------------------------------------------------------------------------------

bool MoveRotationComponent::Create()
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveRotationComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void MoveRotationComponent::Update(float elapsed_time)
{
	if (mIsValid == false) return;

	static float yaw = 0.0f;
	yaw += elapsed_time * 30.0f;
	
	Quaternion q = Quaternion::CreateFromEulerAngle(0.0f, yaw * 0.01745f, 0);
	m_pOwner.lock()->SetQuaternion(q);
}

//----------------------------------------------------------------------------------------------------------------------------
