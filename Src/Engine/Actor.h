#pragma once
#include <map>
#include <memory>
#include <cassert>
#include <DirectXMath.h>



#include "./Component/Component.h"

#include "./Utilities/Matrix.h"
#include "./Utilities/Quaternion.h"
#include "./Utilities/Log.h"




class Shader;
//class MeshRenderer;
//class MeshComponent;

class Actor final
{
private: // Variables
	std::map<int, std::unique_ptr<Component>>       mpComponentTable;
	Vector3											mPosition;
	Vector3											mScale;
	Vector3											mAdditiveRotation;
	DirectX::XMFLOAT4X4								mRotation;
	Quaternion										mRot;
	int												mId;


public:

//--< SETTER >-----------------------------------------------------------------------------
	inline void SetID(int id) { mId = static_cast<int>(id); }
	inline void SetPosition(const Vector3& pos) { mPosition = pos; }
	inline void SetRotationQuaternion(float pitch, float yaw, float roll)
	{
		mRot = Quaternion::CreateFromEulerAngle(pitch, yaw, roll);
	}
	inline void SetQuaternion(const Quaternion& q) { mRot = q; }
	inline void SetAdditiveRotation(float pitch, float yaw, float roll) { mAdditiveRotation = Vector3(pitch, yaw, roll) * 0.01745f; };
	inline void SetScale(float scaleX, float scaleY, float scaleZ) { mScale = Vector3(scaleX, scaleY, scaleZ); }

//--< GETTER >-----------------------------------------------------------------------------
	inline int         GetID() { return mId; }
	inline Vector3&    GetPosition() { return mPosition; }
	inline Quaternion& GetQuaternion() { return mRot; }
	DirectX::XMMATRIX  GetWorldMatrix();
	inline Vector3&    GetScale() { return mScale; }
	inline float       GetScaleValue() { return mScale.x; }



public:
	Actor();
	~Actor() = default;

	bool Initialize(int id);
	void Update(float elapsed_time);

	void RenderUI();

	template<typename T>
	T* AddComponent();

	template<typename T>
	T* GetComponent();
};

//----------------------------------------------------------------------------------------------------------------------------

template<typename T>
T* Actor::AddComponent()
{
	assert(GetComponent<T>() == nullptr);
	T* component = new T(this);
	component->Create();
	mpComponentTable.emplace(T::GetID(), std::unique_ptr<T>(component));

	return component;
}

//----------------------------------------------------------------------------------------------------------------------------

template<typename T>
T* Actor::GetComponent()
{
	if (mpComponentTable.empty()) return nullptr;

	auto it = mpComponentTable.find(T::GetID());
	if (it != mpComponentTable.end())
	{
		it->second->Destroy();
		T* component = static_cast<T*>(it->second.get());
		return component;
	}
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------------
