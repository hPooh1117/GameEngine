#pragma once
#include <memory>
#include <array>
#include <DirectXMath.h>
#include <string>

#include "UIClient.h"

#include "./Renderer/D3D_Helper.h"

#include "./Utilities/Matrix.h"

namespace Settings { struct Camera; }

class Actor;
class Camera;
class OrthoView;

class CameraController final : public UIClient
{
private:
	enum CameraMode
	{
		kMoveable, // ���J����
		kTrace,    // �ǐՃJ����

		kNumMax
	};

private:
	// �J�����Ǘ��R���e�i
	std::array<std::shared_ptr<Camera>, CameraMode::kNumMax> m_pCameras;
	std::array < std::string, CameraMode::kNumMax > mCameraNameTable;

	// ���s���e�J����
	std::shared_ptr<OrthoView> m_pOrthoView;


	unsigned int mCurrentMode = kMoveable; // ���݂̃J�������[�h
	unsigned int mPrevMode = kMoveable;    // �ȑO�̃J�������[�h
	bool         m_bIsBlended = false;     // TRUE : BLEND ENABLE / FALSE : DISABLE
	int          mBlendingTime = 0;            // �J�������m�̃u�����h�ɂ����鎞��(�t���[��)�v���p�ϐ�
	int          mBlendingTimeMax = 90;         // �J�������m�̃u�����h�ɂ����鎞��(�t���[��)

	float        mFovWidth;
	float        mFovHeight;
	float        mAspect;
	float        mNearPlane;
	float        mFarPlane;

	float        mOrthoWidth;
	float        mOrthoHeight;
	float        mOrthoNear;
	float        mOrthoFar;

private:
	Vector3 mCameraPos;      // �J�����̈ʒu
	Vector3 mCameraTarget;   // �J�����̖ڕW
	Vector3 mCameraUp;       // �J�����̏����
	Matrix mView;        // ���݃J�����̃r���[�s��
	Matrix mProjection;  // ���݃J�����̎ˉe�s��
	Matrix mInvView;
	Matrix mInvProj;

public:
	CameraController();
	~CameraController() = default;

	//--< CORE INTERFACE >--------------------------------------------------------------
	bool Initialize(Settings::Camera& cameraSettings);
	void Update(float elapsed_time);
	void SetMatrix(D3D::DeviceContextPtr& p_imm_context);

private:
	void CreateViewMatrix();
	void CreateProjectionMatrix(D3D::DeviceContextPtr& p_imm_context);
	void NotifyCurrentMode();
	
public:
	//--< GETTER >----------------------------------------------------------------------
	inline const Vector3& GetCameraPosition() { return mCameraPos; }
	inline const Vector3& GetCameraTarget() { return mCameraTarget; }
	inline const float GetNearZ() { return mNearPlane; }
	inline const float GetFarZ() { return mFarPlane; }
	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetInvViewMatrix();
	DirectX::XMMATRIX GetProjMatrix(D3D::DeviceContextPtr& p_imm_context);
	DirectX::XMMATRIX GetInvProjMatrix(D3D::DeviceContextPtr& p_imm_context);
	DirectX::XMMATRIX GetInvProjViewMatrix(D3D::DeviceContextPtr& p_imm_context);
	//const std::shared_ptr<Camera>& GetCameraPtr(unsigned int mode);
	DirectX::XMMATRIX GetOrthoView();
	DirectX::XMMATRIX GetOrthoProj(D3D::DeviceContextPtr& p_imm_context);

	//--< SETTER >----------------------------------------------------------------------
	void SetTarget(std::shared_ptr<Actor>& p_actor);
	void SetOrthoPos(Vector3 pos);
	void SetPositionOfMoveableCamera(const Vector3& pos);
	void SetFocusPointOfMovableCamera(const Vector3& pos);

	//--< RENDER UI >-------------------------------------------------------------------
	virtual void RenderUI();

private:
	CameraController(const CameraController& other) = delete;
	CameraController& operator=(const CameraController& other) = delete;
};

using CameraPtr = std::shared_ptr<CameraController>;
