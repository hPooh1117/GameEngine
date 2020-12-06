#include "MoveRotationComponent.h"

#include <iostream>

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

MoveRotationComponent::MoveRotationComponent(Actor* owner):MoveComponent(owner)
{
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
	mpOwner->SetQuaternion(q);
}

//----------------------------------------------------------------------------------------------------------------------------
