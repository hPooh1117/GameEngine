#include "Actor.h"

#include "./Component/MeshComponent.h"
#include "MeshRenderer.h"

#include "./Utilities/ImGuiSelf.h"
#include "./Utilities/MyArrayFromVector.h"
//-----------------------------------------------------
// NS
//-----------------------------------------------------
using namespace DirectX;

//----------------------------------------------------------------------------------------------------------------------------

Actor::Actor()
	:mId(0),
	mPosition({0,0,0}),
	mScale({1,1,1}),
	mAdditiveRotation({0,0,0})
{
    XMMATRIX r = XMMatrixIdentity();
    XMStoreFloat4x4(&mRotation, r);
}


//----------------------------------------------------------------------------------------------------------------------------

bool Actor::Initialize(int id)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------

void Actor::Update(float elapsed_time)
{
    for (auto& component : mpComponentTable)
    {
        component.second->Update(elapsed_time);
    }
}

void Actor::RenderUI()
{
	//std::string actorName = "Actor" + std::to_string(mId);
	//if (ImGui::TreeNode(actorName.c_str()))
	//{
	//	MyArrayFromVector pos = MyArrayFromVector(mPosition);
	//	ImGui::InputFloat3("Pos", pos.SetArray());
	//	float scale = mScale.x;
	//	ImGui::InputFloat("Scale", &scale);
	//	


	//	ImGui::TreePop();
	//}
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX Actor::GetWorldMatrix()
{
	mRotation = Matrix::CreateFromQuaternion(mRot);

	DirectX::XMMATRIX additive = DirectX::XMMatrixIdentity();
	if (mAdditiveRotation != Vector3(0, 0, 0))
		additive = DirectX::XMMatrixRotationRollPitchYaw(mAdditiveRotation.x, mAdditiveRotation.y, mAdditiveRotation.z);

	DirectX::XMMATRIX world;
	world = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	world *= DirectX::XMLoadFloat4x4(&mRotation) * additive;
	world *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	return world;
}


//----------------------------------------------------------------------------------------------------------------------------
