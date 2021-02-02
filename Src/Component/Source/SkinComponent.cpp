#include "../SkinComponent.h"
#include "./Utilities/Util.h"


//----------------------------------------------------------------------------------------------------------------------------

SkinComponent::SkinComponent(Actor* p_owner)
	:Component(p_owner)
{
}

//----------------------------------------------------------------------------------------------------------------------------

bool SkinComponent::Create()
{
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinComponent::Destroy()
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinComponent::Update(float elapsed_time)
{
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinComponent::RegisterMotion(const char* name, const wchar_t* motion_filename)
{
	if (mMotionFileTable.find(name) != mMotionFileTable.end()) return;
	mMotionFileTable[name] = motion_filename;
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinComponent::Play(const char* name)
{
	mCurrentMotionKey = name;
	mbIsChangedMotion = true;
}

//----------------------------------------------------------------------------------------------------------------------------

void SkinComponent::Play(std::string name, int blend_time, bool isLooped)
{
}

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX SkinComponent::CalculateMotionMatrix(int frame, int bone_id, MyFbxMesh& mesh)
{
	//DirectX::XMFLOAT4X4& current = mesh.mMotions[mCurrentMotion].keys.at(bone_id).at(frame);
	//if (mFrameInterpolation.IsStopped()) return XMLoadFloat4x4(&current);

	////std::cout << "Interpolation : " << mFrameInterpolation.GetTime() << std::endl;

	//unsigned int anotherFrame = frame;
	//FbxInfo::Motion& previousMotion = mesh.mMotions[mPreviousMotion];
	//if (frame > previousMotion.frameSize) anotherFrame = frame - (previousMotion.frameSize);

	//DirectX::XMFLOAT4X4 previous = mesh.mMotions[mPreviousMotion].keys.at(bone_id).at(anotherFrame);



	//return Lerp(previous, current, mFrameInterpolation.GetTimeRate());
	return XMMATRIX();
}

//----------------------------------------------------------------------------------------------------------------------------
