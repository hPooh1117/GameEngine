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
		kMoveable, // 可動カメラ
		kTrace,    // 追跡カメラ

		kNumMax
	};

private:
	// カメラ管理コンテナ
	std::array<std::shared_ptr<Camera>, CameraMode::kNumMax> m_pCameras;
	std::array < std::string, CameraMode::kNumMax > mCameraNameTable;

	// 平行投影カメラ
	std::shared_ptr<OrthoView> m_pOrthoView;


	unsigned int mCurrentMode = kMoveable; // 現在のカメラモード
	unsigned int mPrevMode = kMoveable;    // 以前のカメラモード
	bool         m_bIsBlended = false;     // TRUE : BLEND ENABLE / FALSE : DISABLE
	int          mBlendingTime = 0;            // カメラ同士のブレンドにかける時間(フレーム)計測用変数
	int          mBlendingTimeMax = 90;         // カメラ同士のブレンドにかける時間(フレーム)

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
	Vector3 mCameraPos;      // カメラの位置
	Vector3 mCameraTarget;   // カメラの目標
	Vector3 mCameraUp;       // カメラの上方向
	Matrix mView;        // 現在カメラのビュー行列
	Matrix mProjection;  // 現在カメラの射影行列
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
