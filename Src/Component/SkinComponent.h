#pragma once
#include "Component.h"
#include "./Utilities/CommonInclude.h"
#include <vector>
#include <unordered_map>
#include <string>

// NOT IMPLEMENTED.

class SkinComponent : public Component
{
public:
	struct Bone
	{
		std::string name = "";
		XMFLOAT4X4 transform = {};
		XMFLOAT4X4 offset = {};
	};

	struct Motion
	{
		float          animeTick = 0.0f;
		bool           isLoop = true;
		XMFLOAT4X4*    keys;
		uint32_t       frameCountPerBone = 0;
	};
	using MotionDataTable = std::unordered_map<std::string, Motion>;
	using MotionFilePathTable = std::unordered_map<std::string, std::wstring>;


private:
	std::vector<Bone>      mBoneDataTable;
	MotionDataTable		   mMotionDataTable;
	MotionFilePathTable    mMotionFileTable;
	bool				   mbIsChangedMotion = false;
	std::string			   mCurrentMotionKey = "";
	std::string            mPreviousMotionKey = "";


	
public:
	SkinComponent(Actor* p_owner);
	virtual ~SkinComponent() = default;

	virtual bool Create() override;
	virtual void Destroy() override;
	virtual void Update(float elapsed_time) override;

	void RegisterMotion(const char* name, const wchar_t* motion_filename);

	void Play(const char* name);
	void DeactivateChangedMotion() { mbIsChangedMotion = !mbIsChangedMotion; }

	void Play(std::string name, int blend_time, bool isLooped = false);
	struct MyFbxMesh;
	DirectX::XMMATRIX CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh);

};