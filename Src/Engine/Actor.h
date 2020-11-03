#pragma once
#include <map>
#include <memory>
#include <cassert>
#include <DirectXMath.h>



#include "./Component/Component.h"

#include "./Utilities/Matrix.h"
#include "./Utilities/Quaternion.h"
#include "./Utilities/Log.h"



enum ActorID
{
	kPlayer,
	kNonPlayer,
	kEnemy,
	kFloor,
	kNonPlayer0,
	kNonPlayer1,
	kNonPlayer2,
	kNonPlayer3,
	kNonPlayer4,
	kNonPlayer5,
	kNonPlayer6,
	kNonPlayer7,

	kPlane,
	ACTOR_ID_MAX,
};

class Shader;
//class MeshRenderer;
//class MeshComponent;

class Actor : public std::enable_shared_from_this<Actor>
{
private: // Variables
	std::multimap<int, std::shared_ptr<Component>> m_pComponents;
	Vector3 mPosition = { 0, 0, 0 };
	Vector3 mScale = { 1.0f, 1.0f, 1.0f };
	Vector3 mAdditiveRotation = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mRotation;
	Quaternion mRot;
	int mId = -1;


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



private: 
	Actor(int id);
public:
	static std::shared_ptr<Actor> Initialize(int id);
	~Actor();

	void Update(float elapsed_time);


public: 
	template<typename T>
	std::shared_ptr<T> AddComponent();

	//std::shared_ptr<MeshComponent> AddComponent(
	//	int mesh_id,
	//	std::shared_ptr<MeshRenderer>& renderer,
	//	std::shared_ptr<Shader>& shader
	//);

	//std::shared_ptr<MeshComponent> AddComponent(
	//	int mesh_id,
	//	std::shared_ptr<MeshRenderer>& renderer,
	//	std::shared_ptr<Shader>& shader,
	//	const char* filename
	//);

	//std::shared_ptr<MeshComponent> AddComponent(
	//	int mesh_id,
	//	std::shared_ptr<MeshRenderer>& renderer,
	//	std::shared_ptr<Shader>& shader,
	//	const wchar_t* filename
	//);

	template<typename T>
	std::shared_ptr<T> GetComponent();



	//std::shared_ptr<MeshComponent> GetComponent(int no);
};

//----------------------------------------------------------------------------------------------------------------------------

template<typename T>
std::shared_ptr<T> Actor::AddComponent()
{
	assert(GetComponent<T>() == nullptr);
	auto component = T::Initialize(shared_from_this());
	component->Create();
	m_pComponents.emplace(T::GetID(), component);

	return component;
}

//----------------------------------------------------------------------------------------------------------------------------

template<typename T>
std::shared_ptr<T> Actor::GetComponent()
{
	if (m_pComponents.empty()) return nullptr;

	auto it = m_pComponents.find(T::GetID());
	if (it != m_pComponents.end())
	{
		it->second->Destroy();
		std::shared_ptr<T> component = std::static_pointer_cast<T>(it->second);
		return component;
	}
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------------
